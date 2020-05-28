#include "grid.h"
#include "GridStatistics.h"
#include "GridControlEvent.h"



std::vector< peano4::grid::GridControlEvent > peano4::grid::consolidate( std::vector< GridControlEvent>   events ) {
  static tarch::logging::Log _log( "peano4::grid" );
  logTraceInWith1Argument( "consolidate(...)", events.size() );

  std::vector< peano4::grid::GridControlEvent > result;
  bool hasFusedOrErasedEvents = false;

  std::vector< GridControlEvent >::iterator i = events.begin();
  while ( i != events.end() ) {
    std::vector< GridControlEvent >::iterator j = i;
    j++;

    bool hasFusedOrErasedCurrentEvent = false;

    while ( j != events.end() ) {
  	  if (
        i->getRefinementControl()==GridControlEvent::RefinementControl::Refine
        and
        j->getRefinementControl()==GridControlEvent::RefinementControl::Refine
        and
		tarch::la::equals( i->getH(), j->getH() )
      ) {
  		auto boundingEventOffset = tarch::la::min( i->getOffset(), j->getOffset() );
  		auto boundingEventSize   = tarch::la::max( i->getOffset()+i->getWidth(), j->getOffset()+j->getWidth() ) - boundingEventOffset;

  		if ( tarch::la::volume(boundingEventSize) <= 1.1 * (tarch::la::volume(i->getWidth()) + tarch::la::volume(j->getWidth())) ) {
  	  	  GridControlEvent boundingEvent(
  	        GridControlEvent::RefinementControl::Refine,
  	        boundingEventOffset, boundingEventSize, i->getH()
          );
          result.push_back( boundingEvent );
          i = events.erase(i);
          j = events.end();
          hasFusedOrErasedCurrentEvent = true;
          hasFusedOrErasedEvents       = true;
  		}
      }
  	  else {
  		j++;
  	  }
    }

    if (not hasFusedOrErasedCurrentEvent) {
      result.push_back( *i );
  	  i++;
    }
  }
  logWarning( "consolidate(...)", "size=" << result.size() );

  logTraceOutWith1Argument( "consolidate(...)", result.size() );
  return result;
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

