# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .Visualiser import Visualiser
from ..input.PatchFileParser import unknown_names_from_description

import csv
import itertools
import math
import multiprocessing
from concurrent.futures import ProcessPoolExecutor

import numpy as np


class PatchContainsPoint:
    """!
    Patch filter (@see PatchFileParser.patch_filter) which accepts only
    patches whose closed bounding box contains a given point

    It is a class rather than a lambda, as it has to be picklable when
    we farm snapshots out to worker processes.
    """

    def __init__(self, point, tolerance):
        self.point = point
        self.tolerance = tolerance

    def __call__(self, offset, size):
        return all(
            offset[d] - self.tolerance * size[d]
            <= self.point[d]
            <= offset[d] + size[d] + self.tolerance * size[d]
            for d in range(len(offset))
        )


class PointProbe(Visualiser):
    """!
    Probe the data within a Peano patch file at one point over time

    This is not a visualiser but a postprocessing tool, but it reuses the
    whole meta file and snapshot logic of the Visualiser. Per snapshot,
    we parse only those patches which contain the probe point (@see
    PatchContainsPoint), pick the finest one of them, and take the value
    of the cell (for cell data) or of the nearest dof (for vertex data)
    which contains the point. There is no interpolation.

    The result is a csv file with one row per snapshot. The first column
    is the snapshot's time stamp, followed by all unknowns of all data
    sets found in the file. If the point is not covered by any patch in a
    snapshot, we write NaNs.

    If the point sits exactly on the face between two patches of the same
    size, we take the patch to the right/top/front, i.e. we use half-open
    intervals [offset,offset+size). Only if no such patch exists (domain
    boundary), we fall back to the patch to the left.
    """

    Tolerance = 1e-8

    def __init__(self, file_name, point, output_file, start, end, verbose=False):
        super(PointProbe, self).__init__(file_name, start, end, verbose)
        self._point = tuple(float(x) for x in point)
        self._output_file = output_file

    def _probe_patch_data(self, patch_file_data, point):
        """!
        Return list of unknowns at point or None if point is not covered
        """
        dimensions = patch_file_data._dimensions
        point = point[:dimensions]
        dof = patch_file_data._dof
        unknowns = patch_file_data._unknowns

        candidates = [
            patch
            for patch in patch_file_data._cell_data
            if all(
                patch.offset[d] - self.Tolerance * patch.size[d]
                <= point[d]
                <= patch.offset[d] + patch.size[d] * (1.0 + self.Tolerance)
                for d in range(dimensions)
            )
        ]
        if not candidates:
            return None

        finest_size = min(patch.size[0] for patch in candidates)
        candidates = [
            patch
            for patch in candidates
            if math.isclose(patch.size[0], finest_size, rel_tol=1e-6)
        ]

        # Prefer patch where point is in half-open interval [offset,offset+size)
        def distance_to_upper_faces(patch):
            return sum(
                1
                for d in range(dimensions)
                if point[d] >= patch.offset[d] + patch.size[d] * (1.0 - self.Tolerance)
            )

        patch = min(candidates, key=distance_to_upper_faces)

        relative_position = [
            min(max((point[d] - patch.offset[d]) / patch.size[d], 0.0), 1.0)
            for d in range(dimensions)
        ]

        if patch_file_data._is_data_associated_to_cell:
            self._assert_default_cell_mapping(patch_file_data)
            index = 0
            for d in reversed(range(dimensions)):
                index_along_axis = min(int(relative_position[d] * dof), dof - 1)
                index = index * dof + index_along_axis
        else:
            dof_positions = np.asarray(patch_file_data._mapping, dtype=float).reshape(
                -1, dimensions
            )
            index = int(
                np.argmin(
                    np.sum((dof_positions - np.asarray(relative_position)) ** 2, axis=1)
                )
            )

        return [
            float(x) for x in patch.values[index * unknowns : (index + 1) * unknowns]
        ]

    def _assert_default_cell_mapping(self, patch_file_data):
        """!
        Cell data with a user-defined (distorted) mapping is not supported
        """
        dimensions = patch_file_data._dimensions
        vertices_per_axis = patch_file_data._dof + 1
        axis = np.linspace(0.0, 1.0, vertices_per_axis)
        # x runs fastest, i.e. reverse the itertools order
        default_mapping = np.asarray(
            [
                coordinate
                for vertex in itertools.product(axis, repeat=dimensions)
                for coordinate in reversed(vertex)
            ]
        )
        mapping = np.asarray(patch_file_data._mapping, dtype=float)
        if mapping.size != default_mapping.size or not np.allclose(
            mapping, default_mapping
        ):
            raise Exception(
                "Probing cell data with a non-default mapping is not supported yet ("
                + patch_file_data._unknown_name
                + ")"
            )

    def process_snapshot(self, snapshot, i):
        """!
        Return (timestamp, dict from unknown name onto (number of unknowns,
        description, values or None))
        """
        if self.verbose:
            print(f"Probe snapshot {i} (t={snapshot.timestamp})")
        data = self.parse_snapshot(snapshot)
        result = {}
        for unknown_name, patch_file_data in data.items():
            result[unknown_name] = (
                patch_file_data._unknowns,
                patch_file_data._description,
                self._probe_patch_data(patch_file_data, self._point),
            )
        return snapshot.timestamp, result

    def _run(self, max_workers):
        self.patch_filter = PatchContainsPoint(self._point, self.Tolerance)
        self.show_progress = max_workers == 1

        if max_workers == 1:
            results = [
                self.process_snapshot(snapshot, i)
                for i, snapshot in enumerate(self.snapshots)
            ]
        else:
            # Use spawn for same reasons as VTUUnstructuredGrid.display()
            with ProcessPoolExecutor(
                max_workers=max_workers,
                mp_context=multiprocessing.get_context("spawn"),
            ) as executor:
                futures = [
                    executor.submit(self.process_snapshot, snapshot, i)
                    for i, snapshot in enumerate(self.snapshots)
                ]
                results = [future.result() for future in futures]

        self._write_csv(sorted(results, key=lambda result: result[0]))

    def _column_names(self, columns, descriptions):
        """!

        One column name per unknown of each data set

        If a data set names its unknowns (@see unknown_names_from_description()),
        we use these names, e.g. h, hu, hv, b. If two data sets use the same
        name, we prefix the names with the data set, e.g. SWEQ_h. Data sets
        without names yield SWEQ_0, SWEQ_1, ...

        """
        names = {
            unknown_name: unknown_names_from_description(
                descriptions.get(unknown_name, ""), unknowns
            )
            for unknown_name, unknowns in columns.items()
        }
        all_names = [name for n in names.values() if n is not None for name in n]
        unique = len(all_names) == len(set(all_names))
        header = []
        for unknown_name, unknowns in columns.items():
            if names[unknown_name] is None:
                header += [f"{unknown_name}_{i}" for i in range(unknowns)]
            elif unique:
                header += names[unknown_name]
            else:
                header += [f"{unknown_name}_{name}" for name in names[unknown_name]]
        return header

    def _write_csv(self, results):
        columns = {}
        descriptions = {}
        for _, result in results:
            for unknown_name, (unknowns, description, _) in result.items():
                columns[unknown_name] = max(unknowns, columns.get(unknown_name, 0))
                descriptions.setdefault(unknown_name, description)

        uncovered_snapshots = 0
        with open(self._output_file, "w", newline="") as csv_file:
            writer = csv.writer(csv_file, delimiter=",")
            writer.writerow(["time"] + self._column_names(columns, descriptions))

            for timestamp, result in results:
                row = [timestamp]
                for unknown_name, unknowns in columns.items():
                    values = result.get(unknown_name, (unknowns, "", None))[2]
                    if values is None:
                        values = [float("nan")] * unknowns
                        uncovered_snapshots += 1
                    row += values
                writer.writerow(row)

        if uncovered_snapshots > 0:
            print(
                f"Warning: point {self._point} not covered by data in {uncovered_snapshots} (snapshot,data set) pair(s), wrote NaN"
            )
        print(f"Wrote {len(results)} snapshots into {self._output_file}")

    def display(self, max_workers=1):
        super(PointProbe, self).display()
        self._run(max_workers)

    def display_single_file(self):
        self.snapshots = [super(PointProbe, self).display_single_file()]
        self._run(1)
