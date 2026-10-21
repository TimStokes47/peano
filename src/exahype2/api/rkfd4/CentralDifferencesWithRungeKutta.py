# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import peano4.api
import peano4.api.dastgen2
import peano4.api.datamodel
import toolbox.blockstructured.api

from ..Solver import Solver
from ..rungekutta import ButcherTableau


class CentralDifferencesWithRungeKutta(Solver.NumericalScheme):
    """!

    Fourth-order central Finite Differences with explicit Runge-Kutta time
    stepping, realised over a block-structured mesh

    Each mesh cell hosts a patch of patch_size^d grid cells. The unknowns
    sit in the grid cells' centres. The scheme evaluates the right-hand side
    of

    @f$ \\partial _t Q = F(Q) @f$

    through fourth-order central differences, plus an optional Kreiss-Oliger
    dissipation term of sixth order, and integrates in time with the
    explicit Runge-Kutta scheme from @ref exahype2.api.rungekutta.ButcherTableau.
    The stencils reach three grid cells into the neighbouring patches, i.e.
    the halo is HALO_SIZE=3 grid cells wide.

    ## Data flow

    @ref __create_data_structures() documents the data per mesh cell and
    face. Per time step, the scheme runs through the Runge-Kutta stages
    k=1,...,s. Each stage

    1. reconstructs the stage's input
       @f$ Q^n + \\Delta t \\sum _{j<k} a_{kj} K_j @f$ with its halo. The
       interior is computed from the patch and the estimates of the
       previous stages, the halo is taken from the faces;
    2. evaluates the right-hand side @f$ K_k @f$ over this input and stores
       it in the stage's slot of the estimates;
    3. projects the next stage's input onto the faces, so the neighbours
       can reconstruct their halo from it.

    After the last stage, the patch is updated to
    @f$ Q^{n+1} = Q^n + \\Delta t \\sum _k b_k K_k @f$ and projected onto
    the faces, where the first stage of the next time step picks it up.

    @todo The task graph realising the time step is not there yet:
      construct_output() adds the data to the Peano 4 project, but does not
      add any tasks.

    """

    HALO_SIZE = 3

    def __init__(
        self,
        solver: Solver,
        patch_size: int,
        rk_order: int,
        kreiss_oliger_sigma: float = 0.0,
    ):
        """!

        The constructor memorises the attributes and creates the data
        structures. It does not add the data to the solver's Peano 4
        project yet, but it assigns them to the solver, so anybody else
        working against this solver, such as the grid construction or the
        plotter, finds them there.

        @param solver: exahype2.api.Solver
          The solver this numerical scheme realises.

        @param patch_size: int
          Number of grid cells per axis within one patch, i.e. within one
          mesh cell. Has to be at least HALO_SIZE, as the projection onto
          the faces takes the HALO_SIZE outermost layers of the patch.

        @param rk_order: int
          Order of the Runge-Kutta scheme, i.e. its number of stages. Has
          to be between one and four, @ref
          exahype2.api.rungekutta.ButcherTableau.

        @param kreiss_oliger_sigma: float
          Scaling of the Kreiss-Oliger dissipation. 0.0 switches the
          dissipation off.

        """
        super().__init__(solver)
        if patch_size < self.HALO_SIZE:
            raise ValueError(
                f"Solver {solver.name}: patch_size has to be at least {self.HALO_SIZE}, "
                f"as the fourth-order stencils reach {self.HALO_SIZE} grid cells into "
                f"the neighbouring patches, but got {patch_size}"
            )
        if kreiss_oliger_sigma < 0.0:
            raise ValueError(
                f"Solver {solver.name}: kreiss_oliger_sigma has to be non-negative, "
                f"but got {kreiss_oliger_sigma}"
            )
        self.patch_size = patch_size
        self.butcher_tableau = ButcherTableau(rk_order)
        self.kreiss_oliger_sigma = kreiss_oliger_sigma
        self.__create_data_structures()

    @property
    def overlap(self) -> int:
        """!
        Width of the halo, i.e. number of grid cells each face holds per
        adjacent patch. Always HALO_SIZE.
        """
        return self.HALO_SIZE

    @property
    def rk_order(self) -> int:
        return self.butcher_tableau.order

    def __create_data_structures(self):
        """!

        Create the mesh data structures this numerical scheme requires

        All of them are assigned to self._solver rather than self, so the
        solver's grid construction and plotter, as well as the action sets,
        find them there:

        - _patch: the solution @f$ Q^n @f$, i.e. patch_size^d grid cells per
          mesh cell, with the unknowns and the auxiliary variables per grid
          cell.
        - _patch_with_halo: the input of the current Runge-Kutta stage,
          i.e. (patch_size+2*HALO_SIZE)^d grid cells, reconstructed from
          _patch, _rhs_estimates and the faces (@ref
          toolbox.blockstructured.api.ReconstructPatchWithHalo). It is
          logically temporary.
        - _rhs_estimates: the right-hand side estimates @f$ K_k @f$ of all
          rk_order stages. They hold the unknowns only, as the auxiliary
          variables do not evolve in time. The estimates of the stages are
          stored one after the other, i.e. as rk_order patches of
          patch_size^d grid cells each. This is why the first dimension is
          rk_order*patch_size.
        - _patch_overlap_current_time_step: the face data the halo is
          reconstructed from, i.e. 2*HALO_SIZE layers along the face's
          normal. The face data are exchanged between ranks.
        - _patch_overlap_update: the projection of the patches onto the
          faces which the current sweep accumulates (@ref
          exahype2.api.rkfd4.actionsets.ProjectPatchOntoFaces). It is
          rolled over into _patch_overlap_current_time_step once complete
          (@ref exahype2.api.rkfd4.actionsets.RollOverUpdatedFace).
        - _max_eigenvalue: one double per cell, named after the solver
          followed by MaxEigenvalue. It holds the maximum eigenvalue of the
          patch after its last update and feeds into adaptive time step
          sizes. It is modelled via DaStGen2 as array of cardinality one
          on the heap (peano4.api.dastgen2.Peano4SmartPointerDoubleArray).

        All patches use smart pointers (@ref
        peano4.api.datamodel.PatchToDoubleArrayWithSmartPointer), like the
        Finite Volumes of exahype2.api.fv. The legacy
        exahype2.solvers.rkfd.CellCenteredFiniteDifferences held a third
        face overlap with the data of the previous time step. It is
        required for AMR and local time stepping only, so we do not create
        it.

        """
        total_unknowns = self._solver.unknowns + self._solver.auxiliary_variables
        identifier = self._solver._unknown_identifier()
        n = self.patch_size
        h = self.HALO_SIZE

        self._solver._patch = peano4.api.datamodel.Patch(
            (n, n, n),
            total_unknowns,
            identifier,
        )
        self._solver._patch_with_halo = peano4.api.datamodel.Patch(
            (n + 2 * h, n + 2 * h, n + 2 * h),
            total_unknowns,
            identifier + "WithHalo",
        )
        self._solver._rhs_estimates = peano4.api.datamodel.Patch(
            (self.rk_order * n, n, n),
            self._solver.unknowns,
            identifier + "RhsEstimates",
        )
        self._solver._patch_overlap_current_time_step = peano4.api.datamodel.Patch(
            (2 * h, n, n),
            total_unknowns,
            identifier + "CurrentTimeStep",
        )
        self._solver._patch_overlap_update = peano4.api.datamodel.Patch(
            (2 * h, n, n),
            total_unknowns,
            identifier + "Update",
        )
        self._solver._max_eigenvalue = peano4.api.datamodel.DaStGen2(
            self._solver.name + "MaxEigenvalue"
        )
        self._solver._max_eigenvalue.data.add_attribute(
            peano4.api.dastgen2.Peano4SmartPointerDoubleArray("maxEigenvalue", 1)
        )

    def _data(self):
        """!
        All data of this numerical scheme, in the order they are added to
        the Peano 4 project.
        """
        return [
            self._solver._patch,
            self._solver._patch_with_halo,
            self._solver._rhs_estimates,
            self._solver._patch_overlap_current_time_step,
            self._solver._patch_overlap_update,
            self._solver._max_eigenvalue,
        ]

    def __add_data_to_repository(self, peano_project):
        """!

        Add the data from __create_data_structures() to the Peano 4 project
        and configure their generators

        Only _patch_overlap_current_time_step is exchanged between ranks,
        and only on the faces the solver's grid selector holds for. The
        task graph compiler uses, loads and stores data only where the
        solver's grid selector holds, too (@ref peano4.api.datamodel.DoF).

        """
        peano4.api.Logger().log(
            """
Patch data
----------
{}
Right-hand side estimates
-------------------------
{}
Patch overlap data
------------------
{}
{}""".format(
                str(self._solver._patch),
                str(self._solver._rhs_estimates),
                str(self._solver._patch_overlap_current_time_step),
                str(self._solver._patch_overlap_update),
            )
        )

        peano_project.datamodel.add_cell(self._solver._patch)
        peano_project.datamodel.add_cell(self._solver._patch_with_halo)
        peano_project.datamodel.add_cell(self._solver._rhs_estimates)
        peano_project.datamodel.add_face(self._solver._patch_overlap_current_time_step)
        peano_project.datamodel.add_face(self._solver._patch_overlap_update)
        peano_project.datamodel.add_cell(self._solver._max_eigenvalue)

        for data in self._data():
            data.grid_selector = self._solver.grid_selector

        for patch in self._data()[:-1]:
            patch.generator = peano4.api.datamodel.PatchToDoubleArrayWithSmartPointer(
                patch, "double"
            )

        current_time_step = self._solver._patch_overlap_current_time_step
        current_time_step.generator.merge_method_definition = (
            toolbox.blockstructured.api.get_face_merge_implementation(current_time_step)
        )
        current_time_step.generator.includes += """
#include "peano4/utils/Loop.h"
"""
        current_time_step.generator.send_condition = (
            self._solver.grid_selector.createFaceGuard()
        )
        current_time_step.generator.receive_and_merge_condition = (
            self._solver.grid_selector.createFaceGuard()
        )

        self._solver._patch_overlap_update.generator.send_condition = "false"
        self._solver._patch_overlap_update.generator.receive_and_merge_condition = (
            "false"
        )

    def construct_output(self, exahype2_project, peano_project, time_step):
        """!

        Add the data to the Peano 4 project

        @todo Add the tasks of the Runge-Kutta stages to time_step.

        """
        peano4.api.Logger().log(
            "Create time stepping task graph for fourth-order Finite Differences "
            f"with Runge-Kutta of order {self.rk_order} over block-structured meshes"
        )
        self.__add_data_to_repository(peano_project)
