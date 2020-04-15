#include "RecursiveGuided.h"


#include "tarch/Assertions.h"
#include "tarch/mpi/Rank.h"
#include "tarch/multicore/Core.h"



toolbox::loadbalancing::RecursiveGuided::RecursiveGuided():
  _hasSpreadOutOverAllRanks( tarch::mpi::Rank::getInstance().getNumberOfRanks()<=1 ),
  _localNumberOfInnerUnrefinedCell( 0 ),
  _globalNumberOfInnerUnrefinedCell( 0 )
{

}


void toolbox::loadbalancing::RecursiveGuided::finishTimeStep( int totalNumberOfInnerUnrefinedCellsOnThisRank ) {
  _localNumberOfInnerUnrefinedCell = totalNumberOfInnerUnrefinedCellsOnThisRank;
  if (tarch::mpi::Rank::getInstance().getNumberOfRanks()<=1) {
    _globalNumberOfInnerUnrefinedCell = _localNumberOfInnerUnrefinedCell;
  }
  else {
    assertionMsg( false, "not implemented yet. Need global reduction here" );
  }
}
