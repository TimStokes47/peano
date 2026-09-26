# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import peano4.api
import peano4.api.actionsets

from ..InitPointWisely import InitPointWisely
from ..Solver import Solver
from .actionsets import InitialCondition
from .actionsets import ProjectPatchOntoFaces


class SetupSimulationOverRegularGrid(Solver.GridConstruction):
    """!

    Setup phase counterpart to RusanovWithExplicitEuler: sets the initial
    condition for a Finite Volume scheme with a Rusanov Riemann solver,
    realised over a block-structured mesh, and prepares the data required
    for the first time step.

    Unlike SetupSimulationWithUserCodedRefinementCriterion, this variant
    does not refine the mesh at all: it assumes the mesh is already the
    regular grid it is going to stay, so setup only has to fill in the
    initial condition and project it onto the faces.

    The solver this is attached to has to have an exahype2.api.InitPointWisely
    PDE term registered (via solver.add_pde_term(...)), as that is what fills
    in the initial condition this class' setup task graph relies on;
    construct_output() asserts this before it builds anything.

    """

    def __init__(self, solver: Solver, volume_size: float):
        """!

        volume_size: float
          Edge length of one Finite Volume on the regular grid this setup
          phase assumes.

        """
        super().__init__(solver)
        self.volume_size = volume_size
        
        

    def construct_output(self,
                         exahype2_project,
                         peano_project,
                         setup_simulation):
        """!
        
        Create the actual sequence of tasks following the description
        from @ref "Adding a numerical scheme", as grid construction, 
        numerical scheme, but also plotting all follow the same pattern 
        of tasks and dependencies.

        """
        peano4.api.Logger().log("Create setup task graph for Finite Volume scheme over a regular, block-structured mesh with Rusanov flux")

        assert any(isinstance(term, InitPointWisely) for term in self._solver.pde_terms), (
            f"Solver {self._solver.name} has no InitPointWisely PDE term registered. "
            "SetupSimulationOverRegularGrid needs one to fill in the initial condition - "
            "add one via solver.add_pde_term(exahype2.api.InitPointWisely(...))."
        )

        refine_task                 = peano4.api.taskgraph.Task(name=f"{self._solver.name}_Refine mesh",
                                                                entity=peano4.api.taskgraph.Task.GridEntity.GLOBAL,
                                                                task_has_to_repeat="not peano4::grid::isGridStationary( peano4::parallel::SpacetreeSet::getInstance().getGridStatistics() )"
                                                                )
        # @todo Wie also have to test against load balancing!

        assert hasattr(self._solver.numerical_scheme, "patch_size"), (
            f"Solver {self._solver.name}'s numerical scheme has no attribute "
            "patch_size. SetupSimulationOverRegularGrid assumes the numerical "
            "scheme has added this field to itself immediately upon construction."
        )
        refine_task.add_action_set(peano4.api.actionsets.ConstructRegularGrid(self.volume_size * self._solver.numerical_scheme.patch_size))

        initial_condition_task        = peano4.api.taskgraph.Task(name=f"{self._solver.name} Initial condition", entity=peano4.api.taskgraph.Task.GridEntity.CELL)
        project_on_faces_task         = peano4.api.taskgraph.Task(name=f"{self._solver.name} Project on faces", entity=peano4.api.taskgraph.Task.GridEntity.CELL)
        roll_over_face_solution_task  = peano4.api.taskgraph.Task(name=f"{self._solver.name} Roll over solution faces", entity=peano4.api.taskgraph.Task.GridEntity.FACE)

        initial_condition_task.add_dependency(refine_task)
        project_on_faces_task.add_dependency(initial_condition_task)

        initial_condition_task.add_action_set(
            InitialCondition(self._solver, guard="true")
        )
        project_on_faces_task.add_action_set(
            ProjectPatchOntoFaces(self._solver, guard="true")
        )

        setup_simulation.add_task(refine_task)
        setup_simulation.add_task(initial_condition_task)
        setup_simulation.add_task(project_on_faces_task)

        initial_condition_task.use_cell(self._solver._patch, peano4.api.taskgraph.Task.DataAccess.WRITE)
        project_on_faces_task.use_cell(self._solver._patch, peano4.api.taskgraph.Task.DataAccess.READ_AND_WRITE)
        project_on_faces_task.use_face(self._solver._patch_overlap_update, peano4.api.taskgraph.Task.DataAccess.WRITE)

        if peano4.api.Logger().is_visualisation:
            setup_simulation.visualise(title="Setup task graph for {}".format(self._solver.name))
        elif peano4.api.Logger().is_text:
            setup_simulation.log_as_text(title="Setup task graph for {}".format(self._solver.name))
