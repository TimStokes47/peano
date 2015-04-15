// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_HEAP_SYNCHRONOUS_DATA_EXCHANGER_H_
#define _PEANO_HEAP_SYNCHRONOUS_DATA_EXCHANGER_H_


namespace peano {
  namespace heap {
    template<class Data, bool CreateCopiesOfSentData>
    class SynchronousDataExchanger;
  }
}


/**
 * !!! CreateCopiesOfSentData
 *
 * This flag indicates how to handle the exchange of synchronous data. We
 * either can copy all data for each send and then free this data manually.
 * Or we can send away data directly from the heap and rely on the user that
 * this data remains persistent.
 */
template<class Data, bool CreateCopiesOfSentData>
class peano::heap::SynchronousDataExchanger {
  private:
    /**
     * Logging device.
     */
    static tarch::logging::Log _log;

    const std::string    _identifier;
    const int            _tag;

    std::list<SendReceiveTask<Data> >   _sendTasks;
    std::list<SendReceiveTask<Data> >   _receiveTasks;

    int  _numberOfSentMessages;
    int  _numberOfSentRecords;

    int  _numberOfReceivedMessages;
    int  _numberOfReceivedRecords;

    #ifdef Asserts
    bool _isCurrentlySending;
    #endif

    /**
     * Find Message in Buffer
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
    typename std::list< SendReceiveTask<Data> >::iterator findMessageFromRankInReceiveBuffer(int ofRank);

    std::vector< Data > extractMessageFromReceiveBuffer(
      typename std::list< SendReceiveTask<Data> >::iterator messageTask,
      const tarch::la::Vector<DIMENSIONS, double>&          position,
      int                                                   level
    );
  public:
    /**
     * @param identifier Only required by the plot
     */
    SynchronousDataExchanger(const std::string& identifier, int tag);

    void startToSendData();

    /**
     * Release the sent messages
     *
     * Wait until test holds for all messages in the send queue and then erase
     * that buffer.
     */
    void finishedToSendData();

    /**
     * Check for data in the mpi queue
     *
     * If there are new messages, add a new task to the receive queue, trigger
     * the corresponding ireceive if the message comes along with data, and
     * return.
     */
    void receiveDanglingMessages();

    void sendData(
      const std::vector<Data>&                      data,
      int                                           toRank,
      const tarch::la::Vector<DIMENSIONS, double>&  position,
      int                                           level
    );

    std::vector< Data > receiveData(
      int                                           fromRank,
      const tarch::la::Vector<DIMENSIONS, double>&  position,
      int                                           level
    );

    void plotStatistics() const;
    void clearStatistics();
};


#ifdef Parallel
#include "peano/heap/SynchronousDataExchanger.cpph"
#endif

#endif
