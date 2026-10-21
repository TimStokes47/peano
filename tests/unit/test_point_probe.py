import csv
import os
import math

import pytest

from tarch.plotter.api.input.PatchFileParser import PatchFileParser
from tarch.plotter.api.output.PointProbe import PointProbe, PatchContainsPoint


# ---------------------------------------------------------------------------
# Helpers writing small synthetic patch files
#
# Each value encodes where it comes from: 100*patch_id + 10*dof_index + unknown,
# so a probed value tells us which patch and which dof have been picked.
# ---------------------------------------------------------------------------


def patch_file(
    dimensions,
    metadata_kind,
    name,
    dof,
    unknowns,
    patches,
    mapping=None,
    description="test data",
):
    """
    patches: list of (patch_id, offset, size)
    metadata_kind: "cell" or "vertex"
    """
    lines = [
        "# ",
        "# Peano patch file ",
        "# Version 0.2 ",
        "# ",
        "format ASCII",
        f"dimensions {dimensions}",
        "",
        f'begin {metadata_kind}-metadata "{name}"',
        f"  number-of-unknowns {unknowns}",
        f"  number-of-dofs-per-axis {dof}",
        f"  description {description}",
    ]
    if mapping is not None:
        lines.append("  mapping " + " ".join(str(x) for x in mapping))
    lines.append(f"end {metadata_kind}-metadata")
    lines.append("")
    for patch_id, offset, size in patches:
        lines += [
            "begin patch",
            "  offset " + " ".join(str(x) for x in offset),
            "  size " + " ".join(str(x) for x in size),
            f'  begin {metadata_kind}-values "{name}"',
            "    "
            + " ".join(
                str(100 * patch_id + 10 * k + u)
                for k in range(dof**dimensions)
                for u in range(unknowns)
            ),
            f"  end {metadata_kind}-values",
            "end patch",
            "",
        ]
    return "\n".join(lines) + "\n"


def meta_file(snapshots):
    """
    snapshots: list of (timestamp, [file names])
    """
    lines = ["# ", "# Peano patch file ", "# Version 0.2 ", "# ", "format ASCII", ""]
    for timestamp, file_names in snapshots:
        lines += ["begin dataset", f"  timestamp  {timestamp:e}", ""]
        lines += [f'  include "{file_name}"' for file_name in file_names]
        lines += ["end dataset", ""]
    return "\n".join(lines) + "\n"


def read_csv(file_name):
    with open(file_name) as f:
        rows = list(csv.reader(f))
    return rows[0], [[float(x) for x in row] for row in rows[1:]]


def probe_single_file(tmp_path, monkeypatch, content, point):
    monkeypatch.chdir(tmp_path)
    (tmp_path / "data.peano-patch-file").write_text(content)
    probe = PointProbe("data.peano-patch-file", point, "probe.csv", 0, 0)
    probe.display_single_file()
    return read_csv("probe.csv")


# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------

TWO_PATCHES_2D = [(1, (0.0, 0.0), (0.5, 0.5)), (2, (0.5, 0.0), (0.5, 0.5))]


def test_cell_data_2d_picks_right_cell(tmp_path, monkeypatch):
    content = patch_file(2, "cell", "Q", 3, 2, TWO_PATCHES_2D)
    # Point in patch 2, cell (x=1,y=2) -> dof index 2*3+1=7
    header, rows = probe_single_file(tmp_path, monkeypatch, content, (0.75, 0.45))
    assert header == ["time", "Q_0", "Q_1"]
    assert rows == [[0.0, 270.0, 271.0]]


def test_point_on_face_takes_upper_patch(tmp_path, monkeypatch):
    content = patch_file(2, "cell", "Q", 3, 1, TWO_PATCHES_2D)
    # x=0.5 is the face between patch 1 and 2 -> patch 2, cell x=0
    _, rows = probe_single_file(tmp_path, monkeypatch, content, (0.5, 0.1))
    assert rows == [[0.0, 200.0]]


def test_point_on_domain_boundary_takes_lower_patch(tmp_path, monkeypatch):
    content = patch_file(2, "cell", "Q", 3, 1, TWO_PATCHES_2D)
    # x=1.0, y=0.5 is the upper right corner of patch 2 -> last cell
    _, rows = probe_single_file(tmp_path, monkeypatch, content, (1.0, 0.5))
    assert rows == [[0.0, 280.0]]


def test_adaptive_mesh_takes_finest_patch(tmp_path, monkeypatch):
    patches = [(1, (0.0, 0.0), (1.0, 1.0)), (2, (0.0, 0.0), (0.5, 0.5))]
    content = patch_file(2, "cell", "Q", 2, 1, patches)
    _, rows = probe_single_file(tmp_path, monkeypatch, content, (0.3, 0.1))
    assert rows == [[0.0, 210.0]]


def test_cell_data_3d(tmp_path, monkeypatch):
    patches = [(1, (0.0, 0.0, 0.0), (1.0, 1.0, 1.0))]
    content = patch_file(3, "cell", "Q", 2, 1, patches)
    # cell (x=1,y=0,z=1) -> 1*4+0*2+1 = 5
    _, rows = probe_single_file(tmp_path, monkeypatch, content, (0.9, 0.1, 0.6))
    assert rows == [[0.0, 150.0]]


def test_2d_data_ignores_z(tmp_path, monkeypatch):
    content = patch_file(2, "cell", "Q", 3, 1, TWO_PATCHES_2D)
    _, rows = probe_single_file(tmp_path, monkeypatch, content, (0.75, 0.45, 42.0))
    assert rows == [[0.0, 270.0]]


def test_vertex_data_nearest_dof_with_mapping(tmp_path, monkeypatch):
    # Non-uniform 3x3 dof layout along each axis: 0, 0.1, 1
    axis = [0.0, 0.1, 1.0]
    mapping = [c for y in axis for x in axis for c in (x, y)]
    patches = [(1, (0.0, 0.0), (1.0, 1.0))]
    content = patch_file(2, "vertex", "u", 3, 1, patches, mapping)
    # x=0.4 is closer to 0.1 than to 1.0, y=0.6 closer to 1.0 -> dof (1,2) = 7
    _, rows = probe_single_file(tmp_path, monkeypatch, content, (0.4, 0.6))
    assert rows == [[0.0, 170.0]]


def test_point_outside_domain_yields_nan(tmp_path, monkeypatch):
    content = patch_file(2, "cell", "Q", 3, 1, TWO_PATCHES_2D)
    _, rows = probe_single_file(tmp_path, monkeypatch, content, (2.0, 0.1))
    assert rows[0][0] == 0.0 and math.isnan(rows[0][1])


def test_cell_data_with_non_default_mapping_is_rejected(tmp_path, monkeypatch):
    mapping = [0.0, 0.0, 0.7, 0.0, 0.0, 1.0, 1.0, 1.0]
    patches = [(1, (0.0, 0.0), (1.0, 1.0))]
    content = patch_file(2, "cell", "Q", 1, 1, patches, mapping)
    with pytest.raises(Exception, match="non-default mapping"):
        probe_single_file(tmp_path, monkeypatch, content, (0.5, 0.5))


def test_meta_file_with_partial_files_and_multiple_snapshots(tmp_path, monkeypatch):
    monkeypatch.chdir(tmp_path)
    # Each snapshot is split over two partial files (two ranks)
    for snapshot in range(2):
        for rank, (patch_id, offset, size) in enumerate(TWO_PATCHES_2D):
            (tmp_path / f"s{snapshot}-r{rank}.peano-patch-file").write_text(
                patch_file(
                    2, "cell", "Q", 3, 1, [(patch_id + 10 * snapshot, offset, size)]
                )
            )
    # Snapshots deliberately written in reverse order to check sorting
    (tmp_path / "data.peano-patch-file").write_text(
        meta_file(
            [
                (0.5, ["s1-r0.peano-patch-file", "s1-r1.peano-patch-file"]),
                (0.0, ["s0-r0.peano-patch-file", "s0-r1.peano-patch-file"]),
            ]
        )
    )
    probe = PointProbe("data.peano-patch-file", (0.75, 0.45), "probe.csv", 0, 10)
    probe.display(1)
    header, rows = read_csv("probe.csv")
    assert header == ["time", "Q_0"]
    assert rows == [[0.0, 270.0], [0.5, 1270.0]]


def test_meta_file_in_other_directory(tmp_path, monkeypatch):
    # The meta file includes its snapshots relative to its own directory, so
    # the probe has to work from any working directory.
    monkeypatch.chdir(tmp_path)
    output = tmp_path / "output"
    output.mkdir()
    for snapshot in range(2):
        (output / f"s{snapshot}.peano-patch-file").write_text(
            patch_file(2, "cell", "Q", 3, 1, [(10 * snapshot, (0.0, 0.0), (1.0, 1.0))])
        )
    (output / "data.peano-patch-file").write_text(
        meta_file([(0.0, ["./s0.peano-patch-file"]), (0.5, ["s1.peano-patch-file"])])
    )
    probe = PointProbe(
        os.path.join("output", "data.peano-patch-file"), (0.5, 0.5), "probe.csv", 0, 10
    )
    probe.display(1)
    _, rows = read_csv("probe.csv")
    assert [row[0] for row in rows] == [0.0, 0.5]
    assert rows[0][1] != rows[1][1]


def test_named_unknowns_become_column_names(tmp_path, monkeypatch):
    content = patch_file(2, "cell", "Q", 3, 2, TWO_PATCHES_2D, description="h, hu")
    header, rows = probe_single_file(tmp_path, monkeypatch, content, (0.75, 0.45))
    assert header == ["time", "h", "hu"]
    assert rows == [[0.0, 270.0, 271.0]]


def test_same_names_in_two_data_sets_are_prefixed(tmp_path, monkeypatch):
    monkeypatch.chdir(tmp_path)
    for name, (patch_id, offset, size) in zip(["A", "B"], TWO_PATCHES_2D):
        (tmp_path / f"{name}.peano-patch-file").write_text(
            patch_file(
                2, "cell", name, 3, 1, [(1, (0.0, 0.0), (1.0, 1.0))], description="h"
            )
        )
    (tmp_path / "data.peano-patch-file").write_text(
        meta_file([(0.0, ["A.peano-patch-file", "B.peano-patch-file"])])
    )
    probe = PointProbe("data.peano-patch-file", (0.5, 0.5), "probe.csv", 0, 10)
    probe.display(1)
    header, _ = read_csv("probe.csv")
    assert header == ["time", "A_h", "B_h"]


def test_patch_filter_skips_patches(tmp_path):
    content = patch_file(
        2, "cell", "Q", 3, 1, TWO_PATCHES_2D + [(3, (0.0, 0.5), (0.5, 0.5))]
    )
    (tmp_path / "data.peano-patch-file").write_text(content)
    parser = PatchFileParser(
        str(tmp_path / "data.peano-patch-file"),
        "",
        0,
        patch_filter=PatchContainsPoint((0.75, 0.25), 1e-8),
    )
    parser.parse_file()
    assert parser.parsed
    patches = parser.unknown_attributes["Q"].cell_data
    assert [patch.offset for patch in patches] == [(0.5, 0.0)]
