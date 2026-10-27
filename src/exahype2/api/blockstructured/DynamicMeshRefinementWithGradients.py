# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .DynamicMeshRefinement import DynamicMeshRefinement


class DynamicMeshRefinementWithGradients(DynamicMeshRefinement):
    """!

    Evaluate the gradient-based refinement criterion and collect the outcome
    as refine and erase requests

    The action set is the counterpart of
    exahype2.api.blockstructured.DynamicMeshRefinement for solvers with an
    exahype2.api.GradientRefinement PDE term. It differs only in what it
    evaluates per volume: For each coordinate axis, it computes the left and
    the right difference quotient of the solution
    (@ref exahype2::fv::computeOneSidedGradient()) and hands each of them to
    the term, i.e. it calls the term 2*Dimensions times per volume. Each
    call starts from Keep.

    A volume asks for a refinement if any call says Refine, and it may be
    erased only if all calls say Erase. As the action set combines the
    volumes of a cell the same way, it simply treats each call like the
    outcome of one further volume. Everything else, i.e. the mesh size
    bounds and the bookkeeping within the refinement service, is inherited.

    """

    ## Per-volume evaluation of the gradient-based criterion
    TemplateEvaluateVolume = """
    for (int normal=0; normal<Dimensions; normal++) {
      for (int side=0; side<2; side++) {
        double gradient[{{NUMBER_OF_UNKNOWNS}} + {{NUMBER_OF_AUXILIARY_VARIABLES}}];
        ::exahype2::fv::computeOneSidedGradient(
          fineGridCell{{UNKNOWN_IDENTIFIER}}.value,
          {{NUMBER_OF_UNKNOWNS}} + {{NUMBER_OF_AUXILIARY_VARIABLES}},
          {{NUMBER_OF_VOLUMES_PER_AXIS}},
          volume,
          normal,
          side==1,
          ::exahype2::fv::getVolumeLength( marker.h(), {{NUMBER_OF_VOLUMES_PER_AXIS}} ),
          gradient
        );
        ::exahype2::RefinementCommand command = ::exahype2::RefinementCommand::Keep;
        repositories::{{SOLVER_INSTANCE}}.refineEraseWithGradient(
          gradient,
          normal,
          &command
        );
        anyVolumeWantsToRefine = anyVolumeWantsToRefine or command == ::exahype2::RefinementCommand::Refine;
        allVolumesWantToErase  = allVolumesWantToErase  and command == ::exahype2::RefinementCommand::Erase;
      }
    }
"""
