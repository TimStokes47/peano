#include "RecursiveSubdivision.h"
#include "peano4/utils/Globals.h"
#include "peano4/parallel/SpacetreeSet.h"

#include "tarch/Assertions.h"
#include "tarch/mpi/Rank.h"
#include "tarch/multicore/Core.h"




tarch::logging::Log  toolbox::loadbalancing::RecursiveSubdivision::_log( "toolbox::loadbalancing::RecursiveSubdivision" );


toolbox::loadbalancing::RecursiveSubdivision::RecursiveSubdivision():
  _hasSpreadOutOverAllRanks( tarch::mpi::Rank::getInstance().getNumberOfRanks()<=1 ),
  _localNumberOfInnerUnrefinedCell( 0 ),
  _globalNumberOfInnerUnrefinedCell( 0 )
{

}


void toolbox::loadbalancing::RecursiveSubdivision::updateGlobalView() {
  _localNumberOfInnerUnrefinedCell = peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells();

  if (tarch::mpi::Rank::getInstance().getNumberOfRanks()<=1) {
    _globalNumberOfInnerUnrefinedCell = _localNumberOfInnerUnrefinedCell;
  }
  else {
    assertionMsg( false, "not implemented yet. Need global reduction here" );
  }
}


int toolbox::loadbalancing::RecursiveSubdivision::getMaximumSpacetreeSize() const {
  return std::max(
    _globalNumberOfInnerUnrefinedCell
     / tarch::mpi::Rank::getInstance().getNumberOfRanks()
     / tarch::multicore::Core::getInstance().getNumberOfThreads()
     / 2, 1 );
}


int toolbox::loadbalancing::RecursiveSubdivision::getIdOfHeaviestLocalSpacetree() const {
  std::set<int> idsOfLocalSpacetrees = peano4::parallel::SpacetreeSet::getInstance().getLocalSpacetrees();
  int result = -1;
  int maxLocalUnrefinedCells = -1;
  for (auto p: idsOfLocalSpacetrees) {
    if ( peano4::parallel::SpacetreeSet::getInstance().getGridStatistics(p).getNumberOfLocalUnrefinedCells()>maxLocalUnrefinedCells ) {
      maxLocalUnrefinedCells = peano4::parallel::SpacetreeSet::getInstance().getGridStatistics(p).getNumberOfLocalUnrefinedCells();
      result = p;
    }
  }
  return result;
}


bool toolbox::loadbalancing::RecursiveSubdivision::doesBiggestLocalSpactreeViolateOptimalityCondition() const {
  bool result = false;
  std::set<int> idsOfLocalSpacetrees = peano4::parallel::SpacetreeSet::getInstance().getLocalSpacetrees();
  for (auto p: idsOfLocalSpacetrees) {
    result |= peano4::parallel::SpacetreeSet::getInstance().getGridStatistics(p).getNumberOfLocalUnrefinedCells()>getMaximumSpacetreeSize();
  }
  return result;
}


void toolbox::loadbalancing::RecursiveSubdivision::finishStep() {
  updateGlobalView();

  if (
    not _hasSpreadOutOverAllRanks
    and
    _globalNumberOfInnerUnrefinedCell > tarch::mpi::Rank::getInstance().getNumberOfRanks()
  ) {
    int cells             = getMaximumSpacetreeSize();
    assertion( cells>tarch::mpi::Rank::getInstance().getNumberOfRanks() );
    int cellsPerRank = cells / tarch::mpi::Rank::getInstance().getNumberOfRanks();

    _hasSpreadOutOverAllRanks = true;

    logInfo( "finishTimeStep()", "spread over all ranks. Assign each rank " << cellsPerRank << " cells" );
    for (int targetRank=1; targetRank<tarch::mpi::Rank::getInstance().getNumberOfRanks(); targetRank++ ) {
      if (not peano4::parallel::SpacetreeSet::getInstance().split(0,cellsPerRank,targetRank)) {
        logWarning( "finishTimeStep()", "wanted to split local rank's only tree such that rank " << targetRank << " becomes involed. Failed" );
      }
    }
  }
  else if (
    _hasSpreadOutOverAllRanks
    and
    peano4::parallel::SpacetreeSet::getInstance().getLocalSpacetrees().size() > 0
    and
    peano4::parallel::SpacetreeSet::getInstance().getLocalSpacetrees().size() < tarch::multicore::Core::getInstance().getNumberOfThreads()
  ) {
    int heaviestSpacetree = getIdOfHeaviestLocalSpacetree();
    int cellsPerCore      = std::max(
      peano4::parallel::SpacetreeSet::getInstance().getGridStatistics(heaviestSpacetree).getNumberOfLocalUnrefinedCells()/2,
      getMaximumSpacetreeSize()
    );

    logInfo( "finishTimeStep()", "not all cores are yet occupied on this rank, so split " << cellsPerCore << " cells from tree " << heaviestSpacetree );
    peano4::parallel::SpacetreeSet::getInstance().split(heaviestSpacetree,cellsPerCore,tarch::mpi::Rank::getInstance().getRank());
  }
  else if (
    _hasSpreadOutOverAllRanks
    and
    peano4::parallel::SpacetreeSet::getInstance().getLocalSpacetrees().size() >= tarch::multicore::Core::getInstance().getNumberOfThreads()
    and
    doesBiggestLocalSpactreeViolateOptimalityCondition()
  ) {
    int heaviestSpacetree = getIdOfHeaviestLocalSpacetree();
    logWarning( "finishTimeStep()", "biggest local tree " << heaviestSpacetree << " is too heavy. Split it up" );
    //assertionMsg(false, "not implemented yet");
  }
}
