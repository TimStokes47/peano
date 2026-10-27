#include "Statistics.h"

#include "peano4/parallel/SpacetreeSet.h"
#include "tarch/Assertions.h"
#include "tarch/mpi/Rank.h"
#include "tarch/services/ServiceRepository.h"

#include <algorithm>
#include <sstream>


tarch::logging::Log  toolbox::loadbalancing::Statistics::_log( "toolbox::loadbalancing::Statistics" );


toolbox::loadbalancing::Statistics::Statistics():
  _localLoadBalancingEnabled(true),
  _localNumberOfSplits(0),
  _globalNumberOfTrees(1),
  _globalNumberOfRanksWithEnabledLoadBalancing(0),
  _globalNumberOfSplits(0),
  _numberOfStateUpdatesWithoutAnySplit(MaxNumberOfStateUpdatesWithoutAnySplit),
  _maximumTreesPerRank(1) {}


void toolbox::loadbalancing::Statistics::notifyOfStateChange( State state ) {
  _localLoadBalancingEnabled = state!=State::SwitchedOff;
}


std::string  toolbox::loadbalancing::Statistics::toString() const {
  std::ostringstream msg;

  msg << "(global-number-of-trees=" << _globalNumberOfTrees
      << ",max-trees-per-rank=" << _maximumTreesPerRank
      << ",local-load-balancing-enabled=" << _localLoadBalancingEnabled
      << ",global-number-of-ranks-with-enabled-lb=" << _globalNumberOfRanksWithEnabledLoadBalancing
      << ",local-number-of-splits=" << _localNumberOfSplits
      << ",global-number-of-splits=" << _globalNumberOfSplits
      << ",number-of-state-updates-without-any-split=" << _numberOfStateUpdatesWithoutAnySplit
      << ")";

  return msg.str();
}


void toolbox::loadbalancing::Statistics::updateGlobalView() {
  const int localNumberOfTrees = peano4::parallel::SpacetreeSet::getInstance().getLocalSpacetrees().size();

  _globalNumberOfTrees                         = localNumberOfTrees;
  _globalNumberOfRanksWithEnabledLoadBalancing = _localLoadBalancingEnabled ? 1 : 0;
  _globalNumberOfSplits                        = _localNumberOfSplits;
  _maximumTreesPerRank                         = localNumberOfTrees;

  #ifdef Parallel
  const int localSums[3] = {
    _globalNumberOfTrees,
    _globalNumberOfRanksWithEnabledLoadBalancing,
    _globalNumberOfSplits
  };
  int globalSums[3] = { localSums[0], localSums[1], localSums[2] };
  tarch::mpi::Rank::getInstance().allReduce(
    localSums, globalSums, 3, MPI_INT, MPI_SUM,
    [&]() -> void { tarch::services::ServiceRepository::getInstance().receiveDanglingMessages(); }
  );
  _globalNumberOfTrees                         = globalSums[0];
  _globalNumberOfRanksWithEnabledLoadBalancing = globalSums[1];
  _globalNumberOfSplits                        = globalSums[2];

  tarch::mpi::Rank::getInstance().allReduce(
    &localNumberOfTrees, &_maximumTreesPerRank, 1, MPI_INT, MPI_MAX,
    [&]() -> void { tarch::services::ServiceRepository::getInstance().receiveDanglingMessages(); }
  );
  #endif

  if ( _globalNumberOfSplits==0 ) {
    _numberOfStateUpdatesWithoutAnySplit = std::min( _numberOfStateUpdatesWithoutAnySplit+1, MaxNumberOfStateUpdatesWithoutAnySplit );
  }
  else {
    _numberOfStateUpdatesWithoutAnySplit = 0;
  }

  _localNumberOfSplits = 0;
}


void toolbox::loadbalancing::Statistics::incLocalNumberOfSplits( int delta ) {
  assertion( delta>0 );
  _localNumberOfSplits += delta;
}


int toolbox::loadbalancing::Statistics::getLocalNumberOfSplits() const {
  return _localNumberOfSplits;
}


int toolbox::loadbalancing::Statistics::getGlobalNumberOfRanksWithEnabledLoadBalancing() const {
  return _globalNumberOfRanksWithEnabledLoadBalancing;
}


int toolbox::loadbalancing::Statistics::getGlobalNumberOfTrees() const {
  return _globalNumberOfTrees;
}


int toolbox::loadbalancing::Statistics::getMaximumTreesPerRank() const {
  return _maximumTreesPerRank;
}


int toolbox::loadbalancing::Statistics::getNumberOfStateUpdatesWithoutAnySplit() const {
  return _numberOfStateUpdatesWithoutAnySplit;
}



