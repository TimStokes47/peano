import vtk

from tarch.plotter.api.input.PatchFileParser import unknown_names_from_description
from tarch.plotter.api.output.VTUUnstructuredGrid import (
    VTUUnstructuredGrid,
    group_unknown_names,
)

from test_point_probe import patch_file


# ---------------------------------------------------------------------------
# Reading the names from the description
# ---------------------------------------------------------------------------


def test_one_name_per_unknown():
    assert unknown_names_from_description(" h, hu, hv, b", 4) == ["h", "hu", "hv", "b"]


def test_names_with_index():
    assert unknown_names_from_description("p, v[0], v[1]", 3) == ["p", "v[0]", "v[1]"]


def test_wrong_number_of_names_is_ignored():
    assert unknown_names_from_description("h, hu, hv", 4) is None


def test_free_text_is_ignored():
    assert unknown_names_from_description("SWEQ", 4) is None
    assert unknown_names_from_description("test data", 1) is None
    assert unknown_names_from_description("", 1) is None


def test_group_names_into_scalars_and_vectors():
    assert group_unknown_names(["p", "v[0]", "v[1]", "b"]) == [
        ("p", [0]),
        ("v", [1, 2]),
        ("b", [3]),
    ]


# ---------------------------------------------------------------------------
# VTU output
# ---------------------------------------------------------------------------


def convert(tmp_path, monkeypatch, description, split_unknowns):
    """Convert one patch with 2x2 cells and three unknowns into a vtu file"""
    monkeypatch.chdir(tmp_path)
    (tmp_path / "data.peano-patch-file").write_text(
        patch_file(
            2, "cell", "Q", 2, 3, [(1, (0.0, 0.0), (1.0, 1.0))], description=description
        )
    )
    visualiser = VTUUnstructuredGrid(
        "data.peano-patch-file", output_directory=".", split_unknowns=split_unknowns
    )
    file_name = visualiser.display_single_file()["Q"]
    reader = vtk.vtkXMLUnstructuredGridReader()
    reader.SetFileName(file_name)
    reader.Update()
    return reader.GetOutput().GetCellData()


def test_components_carry_names(tmp_path, monkeypatch):
    cell_data = convert(tmp_path, monkeypatch, "p, v[0], v[1]", False)
    assert cell_data.GetNumberOfArrays() == 1
    data = cell_data.GetArray(0)
    assert [data.GetComponentName(i) for i in range(4)] == [
        "p",
        "v[0]",
        "v[1]",
        "subdomain",
    ]


def test_split_unknowns_writes_scalar_and_padded_vector(tmp_path, monkeypatch):
    cell_data = convert(tmp_path, monkeypatch, "p, v[0], v[1]", True)
    p = cell_data.GetArray("p")
    v = cell_data.GetArray("v")
    assert p.GetNumberOfComponents() == 1
    assert v.GetNumberOfComponents() == 3
    # Values encode 100*patch + 10*dof + unknown, cf. test_point_probe
    assert p.GetTuple(3) == (130.0,)
    assert v.GetTuple(3) == (131.0, 132.0, 0.0)


def test_no_names_no_labels(tmp_path, monkeypatch):
    cell_data = convert(tmp_path, monkeypatch, "test data", True)
    assert cell_data.GetNumberOfArrays() == 1
    assert cell_data.GetArray(0).GetComponentName(0) is None
