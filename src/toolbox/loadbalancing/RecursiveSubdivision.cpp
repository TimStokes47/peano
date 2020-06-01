#include "RecursiveSubdivision.h"
#include "loadbalancing.h"

#include "peano4/utils/Globals.h"
#include "peano4/parallel/SpacetreeSet.h"

#include "tarch/Assertions.h"
#include "tarch/mpi/Rank.h"
#include "tarch/multicore/Core.h"




tarch::logging::Log  toolbox::loadbalancing::RecursiveSubdivision::_log( "toolbox::loadbalancing::RecursiveSubdivision" );


toolbox::loadbalancing::RecursiveSubdivision::RecursiveSubdivision(double percentageOfCoresThatShouldInTheoryGetAtLeastOneCell):
  _PercentageOfCoresThatShouldInTheoryGetAtLeastOneCell( percentageOfCoresThatShouldInTheoryGetAtLeastOneCell ),
  _blacklist(),
  _hasSpreadOutOverAllRanks( false ),
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


// @todo Ich muss die Core-Zahl in Relation setzen zur maximalen Anzahl an Cores pro Ranks


std::string toolbox::loadbalancing::RecursiveSubdivision::toString() const {
  std::ostringstream msg;

  std::set<int> idsOfLocalSpacetrees = peano4::parallel::SpacetreeSet::getInstance().getLocalSpacetrees();
  for (auto p: idsOfLocalSpacetrees) {
    msg << "tree " << p << ": " << peano4::parallel::SpacetreeSet::getInstance().getGridStatistics(p).getNumberOfLocalUnrefinedCells()
	  << ( _blacklist.count(p)>0 ? " (on blacklist with weight=" + std::to_string(_blacklist.at(p)) + ")" : "" );
  }

  return msg.str();
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


int toolbox::loadbalancing::RecursiveSubdivision::getMaximumSpacetreeSize(int localSize) const {
  const double alphaP = _PercentageOfCoresThatShouldInTheoryGetAtLeastOneCell
		              * tarch::mpi::Rank::getInstance().getNumberOfRanks()
    	              * tarch::multicore::Core::getInstance().getNumberOfThreads();

  double k      = 1.0;
  double result = localSize - 1;

  while ( localSize - result < result ) {
	k      += 1.0;
	result  = _globalNumberOfInnerUnrefinedCell / alphaP / k;
  }

  return std::max(  static_cast<int>(std::round(result)),  1 );
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


void toolbox::loadbalancing::RecursiveSubdivision::updateCoolDownState() {
  if ( _isInCoolDownPhase and _totalNumberOfSplits>0 ) {
    _totalNumberOfSplits/=2;
  }
  else if ( _isInCoolDownPhase and _totalNumberOfSplits<=0 ) {
    logInfo( "isInCoolDownPhase()", "terminate cool-down phase and continue to load balance" );
    _isInCoolDownPhase   = false;
    _totalNumberOfSplits = 0;
  }
  else if ( not _isInCoolDownPhase and _totalNumberOfSplits<tarch::multicore::Core::getInstance().getNumberOfThreads() ) {
  }
  else {
    _isInCoolDownPhase = true;
  }
}


std::string toolbox::loadbalancing::RecursiveSubdivision::toString( StrategyStep step ) {
  switch ( step ) {
    case StrategyStep::Wait:
      return "wait";
    case StrategyStep::SpreadEquallyOverAllRanks:
      return "spread-equally-over-all-ranks";
    case StrategyStep::SplitHeaviestLocalTreeMultipleTimes_UseLocalRank_UseRecursivePartitioning:
      return "split-heaviest-tree(multiple-times,use-local-rank,use-recursive-partitioning)";
    case StrategyStep::SplitHeaviestLocalTreeOnce_UseAllRanks_UseRecursivePartitioning:
      return "split-heaviest-tree(once,use-all-ranks,use-recursive-partitioning)";
  }
  return "<undef>";
}


toolbox::loadbalancing::RecursiveSubdivision::StrategyStep toolbox::loadbalancing::RecursiveSubdivision::getStrategyStep() const {
  if (
    peano4::parallel::SpacetreeSet::getInstance().getLocalSpacetrees().size() > peano4::parallel::Node::MaxSpacetreesPerRank/4*3
  ) {
    logDebug( "getStrategyStep()", "afraid to use too many trees and overbook system" );
    return StrategyStep::Wait;
  }

  if (
    static_cast<double>(_globalNumberOfInnerUnrefinedCell)
    <
    _PercentageOfCoresThatShouldInTheoryGetAtLeastOneCell * std::min(
      tarch::mpi::Rank::getInstance().getNumberOfRanks(),
      tarch::multicore::Core::getInstance().getNumberOfThreads()
    )
  ) {
    return StrategyStep::Wait;
  }

  if ( _isInCoolDownPhase ) {
    return StrategyStep::Wait;
  }

  if (
    not _hasSpreadOutOverAllRanks
    and
    tarch::mpi::Rank::getInstance().getNumberOfRanks()>1
    and
    tarch::mpi::Rank::getInstance().isGlobalMaster()
  ) {
    return StrategyStep::SpreadEquallyOverAllRanks;
  }

  if (
    peano4::parallel::SpacetreeSet::getInstance().getLocalSpacetrees().size() > 0
    and
    static_cast<double>(_localNumberOfInnerUnrefinedCell)
    <
    _PercentageOfCoresThatShouldInTheoryGetAtLeastOneCell *
    tarch::multicore::Core::getInstance().getNumberOfThreads()
  ) {
    return StrategyStep::Wait;
  }

  if (
    peano4::parallel::SpacetreeSet::getInstance().getLocalSpacetrees().size() > 0
    and
    static_cast<double>(peano4::parallel::SpacetreeSet::getInstance().getLocalSpacetrees().size()) < _PercentageOfCoresThatShouldInTheoryGetAtLeastOneCell * tarch::multicore::Core::getInstance().getNumberOfThreads()
  ) {
    return StrategyStep::SplitHeaviestLocalTreeMultipleTimes_UseLocalRank_UseRecursivePartitioning;
  }

  if (
    peano4::parallel::SpacetreeSet::getInstance().getLocalSpacetrees().size() > 0
    and
    doesBiggestLocalSpactreeViolateOptimalityCondition()
  ) {
    return StrategyStep::SplitHeaviestLocalTreeOnce_UseAllRanks_UseRecursivePartitioning;
  }

  return StrategyStep::Wait;
}


void toolbox::loadbalancing::RecursiveSubdivision::finishStep() {
  updateGlobalView();
  updateBlacklist();
  updateCoolDownState();

  logDebug( "finishStep()", toString( getStrategyStep() ) );

  switch ( getStrategyStep() ) {
    case StrategyStep::Wait:
      break;
    case StrategyStep::SpreadEquallyOverAllRanks:
      {
   	    int cellsPerRank = std::max( static_cast<int>(std::round(_globalNumberOfInnerUnrefinedCell / tarch::mpi::Rank::getInstance().getNumberOfRanks())), 1 );

        _hasSpreadOutOverAllRanks = true;

        for (int targetRank=1; targetRank<tarch::mpi::Rank::getInstance().getNumberOfRanks(); targetRank++ ) {
          triggerSplit(0, cellsPerRank, targetRank);
        }
      }
      break;
    case StrategyStep::SplitHeaviestLocalTreeMultipleTimes_UseLocalRank_UseRecursivePartitioning:
      {
   	    int heaviestSpacetree                              = getIdOfHeaviestLocalSpacetree();
   	    if (heaviestSpacetree!=NoHeaviestTreeAvailable) {
   	      int numberOfLocalUnrefinedCellsOfHeaviestSpacetree = peano4::parallel::SpacetreeSet::getInstance().getGridStatistics(heaviestSpacetree).getNumberOfLocalUnrefinedCells();
   	      int cellsPerCore      = getMaximumSpacetreeSize(numberOfLocalUnrefinedCellsOfHeaviestSpacetree);
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
      break;
    case StrategyStep::SplitHeaviestLocalTreeOnce_UseAllRanks_UseRecursivePartitioning:
      {
        int heaviestSpacetree                              = getIdOfHeaviestLocalSpacetree();
        if (heaviestSpacetree!=NoHeaviestTreeAvailable) {
          int numberOfLocalUnrefinedCellsOfHeaviestSpacetree = peano4::parallel::SpacetreeSet::getInstance().getGridStatistics(heaviestSpacetree).getNumberOfLocalUnrefinedCells();
          if ( numberOfLocalUnrefinedCellsOfHeaviestSpacetree>getMaximumSpacetreeSize() ) {
            logInfo(
              "finishStep()",
              "biggest local tree " << heaviestSpacetree << " is too heavy as it hosts " <<
              numberOfLocalUnrefinedCellsOfHeaviestSpacetree << " cells (max size should be " << getMaximumSpacetreeSize() << ")"
            );
            int cellsPerCore      = getMaximumSpacetreeSize(numberOfLocalUnrefinedCellsOfHeaviestSpacetree);
            logInfo(
              "finishStep()",
              "lightest global rank is rank " << _lightestRank << ", so assign this rank " << cellsPerCore << " cell(s)"
            );
            triggerSplit(heaviestSpacetree, cellsPerCore, _lightestRank);
          }
        }
      }
      break;
  }

  dumpStatistics();
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
