#include "RecursiveGuided.h"


#include "tarch/mpi/Rank.h"
#include "tarch/multicore/Core.h"


#include "peano4/parallel/SpacetreeSet.h"


toolbox::loadbalancing::RecursiveGuided::RecursiveGuided():
  _hasSpreadOutOverAllRanks( tarch::mpi::Rank::getInstance().getNumberOfRanks()<=1 ),
  _localNumberOfInnerUnrefinedCell( 0 ),
  _globalNumberOfInnerUnrefinedCell( 0 )
{

}


void toolbox::loadbalancing::RecursiveGuided::finishTimeStep() {
  _localNumberOfInnerUnrefinedCell = peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells();
  if (tarch::mpi::Rank::getInstance().getNumberOfRanks()<=1) {
    _globalNumberOfInnerUnrefinedCell = _localNumberOfInnerUnrefinedCell;
  }
  else {
    assertionMsg( false, "not implemented yet. Need global reduction here" );
  }
}
