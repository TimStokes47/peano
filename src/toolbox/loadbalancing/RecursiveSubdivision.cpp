#include "RecursiveSubdivision.h"
#include "loadbalancing.h"

#include "peano4/utils/Globals.h"
#include "peano4/parallel/SpacetreeSet.h"

#include "tarch/Assertions.h"
#include "tarch/mpi/Rank.h"
#include "tarch/multicore/Core.h"

#include "peano4/grid/GridStatistics.h"




tarch::logging::Log  toolbox::loadbalancing::RecursiveSubdivision::_log( "toolbox::loadbalancing::RecursiveSubdivision" );


toolbox::loadbalancing::RecursiveSubdivision::RecursiveSubdivision(double percentageOfCoresThatShouldInTheoryGetAtLeastOneCell):
  _PercentageOfCoresThatShouldInTheoryGetAtLeastOneCell( percentageOfCoresThatShouldInTheoryGetAtLeastOneCell ),
  _blacklist(),
  _hasSpreadOutOverAllRanks( false ),
  _localNumberOfInnerUnrefinedCell( 0 ),
  _globalNumberOfInnerUnrefinedCell( 0 ),
  _totalNumberOfSplits(0),
  _state( StrategyState::Standard ) {
  #ifdef Parallel
  _globalSumRequest          = nullptr;
  _globalLightestRankRequest = nullptr;
  _globalNumberOfInnerUnrefinedCellBufferIn = 1;
  _lightestRankBufferIn._rank               = tarch::mpi::Rank::getInstance().getRank();
  #endif
  _roundRobinToken = tarch::mpi::Rank::getInstance().getRank();
}


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
  assertion(_globalSumRequest == nullptr );
  assertion(_globalLightestRankRequest == nullptr );
  #endif
}


void toolbox::loadbalancing::RecursiveSubdivision::finishSimulation() {
  #ifdef Parallel
  if (_globalSumRequest != nullptr ) {
    MPI_Wait( _globalSumRequest, MPI_STATUS_IGNORE );
    MPI_Wait( _globalLightestRankRequest, MPI_STATUS_IGNORE );
    _globalSumRequest = nullptr;
    _globalLightestRankRequest = nullptr;
  }
  #endif
}


void toolbox::loadbalancing::RecursiveSubdivision::updateGlobalView() {
  _localNumberOfInnerUnrefinedCell = peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells();

  // @todo docu
  // By the time we create it, we might not yet know whether we are global master or not
  if (not tarch::mpi::Rank::getInstance().isGlobalMaster() ) {
    _hasSpreadOutOverAllRanks = true;
  }
 
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

    // rollover
    _globalNumberOfInnerUnrefinedCell = static_cast<int>( std::round(_globalNumberOfInnerUnrefinedCellBufferIn) );
    _lightestRank                     = _lightestRankBufferIn._rank;


    if (
      _globalNumberOfInnerUnrefinedCell <= _localNumberOfInnerUnrefinedCell
      and
      not _hasSpreadOutOverAllRanks
    ) {
      logInfo(
        "updateGlobalView()",
        "global number of cells lags behind local one. Use local number of cells (" << _localNumberOfInnerUnrefinedCell <<
        ") instead of global count of " << _globalNumberOfInnerUnrefinedCell << " to guide partitioning"
      );
      _globalNumberOfInnerUnrefinedCell = _localNumberOfInnerUnrefinedCell;
      _lightestRank                     = tarch::mpi::Rank::getInstance().getRank();
    }
    else if (_globalNumberOfInnerUnrefinedCell <= _localNumberOfInnerUnrefinedCell ) {
      _globalNumberOfInnerUnrefinedCell = _localNumberOfInnerUnrefinedCell * tarch::mpi::Rank::getInstance().getNumberOfRanks();
      logInfo(
        "updateGlobalView()",
        "global number of cells lags behind local one. Code has spread over ranks already. Therefore, assume that we use global data from previous time step. Extrapolate " << _localNumberOfInnerUnrefinedCell 
        << " to guide partitioning. Assume it equals " << _globalNumberOfInnerUnrefinedCell
      );
      _lightestRank                     = tarch::mpi::Rank::getInstance().getRank();
    }


    _globalSumRequest          = new MPI_Request();
    _globalLightestRankRequest = new MPI_Request();

    _lightestRankBufferOut._localUnrefinedCells = _localNumberOfInnerUnrefinedCell;
    _lightestRankBufferOut._rank                = tarch::mpi::Rank::getInstance().getRank();

    MPI_Iallreduce(
      &_localNumberOfInnerUnrefinedCell,            // send
      &_globalNumberOfInnerUnrefinedCellBufferIn,   // receive
      1,             // count
      MPI_INT,
      MPI_SUM,
      tarch::mpi::Rank::getInstance().getCommunicator(),
      _globalSumRequest
    );
    MPI_Iallreduce(
      &_lightestRankBufferOut,   // send
      &_lightestRankBufferIn,    // receive
      1,                         // count
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

  logTraceInWith3Arguments( "getMaximumSpacetreeSize(int)", localSize, _globalNumberOfInnerUnrefinedCell, alphaP );
/*
  double k      = 1.0;
  double result = localSize - 1;

  while ( localSize - result < result ) {
    //logInfo( "getMax(...)", "result " << result << " is too big" );
    k      += 1.0;
    result  = _globalNumberOfInnerUnrefinedCell / alphaP / k;
  }

  int roundedResult = std::max(  static_cast<int>(std::round(result+0.5)),  1 );
  logTraceOutWith1Argument( "getMaximumSpacetreeSize(int)", result );
  return roundedResult;
*/
  int result = 0;
  int optimalPartitionSize = static_cast<int>( std::round(_globalNumberOfInnerUnrefinedCell / alphaP ));
  while (
    localSize<optimalPartitionSize*2 
    and 
    optimalPartitionSize>2
  ) {
    optimalPartitionSize /= 2;
  }
  logTraceOutWith1Argument( "getMaximumSpacetreeSize(int)", optimalPartitionSize );
  return optimalPartitionSize;
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


void toolbox::loadbalancing::RecursiveSubdivision::updateState() {
  _roundRobinToken++;
  _roundRobinToken = _roundRobinToken % tarch::mpi::Rank::getInstance().getNumberOfRanks();

  if (
    _localNumberOfInnerUnrefinedCell
    <
    std::max( 
      static_cast<int>( std::round(_PercentageOfCoresThatShouldInTheoryGetAtLeastOneCell * tarch::multicore::Core::getInstance().getNumberOfThreads() )), 
      tarch::mpi::Rank::getInstance().getNumberOfRanks()
    )
  ) {
    _state = StrategyState::PostponedDecisionDueToLackOfCells;
  }
  else if ( _state==StrategyState::CoolDown and _totalNumberOfSplits>0 ) {
    _totalNumberOfSplits/=2;
  }
  else if ( _state==StrategyState::CoolDown and _totalNumberOfSplits<=0 ) {
    logInfo( "isInCoolDownPhase()", "terminate cool-down phase and continue to load balance" );
    _state = StrategyState::Standard;
    _totalNumberOfSplits = 0;
  }
  else if ( _state==StrategyState::Standard and _totalNumberOfSplits>=tarch::multicore::Core::getInstance().getNumberOfThreads()) {
    _state = StrategyState::CoolDown;
  }
  else {
    _state = StrategyState::Standard;
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
    case StrategyStep::SplitHeaviestLocalTreeOnce_DontUseLocalRank_UseRecursivePartitioning:
      return "split-heaviest-tree(once,dont-use-local-rank,use-recursive-partitioning)";

  }
  return "<undef>";
}


toolbox::loadbalancing::RecursiveSubdivision::StrategyStep toolbox::loadbalancing::RecursiveSubdivision::getStrategyStep() const {
  if (
    peano4::parallel::SpacetreeSet::getInstance().getLocalSpacetrees().size() > peano4::parallel::Node::MaxSpacetreesPerRank/4*3
  ) {
    logDebug( "getStrategyStep()", "afraid to use too many trees and hence to overbook system" );
    return StrategyStep::Wait;
  }


  if (
    _state==StrategyState::PostponedDecisionDueToLackOfCells
    or
    _state == StrategyState::CoolDown
  ) {
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
    static_cast<double>(peano4::parallel::SpacetreeSet::getInstance().getLocalSpacetrees().size()) < _PercentageOfCoresThatShouldInTheoryGetAtLeastOneCell * tarch::multicore::Core::getInstance().getNumberOfThreads()
  ) {
    return StrategyStep::SplitHeaviestLocalTreeMultipleTimes_UseLocalRank_UseRecursivePartitioning;
  }


  if (
    peano4::parallel::SpacetreeSet::getInstance().getLocalSpacetrees().size() > 0
    and
    doesBiggestLocalSpactreeViolateOptimalityCondition()
  ) {
    if (_roundRobinToken!=0) {
      logInfo( "getStrategyStep()", "should fork further but decided to wait to ensure that not all ranks outsource at the same time to the rank" );
      return StrategyStep::Wait;
    }
    else if (peano4::parallel::SpacetreeSet::getInstance().getLocalSpacetrees().size() >= tarch::multicore::Core::getInstance().getNumberOfThreads()*2)
      return StrategyStep::SplitHeaviestLocalTreeOnce_DontUseLocalRank_UseRecursivePartitioning;
    else
      return StrategyStep::SplitHeaviestLocalTreeOnce_UseAllRanks_UseRecursivePartitioning;
  }


  return StrategyStep::Wait;
}


std::string toolbox::loadbalancing::RecursiveSubdivision::toString( StrategyState state ) {
  switch (state) {
    case StrategyState::Standard:
      return "standard";
    case StrategyState::CoolDown:
      return "cool-down";
    case StrategyState::PostponedDecisionDueToLackOfCells:
      return "postponed-due-to-lack-of-cells";
  }
  return "<undef>";
}


int toolbox::loadbalancing::RecursiveSubdivision::getWeightOfHeaviestLocalSpacetree() const {
  const int heaviestSpacetree = getIdOfHeaviestLocalSpacetree();
  return heaviestSpacetree==NoHeaviestTreeAvailable ? -1 : peano4::parallel::SpacetreeSet::getInstance().getGridStatistics(heaviestSpacetree).getNumberOfLocalUnrefinedCells();
}


void toolbox::loadbalancing::RecursiveSubdivision::finishStep() {
  updateGlobalView();
  updateBlacklist();
  updateState();

  auto step = getStrategyStep();

  logInfo(
    "finishStep()",
    toString( step ) << " in state " << toString( _state ) << " (global cell count=" << _globalNumberOfInnerUnrefinedCell <<
    ", heaviest local tree=" << getIdOfHeaviestLocalSpacetree() << ", heaviest local weight=" << getWeightOfHeaviestLocalSpacetree() << 
    ",lightest-rank=" << _lightestRank << ",max-tree-size=" << getMaximumSpacetreeSize() << ",has-spread=" << _hasSpreadOutOverAllRanks << ")"
  );

  switch ( step ) {
    case StrategyStep::Wait:
      break;
    case StrategyStep::SpreadEquallyOverAllRanks:
      {
// @todo Docu:
// This routine is usually called
// early throughout the grid construction. At this point, we want to bring in ranks as soon as possible. However, it is
// very unlikely that the grid at that point will allow us to balance properly. We will likely have something like 27
// cells for 8 ranks. With integer arithmetics, we would now get 3 cells per rank with the remainder of cells remaining
// on rank 0. This is not a big issue for small meshes, but for large meshes even a tiny overloading of rank 0 can result
// in an out-of-memory situation. I therefore have this module increement within the for loop.
        int cellsPerRank = std::max( static_cast<int>(std::round(_globalNumberOfInnerUnrefinedCell / tarch::mpi::Rank::getInstance().getNumberOfRanks())), 1);

        _hasSpreadOutOverAllRanks = true;

        for (int targetRank=1; targetRank<tarch::mpi::Rank::getInstance().getNumberOfRanks(); targetRank++ ) {
          int thisRanksCells = cellsPerRank;
          if (static_cast<int>(_globalNumberOfInnerUnrefinedCell) % tarch::mpi::Rank::getInstance().getNumberOfRanks() >= targetRank) {
            thisRanksCells++;
          }
          triggerSplit(0, thisRanksCells, targetRank);
        }
      }
      break;
    case StrategyStep::SplitHeaviestLocalTreeMultipleTimes_UseLocalRank_UseRecursivePartitioning:
      {
        int heaviestSpacetree                              = getIdOfHeaviestLocalSpacetree();
        if (heaviestSpacetree!=NoHeaviestTreeAvailable) {
          int numberOfLocalUnrefinedCellsOfHeaviestSpacetree = getWeightOfHeaviestLocalSpacetree();
          int cellsPerCore      = getMaximumSpacetreeSize(numberOfLocalUnrefinedCellsOfHeaviestSpacetree);
          int numberOfSplits    = numberOfLocalUnrefinedCellsOfHeaviestSpacetree/cellsPerCore;
          logInfo( "finishStep()", "insufficient number of cores occupied on this rank, so split " << cellsPerCore << " cells iteratively (" << numberOfSplits << " splits) from tree " << heaviestSpacetree << " on local rank (hosts " << numberOfLocalUnrefinedCellsOfHeaviestSpacetree << " unrefined cells)" );

          for (int i=0; i<numberOfSplits; i++) {
            triggerSplit(heaviestSpacetree, cellsPerCore, tarch::mpi::Rank::getInstance().getRank());
          }
        }
      }
      break;
    case StrategyStep::SplitHeaviestLocalTreeOnce_UseAllRanks_UseRecursivePartitioning:
      {
        int heaviestSpacetree                              = getIdOfHeaviestLocalSpacetree();
        if (heaviestSpacetree!=NoHeaviestTreeAvailable) {
          int numberOfLocalUnrefinedCellsOfHeaviestSpacetree = getWeightOfHeaviestLocalSpacetree();
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
    case StrategyStep::SplitHeaviestLocalTreeOnce_DontUseLocalRank_UseRecursivePartitioning:
      {
        int heaviestSpacetree                              = getIdOfHeaviestLocalSpacetree();
        if (heaviestSpacetree!=NoHeaviestTreeAvailable and _lightestRank!=tarch::mpi::Rank::getInstance().getRank()) {
          int numberOfLocalUnrefinedCellsOfHeaviestSpacetree = getWeightOfHeaviestLocalSpacetree();
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
