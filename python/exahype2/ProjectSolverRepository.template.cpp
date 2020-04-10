#include "SolverRepository.h"

#include <algorithm>


{OPEN_NAMESPACE}
{SOLVER_DEFINITIONS}


double getMinTimeStamp() {{
  return std::min( {{ {SEQUENCE_OF_GET_MIN_TIME_STAMP_CALLS} }} );
}}


double getMaxTimeStamp() {{
  return std::max( {{ {SEQUENCE_OF_GET_MAX_TIME_STAMP_CALLS} }} );
}}


double getMinTimeStepSize() {{
  return std::min( {{ {SEQUENCE_OF_GET_MIN_TIME_STEP_SIZE_CALLS} }} );
}}


double getMaxTimeStepSize() {{
  return std::max( {{ {SEQUENCE_OF_GET_MAX_TIME_STEP_SIZE_CALLS} }} );
}}


void startTimeStep(
  double minTimeStamp,
  double maxTimeStamp,
  double minTimeStepSize,
  double maxTimeStepSize
) {{
  {SEQUENCE_OF_START_TIME_STEP_CALLS}
}}


void finishTimeStep() {{
  {SEQUENCE_OF_FINISH_TIME_STEP_CALLS}
}}

{CLOSE_NAMESPACE}

