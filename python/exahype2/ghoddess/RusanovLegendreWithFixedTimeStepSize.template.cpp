#include "{CLASSNAME}.h"


{FULL_QUALIFIED_CLASSNAME}::{CLASSNAME}():
  _timeStamp(0.0),
  _projectOntoFace(true),
  _solveCell(false) {{
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
  if ( _projectOntoFace ) {{
    _projectOntoFace = false;
    _solveCell       = true;
  }}
  else if (_solveCell) {{
    _solveCell       = false;
    _projectOntoFace = true;
    _timeStamp      += {TIME_STEP_SIZE};
  }}
}}


bool {FULL_QUALIFIED_CLASSNAME}::startNewTimeStepOnCellAndProjectOntoFace() const {{
  return _projectOntoFace;
}}


bool {FULL_QUALIFIED_CLASSNAME}::solveCellProblemAndFinishTimeStepOnCell() const {{
  return _solveCell;
}}


::exahype2::RefinementCommand {FULL_QUALIFIED_CLASSNAME}::refinementCriterion(
  double* Q,
  const tarch::la::Vector<Dimensions,double>&  cellCentre,
  const tarch::la::Vector<Dimensions,double>&  cellH,
  double                                       t
) {{
  return ::exahype2::RefinementCommand::Keep;
}}


void {FULL_QUALIFIED_CLASSNAME}::adjustSolution(
  double* Q,
  const tarch::la::Vector<Dimensions,double>&  cellCentre,
  const tarch::la::Vector<Dimensions,double>&  cellH,
  double                                       t
) {{

}}


void {FULL_QUALIFIED_CLASSNAME}::boundaryConditions(
  double* Q,
  const tarch::la::Vector<Dimensions,double>&  faceCentre,
  const tarch::la::Vector<Dimensions,double>&  faceH,
  double                                       t,
  const tarch::la::Vector<Dimensions,double>&  normal
) {{

}}



void {FULL_QUALIFIED_CLASSNAME}::solveCell(
  double* QCell,
  double* QLeftFace,
  double* QBottomFace,
  double* QRightFace,
  double* QUpperFace,
  const tarch::la::Vector<Dimensions,double>&  cellCentre,
  const tarch::la::Vector<Dimensions,double>&  cellH,
  double                                       t
) {{

}}
