// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_HEAP_BOUNDARY_DATA_EXCHANGER_H_
#define _PEANO_HEAP_BOUNDARY_DATA_EXCHANGER_H_


namespace peano {
  namespace heap {
    template<class Data>
    class BoundaryDataExchanger;
  }
}


template<class Data>
class peano::heap::BoundaryDataExchanger {
  private:
    /**
     * Logging device.
     */
    static tarch::logging::Log _log;

    const std::string    _identifier;
    const int            _tag;
    const int            _rank;

    std::vector<SendReceiveTask<Data> >   _sendTasks;
    std::vector<SendReceiveTask<Data> >   _receiveTasks[2];

    int  _numberOfSentMessages;
    int  _numberOfSentRecords;
    int  _numberOfReceivedMessages;
    int  _numberOfReceivedRecords;

    /**
     * Is either 0 or 1 and identifies which element of _receiveDeployTasks
     * currently is the receive buffer and which one is the deploy buffer.
     */
    int _currentReceiveBuffer;


    /**
     * Shall the deploy buffer data be read in reverse order?
     *
     * Usually, the elements of the deploy buffer are delivered in reverse
     * order compared to the order they are received. See the class
     * documentation for the rationale and a reasoning. That means that this
     * flag usually is set. However, if you send out data in an adapter, then
     * do an iteration without communication, and then receive the data from
     * the heap, this flag has to be set to false.
     *
     * As a consequence, this flag is by default true. If you finishedToSendOrReceiveHeapData()
     * and the content of the receive buffer is moved to the buffer (due
     * to a transition of _currentReceiveBuffer to 1 or 0, respectively), this
     * flag also is set true. If no switching is performed in
     * relesaseMessages() as the buffer still was filled but the receive
     * buffer was empty, solely this flag is inverted.
     */
    bool  _readDeployBufferInReverseOrder;

    bool  _wasTraversalInvertedThroughoutLastSendReceiveTraversal;

    int findMessageFromRankInNeighbourDataDeployBuffer(int ofRank) const;

    /**
     * Wait until number of received messages equals sent messages
     *
     * The operation is a big while loop around receiveDanglingMessages() with
     * some deadlocking aspects added, i.e. it can time out. It is not const as
     * receiveDanglingMessages() cannot be const.
     *
     * Besides waiting for MPI to release some handles, the operation also
     * invokes all the services to receive any dangling messages.
     */
    void waitUntilNumberOfReceivedNeighbourMessagesEqualsNumberOfSentMessages(int numberOfMessagesSentThisIteration);

    /**
     * Switches receive and deploy buffer between two iterations.
     *
     * This method switches receive and deploy buffer for heap data
     * exchanged between neighbours. This happens between two iterations
     * in which heap data is exchanged and results in a situation where
     * the former receive buffer is the deploy buffer afterwards and vice
     * versa.
     *
     * Due to the polling mechanism in receiveDanglingMessages(...) it
     * might happen that the receive buffer before the switch contains
     * messages which belong to the following iteration, since a
     * neighbouring rank may already be an iteration ahead. These
     * messages need to reside in the new receive buffer (i.e. the
     * former deploy buffer). Thus, after switching both buffers the
     * information of the statistics map is used to copy all messages
     * that do not belong to the previous iteration to the new receive
     * buffer.
     *
     * @param statistics This map contains the number of messages that
     * where sent to every neighbouring rank. That means the key for the
     * map is a neighbouring rank and the according value is the number
     * of messages sent to this rank in the last grid iteration. If a
     * key (i.e. a rank) does not exist in this map, this means it is
     * either no neighbouring rank or no messages have been sent to it.
     */
    void switchReceiveAndDeployBuffer(int numberOfMessagesSentThisIteration);

    /**
     * Release Requests for Received Messages
     *
     * This operation runs through all received messages. For each one it
     * waits until the receive operation has been finished. The operation
     * basically should be const. However, it calls MPI_Test/MPI_Wait on the
     * request objects associated to the heap. This test modifies the request
     * object which makes the operation (basically via a callback) non-const.
     *
     * Besides waiting for MPI to release some handles, the operation also
     * invokes all the services to receive any dangling messages.
     */
    void releaseReceivedNeighbourMessagesRequests();

    /**
     * Release all sent messages
     *
     * This operation waits until all messages are successfully sent. Then, it
     * clears the send buffer and finally returns the number of messages
     * delivered. This number might differ from the _numberOfSentMessages, as
     * the latter is cleared only if you call clearStatistics(). Otherwise, it
     * can also accumulate throughout multiple iterations.
     *
     * @return Number of messages sent
     */
    int releaseSentMessages();
  public:
    BoundaryDataExchanger();

    /**
     * @param identifier Only required by the plot
     */
    BoundaryDataExchanger(const std::string& identifier, int tag, int rank);

    void startToSendData(bool isTraversalInverted);
    void finishedToSendData();

    void receiveDanglingMessages();

    void sendData(
      const std::vector<Data>&                      data,
      const tarch::la::Vector<DIMENSIONS, double>&  position,
      int                                           level
    );

    std::vector< Data > receiveData(
      const tarch::la::Vector<DIMENSIONS, double>&  position,
      int                                           level
    );

    void plotStatistics() const;
    void clearStatistics();
};


#ifdef Parallel
#include "peano/heap/BoundaryDataExchanger.cpph"
#endif

#endif
