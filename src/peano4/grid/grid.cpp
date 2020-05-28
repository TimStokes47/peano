#include "grid.h"
#include "GridStatistics.h"
#include "GridControlEvent.h"



std::vector< peano4::grid::GridControlEvent > peano4::grid::consolidate( std::vector< GridControlEvent>   events ) {
  static tarch::logging::Log _log( "peano4::grid" );
  logTraceInWith1Argument( "consolidate(...)", events.size() );

  bool hasFusedOrErasedEvents = true;

  std::vector< peano4::grid::GridControlEvent > result;
  while (not events.empty() and hasFusedOrErasedEvents) {
    hasFusedOrErasedEvents = false;

    peano4::grid::GridControlEvent currentEvent = events.back();
    events.pop_back();

    for ( std::vector< GridControlEvent >::iterator i = events.begin(); i!=events.end(); ) {
      auto boundingEventOffset = tarch::la::min( i->getOffset(), currentEvent.getOffset() );
      auto boundingEventSize   = tarch::la::max( i->getOffset()+i->getWidth(), currentEvent.getOffset()+currentEvent.getWidth() ) - boundingEventOffset;
      if (
        i->getRefinementControl()==GridControlEvent::RefinementControl::Refine
        and
        currentEvent.getRefinementControl()==GridControlEvent::RefinementControl::Refine
        and
        tarch::la::equals( i->getH(), currentEvent.getH() )
        and
        tarch::la::volume(boundingEventSize) <= 1.1 * (tarch::la::volume(i->getWidth()) + tarch::la::volume(currentEvent.getWidth()))
      ) {
    	currentEvent = GridControlEvent(
          GridControlEvent::RefinementControl::Refine,
          boundingEventOffset, boundingEventSize, i->getH()
        );
        i = events.erase(i);
      }
      else {
        i++;
      }
    }

    result.push_back( currentEvent );
  }

  logTraceOutWith1Argument( "consolidate(...)", result.size() );
  return events;
}


peano4::grid::GridStatistics operator+( peano4::grid::GridStatistics lhs, peano4::grid::GridStatistics rhs ) {
  return peano4::grid::GridStatistics(
    lhs.getNumberOfLocalUnrefinedCells()  + rhs.getNumberOfLocalUnrefinedCells(),
    lhs.getNumberOfRemoteUnrefinedCells() + rhs.getNumberOfRemoteUnrefinedCells(),
    lhs.getNumberOfLocalRefinedCells()  + rhs.getNumberOfLocalRefinedCells(),
    lhs.getNumberOfRemoteRefinedCells() + rhs.getNumberOfRemoteRefinedCells(),
    std::min(lhs.getStationarySweeps(),rhs.getStationarySweeps()),
    lhs.getCoarseningHasBeenVetoed() | rhs.getCoarseningHasBeenVetoed()
  );
}


void peano4::grid::clear( GridStatistics& statistics, bool isGlobalMasterTree ) {
  statistics.setNumberOfLocalUnrefinedCells(0);
  statistics.setNumberOfRemoteUnrefinedCells(0);
  statistics.setNumberOfLocalRefinedCells(isGlobalMasterTree ? 1 : 0);
  statistics.setNumberOfRemoteRefinedCells(isGlobalMasterTree ? 0 : 1);
  statistics.setCoarseningHasBeenVetoed(false);
  statistics.setStationarySweeps( statistics.getStationarySweeps()+1 );
}

