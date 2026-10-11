# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .Visualiser import Visualiser
from ..input.PatchFileParser import unknown_names_from_description

import re
import vtk
import time
import traceback
import multiprocessing
from concurrent.futures import ProcessPoolExecutor


def prepare_2D_patches_unstructured_grid(
    cell_data,
    dof,
    unknowns,
    depth_scaling,
    description,
    is_data_associated_to_cell,
    mapping,
):
    """
    Prepare 2D patches in a unstructured grid for rendering

    Parameters:
    ----------
    cell_data: list of patches
      List of Patches with file data

    dof: Integer
      Number of degrees of freedom per axis

    unknowns: Integer
      Number of unknowns per patch volume

    Returns:
    ----------
    grid: vtk.vtkUnstructuredGrid
      Grid cells
    """

    points = vtk.vtkPoints()
    cells = vtk.vtkCellArray()
    grid = vtk.vtkUnstructuredGrid()

    data = vtk.vtkDoubleArray()
    data.SetNumberOfComponents(unknowns + 1)
    if description != "":
        data.SetName("Unknowns (" + description + ", subdomain)")
    else:
        data.SetName("Unknowns (..., subdomain)")

    # Adjust dof per axis based on association type
    dofs_in_patch_per_axis = dof - 1 if not is_data_associated_to_cell else dof

    points.SetNumberOfPoints(len(cell_data) * (dofs_in_patch_per_axis + 1) ** 2)
    cells.SetNumberOfCells(len(cell_data) * dofs_in_patch_per_axis**2)
    point_counter = 0

    for p in range(len(cell_data)):
        patch_x_0, patch_y_0 = cell_data[p].offset
        patch_width, patch_height = cell_data[p].size
        z = patch_width * depth_scaling

        mapping_count = 0
        for k in range(dofs_in_patch_per_axis + 1):
            for l in range(dofs_in_patch_per_axis + 1):
                x = patch_x_0 + mapping[mapping_count] * patch_width
                mapping_count += 1
                y = patch_y_0 + mapping[mapping_count] * patch_height
                mapping_count += 1
                points.SetPoint(point_counter, x, y, z)
                point_counter += 1

        for k in range(dofs_in_patch_per_axis):
            for l in range(dofs_in_patch_per_axis):
                cells.InsertNextCell(4)
                cells.InsertCellPoint(
                    p * (dofs_in_patch_per_axis + 1) * (dofs_in_patch_per_axis + 1)
                    + (k + 0) * (dofs_in_patch_per_axis + 1)
                    + l
                    + 0
                )
                cells.InsertCellPoint(
                    p * (dofs_in_patch_per_axis + 1) * (dofs_in_patch_per_axis + 1)
                    + (k + 0) * (dofs_in_patch_per_axis + 1)
                    + l
                    + 1
                )
                cells.InsertCellPoint(
                    p * (dofs_in_patch_per_axis + 1) * (dofs_in_patch_per_axis + 1)
                    + (k + 1) * (dofs_in_patch_per_axis + 1)
                    + l
                    + 0
                )
                cells.InsertCellPoint(
                    p * (dofs_in_patch_per_axis + 1) * (dofs_in_patch_per_axis + 1)
                    + (k + 1) * (dofs_in_patch_per_axis + 1)
                    + l
                    + 1
                )

        if is_data_associated_to_cell:
            for k in range(dofs_in_patch_per_axis):
                for l in range(dofs_in_patch_per_axis):
                    cell_number = k * dof + l
                    new_data = [
                        x
                        for x in cell_data[p].values[
                            cell_number * unknowns : cell_number * unknowns + unknowns
                        ]
                    ]
                    new_data.append(cell_data[p].subdomain_number)
                    data.InsertNextTuple(new_data)
        else:
            for k in range(dofs_in_patch_per_axis + 1):
                for l in range(dofs_in_patch_per_axis + 1):
                    vertex_number = k * (dofs_in_patch_per_axis + 1) + l
                    new_data = [
                        x
                        for x in cell_data[p].values[
                            vertex_number * unknowns : vertex_number * unknowns
                            + unknowns
                        ]
                    ]
                    new_data.append(cell_data[p].subdomain_number)
                    data.InsertNextTuple(new_data)

    grid.SetPoints(points)
    grid.SetCells(vtk.VTK_PIXEL, cells)

    if is_data_associated_to_cell:
        grid.GetCellData().SetScalars(data)
    else:
        grid.GetPointData().SetScalars(data)

    return grid


def prepare_3D_patches_unstructured_grid(
    cell_data, dof, unknowns, description, is_data_associated_to_cell, mapping
):
    """
    Prepare 3D patches in a unstructured grid for rendering

    Parameters:
    ----------
    cell_data: list of patches
      List of Patches with file data

    dof: Integer
      Number of degrees of freedom per axis

    unknowns: Integer
      Number of unknowns per patch volume

    Returns:
    ----------
    grid: vtk.vtkUnstructuredGrid
      Grid cells
    """
    dofs_in_patch_per_axis = dof
    if not is_data_associated_to_cell:
        dofs_in_patch_per_axis -= 1

    points = vtk.vtkPoints()
    cells = vtk.vtkCellArray()
    grid = vtk.vtkUnstructuredGrid()

    data = vtk.vtkDoubleArray()
    data.SetNumberOfComponents(unknowns + 1)
    if description != "":
        data.SetName("Unknowns (" + description + ", subdomain)")
    else:
        data.SetName("Unknowns (..., subdomain)")

    point_counter = 0
    points.SetNumberOfPoints(len(cell_data) * (dofs_in_patch_per_axis + 1) ** 3)

    for p in range(len(cell_data)):
        patch_x_0 = cell_data[p].offset[0]
        patch_y_0 = cell_data[p].offset[1]
        patch_z_0 = cell_data[p].offset[2]

        mapping_count = 0
        for k in range(dofs_in_patch_per_axis + 1):
            for l in range(dofs_in_patch_per_axis + 1):
                for m in range(dofs_in_patch_per_axis + 1):
                    x = patch_x_0 + mapping[mapping_count] * cell_data[p].size[0]
                    mapping_count += 1
                    y = patch_y_0 + mapping[mapping_count] * cell_data[p].size[1]
                    mapping_count += 1
                    z = patch_z_0 + mapping[mapping_count] * cell_data[p].size[2]
                    mapping_count += 1
                    points.SetPoint(point_counter, x, y, z)
                    point_counter += 1

        for k in range(dofs_in_patch_per_axis):
            for l in range(dofs_in_patch_per_axis):
                for m in range(dofs_in_patch_per_axis):
                    cells.InsertNextCell(8)
                    cells.InsertCellPoint(
                        p
                        * (dofs_in_patch_per_axis + 1)
                        * (dofs_in_patch_per_axis + 1)
                        * (dofs_in_patch_per_axis + 1)
                        + (k + 0)
                        * (dofs_in_patch_per_axis + 1)
                        * (dofs_in_patch_per_axis + 1)
                        + (l + 0) * (dofs_in_patch_per_axis + 1)
                        + m
                        + 0
                    )
                    cells.InsertCellPoint(
                        p
                        * (dofs_in_patch_per_axis + 1)
                        * (dofs_in_patch_per_axis + 1)
                        * (dofs_in_patch_per_axis + 1)
                        + (k + 0)
                        * (dofs_in_patch_per_axis + 1)
                        * (dofs_in_patch_per_axis + 1)
                        + (l + 0) * (dofs_in_patch_per_axis + 1)
                        + m
                        + 1
                    )
                    cells.InsertCellPoint(
                        p
                        * (dofs_in_patch_per_axis + 1)
                        * (dofs_in_patch_per_axis + 1)
                        * (dofs_in_patch_per_axis + 1)
                        + (k + 0)
                        * (dofs_in_patch_per_axis + 1)
                        * (dofs_in_patch_per_axis + 1)
                        + (l + 1) * (dofs_in_patch_per_axis + 1)
                        + m
                        + 0
                    )
                    cells.InsertCellPoint(
                        p
                        * (dofs_in_patch_per_axis + 1)
                        * (dofs_in_patch_per_axis + 1)
                        * (dofs_in_patch_per_axis + 1)
                        + (k + 0)
                        * (dofs_in_patch_per_axis + 1)
                        * (dofs_in_patch_per_axis + 1)
                        + (l + 1) * (dofs_in_patch_per_axis + 1)
                        + m
                        + 1
                    )
                    cells.InsertCellPoint(
                        p
                        * (dofs_in_patch_per_axis + 1)
                        * (dofs_in_patch_per_axis + 1)
                        * (dofs_in_patch_per_axis + 1)
                        + (k + 1)
                        * (dofs_in_patch_per_axis + 1)
                        * (dofs_in_patch_per_axis + 1)
                        + (l + 0) * (dofs_in_patch_per_axis + 1)
                        + m
                        + 0
                    )
                    cells.InsertCellPoint(
                        p
                        * (dofs_in_patch_per_axis + 1)
                        * (dofs_in_patch_per_axis + 1)
                        * (dofs_in_patch_per_axis + 1)
                        + (k + 1)
                        * (dofs_in_patch_per_axis + 1)
                        * (dofs_in_patch_per_axis + 1)
                        + (l + 0) * (dofs_in_patch_per_axis + 1)
                        + m
                        + 1
                    )
                    cells.InsertCellPoint(
                        p
                        * (dofs_in_patch_per_axis + 1)
                        * (dofs_in_patch_per_axis + 1)
                        * (dofs_in_patch_per_axis + 1)
                        + (k + 1)
                        * (dofs_in_patch_per_axis + 1)
                        * (dofs_in_patch_per_axis + 1)
                        + (l + 1) * (dofs_in_patch_per_axis + 1)
                        + m
                        + 0
                    )
                    cells.InsertCellPoint(
                        p
                        * (dofs_in_patch_per_axis + 1)
                        * (dofs_in_patch_per_axis + 1)
                        * (dofs_in_patch_per_axis + 1)
                        + (k + 1)
                        * (dofs_in_patch_per_axis + 1)
                        * (dofs_in_patch_per_axis + 1)
                        + (l + 1) * (dofs_in_patch_per_axis + 1)
                        + m
                        + 1
                    )

        if is_data_associated_to_cell:
            for k in range(dofs_in_patch_per_axis):
                for l in range(dofs_in_patch_per_axis):
                    for m in range(dofs_in_patch_per_axis):
                        cell_number = k * dof * dof + l * dof + m
                        new_data = [
                            x
                            for x in cell_data[p].values[
                                cell_number * unknowns : cell_number * unknowns
                                + unknowns
                            ]
                        ]
                        new_data.append(cell_data[p].subdomain_number)
                        data.InsertNextTuple(new_data)
        else:
            for k in range(dofs_in_patch_per_axis + 1):
                for l in range(dofs_in_patch_per_axis + 1):
                    for m in range(dofs_in_patch_per_axis + 1):
                        vertex_number = (
                            k
                            * (dofs_in_patch_per_axis + 1)
                            * (dofs_in_patch_per_axis + 1)
                            + l * (dofs_in_patch_per_axis + 1)
                            + m
                        )
                        new_data = [
                            x
                            for x in cell_data[p].values[
                                vertex_number * unknowns : vertex_number * unknowns
                                + unknowns
                            ]
                        ]
                        new_data.append(cell_data[p].subdomain_number)
                        data.InsertNextTuple(new_data)

    grid.SetPoints(points)
    grid.SetCells(vtk.VTK_VOXEL, cells)

    if is_data_associated_to_cell:
        grid.GetCellData().SetScalars(data)
    else:
        grid.GetPointData().SetScalars(data)

    return grid


def group_unknown_names(names):
    """!

    Group the names of the unknowns into the arrays we write

    A name v[i] belongs to the vector v, all other names are scalars. Returns
    a list of (array name, list of indices of the unknowns), in the order in
    which the arrays first show up, e.g. ["p", "v[0]", "v[1]"] yields
    [("p", [0]), ("v", [1, 2])].

    """
    groups = {}
    for index, name in enumerate(names):
        match = re.fullmatch(r"(.+)\[\d+\]", name)
        groups.setdefault(match.group(1) if match else name, []).append(index)
    return list(groups.items())


def name_unknowns(
    grid, description, unknowns, is_data_associated_to_cell, split_unknowns
):
    """!

    Label the unknowns of a grid with their symbolic names

    If the description of the unknowns holds one name per unknown (@see
    unknown_names_from_description()), we name the components of the data
    array accordingly, plus the trailing subdomain component. Paraview then
    offers the names when you colour by the array. Otherwise, we leave the
    grid alone.

    split_unknowns: Boolean
      If set, we additionally add one array per variable, so filters such as
      Paraview's Calculator or Contour can address it by name. A scalar
      becomes an array with one component. The entries v[0], v[1], ... of a
      variable with multiplicity become one vector array v. As Paraview only
      treats arrays with three components as vectors, e.g. for glyphs, we
      pad a vector with two entries with a zero.

    """
    names = unknown_names_from_description(description, unknowns)
    if names is None:
        return

    field_data = (
        grid.GetCellData() if is_data_associated_to_cell else grid.GetPointData()
    )
    data = field_data.GetScalars()
    for index, name in enumerate(names):
        data.SetComponentName(index, name)
    data.SetComponentName(unknowns, "subdomain")

    if split_unknowns:
        for array_name, indices in group_unknown_names(names):
            array = vtk.vtkDoubleArray()
            array.SetName(array_name)
            array.SetNumberOfComponents(3 if len(indices) == 2 else len(indices))
            array.SetNumberOfTuples(data.GetNumberOfTuples())
            if len(indices) == 2:
                array.FillComponent(2, 0.0)
            for component, index in enumerate(indices):
                array.CopyComponent(component, data, index)
            field_data.AddArray(array)


class VTUUnstructuredGrid(Visualiser):
    """!
    The visualiser is first and foremost a persistency layer around
    datasets. It does not work on the command line.

    Use this class by feeding the patch meta file you want to render
    into render.py, which will call this class.

    Everything happens in either display() or display_single_file().
    Please see documentation of these functions for more info.
    """

    def __init__(
        self,
        file_name,
        output_directory=".",
        strip_relative_paths=True,
        start=0,
        end=0,
        verbose=False,
        split_unknowns=False,
    ):
        """!

        split_unknowns: Boolean
          Write one additional array per (named) variable. @see name_unknowns()

        """
        super(VTUUnstructuredGrid, self).__init__(file_name, start, end, verbose)
        self.split_unknowns = split_unknowns
        self._tp = None
        self._grid = None
        self.output_directory = output_directory
        self.display_as_tree = False
        self._strip_relative_paths = strip_relative_paths

        # This is the standard header that we can put at the top of all pvd files. Don't overwrite it
        self.pvd_file = """<?xml version="1.0"?>
<VTKFile type="Collection" version="0.1"
         byte_order="LittleEndian"
         compressor="vtkZLibDataCompressor">
  <Collection>
"""

    def process_snapshot(self, snapshot, i):
        """
        Helper function to process a single snapshot.
        Calls parse_and_render_snapshot and returns the snapshot timestamp and filenames.
        """
        if self.verbose:
            print("==================================")
            print(f"Process snapshot {i}")
            print("==================================")
        written_filenames = self.parse_and_render_snapshot(snapshot, i)
        return snapshot.timestamp, written_filenames

    def display(self, max_workers=None):
        """
        Should be called only once.

        How does this work?

        - First, hand over to the superclass display() function.
          This reads the metadata file, which should be available
          as self._file_name. This will populate self.snapshots
          with a list of the Snapshots helper classes. This helper
          class just contains timestamp plus a list of all patch
          files to parse for this timestamp

        - Once the list of snapshots has been populated, we loop
          over each snapshot, calling parse_and_render_snapshot()

        For each unknown that we see on each cell/vertex, we
        produce an ordered list of VTUUnstructuredGrid files, and a PVD file
        to glue them together.
        """

        super(VTUUnstructuredGrid, self).display()

        # This will be the file name that we output
        self._file_name = self._file_name.replace(".peano-patch-file", "")

        # Per-file progress bars (@see Visualiser.show_progress) only
        # produce clean output when this process is the only one parsing
        # files at any given time - true for max_workers==1 (render.py's
        # own default), but not once snapshots get farmed out to worker
        # processes below, where several processes would otherwise write
        # their own \r-updated line to the same shared terminal.
        self.show_progress = max_workers == 1

        start_time = time.time()
        try:
            filenames = {}
            total_snapshots = len(self.snapshots)
            completed_snapshots = 0

            """
            Here's how this loop works:
              - call parse_and_render for each dataset
              - this should write the VTUUnstructuredGrid file and return the filename it wrote
              - we collect these filenames and then write the PVD files at the end
            """
            if max_workers == 1:
                # A process pool with exactly one worker buys no parallelism,
                # and spinning one up at all touches Python's multiprocessing
                # machinery - including its resource-tracker helper process,
                # which is always forked regardless of start method (see the
                # comment on the ProcessPoolExecutor branch below). That is
                # reliably unsafe under pvpython, whose process already has
                # VTK/ParaView's C++ runtime initialised. So for the common,
                # default case of a single worker, just process every
                # snapshot directly in this process - no subprocess at all.
                workers_used = 1
                print("Processing snapshots sequentially (1 worker)")
                results = [
                    self.process_snapshot(snapshot, i)
                    for i, snapshot in enumerate(self.snapshots)
                ]
            else:
                # Explicitly use "spawn" rather than the platform default
                # ("fork" on Linux): render.py is frequently run through
                # pvpython, whose process already has VTK/ParaView's C++
                # runtime (and possibly MPI) fully initialised before this
                # script starts. Forking that process to create worker
                # processes inherits that half-initialised state and
                # reliably crashes the worker (BrokenProcessPool / heap
                # corruption). "spawn" starts genuinely fresh interpreters
                # instead. Note this does not protect against
                # multiprocessing's own resource-tracker helper process,
                # which is always forked regardless of context - real
                # (>1-worker) parallelism may therefore still be unsafe
                # under some pvpython builds; if so, run render.py through
                # plain python3 instead (with vtk installed from Peano's
                # requirements.txt), not through pvpython.
                with ProcessPoolExecutor(
                    max_workers=max_workers,
                    mp_context=multiprocessing.get_context("spawn"),
                ) as executor:
                    workers_used = executor._max_workers
                    print(
                        f"Processing snapshots in parallel using {workers_used} workers"
                    )
                    # Loop over all the snapshots we detected whilst reading metadata
                    # Submit all snapshots to be processed in parallel
                    futures = [
                        executor.submit(self.process_snapshot, snapshot, i)
                        for i, snapshot in enumerate(self.snapshots)
                    ]
                    results = [future.result() for future in futures]

            for timestamp, written_filenames in results:
                # Update filenames dictionary with the results from each snapshot
                for unknown_name, file_name in written_filenames.items():
                    if unknown_name not in filenames.keys():
                        # Haven't seen the name of this unknown before
                        filenames[unknown_name] = []
                    # Add the vtu filename we've written to the list for this unknown
                    filenames[unknown_name].append(
                        (timestamp, file_name)
                    )  # Assuming file_name is not a list

                completed_snapshots += 1
                progress = (completed_snapshots / total_snapshots) * 100
                print(
                    f"Progress: {completed_snapshots}/{total_snapshots} snapshots processed ({progress:.2f}%)"
                )

        except Exception as e:
            print("Got an exception: " + str(e))
            traceback.print_exc()
            print(
                "Aborting: not all snapshots were rendered, so no .pvd file(s) will "
                "be written. Re-run once the underlying problem above is fixed."
            )
            return
        end_time = time.time()

        print(
            f"Completed in {end_time - start_time:.2f} seconds with {workers_used} workers"
        )
        print(f"Dumped {sum([len(v) for _,v in filenames.items()])} datasets")

        # Next, we need to write a pvd "glue" file for each of these data series
        any_relative_path_written = False
        for unknown_name, file_data in filenames.items():
            # Make a copy of self.pvd file
            pvd_file = self.pvd_file

            # Now we need to loop over all of the pairs of (timestamp, file_name)
            for timestamp, file_name in file_data:
                if self._strip_relative_paths:
                    file_name = file_name.split("/")[-1]
                elif "/" in file_name:
                    any_relative_path_written = True

                # Add the vtu filename to the pvd file.
                pvd_file += f"""
    <DataSet timestep=\"{timestamp}\" group=\"\" part=\"0\" file=\"{file_name}\" />
"""
            pvd_file += """
  </Collection>
</VTKFile>
"""
            # Finally, let's write this pvd file.
            with open(
                f"{self.output_directory}/{self._file_name}.{unknown_name}.pvd", "w"
            ) as meta_file:
                meta_file.write(pvd_file)

        if any_relative_path_written:
            print(
                """
WARNING: The conversion tool has processed a dataset stored in a different
directory. If the resulting .pvd file(s)
contains relative paths, you will have to copy it into the present working
directory before you load the outcome into ParaView. Otherwise, ParaView
will not be able to locate your actual data files.

Alternatively, you can rerun the postprocessing and eliminate relative
paths (see documentation/help of your conversion script).
"""
            )

    def display_single_file(self):
        snapshot = super(VTUUnstructuredGrid, self).display_single_file()
        return self.parse_and_render_snapshot(snapshot)

    def write_vtu(self, unknown_name, grid_data, index=None):
        """!
        unknown_name should describe which unknown we are
        writing here.

        Add an index to be placed in the filename
        """
        if index is None:
            # We splice this number into the filename.
            # So as to avoid another if/else statement,
            # we make this the empty string if not
            # supplied any integer.
            index = ""
        else:
            index = f"-{index}"  # Add a dash in
        if self._file_name.endswith(".vtu"):
            # Strip the vtu off
            self._file_name = self._file_name[:-4]
        file_name = (
            f"{self.output_directory}/{self._file_name}.{unknown_name}{index}.vtu"
        )

        writer = vtk.vtkXMLUnstructuredGridWriter()
        writer.SetDataModeToBinary()
        writer.SetFileName(file_name)
        writer.SetInputData(grid_data)
        writer.Write()
        del writer  # Explicitly destroy object to speed up things

        if self.verbose:
            print("Wrote file " + file_name)

        return file_name

    def parse_and_render_snapshot(self, snapshot, index=0):
        """!
        We want to call the method in superclass,
        and then process this data into VTUUnstructuredGrid.

        Returns the filenames that we wrote, for each
        unknown. We combine these into vtk at the end.
        """
        data_to_be_rendered = super(
            VTUUnstructuredGrid, self
        ).parse_and_render_snapshot(snapshot)

        written_filenames = {}
        grid = None

        for unknown_name, unknown_data in data_to_be_rendered.items():
            """
            Need to convert all of this data into 2d/3d patches
            """
            if unknown_data._cell_data != []:
                if unknown_data._dimensions == 2:
                    grid = prepare_2D_patches_unstructured_grid(
                        unknown_data._cell_data,
                        unknown_data._dof,
                        unknown_data._unknowns,
                        1.0 if self.display_as_tree else 0.0,
                        unknown_data._description,
                        unknown_data._is_data_associated_to_cell,
                        unknown_data._mapping,
                    )
                else:
                    grid = prepare_3D_patches_unstructured_grid(
                        unknown_data._cell_data,
                        unknown_data._dof,
                        unknown_data._unknowns,
                        unknown_data._description,
                        unknown_data._is_data_associated_to_cell,
                        unknown_data._mapping,
                    )
                name_unknowns(
                    grid,
                    unknown_data._description,
                    unknown_data._unknowns,
                    unknown_data._is_data_associated_to_cell,
                    self.split_unknowns,
                )
            else:
                assert False, "Found empty cell data"
                # Write this data to vtu, return the filename for later processing
            if grid is not None:
                written_filenames[unknown_name] = self.write_vtu(
                    unknown_name, grid, index
                )

        return written_filenames
