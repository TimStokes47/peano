#include "RefinementControlService.h"

#include "RefinementControl.h"

#include <cmath>
#include <map>
#include <set>
#include <vector>

#include "peano4/utils/Globals.h"
#include "peano4/utils/Loop.h"
#include "peano4/grid/grid.h"
#include "tarch/multicore/RecursiveLock.h"
#include "tarch/services/ServiceRepository.h"

tarch::logging::Log exahype2::RefinementControlService::_log("exahype2::RefinementControlService");

int exahype2::RefinementControlService::_reductionTag(tarch::mpi::Rank::reserveFreeTag("exahype2::"
                                                                                       "RefinementControlService"));

tarch::multicore::RecursiveSemaphore exahype2::RefinementControlService::_semaphore;

exahype2::RefinementControlService& exahype2::RefinementControlService::getInstance() {
  static exahype2::RefinementControlService result;
  return result;
}

exahype2::RefinementControlService::RefinementControlService() {
  tarch::services::ServiceRepository::getInstance().addService(this, "exahype2::RefinementControlService");
}

exahype2::RefinementControlService::~RefinementControlService() {
  tarch::services::ServiceRepository::getInstance().removeService(this);
}

void exahype2::RefinementControlService::shutdown() {}

void exahype2::RefinementControlService::receiveDanglingMessages() {
#ifdef Parallel
  int guardFlag;
  MPI_Iprobe(
    MPI_ANY_SOURCE, _reductionTag, tarch::mpi::Rank::getInstance().getCommunicator(), &guardFlag, MPI_STATUS_IGNORE
  );

  if (guardFlag) {
    tarch::multicore::RecursiveLock lock(_semaphore);

    MPI_Status status;
    int        flag;
    MPI_Iprobe(MPI_ANY_SOURCE, _reductionTag, tarch::mpi::Rank::getInstance().getCommunicator(), &flag, &status);
    if (flag) {
      const int rank = status.MPI_SOURCE;
      int       numberOfMessages;
      MPI_Get_count(&status, peano4::grid::GridControlEvent::getGlobalCommunciationDatatype(), &numberOfMessages);

      logDebug("receiveDanglingMessages()", "got " << numberOfMessages << " event(s) from rank " << rank);

      std::vector<peano4::grid::GridControlEvent> receiveBuffer;
      receiveBuffer.resize(numberOfMessages);
      MPI_Recv(
        receiveBuffer.data(),
        numberOfMessages,
        peano4::grid::GridControlEvent::getGlobalCommunciationDatatype(),
        rank,
        _reductionTag,
        tarch::mpi::Rank::getInstance().getCommunicator(),
        MPI_STATUS_IGNORE
      );

      for (auto p : receiveBuffer) {
        _remoteNewEvents.push_back(p);
        _committedEvents.push_back(p);
      }

      logDebug(
        "receiveDanglingMessages()",
        "now hold "
          << _remoteNewEvents.size() << " new remote events and " << _committedEvents.size()
          << " committed events (valid for this traversal)"
      );
    }
  }
#endif
}

std::string exahype2::RefinementControlService::toString() const {
  std::ostringstream msg;
  msg
    << "("
    << "#new-local-events=" << _localNewEvents.size() << "#new-remote-events=" << _remoteNewEvents.size()
    << ",#committed-events=" << _committedEvents.size() << ")";
  return msg.str();
}

void exahype2::RefinementControlService::merge(const RefinementControl& control) {
  tarch::multicore::RecursiveLock lock(_semaphore);
  _localNewEvents.insert(_localNewEvents.end(), control._newEvents.begin(), control._newEvents.end());
  _localNewSiblingEraseEvents.insert(
    _localNewSiblingEraseEvents.end(),
    control._newSiblingEraseEvents.begin(),
    control._newSiblingEraseEvents.end()
  );
}

void exahype2::RefinementControlService::keepOnlyCompleteSiblingEraseEvents() {
  if (_localNewSiblingEraseEvents.empty()) {
    return;
  }

  // Identify a parent by its mesh size and its centre in units of half its
  // mesh size, which are integers up to round-off
  typedef std::vector<long long> ParentKey;
  auto key = [](const RefinementControl::SiblingEraseEvent& event) -> ParentKey {
    ParentKey result;
    for (int d = 0; d < Dimensions; d++) {
      result.push_back(std::llround(1e9 * event.parentH(d)));
      result.push_back(std::llround(2.0 * event.parentCentre(d) / event.parentH(d)));
    }
    return result;
  };

  std::map<ParentKey, std::set<int>> childrenPerParent;
  for (const auto& event : _localNewSiblingEraseEvents) {
    childrenPerParent[key(event)].insert(event.positionWithinParent);
  }

  std::set<ParentKey> completeParents;
  for (const auto& [parent, children] : childrenPerParent) {
    if (static_cast<int>(children.size()) == ThreePowerD) {
      completeParents.insert(parent);
    }
  }

  // The 3^d parents around a parent, including the parent itself. A centre
  // in units of half the mesh size changes by two per neighbour.
  auto neighbourhood = [](const ParentKey& parent) -> std::vector<ParentKey> {
    std::vector<ParentKey> result;
    dfor3(offset) {
      ParentKey neighbour = parent;
      for (int d = 0; d < Dimensions; d++) {
        neighbour[2 * d + 1] += 2 * (offset(d) - 1);
      }
      result.push_back(neighbour);
    }
    enddforx
    return result;
  };

  // One representative request per parent, which provides the parent's
  // geometry, the mesh size of the event and the lifetime
  std::map<ParentKey, RefinementControl::SiblingEraseEvent> representative;
  for (const auto& event : _localNewSiblingEraseEvents) {
    representative.emplace(key(event), event);
  }

  int erasableParents = 0;
  for (const auto& parent : completeParents) {
    bool wholeNeighbourhoodIsComplete = true;
    for (const auto& neighbour : neighbourhood(parent)) {
      wholeNeighbourhoodIsComplete &= completeParents.count(neighbour) > 0;
    }
    if (wholeNeighbourhoodIsComplete) {
      // One erase event which covers exactly the parent plus one parent-sized
      // neighbour along each direction, enlarged by a relative tolerance
      const RefinementControl::SiblingEraseEvent& child = representative.at(parent);
      const tarch::la::Vector<Dimensions, double> width  = (3.0 + 0.03) * child.parentH;
      const tarch::la::Vector<Dimensions, double> offset = child.parentCentre - 0.5 * width;
      _localNewEvents.push_back(std::pair<peano4::grid::GridControlEvent, int>(
        peano4::grid::GridControlEvent(
          peano4::grid::GridControlEvent::RefinementControl::Erase, offset, width, child.event.getH()
        ),
        child.lifetime
      ));
      erasableParents++;
    }
  }

  logInfo(
    "keepOnlyCompleteSiblingEraseEvents()",
    "replace " << _localNewSiblingEraseEvents.size() << " erase request(s) of " << childrenPerParent.size()
               << " parent(s), of which " << completeParents.size() << " ask to erase all their children, by "
               << erasableParents << " erase request(s) for parents whose whole neighbourhood can be erased"
  );
  _localNewSiblingEraseEvents.clear();
}


void exahype2::RefinementControlService::finishStep() {
  tarch::multicore::RecursiveLock lock(_semaphore);

  keepOnlyCompleteSiblingEraseEvents();

  _committedEvents.clear();

  int                                    maxLifetime = 0;
  RefinementControl::NewEvents::iterator p           = _localNewEvents.begin();
  while (p != _localNewEvents.end()) {
    _committedEvents.push_back(p->first);
    p->second--;

    if (p->second <= 0) {
      p = _localNewEvents.erase(p);
    } else {
      maxLifetime = std::max(maxLifetime, p->second);
      p++;
    }
  }

  if (not _committedEvents.empty()) {
    logInfo(
      "finishStep()",
      "activate "
        << _committedEvents.size()
        << " refinement/erase instructions (can be taken into account in next grid sweep). Keep "
        << _localNewEvents.size() << " local event(s) to be re-delivered later (max lifetime " << maxLifetime
        << ") besides the " << _remoteNewEvents.size() << " remote events which will also be re-delivered next"
    );
  }

#ifdef Parallel
  freeAllPendingSendRequests();
  _copyOfCommittedEvents = _committedEvents;
  triggerSendOfCopyOfCommittedEvents();
#endif

  for (auto p : _remoteNewEvents) {
    _committedEvents.push_back(p);
  }
  _remoteNewEvents.clear();

  _committedEvents = ::peano4::grid::merge(_committedEvents);
}

#ifdef Parallel
void exahype2::RefinementControlService::freeAllPendingSendRequests() {
  while (not _sendRequests.empty()) {
    std::vector<MPI_Request*>::iterator p = _sendRequests.begin();
    while (p != _sendRequests.end()) {
      if (*p == nullptr) {
        p = _sendRequests.erase(p);
      } else {
        int flag;
        MPI_Test(*p, &flag, MPI_STATUS_IGNORE);
        if (flag) {
          *p = nullptr;
        }
        p++;
      }
    }
    tarch::services::ServiceRepository::getInstance().receiveDanglingMessages();
  }
}

void exahype2::RefinementControlService::triggerSendOfCopyOfCommittedEvents() {
  _sendRequests.clear();

  logInfo(
    "triggerSendOfCopyOfCommittedEvents()", "share my " << _copyOfCommittedEvents.size() << " event(s) with others"
  );
  if (_copyOfCommittedEvents.size() > 0) {
    _sendRequests = std::vector<MPI_Request*>(tarch::mpi::Rank::getInstance().getNumberOfRanks(), nullptr);
    for (int rank = 0; rank < tarch::mpi::Rank::getInstance().getNumberOfRanks(); rank++) {
      if (rank != tarch::mpi::Rank::getInstance().getRank()) {
        _sendRequests[rank] = new MPI_Request;
        MPI_Isend(
          _copyOfCommittedEvents.data(),
          _copyOfCommittedEvents.size(),
          peano4::grid::GridControlEvent::getGlobalCommunciationDatatype(),
          rank,
          _reductionTag,
          tarch::mpi::Rank::getInstance().getCommunicator(),
          _sendRequests[rank]
        );
      }
    }
  }
}
#endif

std::vector<peano4::grid::GridControlEvent> exahype2::RefinementControlService::createGridControlEventsForNextTraversal() const {
  return _committedEvents;
}
