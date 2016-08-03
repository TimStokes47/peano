#include "peano/parallel/loadbalancing/OracleForOnePhase.h"
#include "tarch/Assertions.h"
#include "tarch/parallel/Node.h"


#include <sstream>


std::string peano::parallel::loadbalancing::convertLoadBalancingFlagToString(int flag) {
  assertion1( flag>=ContinueButTryToJoinWorkers, flag );
  assertion1( flag<=ForkAllChildrenAndBecomeAdministrativeRank, flag );

  std::ostringstream result;
  switch (flag) {
    case ContinueButTryToJoinWorkers:
      result << "continue-but-try-to-join-workers";
      break;
    case Continue:
      result << "continue";
      break;
    case Join:
      result << "join";
      break;
    case UndefinedLoadBalancingFlag:
      result << "undefined-lb-flag";
      break;
    case ForkOnce:
      result << "Fork-once";
      break;
    case ForkGreedy:
      result << "Fork-greedy";
      break;
    case ForkAllChildrenAndBecomeAdministrativeRank:
      result << "fork-all-children/become-administrative-rank";
      break;
    default:
      result << "fork-" << static_cast<int>(flag) << "-times";
      break;
  }
  return result.str();
}
