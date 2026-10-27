# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from peano4.api.solversteps.ActionSet import ActionSet


class ConsumeRefinementRequests(ActionSet):
    """!

    Hand the refine and erase requests of the last time step over to Peano,
    and restart the time stepping if the mesh has changed

    The last task of each time step collects refine and erase requests
    within ::exahype2::RefinementControlService
    (@ref exahype2.api.blockstructured.DynamicMeshRefinement). The mesh
    adaptation phase hosts this action set in its repeating task. The action
    set does not issue any further request. It

    - hands the service's committed events over to Peano
      (createGridControlEventsForNextTraversal()), so Peano realises them,
    - calls the service's finishStep() once per rank after each mesh
      sweep, so the events expire after their lifetime. The repeating task
      runs until the service holds no events anymore, i.e. no request
      survives into the next time step, and
    - keeps track of whether a mesh sweep has created or destroyed any cell.
      If so, it resets the solver's time step size to zero
      (exahype2::Solver::resetTimeStepSize()) at the end of the sweep. The
      time step size of the old mesh might violate the CFL condition on the
      new mesh. With a time step size of zero, the subsequent time step does
      not advance in time but derives a new admissible time step size from
      the new mesh.

    Whether the mesh has changed is reduced over all ranks, so all ranks
    reset the time step size consistently.

    """

    def __init__(self, solver):
        """!

        @param solver: exahype2.api.Solver
          The solver whose time step size is reset if the mesh changes.

        """
        super().__init__(descend_invocation_order=1, parallel=False)
        self._solver = solver

    def get_constructor_body(self):
        return ""

    def get_destructor_body(self):
        return ""

    def get_body_of_createGridControlEventsForNextTraversal(self):
        return "  return ::exahype2::RefinementControlService::getInstance().createGridControlEventsForNextTraversal();\n"

    def get_body_of_prepareTraversal(self):
        return "  _meshHasChanged = false;\n"

    def get_body_of_unprepareTraversal(self):
        return f"""
  ::exahype2::RefinementControlService::getInstance().finishStep();

  int localMeshHasChanged  = _meshHasChanged ? 1 : 0;
  int globalMeshHasChanged = localMeshHasChanged;
  #ifdef Parallel
  tarch::mpi::Rank::getInstance().allReduce(
    &localMeshHasChanged, &globalMeshHasChanged, 1, MPI_INT, MPI_MAX,
    [&]() -> void {{ tarch::services::ServiceRepository::getInstance().receiveDanglingMessages(); }}
  );
  #endif
  if (globalMeshHasChanged > 0) {{
    logInfo( "unprepareTraversal()", "mesh has changed, reset time step size of {self._solver.name} to zero" );
    repositories::{self._solver.get_name_of_global_instance()}.resetTimeStepSize();
  }}
"""

    def get_body_of_operation(self, operation_name):
        if operation_name in [
            ActionSet.OPERATION_CREATE_CELL,
            ActionSet.OPERATION_DESTROY_CELL,
        ]:
            return "  _meshHasChanged = true;\n"
        return ""

    def get_attributes(self):
        return """
  static std::atomic<bool> _meshHasChanged;
"""

    def get_static_initialisations(self, full_qualified_classname):
        return (
            f"std::atomic<bool> {full_qualified_classname}::_meshHasChanged(false);\n"
        )

    def get_action_set_name(self):
        return __name__.replace(".py", "").replace(".", "_")

    def user_should_modify_template(self):
        return False

    def get_includes(self):
        return (
            """
#include <atomic>
#include "exahype2/RefinementControlService.h"
#include "tarch/mpi/Rank.h"
#include "tarch/services/ServiceRepository.h"
"""
            + self._solver._get_default_includes()
            + self._solver.user_solver_includes
        )
