#include "CostMetrics.h"

#include "peano4/parallel/SpacetreeSet.h"
#include "tarch/Assertions.h"
#include "tarch/mpi/Rank.h"
#include "tarch/services/ServiceRepository.h"

#include <set>
#include <sstream>


tarch::logging::Log toolbox::loadbalancing::CostMetrics::_log( "toolbox::loadbalancing::CostMetrics" );


toolbox::loadbalancing::CostMetrics::CostMetrics():
  _localRankWeight(0.0),
  _globalWeight(0.0),
  _lightestRank{0.0, 0},
  _minimumOfMaximumOfRankWeights(0.0) {}


std::string toolbox::loadbalancing::CostMetrics::toString(const std::string& metricName) const {
  std::ostringstream msg;
  msg << "("
      << metricName
      << ",local-rank-weight=" << _localRankWeight
      << ",global-weight="     << _globalWeight
      << ",lightest-rank="     << _lightestRank._rank
      << ",lightest-ranks-weight=" << _lightestRank._weight
      << ",min-of-max-of-rank-weights=" << _minimumOfMaximumOfRankWeights
      << ")";
  return msg.str();
}


double toolbox::loadbalancing::CostMetrics::getCostOfLocalRank() const {
  double localCost = 0;
  std::set<int> idsOfLocalSpacetrees = peano4::parallel::SpacetreeSet::getInstance().getLocalSpacetrees();
  for (auto p: idsOfLocalSpacetrees) {
      localCost += getCostOfLocalTree(p);
  }
  return localCost;
}


void toolbox::loadbalancing::CostMetrics::updateGlobalView() {
  _globalWeight                  = _localRankWeight;
  _lightestRank._weight          = _localRankWeight;
  _lightestRank._rank            = tarch::mpi::Rank::getInstance().getRank();
  _minimumOfMaximumOfRankWeights = getCostOfLocalRank();

  #ifdef Parallel
  const double          localWeight                   = _globalWeight;
  const ReductionBuffer localLightestRank             = _lightestRank;
  const double          localMaximumOfRankWeights     = _minimumOfMaximumOfRankWeights;

  tarch::mpi::Rank::getInstance().allReduce(
    &localWeight, &_globalWeight, 1, MPI_DOUBLE, MPI_SUM,
    [&]() -> void { tarch::services::ServiceRepository::getInstance().receiveDanglingMessages(); }
  );
  tarch::mpi::Rank::getInstance().allReduce(
    &localLightestRank, &_lightestRank, 1, MPI_DOUBLE_INT, MPI_MINLOC,
    [&]() -> void { tarch::services::ServiceRepository::getInstance().receiveDanglingMessages(); }
  );
  tarch::mpi::Rank::getInstance().allReduce(
    &localMaximumOfRankWeights, &_minimumOfMaximumOfRankWeights, 1, MPI_DOUBLE, MPI_MIN,
    [&]() -> void { tarch::services::ServiceRepository::getInstance().receiveDanglingMessages(); }
  );
  #endif
}


double toolbox::loadbalancing::CostMetrics::getGlobalCost() const {
  return _globalWeight;
}


int toolbox::loadbalancing::CostMetrics::getLightestRank() const {
  assertion1( _lightestRank._rank>=0, toString() );
  assertion1( _lightestRank._rank<tarch::mpi::Rank::getInstance().getNumberOfRanks(), toString() );
  return _lightestRank._rank;
}


double toolbox::loadbalancing::CostMetrics::getMinimumOfMaximumRankWeights() const {
  return _minimumOfMaximumOfRankWeights;
}
