#include "RefinementControl.h"
#include "tarch/multicore/Lock.h"


tarch::multicore::BooleanSemaphore  exahype2::RefinementControl::_semaphore;


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


Da stimmt im Multithreaded-Mode irgendwas net. Zumindest sehe ich abwechselnd 2 und 3 Events auf einem Rank, wo es doch nur
1 grosses sein sollte, was einfach alles abspannt.


exahype2::RefinementControl::RefinementControl(double tolerance):
  _Tolerance( tolerance ),
  _accumulatingCopiesOfGlobalObject(0) {
}


void exahype2::RefinementControl::clear() {
  logDebug( "clear()", "clear list of control events" );
  _events.clear();
  _validEventsFromPreviousSweeps.clear();
}


std::vector< peano4::grid::GridControlEvent >  exahype2::RefinementControl::getGridControlEvents() const {
  logDebug( "getGridControlEvents()", "return " << _events.size() << " grid control events" );
  // @todo Has to be removed as soon as we have non-cubic cells
  for ( auto p: _validEventsFromPreviousSweeps ) {
    assertionNumericalEquals2( p.getWidth(0), p.getWidth(1), p.toString(), _validEventsFromPreviousSweeps.size() );
  }
  return _validEventsFromPreviousSweeps;
}


void exahype2::RefinementControl::addCommand(
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h,
  exahype2::RefinementCommand                  command,
  bool                                         invokedByGridConstruction
) {
  logTraceInWith4Arguments( "addCommand()", x, h, toString(command), invokedByGridConstruction );
  switch (command) {
    case ::exahype2::RefinementCommand::Refine:
      {
        tarch::la::Vector<Dimensions,double> expandedH = (1.0+_Tolerance) * h;
        tarch::la::Vector<Dimensions,double> shift     = 0.5 * _Tolerance * h;

        peano4::grid::GridControlEvent newEvent(
          peano4::grid::GridControlEvent::RefinementControl::Refine,
          x-0.5 * h - shift,
          expandedH,
          1.0/3.0 * h
        );
        assertionNumericalEquals1( newEvent.getWidth(0), newEvent.getWidth(1), newEvent.toString() );
        assertionNumericalEquals1( newEvent.getH(0), newEvent.getH(1), newEvent.toString() );
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



void exahype2::RefinementControl::merge( const RefinementControl& control ) {
  tarch::multicore::Lock lock(_semaphore);
  _events.insert( _events.end(), control._events.begin(), control._events.end() );

  _accumulatingCopiesOfGlobalObject--;

  assertion(_accumulatingCopiesOfGlobalObject>=0);

  if (_accumulatingCopiesOfGlobalObject==0 and not _events.empty()) {
	logDebug( "merge(...)", "have got back the data from all other controls and accumulated new control set is not empty, so roll over" );
    _validEventsFromPreviousSweeps = _events;
    _events.clear();
  }
}


void exahype2::RefinementControl::startToAccumulateLocally() {
  tarch::multicore::Lock lock(_semaphore);
  _accumulatingCopiesOfGlobalObject++;
}

