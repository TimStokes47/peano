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
 *
 * If you set SendDataBeforeDescendIntoLocalSubtree, beginIteration() and multiple
 * touchVertexFirstTime() are called before the state actually is received, as
 * the state is received before the rank descends into its local partition not
 * before it starts to traverse the data structures.
 *
 * !!! exchangeWorkerMasterData
 *
 * Typically, all data of the coarsest level are sent back to the master after
 * the very last touchVertexLastTime() call. If you do not depend on this
 * touchVertexLastTime(), you may decide that the cell and the 2^d vertices on
 * the coarsest level are sent back immediately when all local cells of a rank
 * have been processed.
 *
 * If the flags for the vertex and cell data are set appropriately, one might
 * have the idea to send the state to the worker also only prior to the
 * traversal of local data and send it back immediately when all local cells
 * have been processed. In this case, all the statistics about vertices and
 * spacetree nodes however become wrong, as the state it sent 'too late' to the
 * worker and 'too early' up again. It works only anyway if no lb is done.
 *
 * However, if it is speed that matters, you might decide to life with these
 * corrumpted states.
 *
 * !!! Reduction
 *
 * Please note that the reduction also interplays with the specification of the
 * worker-master communication. If the reduction is switched off, the
 * specification is ignored.
 *
 * !!! Optimistic tuning
 *
 * The most aggressive optimisations work if and only if you perform multiple
 * iterations which means in turn that you switch of the load balancing.
 * Otherwise, the runner should/will complain. If lb were active, you cannot
 * delay or even skip the exchange of the state, and thus these two things
 * interfere. As such, the specification is a hint what Peano might do to
 * tune the code. Sending the state late or even skipping whole data exchange
 * then is up to Peano. It might optimise, it might also stick with the old
 * version.
 */
struct peano::CommunicationSpecification {
  public:
    enum ExchangeMasterWorkerData {
      SendDataAndStateBeforeFirstTouchVertexFirstTime,
      SendDataBeforeDescendIntoLocalSubtreeSendStateBeforeFirstTouchVertexFirstTime,
      SendDataAndStateBeforeDescendIntoLocalSubtree,
      MaskOutMasterWorkerDataAndStateExchange
    };

    enum ExchangeWorkerMasterData {
      SendDataAndStateAfterLastTouchVertexLastTime,
      SendDataAfterProcessingOfLocalSubtreeSendStateAfterLastTouchVertexLastTime,
      /**
       * Send data and state up to the master as soon as the local spacetree is
       * processed. As a consequence, we call endIteration() and the
       * prepareSendToMaster() at that time as well, i.e. the traversal is not
       * yet finished when we do that.
       */
      SendDataAndStateAfterProcessingOfLocalSubtree,
      /**
       * Do not send any data up to the master. As a consequence, we call
       * endIteration() and the prepareSendToMaster() as soon as we leave the
       * local subtree but not at the end of the traversal.
       */
      MaskOutWorkerMasterDataAndStateExchange
    };

    enum Action {
      Early,
      Late,
      Skip
    };

    static CommunicationSpecification getMinimalSpecification();

    /**
     * This is the maximum specification where everyting is absolutely
     * synchronised and in order. The plotter mappings, e.g., require
     * such a specification as they have to open an output stream prior
     * to any other operation and they may not close it before everything
     * has been written.
     */
    static CommunicationSpecification getPessimisticSpecification();

    /**
     * @param exchangeMasterWorkerData
     * @param exchangeWorkerMasterData
     * @param exchangeStateAsPreamblePostamble
     */
    CommunicationSpecification( ExchangeMasterWorkerData  exchangeMasterWorkerData_, ExchangeWorkerMasterData  exchangeWorkerMasterData_ );


    Action sendStateBackToMaster() const;
    Action sendDataBackToMaster() const;

    Action receiveDataFromMaster(bool stateMayUseLazyStateAndDataReceives) const;
    Action receiveStateFromMaster(bool stateMayUseLazyStateAndDataReceives) const;

    std::string toString() const;

    static peano::CommunicationSpecification combine(const peano::CommunicationSpecification& lhs, const peano::CommunicationSpecification& rhs);
  private:
    ExchangeMasterWorkerData  exchangeMasterWorkerData;
    ExchangeWorkerMasterData  exchangeWorkerMasterData;
};



/**
 * Combine two specifications.
 */
peano::CommunicationSpecification operator&(const peano::CommunicationSpecification& lhs, const peano::CommunicationSpecification& rhs);

#endif
