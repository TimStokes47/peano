#include "{{CLASSNAME}}.h"


{% for item in NAMESPACE -%}{{ item }}::{%- endfor %}{{CLASSNAME}}::{{CLASSNAME}}():
  _NumberOfFiniteVolumesPerAxisPerPatch( {{NUMBER_OF_VOLUMES_PER_AXIS}} ),
  _timeStamp(0.0) {
}


double {% for item in NAMESPACE -%}{{ item }}::{%- endfor %}{{CLASSNAME}}::getMinTimeStamp() const {
  return _timeStamp;
}


double {% for item in NAMESPACE -%}{{ item }}::{%- endfor %}{{CLASSNAME}}::getMaxTimeStamp() const {
  return _timeStamp;
}


double {% for item in NAMESPACE -%}{{ item }}::{%- endfor %}{{CLASSNAME}}::getMinTimeStepSize() const {
  return {{TIME_STEP_SIZE}};
}


double {% for item in NAMESPACE -%}{{ item }}::{%- endfor %}{{CLASSNAME}}::getMaxTimeStepSize() const {
  return {{TIME_STEP_SIZE}};
}


void {% for item in NAMESPACE -%}{{ item }}::{%- endfor %}{{CLASSNAME}}::startTimeStep(
  double globalMinTimeStamp,
  double globalMaxTimeStamp,
  double globalMinTimeStepSize,
  double globalMaxTimeStepSize
) {
}



void {% for item in NAMESPACE -%}{{ item }}::{%- endfor %}{{CLASSNAME}}::finishTimeStep() {
  _timeStamp += {{TIME_STEP_SIZE}};
}

