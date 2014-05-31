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
 *
 * Peano applies a clear broadcast/reduction paradigm in principle. Prior to
 * the actual traversal on any rank, the state is broadcasted, vertices and
 * cells are exchanged and then every rank runs. In return, the vertices and
 * cells from a rank are reduced when this rank has finished.
 *
 * This has a fundamental drawback: No rank can start to work before it has
 * received the master's state and its vertices. And it can send back the state
 * and the vertices not before the last touchVertexLastTime() has been called.
 * Unfortunately, each local tree is embedded into 3^d-1 other trees to preserve
 * the global Peano order on all grid entities, i.e. the worker receives data,
 * then runs through a part of this ghost layer, runs through the local partition,
 * through the remaining ghost cells, and then sends state, vertices, and cell
 * data up to the master again. Otherwise, even simple things such as the count
 * of the number of vertices would become invalid. However, sometimes it is
 * speed you need while you can live with inaccurate results. This specification
 * allows you to indicate this.
 *
 * This is important in particular if you perform several operations in a row.
 * In iterative solvers, e.g. There, global reduction typically is not required
 * in each individual step though you might require reduction in the tree to
 * couple different grid hierarchies.
 *
 * !!! exchangeMasterWorkerData
 * !!! exchangeWorkerMasterData
 *
 *
 * !! exchangeStateAsPreamblePostamble
 *
 * If the flags
 *
 * !!! Reduction
 *
 * Please note that the reduction also interplays with the specification of the
 * worker-master communication. If the reduction is switched off, the
 * specification is ignored.
 */
struct peano::CommunicationSpecification {
  enum ExchangeMasterWorkerData {
    BeforeFirstTouchVertexFirstTime, BeforeDescendIntoLocalSubtree
  };

  enum ExchangeWorkerMasterData {
    AfterLastTouchVertexLastTime, AfterProcessingOfLocalSubtree
  };

  bool exchangeStateAsPreamblePostamble;

  ExchangeMasterWorkerData  exchangeMasterWorkerData;
  ExchangeWorkerMasterData  exchangeWorkerMasterData;

  static CommunicationSpecification getMinimalSpecification();

  /**
   * @param exchangeMasterWorkerData
   * @param exchangeWorkerMasterData
   * @param exchangeStateAsPreamblePostamble
   */
  CommunicationSpecification( ExchangeMasterWorkerData  exchangeMasterWorkerData_, ExchangeWorkerMasterData  exchangeWorkerMasterData_, bool exchangeStateAsPreamblePostamble_ );

  bool sendStateAtEndOfTraversal() const;

  std::string toString() const;
};



/**
 * Combine two specifications.
 */
peano::CommunicationSpecification operator&(const peano::CommunicationSpecification& lhs, const peano::CommunicationSpecification& rhs);

#endif
