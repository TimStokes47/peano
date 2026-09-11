#include "peano4/RunMultipleSteps.h"


peano4::RunMultipleSteps::RunMultipleSteps(int firstSubstep):
  _firstSubstep(firstSubstep),
  _watch("peano4::RunMultipleSteps", "run()", false) {
}


tarch::timing::Measurement peano4::RunMultipleSteps::getRuntimeMeasurements() const {
  return _runtimeMeasurement;
}


int peano4::RunMultipleSteps::getFirstSubstep() const {
  return _firstSubstep;
}
