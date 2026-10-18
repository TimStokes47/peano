# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .AbstractFVActionSet import AbstractFVActionSet

import peano4.api
import jinja2


class HandleBoundary(AbstractFVActionSet):
    """

      The global periodic boundary conditions are set in the Constants.h.

      There is no need to update QOld along the faces. QOld is always rolled
      over from QNew, i.e. we may assume it is already set correctly. It is
      not correctly set in the very first time step (after InitGrid, we haven't
      yet set QOld), so apply the boundary conditions here would even confuse
      the assertions.

      @todo get_body_of_operation() currently returns "": the legacy
      implementation below (still kept around as TemplateHandleBoundary_*)
      needs a per-face FaceLabel (fineGridFace<solver>FaceLabel.getBoundary())
      to tell a domain-boundary face apart from an interior one, and that
      label does not exist in the new exahype2.api yet (@see
      exahype2.api.fv.RusanovWithExplicitEuler.__create_data_structures()'s
      own note on this). Look into this separately once face labels exist.

    """
    TemplateHandleBoundary_Prologue = """
    if (
      {{PREDICATE}}
      and
      not repositories::{{SOLVER_INSTANCE}}.PeriodicBC[marker.getSelectedFaceNumber()%Dimensions]
      and
      not marker.hasBeenRefined()
      and
      fineGridFace{{SOLVER_NAME}}FaceLabel.getBoundary()
    ) {
      logTraceInWith3Arguments( "touchFaceFirstTime(...)", fineGridFace{{SOLVER_NAME}}FaceLabel.toString(), (repositories::{{SOLVER_INSTANCE}}.PeriodicBC[marker.getSelectedFaceNumber()%Dimensions]), marker.toString() );

"""

    TemplateHandleBoundary_KernelCalls = """
      ::exahype2::fv::applyBoundaryConditions(
        [&](
          const double * __restrict__                  Qinside,
          double * __restrict__                        Qoutside,
          const tarch::la::Vector<Dimensions,double>&  faceCentre,
          const tarch::la::Vector<Dimensions,double>&  volumeH,
          double                                       t,
          double                                       dt,
          int                                          normal
        ) -> void {
          repositories::{{SOLVER_INSTANCE}}.boundaryConditions( Qinside, Qoutside, faceCentre, volumeH, t, normal );
        },
        marker.x(),
        marker.h(),
        {{FACE_METADATA_ACCESSOR}}.getNewTimeStamp(marker.getSelectedFaceNumber()<Dimensions ? 1 : 0),
        repositories::{{SOLVER_INSTANCE}}.getMinTimeStepSize(),
        {{NUMBER_OF_VOLUMES_PER_AXIS}},
        {{OVERLAP}},
        {{NUMBER_OF_UNKNOWNS}}+{{NUMBER_OF_AUXILIARY_VARIABLES}},
        marker.getSelectedFaceNumber(),
        fineGridFace{{UNKNOWN_IDENTIFIER}}New.value
      );
      ::exahype2::fv::applyBoundaryConditions(
        [&](
          const double * __restrict__                  Qinside,
          double * __restrict__                        Qoutside,
          const tarch::la::Vector<Dimensions,double>&  faceCentre,
          const tarch::la::Vector<Dimensions,double>&  volumeH,
          double                                       t,
          double                                       dt,
          int                                          normal
        ) -> void {
          repositories::{{SOLVER_INSTANCE}}.boundaryConditions( Qinside, Qoutside, faceCentre, volumeH, t, normal );
        },
        marker.x(),
        marker.h(),
        {{FACE_METADATA_ACCESSOR}}.getOldTimeStamp(marker.getSelectedFaceNumber()<Dimensions ? 1 : 0),
        repositories::{{SOLVER_INSTANCE}}.getMinTimeStepSize(),
        {{NUMBER_OF_VOLUMES_PER_AXIS}},
        {{OVERLAP}},
        {{NUMBER_OF_UNKNOWNS}}+{{NUMBER_OF_AUXILIARY_VARIABLES}},
        marker.getSelectedFaceNumber(),
        fineGridFace{{UNKNOWN_IDENTIFIER}}Old.value
      );
"""

    TemplateHandleBoundary_Epilogue = """

      bool isLeftEntryOutside = marker.getSelectedFaceNumber() < Dimensions;
      double innerTimeStamp;

      innerTimeStamp = {{FACE_METADATA_ACCESSOR}}.getUpdatedTimeStamp( isLeftEntryOutside ? 1 : 0 );
      {{FACE_METADATA_ACCESSOR}}.setUpdatedTimeStamp( isLeftEntryOutside ? 0 : 1, innerTimeStamp );

      innerTimeStamp = {{FACE_METADATA_ACCESSOR}}.getNewTimeStamp( isLeftEntryOutside ? 1 : 0 );
      {{FACE_METADATA_ACCESSOR}}.setNewTimeStamp( isLeftEntryOutside ? 0 : 1, innerTimeStamp );

      innerTimeStamp = {{FACE_METADATA_ACCESSOR}}.getOldTimeStamp( isLeftEntryOutside ? 1 : 0 );
      {{FACE_METADATA_ACCESSOR}}.setOldTimeStamp( isLeftEntryOutside ? 0 : 1, innerTimeStamp );

      logTraceOut( "touchFaceFirstTime(...)" );
    }
    else {
      logDebug( "touchFaceFirstTime(...)", "skip face " << marker.toString() );
    }
"""

    def __init__(self, solver, guard):
        AbstractFVActionSet.__init__(self, solver)
        self.guard = guard

    def get_body_of_operation(self, operation_name):
        # @todo Stubbed out - see this class' docstring: needs a per-face
        # FaceLabel that does not exist yet in the new exahype2.api.
        return ""

    def get_includes(self):
        return """
#include "exahype2/fv/BoundaryConditions.h"
""" + AbstractFVActionSet.get_includes(self)

    def get_action_set_name(self):
        return __name__.replace(".py", "").replace(".", "_")
