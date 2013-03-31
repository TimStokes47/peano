#include "peano/parallel/loadbalancing/OracleForOnePhase.h"
#include "tarch/Assertions.h"
#include "tarch/parallel/Node.h"


#include <sstream>


peano::parallel::loadbalancing::LoadBalancingFlag peano::parallel::loadbalancing::getLoadBalancingFlag( int value ) {
  switch (value) {
    case Continue:
      return Continue;
    case Join:
      return Join;
    case ForkOnce:
      return ForkOnce;
    case ForkGreedy:
      return ForkGreedy;
    case ForkAllChildrenAndBecomeAdministrativeRank:
      return ForkAllChildrenAndBecomeAdministrativeRank;
    default:
      assertion3( false, "fork type not supported", value, tarch::parallel::Node::getInstance().getRank() );
      return Continue;
  }
}


std::string peano::parallel::loadbalancing::toString(const LoadBalancingFlag& flag) {
  std::ostringstream result;
  switch (flag) {
    case Continue:
      result << "Continue";
      break;
    case Join:
      result << "Join";
      break;
    case ForkOnce:
      result << "ForkOnce";
      break;
    case ForkGreedy:
      result << "ForkGreedy";
      break;
    case ForkAllChildrenAndBecomeAdministrativeRank:
      result << "ForkAllChildrenAndBecomeAdministrativeRank";
      break;
    default:
      result << "integer-lb-flag=" << static_cast<int>(flag);
      break;
  }
  return result.str();
}
