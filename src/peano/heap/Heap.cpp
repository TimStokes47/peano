#include "peano/heap/Heap.h"


std::string peano::heap::toString(MessageType type) {
  switch (type) {
    case NeighbourCommunication:
      return "neighbour";
    case ForkOrJoinCommunication:
      return "fork-or-join";
    case MasterWorkerCommunication:
      return "master-worker";
  }
}
