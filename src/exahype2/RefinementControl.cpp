#include "RefinementControl.h"


exahype2::RefinementCommand exahype2::getDefaultRefinementCommand() {
  return RefinementCommand::Coarsen;
}


exahype2::RefinementCommand operator&&( exahype2::RefinementCommand lhs, exahype2::RefinementCommand rhs) {
  if (
    lhs == exahype2::RefinementCommand::Refine or rhs == exahype2::RefinementCommand::Refine
  ) {
	return exahype2::RefinementCommand::Refine;
  }
  else if (
    lhs == exahype2::RefinementCommand::Keep or rhs == exahype2::RefinementCommand::Keep
  ) {
    return exahype2::RefinementCommand::Keep;
  }
  else {
	return exahype2::RefinementCommand::Coarsen;
  }
}


std::string toString( exahype2::RefinementCommand value ) {
  switch (value) {
    case exahype2::RefinementCommand::Refine:  return "refine";
    case exahype2::RefinementCommand::Keep:    return "keep";
    case exahype2::RefinementCommand::Coarsen: return "coarsen";
  }
  return "<undef>";
}


tarch::logging::Log  exahype2::RefinementControl::_log( "exahype2::RefinementControl" );


exahype2::RefinementControl::RefinementControl() {
}


void exahype2::RefinementControl::clear() {
  logDebug( "clear()", "clear list of control events" );
  _events.clear();
}


std::vector< peano4::grid::GridControlEvent >  exahype2::RefinementControl::getGridControlEvents() const {
  logDebug( "getGridControlEvents()", "return " << _events.size() << " grid control events" );
  return _events;
}


void exahype2::RefinementControl::addCommand(
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h,
  exahype2::RefinementCommand                  command,
  bool                                         invokedByGridConstruction
) {
  logTraceInWith4Arguments( "addCommand()", x, h, toString(command), invokedByGridConstruction );
  const double Tolerance = 0.10;
  switch (command) {
    case ::exahype2::RefinementCommand::Refine:
      {
        peano4::grid::GridControlEvent newEvent(
          peano4::grid::GridControlEvent::RefinementControl::Refine,
          x-h*(1.0+Tolerance/2.0),
          h*(1.0+Tolerance),
          h/3.0*(1.0+Tolerance)
        );
        _events.push_back( newEvent );
        logDebug( "addCommend()", "added refinement for x=" << x << ", h=" << h << ": " << newEvent.toString() << " (total of " << _events.size() << " instructions)" );
      }
      break;
    case ::exahype2::RefinementCommand::Keep:
      break;
    case ::exahype2::RefinementCommand::Coarsen:
      if (not invokedByGridConstruction) {
        assertionMsg(false, "not implemented yet");
      }
      break;
  }
  logTraceOutWith1Argument( "addCommand()", _events.size() );
}

