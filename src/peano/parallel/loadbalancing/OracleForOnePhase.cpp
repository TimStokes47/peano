#include "peano/parallel/loadbalancing/OracleForOnePhase.h"
#include "tarch/Assertions.h"
#include "tarch/parallel/Node.h"


#include <sstream>


std::string peano::parallel::loadbalancing::convertLoadBalancingFlagToString(int flag) {
  assertion1( flag>=ContinueButTryToJoinWorkers, flag );
  assertion1( flag<=ForkAllChildrenAndBecomeAdministrativeRank, flag );

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
