#include "peano/CommunicationSpecification.h"
#include "tarch/Assertions.h"
#include "tarch/logging/Log.h"


#include <sstream>


peano::CommunicationSpecification::CommunicationSpecification(ExchangeMasterWorkerData  exchangeMasterWorkerData_, ExchangeWorkerMasterData  exchangeWorkerMasterData_, bool exchangeStateAsPreamblePostamble_):
  exchangeStateAsPreamblePostamble(exchangeStateAsPreamblePostamble_),
  exchangeMasterWorkerData( exchangeMasterWorkerData_ ),
  exchangeWorkerMasterData( exchangeWorkerMasterData_ ) {
}


std::string peano::CommunicationSpecification::toString() const {
  std::ostringstream msg;

  msg << "(exchange-master-worker-data_=";
  switch (exchangeMasterWorkerData) {
    case BeforeFirstTouchVertexFirstTime:
      msg << "before-first-touch-vertex-first-time";
      break;
    case BeforeDescendIntoLocalSubtree:
      msg << "before-descend-into-local-subtree";
      break;
  }
  msg << ",exchange-worker-master-data=";
  switch (exchangeWorkerMasterData) {
    case AfterLastTouchVertexLastTime:
      msg << "after-last-touch-vertex-last-time";
      break;
    case AfterProcessingOfLocalSubtree:
      msg << "after-processing-of-local-subtree";
      break;
  }
  msg << ")";
  return msg.str();
}


peano::CommunicationSpecification operator&(const peano::CommunicationSpecification& lhs, const peano::CommunicationSpecification& rhs) {
  static tarch::logging::Log _log( "peano::CommunicationSpecification" );
  logTraceInWith2Arguments("operator&(...)", lhs.toString(), rhs.toString() );

  const peano::CommunicationSpecification result(
    lhs.exchangeMasterWorkerData==peano::CommunicationSpecification::BeforeFirstTouchVertexFirstTime ? peano::CommunicationSpecification::BeforeFirstTouchVertexFirstTime : rhs.exchangeMasterWorkerData,
    lhs.exchangeWorkerMasterData==peano::CommunicationSpecification::AfterLastTouchVertexLastTime    ? peano::CommunicationSpecification::AfterLastTouchVertexLastTime    : rhs.exchangeWorkerMasterData,
    lhs.exchangeStateAsPreamblePostamble | rhs.exchangeStateAsPreamblePostamble
  );

  logTraceOutWith1Argument("operator&(...)",result.toString());
  return result;
}


peano::CommunicationSpecification peano::CommunicationSpecification::getMinimalSpecification() {
  return CommunicationSpecification(peano::CommunicationSpecification::BeforeDescendIntoLocalSubtree,peano::CommunicationSpecification::AfterProcessingOfLocalSubtree,false);
}


bool peano::CommunicationSpecification::sendStateBackToMasterAtEndOfTraversal() const {
  return exchangeWorkerMasterData==peano::CommunicationSpecification::AfterLastTouchVertexLastTime || exchangeStateAsPreamblePostamble;
}
