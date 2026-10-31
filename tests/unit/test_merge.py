import numpy as np
import pytest

from tarch.plotter.api import merge
from tarch.plotter.api.input.PatchFileParser import PatchFileParser

from test_binary_patch_file import write_file, values_of_patch


def write_meta_file(path, files_per_snapshot):
    lines = ["# ", "# Peano patch file ", "# Version 0.2 ", "# ", "format ASCII", ""]
    for index, files in enumerate(files_per_snapshot):
        lines += ["begin dataset", f"  timestamp  {float(index):e}", ""]
        lines += [f'  include "{name}"' for name in files]
        lines += ["end dataset", ""]
    path.write_text("\n".join(lines) + "\n")


@pytest.mark.parametrize("binary", [False, True])
def test_merge_two_partial_files_of_one_snapshot(tmp_path, monkeypatch, binary):
    monkeypatch.chdir(tmp_path)
    # Two partial files with two patches each; the second pair uses ids 2 and 3
    write_file(tmp_path / "a.peano-patch-file", binary, patches=2, evil=binary)
    write_file(tmp_path / "b.peano-patch-file", binary, patches=2, evil=binary)
    write_meta_file(
        tmp_path / "meta.peano-patch-file",
        [["a.peano-patch-file", "b.peano-patch-file"]],
    )

    start, end = 0, 0
    merge.read_meta_file("meta.peano-patch-file", start, end)

    merged = PatchFileParser(
        str(tmp_path / "merged_patch_file_0.peano-patch-file"), "Q", 0
    )
    merged.parse_file()
    assert merged.parsed
    assert merged.file_format == ("Binary" if binary else "ASCII")

    patches = merged.unknown_attributes["Q"].cell_data
    assert len(patches) == 4
    for index, patch in enumerate(patches):
        expected = values_of_patch(index % 2, evil=binary)
        assert patch.values.tobytes() == expected.tobytes()

    meta = (tmp_path / "merged_meta_file.peano-patch-file").read_text()
    assert 'include "merged_patch_file_0.peano-patch-file"' in meta
