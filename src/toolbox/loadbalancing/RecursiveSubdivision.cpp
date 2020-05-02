#include "RecursiveSubdivision.h"
#include "peano4/utils/Globals.h"
#include "peano4/parallel/SpacetreeSet.h"

#include "tarch/Assertions.h"
#include "tarch/mpi/Rank.h"
#include "tarch/multicore/Core.h"




tarch::logging::Log  toolbox::loadbalancing::RecursiveSubdivision::_log( "toolbox::loadbalancing::RecursiveSubdivision" );


toolbox::loadbalancing::RecursiveSubdivision::RecursiveSubdivision(double percentageOfCoresThatShouldInTheoryGetAtLeastOneCell):
  _PercentageOfCoresThatShouldInTheoryGetAtLeastOneCell( percentageOfCoresThatShouldInTheoryGetAtLeastOneCell ),
  _blacklist(),
  _hasSpreadOutOverAllRanks( tarch::mpi::Rank::getInstance().getNumberOfRanks()<=1 ),
  _localNumberOfInnerUnrefinedCell( 0 ),
  _globalNumberOfInnerUnrefinedCell( 0 ) {
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
  int result = NoHeaviestTreeAvailable;
  int maxLocalUnrefinedCells = -1;
  for (auto p: idsOfLocalSpacetrees) {
    if (
      peano4::parallel::SpacetreeSet::getInstance().getGridStatistics(p).getNumberOfLocalUnrefinedCells()>maxLocalUnrefinedCells
      and
      _blacklist.count(p)==0
    ) {
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
  updateBlacklist();

  if (
    static_cast<double>(_globalNumberOfInnerUnrefinedCell) < static_cast<double>(
      tarch::mpi::Rank::getInstance().getNumberOfRanks() * tarch::multicore::Core::getInstance().getNumberOfThreads() * _PercentageOfCoresThatShouldInTheoryGetAtLeastOneCell
    )
  ) {
    logInfo( "finishStep()", "problem size of " << _globalNumberOfInnerUnrefinedCell << " is too small to keep all cores busy - wait for larger mesh to be constructed" );
  }
  else
  if ( not _hasSpreadOutOverAllRanks ) {
    int cells             = getMaximumSpacetreeSize();
    assertion( cells>tarch::mpi::Rank::getInstance().getNumberOfRanks() );
    int cellsPerRank = cells / tarch::mpi::Rank::getInstance().getNumberOfRanks();

    _hasSpreadOutOverAllRanks = true;

    logInfo( "finishStep()", "spread over all ranks. Assign each rank " << cellsPerRank << " cells" );
    for (int targetRank=1; targetRank<tarch::mpi::Rank::getInstance().getNumberOfRanks(); targetRank++ ) {
      triggerSplit(0, cellsPerRank, targetRank);
    }
  }
  else if (
    _hasSpreadOutOverAllRanks
    and
    peano4::parallel::SpacetreeSet::getInstance().getLocalSpacetrees().size() > 0
    and
    peano4::parallel::SpacetreeSet::getInstance().getLocalSpacetrees().size() < tarch::multicore::Core::getInstance().getNumberOfThreads()
  ) {
    int heaviestSpacetree                              = getIdOfHeaviestLocalSpacetree();
    if (heaviestSpacetree!=NoHeaviestTreeAvailable) {
      int numberOfLocalUnrefinedCellsOfHeaviestSpacetree = peano4::parallel::SpacetreeSet::getInstance().getGridStatistics(heaviestSpacetree).getNumberOfLocalUnrefinedCells();
      int cellsPerCore      = std::max(numberOfLocalUnrefinedCellsOfHeaviestSpacetree/2,getMaximumSpacetreeSize());

      logInfo( "finishStep()", "not all cores are occupied on this rank, so split " << cellsPerCore << " cells from tree " << heaviestSpacetree << " (hosts " << numberOfLocalUnrefinedCellsOfHeaviestSpacetree << " unrefined cells)" );
      triggerSplit(heaviestSpacetree, cellsPerCore, tarch::mpi::Rank::getInstance().getRank());
    }
  }
  else if (
    _hasSpreadOutOverAllRanks
    and
    peano4::parallel::SpacetreeSet::getInstance().getLocalSpacetrees().size() >= tarch::multicore::Core::getInstance().getNumberOfThreads()
    and
    doesBiggestLocalSpactreeViolateOptimalityCondition()
  ) {
    int heaviestSpacetree = getIdOfHeaviestLocalSpacetree();
    logWarning( "finishStep()", "biggest local tree " << heaviestSpacetree << " is too heavy. Split it up" );
    //assertionMsg(false, "not implemented yet");
    //_blacklist
  }
}



void toolbox::loadbalancing::RecursiveSubdivision::updateBlacklist() {
  for (std::map<int,int>::iterator p = _blacklist.begin(); p!=_blacklist.end(); ) {
    if (p->second>0) {
      p->second--;
      p++;
    }
    else {
      p = _blacklist.erase(p);
    }
  }
}


void toolbox::loadbalancing::RecursiveSubdivision::triggerSplit( int sourceTree, int numberOfCells, int targetRank ) {
  if (peano4::parallel::SpacetreeSet::getInstance().split(sourceTree,numberOfCells,targetRank)) {
    _blacklist.insert( std::pair<int,int>(sourceTree,3) );
  }
  else {
    logInfo( "triggerSplit()", "wanted to split local rank " << sourceTree << " but failed" );
  }
}
