// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_HEAP_HEAP_H_
#define _PEANO_HEAP_HEAP_H_

#include "peano/heap/records/MetaInformation.h"
#include <map>
#include <vector>
#include <list>
#include "tarch/logging/Log.h"
#include "tarch/services/Service.h"

#include "tarch/multicore/MulticoreDefinitions.h"


#if defined(SharedMemoryParallelisation)
#include "tarch/multicore/BooleanSemaphore.h"
#endif

#ifdef ParallelExchangePackedRecords
   #pragma pack (push, 1)
#endif


namespace peano {
  namespace heap {
    template<class Data>
    class Heap;

    /**
     * Flags to specify which kind of message is sent or
     * received.
     */
    enum MessageType {
      NeighbourCommunication,
      ForkOrJoinCommunication,
      MasterWorkerCommunication
    };
  }
}




/**
 * Heap Singleton - Single Point of Contact for Heap Management
 *
 * HeapData is a template class that manages the storage of
 * vertex data on the heap. The vertex data class has to be generated
 * by DaStGen to provide MPI methods for sending and receiving. The idea of
 * heaps in Peano is that you model your vertex to have one integer index
 * per associated heap object with arbitrary cardinality. Whenever you
 * create a vertex, you ask the heap for a new index and store this index
 * within the vertex. Afterwards, you befill the heap with data.
 *
 * A similar reasoning holds for cells as well.
 *
 * Since a heap is kind of a global thing, this class is a singleton.
 *
 * !!! MPI Handling
 *
 * The elements stored on the heap have to be modeled due to DaStGen. As a
 * result, we can send individual elements as well as sequences of classes
 * away as MPI data types. This is done whenever the user calls send on a
 * specific heap element, i.e. the user calls send with a heap element index
 * and the heap then sends away all elements associated to this heap. This way,
 * the user has full control which heap elements to distribute to which nodes.
 * However, no heap data is exchanged automatically - you have to do this within
 * the mapping's prepare send and merge operations.
 *
 * If an adapter sends or received data, it has first to inform the heap that
 * is will communicate due to MPI. For this, you have to call
 * startToSendOrReceiveHeapData() - typically in beginIteration(). At the end
 * of the iteration, you have to call finishedToSendOrReceiveHeapData().
 *
 * A send operation triggered due to sendData() for a given heap element is
 * a process with three steps. First, the data is copied into a temporary array
 * and meta information is assembled. Then, the meta information is sent away
 * due to a blocking send. Meta data is modeled by
 * \code
\include dastgen/MetaInformation.def
\endcode (see struct MetaInformation). Then, a non-blocking send for the actual
 * data is launched and the send object is pushed on a stack _sendTasks. When
 * one grid traversal terminates, all send processes are finalised. After an
 * iteration, no send operations are dangling anymore, and all data has left
 * the current MPI node.
 *
 * The receive operation is more complex: Peano runs through the spacetree grid
 * forth and back, i.e. the traversal direction of the automaton is inverted
 * after each iteration. As a consequence, we may not simply trigger receive
 * calls when we need data from the heap, as receives in a subsequent traversal
 * have a mirrored order. Instead, the heap has to receive data from a given
 * node in the background until all data has been received. The received data
 * has to be stored in a temporary buffer that we call receive buffer. When all
 * data is available, the read order is the inverse of the receive order, i.e.
 * data from the receive buffer is deployed to the receiving tree automaton in
 * reserve order.
 *
 * To be able to identify whether all data is received, we assume that the
 * number of outgoing messages equals the number of incoming messages. That
 * does however not mean that the cardinalities of the individual messages
 * are the same. We also do not enforce that the data sent in one step is
 * received in the next step. Several additional adapter runs might be
 * triggered before data actually is used due to receiveData() calls. However,
 * we do not support that multiple send traversals may follow each other
 * without receive traversals in-between. The earliest time when you may send
 * heap data again after you've sent out heap data before is the grid traversal
 * where you also receive data.
 *
 * !!! Efficiency note
 *
 * If a lot of heap data is exchanged, the asynchronous exchange of information
 * can block all MPI buffers. In this case, it is useful to call
 * receiveDanglingMessages() from time to time manually within your mappings.
 *
 * !!! Heap data exchange throughout forks, joins, and master-worker exchange
 *
 * The arguing above (with the inversed read order in most cases) does not hold
 * for the forks and joins, and it also does not hold if a master sends heap
 * data to a worker or the other way round. Here, all data is sent-received
 * in-order and synchronous. For these special cases, all communication is
 * deployed to a separate tag to avoid confusion. You switch to this mode due
 * to the synchronous flag of the send and receive operation.
 *
 * !!! Troubleshooting
 *
 * # Is your receiving mapping also calling startToSendOrReceiveHeapData()?
 * # Does each node send exactly the same number of vertices to the other nodes
 *   that it wants to receive? The size of the heap records doesn't have to be
 *   the same but each vertex that wants to receive data also has to send a
 *   (probably empty) record the the corresponding neighbour.
 *
 * @author Kristof Unterweger, Tobias Weinzierl
 */
template <class Data>
class peano::heap::Heap: public tarch::services::Service {
  private:

    #if defined(ParallelExchangePackedRecords)
    typedef peano::heap::records::MetaInformation::Packed  MetaInformation;
    #else
    typedef peano::heap::records::MetaInformation          MetaInformation;
    #endif

    /**
     * Wrapper for a send or receive task
     *
     * Holds the information for a send or receive task, i.e. it holds the MPI
     * request to check weather the task has been finished (both sends and
     * receives are realised as non-blocking MPI data exchange) as well as a
     * pointer to the allocated data where the copy of the data is
     * stored (send task) or is to be stored (receive task).
     */
    struct SendReceiveTask {
      #ifdef Parallel
      MPI_Request     _request;
      #endif
      MetaInformation _metaInformation;
      /**
       * Without semantics for send tasks but important for receive tasks as we
       * have to store from which rank the data arrived from.
       */
      int             _rank;

      #if defined(Parallel) && defined(ParallelExchangePackedRecords)
      typedef typename Data::Packed  MPIData;
      #else
      typedef Data    MPIData;
      #endif
      MPIData*        _data;

      #ifdef Asserts
      SendReceiveTask();
      #endif
    };

    /**
     * Logging device.
     */
    static tarch::logging::Log _log;

    typedef std::map<int, std::vector<Data>*>  HeapContainer;
    /**
     * Map that holds all data that is stored on the heap
     * via this class.
     */
    HeapContainer _heapData;

    std::list<int>   _freedHeapIndices;

    /**
     * Stores the next available index. By now the indices
     * are generated in a linear order, so no attention is
     * payed by now to fragmentation of the index space by
     * deleting data.
     */
    int _nextIndex;

    #ifdef Parallel
    /**
     * MPI tag used for sending data.
     */
    int _mpiTagForNeighbourDataExchange;
    int _mpiTagForMasterWorkerDataExchange;
    int _mpiTagToExchangeForkJoinData;
    #endif

    std::vector<SendReceiveTask> _neighbourDataSendTasks;
    std::vector<SendReceiveTask> _masterWorkerDataSendTasks;
    std::vector<SendReceiveTask> _forkJoinDataSendTasks;

    std::vector<SendReceiveTask> _neighbourDataReceiveTasks[2];
    std::vector<SendReceiveTask> _masterWorkerDataReceiveTasks;
    std::vector<SendReceiveTask> _forkJoinDataReceiveTasks;

    int  _numberOfRecordsSentToNeighbour;
    int  _numberOfRecordsSentToMasterWorker;
    int  _numberOfRecordsSentDueToForkJoin;

    int  _numberOfMessagesSentToNeighbour;
    int  _numberOfMessagesSentToMasterWorker;
    int  _numberOfMessagesSentDueToForkJoin;

    int  _numberOfMessagesReceivedFromNeighbour;
    int  _numberOfMessagesReceivedFromMasterWorker;
    int  _numberOfMessagesReceivedDueToForkJoin;

    /**
     * Is either 0 or 1 and identifies which element of _receiveDeployTasks
     * currently is the receive buffer and which one is the deploy buffer.
     */
    int                          _currentReceiveBuffer;

    #if defined(SharedMemoryParallelisation)
    tarch::multicore::BooleanSemaphore _dataSemaphore;
    #endif

    /**
     * Stores the maximum number of heap objects that was stored
     * in this object at any time.
     */
    int _maximumNumberOfHeapEntries;

    /**
     * Stores the number of heap objects that have been allocated within
     * this object during the program's runtime.
     */
    int _numberOfHeapAllocations;

    int _numberOfHeapFrees;

    /**
     * Name for this heap object. Used for plotting statistics.
     */
    std::string _name;

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

    bool  _heapIsCurrentlySentReceived;

    /**
     * Private constructor to hide the possibility
     * to instantiate an object of this class.
     */
    Heap();

    /**
     * Private destructor to free the MPI datatypes.
     */
    ~Heap();

    int findMessageFromRankInNeighbourDataDeployBuffer(int ofRank) const;

    /**
     * Find Message in Buffer for Master Worker or Fork Join Data
     *
     * !!! Realisation Details
     *
     * If we find a message from the right sender, we have to check whether the
     * corresponding message exchange already has finished. If it has not
     * finished, we may not continue to search in the queue for fitting tasks.
     * We we searched, we might find a newer message from the same rank that
     * already has been completed. If we returned this message's index, we
     * would invalidate the message order.
     *
     * !!! Const
     *
     * This operation is not const as it has to test whether an assynchronous
     * message exchange has already finished. For this, it uses MPI_Test.
     * MPI_Test in turn is not const.
     *
     * @return -1 if no message found
     */
    int findMessageFromRankInMasterWorkerOrForkJoinDataBuffer(int ofRank, std::vector<SendReceiveTask>& tasks);

    void removeMessageFromBuffer(int messageNumber, std::vector<SendReceiveTask>& tasks);

    std::vector< Data > extractMessageFromTaskQueue(
      int                                           messageNumber,
      std::vector<SendReceiveTask>&                 tasks,
      const tarch::la::Vector<DIMENSIONS, double>&  position,
      int                                           level
    );

    /**
     * Release all sent messages
     *
     * This operation runs through all sent messages and waits for each sent
     * message until the corresponding non-blocking MPI request is freed, i.e.
     * until the message has left the system. As the underlying MPI_Test
     * modifies the MPI handles, the operation is not const. The method also
     * has some deadlock detection.
     *
     * When it terminates, all messages successfully have left this node, and
     * you may clear the send buffer. The operation itself however does not
     * invoke the clear, as you might need the number of sent messages before
     * you continue in a different context.
     *
     * I originally intended to call the release within
     * finishedToSendOrReceiveHeapData(). However, this introduces a deadlock
     * on many MPI installations: For big (heap) data, they wait until the
     * corresponding receive is called. And this receive is not called before
     * the receiving traversal begins. So I moved it into an operation called
     * later.
     */
    void releaseSentNeighbourMessages();
    void releaseAndClearSentForkJoinAndMasterWorkerMessages();

    void releaseSentMessages(std::vector<SendReceiveTask>& tasks);

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
    void waitUntilNumberOfReceivedNeighbourMessagesEqualsNumberOfSentMessages();

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

    std::vector< Data > receiveNeighbourData(
      int fromRank,
      const tarch::la::Vector<DIMENSIONS, double>&  position,
      int                                           level
    );

    std::vector< Data > receiveMasterWorkerOrForkJoinData(
      int fromRank,
      const tarch::la::Vector<DIMENSIONS, double>&  position,
      int                                           level,
      MessageType                                   messageType
    );

    void receiveDanglingMessages(int tag, std::vector<SendReceiveTask>& taskQueue);

    /**
     * Returns the correct MPI tag for the given message type.
     *
     * Operation is not const as it updates internal statistics.
     */
    int getTagForMessageType(MessageType messageType, bool isSendTask, int messageSize);

  public:
    /**
     * The HeapData class is a singleton, thus one needs to
     * use the getInstance() method to retrieve the single
     * instance of this class.
     */
    static Heap& getInstance();

    /**
     * Retrieves the data that corresponds to the given index.
     */
    std::vector<Data>& getData(int index);

    /**
     * Retrieves the data that corresponds to the given index.
     */
    const std::vector<Data>& getData(int index) const;

    /**
     * Creates new data on the heap and returns the
     * corresponding index.
     *
     * @return The index return is always a non-negativ number.
     */
    int createData();

    /**
     * Returns, if the given index is a known index and, thus,
     * refers to a valid heap data object.
     */
    bool isValidIndex(int index) const;

    /**
     * Deletes the data with the given index and remove index from heap.
     * First, all data associated to this index is cleared. Afterwards, we
     * clear the heap entry. The user however is responsible not to use
     * index anymore. This is important, as the heap might decide to reuse
     * index already for the next createData() call.
     */
    void deleteData(int index);

    /**
     * Deletes all data that has been allocated by the application
     * by means of the createData() method.
     */
    void deleteAllData();

    /**
     * Returns the number of entries being held by this object.
     */
    int getNumberOfAllocatedEntries() const;

    /**
     * Takes data from fromIndex and reassings it to toIndex.
     *
     * If toIndex already points to data, the data from fromIndex is appended
     * to the original data, i.e. no data is erased at toIndex. The operation
     * does not remove fromIndex though fromIndex will afterwards identify an
     * empty data container.
     */
    void moveData( int toIndex, int fromIndex );

    /**
     * This method discards all heap data and prepares the HeapData
     * management to handle new data requests. After calling this
     * method all indices retrieved earlier are invalid.
     */
    void restart();

    /**
     * Shuts down the memory management for heap data and frees all allocated
     * memory.
     *
     * There is a slight difference between
     * terminate() and shutdown(). The counterpart of shutdown() is init() and
     * both operations are called once throughout the whole application
     * lifetime. In contrast, terminate() and restart() are called several
     * times.
     */
    void shutdown();

    /**
     * Sets the name for this heap object, which shows up in the plotting of the
     * statistics.
     */
    void setName(std::string name);

    /**
     * Sends heap data associated to one index to one rank.
     *
     * To avoid overcrowded MPI buffers, send also calls receiveDanglingMessages()
     * once.
     */
    void sendData(
      int                                           index,
      int                                           toRank,
      const tarch::la::Vector<DIMENSIONS, double>&  position,
      int                                           level,
      MessageType                                   messageType
    );

    void sendData(
      const std::vector<Data>&                      data,
      int                                           toRank,
      const tarch::la::Vector<DIMENSIONS, double>&  position,
      int                                           level,
      MessageType                                   messageType
    );

    /**
     * Receive heap data associated to one index from one rank.
     *
     * Wrapper forwarding to the other receiveData() operation with default
     * values. Operation should be used in release mode, as all additional
     * attributes of the overloaded receiveData() operation are used for
     * validation purposes.
     *
     * !!! Rationale
     *
     * Though the operation only deploys data that has been received before, it
     * is not const as it frees data of the local buffers.
     *
     * This operation is not const, as local handles might be deleted
     * afterwards.
     *
     * @see Heap
     * @see receiveData(int)
     */
    std::vector< Data > receiveData(
      int                                           fromRank,
      const tarch::la::Vector<DIMENSIONS, double>&  position,
      int                                           level,
      MessageType                                   messageType
    );

    /**
     * Receive data and append it to local data.
     *
     * @see receiveData()
     */
    void receiveData(
      int                                           index,
      int                                           fromRank,
      const tarch::la::Vector<DIMENSIONS, double>&  position,
      int                                           level,
      MessageType                                   messageType
    );

    /**
     * @see Heap
     */
    virtual void receiveDanglingMessages();

    int getSizeOfForkJoinDataBuffer() const;
    int getSizeOfMasterWorkerDataBuffer() const;
    int getSizeOfDeployBuffer() const;
    int getSizeOfReceiveBuffer() const;
    int getSizeOfSendBuffer() const;

    std::string toString() const;

    /**
     * Start to send or receive data
     *
     * Please hand in the state's traversal bool that informs the heap about
     * the direction of the Peano space-filling curve. This operation should
     * be called in beginIteration().
     */
    void startToSendOrReceiveHeapData(bool isTraversalInverted);

    /**
     * Stop to send or receive data
     *
     * Counterpart of startToSendOrReceiveHeapData() that should be called in
     * the endIteration() event.
     *
     * @see releaseSentMessages()
     */
    void finishedToSendOrReceiveHeapData();

    /**
     * Plots statistics for this heap data.
     */
    void plotStatistics();

    void clearStatistics();
};


#ifdef ParallelExchangePackedRecords
#pragma pack (pop)
#endif


#include "peano/heap/Heap.cpph"

#endif
