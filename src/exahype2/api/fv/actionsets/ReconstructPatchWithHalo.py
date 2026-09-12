# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import jinja2

from peano4.toolbox.blockstructured.api.ReconstructPatchAndApplyFunctor import ReconstructPatchAndApplyFunctor


class ReconstructPatchWithHalo(ReconstructPatchAndApplyFunctor):
    SolveRiemannProblemsOverPatch = jinja2.Template("""
        double timeStamp = fineGridCell{{SOLVER_NAME}}CellLabel.getTimeStamp();

        // Set the variable
        // double timeStepSize
        {{COMPUTE_TIME_STEP_SIZE}}

        {{PREPROCESS_RECONSTRUCTED_PATCH}}

        assertion2(tarch::la::greaterEquals(timeStamp, 0.0),    timeStamp, timeStepSize);
        assertion2(tarch::la::greaterEquals(timeStepSize, 0.0), timeStamp, timeStepSize);

        ::exahype2::fv::validatePatch(
          oldQWithHalo,
          {{NUMBER_OF_UNKNOWNS}},
          {{NUMBER_OF_AUXILIARY_VARIABLES}},
          {{NUMBER_OF_VOLUMES_PER_AXIS}},
          1, // Halo size
          std::string(__FILE__) + "(" + std::to_string(__LINE__) + "): " + marker.toString()
        ); // Previous time step has to be valid

        ::exahype2::CellData patchData(oldQWithHalo, marker.x(), marker.h(), timeStamp, timeStepSize, newQ);

        {% if STATELESS_PDE_TERMS %}
        if (repositories::{{SOLVER_INSTANCE}}.patchCanUseStatelessPDETerms(
          marker.x(),
          marker.h(),
          timeStamp,
          timeStepSize
        )) {
          ::exahype2::fv::{{KERNEL_NAMESPACE}}::{{COMPUTE_KERNEL_CALL_STATELESS}}
        } else
        {% endif %}

        ::exahype2::fv::{{KERNEL_NAMESPACE}}::{{COMPUTE_KERNEL_CALL}}

        {{POSTPROCESS_UPDATED_PATCH}}

        ::exahype2::fv::validatePatch(
          newQ,
          {{NUMBER_OF_UNKNOWNS}},
          {{NUMBER_OF_AUXILIARY_VARIABLES}},
          {{NUMBER_OF_VOLUMES_PER_AXIS}},
          0, // Halo size
          std::string(__FILE__) + "(" + std::to_string(__LINE__) + "): " + marker.toString()
        ); // Outcome has to be valid

        {% if COMPUTE_MAX_EIGENVALUE %}
        const double maxEigenvalue = patchData.maxEigenvalue[0];
        {% endif %}

        // Set the variable
        // double newTimeStepSize
        {{COMPUTE_NEW_TIME_STEP_SIZE}}

        fineGridCell{{SOLVER_NAME}}CellLabel.setTimeStamp(timeStamp+timeStepSize);
        fineGridCell{{SOLVER_NAME}}CellLabel.setTimeStepSize(newTimeStepSize);
        fineGridCell{{SOLVER_NAME}}CellLabel.setHasUpdated(true);

        repositories::{{SOLVER_INSTANCE}}.update(newTimeStepSize, timeStamp+timeStepSize, marker.h()(0));
    """)

    def __init__(self, solver):
        """
        """
        ReconstructPatchAndApplyFunctor.__init__(self,
            patch = solver._patch,
            patch_overlap = solver._patch_overlap_new,
            functor_implementation = """
#error Please switch to your Riemann solver of choice
""",
            reconstructed_array_memory_location = solver._reconstructed_array_memory_location,
            guard = "not marker.willBeRefined() and not marker.hasBeenRefined()",
            add_assertions_to_halo_exchange = False
        )
        self._solver = solver

        self._Template_TouchCellFirstTime_Epilogue += """
  else if (not marker.willBeRefined() and not marker.hasBeenRefined()) {{
    const double timeStamp    = fineGridCell{SOLVER_NAME}CellLabel.getTimeStamp();
    const double timeStepSize = fineGridCell{SOLVER_NAME}CellLabel.getTimeStepSize();

    assertion2(tarch::la::greaterEquals( timeStepSize, 0.0 ), timeStepSize, timeStamp);
    assertion2(tarch::la::greaterEquals( timeStamp, 0.0 ),    timeStepSize, timeStamp);

    repositories::{SOLVER_INSTANCE}.update(0.0, timeStamp, marker.h()(0) );
  }}
"""

        self._Template_TouchCellFirstTime_Preamble = """
  fineGridCell""" + solver._name + """CellLabel.setHasUpdated(false);
""" + self._Template_TouchCellFirstTime_Preamble

    def _add_action_set_entries_to_dictionary(self, d):
        """
        This is our plug-in point to alter the underlying dictionary
        """
        super(ReconstructPatchWithHalo, self)._add_action_set_entries_to_dictionary(d)

        self._solver._init_dictionary_with_default_parameters(d)
        self._solver.add_entries_to_text_replacement_dictionary(d)

        d["CELL_FUNCTOR_IMPLEMENTATION"] = self.SolveRiemannProblemsOverPatch.render(**d)

    def get_includes(self):
        return """
#include "exahype2/fv/BoundaryConditions.h"
#include "tarch/NonCriticalAssertions.h"
""" + self._solver._get_default_includes() + self._solver.user_action_set_includes
