#include "{CLASSNAME}.h"


{FULL_QUALIFIED_CLASSNAME}::{CLASSNAME}():
  _timeStamp(0.0) {{
}}


double {FULL_QUALIFIED_CLASSNAME}::getMinTimeStamp() const {{
  return _timeStamp;
}}


double {FULL_QUALIFIED_CLASSNAME}::getMaxTimeStamp() const {{
  return _timeStamp;
}}


double {FULL_QUALIFIED_CLASSNAME}::getMinTimeStepSize() const {{
  return {TIME_STEP_SIZE};
}}


double {FULL_QUALIFIED_CLASSNAME}::getMaxTimeStepSize() const {{
  return {TIME_STEP_SIZE};
}}


void {FULL_QUALIFIED_CLASSNAME}::startTimeStep(
  double globalMinTimeStamp,
  double globalMaxTimeStamp,
  double globalMinTimeStepSize,
  double globalMaxTimeStepSize
) {{
}}



void {FULL_QUALIFIED_CLASSNAME}::finishTimeStep() {{
  _timeStamp += {TIME_STEP_SIZE};
}}

