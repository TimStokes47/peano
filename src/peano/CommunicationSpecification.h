// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_COMMUNICATION_SPECIFICATION_H_
#define _PEANO_COMMUNICATION_SPECIFICATION_H_

#include <string>


namespace peano {
  struct CommunicationSpecification;
}



/**
 * Specification of communication behaviour of a mapping
 */
struct peano::CommunicationSpecification {
  enum ExchangeMasterWorkerData {
    BeforeFirstTouchVertexFirstTime, BeforeDescendIntoLocalSubtree
  };

  /**
   * !!! Reduction
   *
   * This flag is overruled by the global reduction. If the master determines
   * that no reduction is required, it becomes irrelevant to what value this
   * attribute is set to.
   */
  enum ExchangeWorkerMasterData {
    AfterLastTouchVertexLastTime, AfterProcessingOfLocalSubtree
  };

  bool sendStateAsLateAsPossible;

  ExchangeMasterWorkerData  exchangeMasterWorkerData;
  ExchangeWorkerMasterData  exchangeWorkerMasterData;

  static CommunicationSpecification getMinimalSpecification();

  CommunicationSpecification( ExchangeMasterWorkerData  exchangeMasterWorkerData_, ExchangeWorkerMasterData  exchangeWorkerMasterData_ );

  std::string toString() const;
};



/**
 * Combine two specifications.
 */
peano::CommunicationSpecification operator&(const peano::CommunicationSpecification& lhs, const peano::CommunicationSpecification& rhs);

#endif
