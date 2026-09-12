#include "peano4/RunMultipleSteps.h"


peano4::RunMultipleSteps::RunMultipleSteps(const std::string& name, int firstSubstep):
  _name(name),
  _firstSubstep(firstSubstep),
  _watch("peano4::RunMultipleSteps", "run()", false) {
}


tarch::timing::Measurement peano4::RunMultipleSteps::getRuntimeMeasurements() const {
  return _runtimeMeasurement;
}


int peano4::RunMultipleSteps::getFirstSubstep() const {
  return _firstSubstep;
}


std::string peano4::RunMultipleSteps::getName() const {
  return _name;
}
