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
  _hasSpreadOutOverAllRanks(false),
  _localNumberOfInnerUnrefinedCell( 0 ),
  _globalNumberOfInnerUnrefinedCell( 0 ),
  _totalNumberOfSplits(0),
  _isInCoolDownPhase(false) {
  #ifdef Parallel
  _globalSumRequest          = nullptr;
  _globalLightestRankRequest = nullptr;
  _globalNumberOfInnerUnrefinedCellBuffer = 1;
  _lightestRankBuffer._rank               = tarch::mpi::Rank::getInstance().getRank();
  #endif
}


void toolbox::loadbalancing::RecursiveSubdivision::dumpStatistics() {
  std::set<int> idsOfLocalSpacetrees = peano4::parallel::SpacetreeSet::getInstance().getLocalSpacetrees();
  for (auto p: idsOfLocalSpacetrees) {
	logInfo(
      "dumpStatistics()",
	  "tree " << p << ": " << peano4::parallel::SpacetreeSet::getInstance().getGridStatistics(p).getNumberOfLocalUnrefinedCells()
	  << ( _blacklist.count(p)>0 ? " (on blacklist with weight=" + std::to_string(_blacklist[p]) + ")" : "" )
    );
  }
}


toolbox::loadbalancing::RecursiveSubdivision::~RecursiveSubdivision() {
  #ifdef Parallel
  if (_globalSumRequest != nullptr ) {
    MPI_Wait( _globalSumRequest, MPI_STATUS_IGNORE );
    MPI_Wait( _globalLightestRankRequest, MPI_STATUS_IGNORE );
  }
  #endif
}


void toolbox::loadbalancing::RecursiveSubdivision::updateGlobalView() {
  _localNumberOfInnerUnrefinedCell = peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells();

  if (tarch::mpi::Rank::getInstance().getNumberOfRanks()<=1) {
    _globalNumberOfInnerUnrefinedCell = _localNumberOfInnerUnrefinedCell;
    _lightestRank                     = 0;
  }
  else {
    #ifdef Parallel
    if (_globalSumRequest != nullptr ) {
      MPI_Wait( _globalSumRequest, MPI_STATUS_IGNORE );
      MPI_Wait( _globalLightestRankRequest, MPI_STATUS_IGNORE );
    }

    _globalSumRequest = new MPI_Request();
    _globalLightestRankRequest = new MPI_Request();

    _globalNumberOfInnerUnrefinedCell = _globalNumberOfInnerUnrefinedCellBuffer;
    _lightestRank                     = _lightestRankBuffer._rank;

    MPI_Iallreduce(
      &_localNumberOfInnerUnrefinedCell,   // send
      &_globalNumberOfInnerUnrefinedCellBuffer,  // receive
      1,             // count
      MPI_DOUBLE,
      MPI_SUM,
      tarch::mpi::Rank::getInstance().getCommunicator(),
      _globalSumRequest
    );
    MPI_Iallreduce(
      &_localNumberOfInnerUnrefinedCell,   // send
      &_lightestRankBuffer,                // receive
      1,             // count
      MPI_DOUBLE_INT,
      MPI_MINLOC,
      tarch::mpi::Rank::getInstance().getCommunicator(),
      _globalLightestRankRequest
    );
    #endif
  }
}


int toolbox::loadbalancing::RecursiveSubdivision::getMaximumSpacetreeSize() const {
  return std::round( std::max(
    _globalNumberOfInnerUnrefinedCell
     / tarch::mpi::Rank::getInstance().getNumberOfRanks()
     / tarch::multicore::Core::getInstance().getNumberOfThreads()
     / 2, 1.0 ));
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


bool toolbox::loadbalancing::RecursiveSubdivision::isInCoolDownPhase() {
  if ( _isInCoolDownPhase and _totalNumberOfSplits>0 ) {
    _totalNumberOfSplits/=2;
    return true;
  }
  else if ( _isInCoolDownPhase and _totalNumberOfSplits<=0 ) {
    logInfo( "isInCoolDownPhase()", "terminate cool-down phase and continue to load balance" );
    _isInCoolDownPhase   = false;
    _totalNumberOfSplits = 0;
    return true;
  }
  else if ( not _isInCoolDownPhase and _totalNumberOfSplits<tarch::multicore::Core::getInstance().getNumberOfThreads() ) {
    return false;
  }
  else {
    logInfo( "isInCoolDownPhase()", "lots of splits triggered, enter cool-down phase, i.e. postpone further splits" );
    _isInCoolDownPhase = true;
    return true;
  }
}


void toolbox::loadbalancing::RecursiveSubdivision::finishStep() {
  updateGlobalView();
  updateBlacklist();

  // @todo Should be an attribute, too, to make this one clean
  static bool hasPostponedDecisionOnce = false;
  if (
    not hasPostponedDecisionOnce
    and
    static_cast<double>(_globalNumberOfInnerUnrefinedCell) < static_cast<double>(
      tarch::mpi::Rank::getInstance().getNumberOfRanks() * tarch::multicore::Core::getInstance().getNumberOfThreads() * _PercentageOfCoresThatShouldInTheoryGetAtLeastOneCell
    )
  ) {
    logInfo( "finishStep()", "problem size of " << _globalNumberOfInnerUnrefinedCell << " is too small to keep all cores busy - wait for one more grid sweep to get better feeling of grid" );
    hasPostponedDecisionOnce = true;
  }
  else if (isInCoolDownPhase()) {
    logDebug( "finishStep()", "currently in cool-down phase" );
  }
  else if (
    not _hasSpreadOutOverAllRanks
    and
    tarch::mpi::Rank::getInstance().getNumberOfRanks()>1
    and
    tarch::mpi::Rank::getInstance().isGlobalMaster()
  ) {
    int cellsPerRank = std::round( std::max( _globalNumberOfInnerUnrefinedCell / tarch::mpi::Rank::getInstance().getNumberOfRanks(), 1.0) );

    _hasSpreadOutOverAllRanks = true;

    logInfo( "finishStep()", "spread over all ranks. Assign each rank " << cellsPerRank << " cells" );
    for (int targetRank=1; targetRank<tarch::mpi::Rank::getInstance().getNumberOfRanks(); targetRank++ ) {
      triggerSplit(0, cellsPerRank, targetRank);
    }
  }
  else if (
    not _hasSpreadOutOverAllRanks
  ) {
    _hasSpreadOutOverAllRanks = true;
  }
  else if (
    _hasSpreadOutOverAllRanks
    and
    peano4::parallel::SpacetreeSet::getInstance().getLocalSpacetrees().size() > 0
    and
    static_cast<double>(peano4::parallel::SpacetreeSet::getInstance().getLocalSpacetrees().size()) < 2.0 * tarch::multicore::Core::getInstance().getNumberOfThreads()
  ) {
    int heaviestSpacetree                              = getIdOfHeaviestLocalSpacetree();
    if (heaviestSpacetree!=NoHeaviestTreeAvailable) {
      int numberOfLocalUnrefinedCellsOfHeaviestSpacetree = peano4::parallel::SpacetreeSet::getInstance().getGridStatistics(heaviestSpacetree).getNumberOfLocalUnrefinedCells();
      int cellsPerCore      = std::min(numberOfLocalUnrefinedCellsOfHeaviestSpacetree/2,getMaximumSpacetreeSize());
      logInfo( "finishStep()", "insufficient number of cores occupied on this rank, so split " << cellsPerCore << " cells iteratively from tree " << heaviestSpacetree << " on local rank (hosts " << numberOfLocalUnrefinedCellsOfHeaviestSpacetree << " unrefined cells)" );
      double maxSplits = 2.0 * tarch::multicore::Core::getInstance().getNumberOfThreads() - static_cast<double>(peano4::parallel::SpacetreeSet::getInstance().getLocalSpacetrees().size());
      while (
        numberOfLocalUnrefinedCellsOfHeaviestSpacetree>getMaximumSpacetreeSize()
        and
        maxSplits>0
      ) {
        triggerSplit(heaviestSpacetree, cellsPerCore, tarch::mpi::Rank::getInstance().getRank());
        numberOfLocalUnrefinedCellsOfHeaviestSpacetree -= cellsPerCore;
        maxSplits -= 1.0;
      }
    }
  }
  else if (
    _hasSpreadOutOverAllRanks
    and
    peano4::parallel::SpacetreeSet::getInstance().getLocalSpacetrees().size() > 0
    and
    doesBiggestLocalSpactreeViolateOptimalityCondition()
  ) {
    int heaviestSpacetree                              = getIdOfHeaviestLocalSpacetree();
    if (heaviestSpacetree!=NoHeaviestTreeAvailable) {
      int numberOfLocalUnrefinedCellsOfHeaviestSpacetree = peano4::parallel::SpacetreeSet::getInstance().getGridStatistics(heaviestSpacetree).getNumberOfLocalUnrefinedCells();
      if ( numberOfLocalUnrefinedCellsOfHeaviestSpacetree>getMaximumSpacetreeSize() ) {
        logInfo(
          "finishStep()",
          "biggest local tree " << heaviestSpacetree << " is too heavy as it hosts " <<
          numberOfLocalUnrefinedCellsOfHeaviestSpacetree << " cells (max size should be " << getMaximumSpacetreeSize() << ")"
        );
        int cellsPerCore      = std::min(numberOfLocalUnrefinedCellsOfHeaviestSpacetree/2,getMaximumSpacetreeSize());
        logInfo(
          "finishStep()",
          "lightest global rank is rank " << _lightestRank << ", so assign this rank " << cellsPerCore << " cell(s)"
        );
        triggerSplit(heaviestSpacetree, cellsPerCore, _lightestRank);
      }
    }
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
  if (not peano4::parallel::SpacetreeSet::getInstance().split(sourceTree,numberOfCells,targetRank)) {
    logInfo( "triggerSplit()", "wanted to split local rank " << sourceTree << " but failed" );
  }
  _blacklist.insert( std::pair<int,int>(sourceTree,3) );
  _totalNumberOfSplits++;
}
