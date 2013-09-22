// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_HEAP_RLE_BOUNDARY_DATA_EXCHANGER_H_
#define _PEANO_HEAP_RLE_BOUNDARY_DATA_EXCHANGER_H_

#include "peano/heap/BoundaryDataExchanger.h"


namespace peano {
  namespace heap {
    template<class Data>
    class RLEBoundaryDataExchanger;
  }
}


template<class Data>
class peano::heap::RLEBoundaryDataExchanger: public peano::heap::BoundaryDataExchanger<Data> {
  private:
    /**
     * Logging device.
     */
    static tarch::logging::Log _log;

    typedef BoundaryDataExchanger<Data> Base;

    int _numberOfEmptyMessagesSinceLastNonEmptySendCall;
    int _totalNumberOfSentMessagesThisTraversal;

    /**
     * Sends all zero-length messages that have not been sent yet and resets
     * their counter to zero.
     */
    void sendAllCompressedEmptyMessages();
  protected:
    virtual int getNumberOfSentMessages() const;

    virtual void postprocessFinishedToSendData();

    virtual void postprocessStartToSendData();

    virtual void handleAndQueueReceivedTask( SendReceiveTask<Data>  receivedTask );

    virtual void handleAndQueueSendTask( SendReceiveTask<Data> sendTask, const std::vector<Data>& data );
  public:
    RLEBoundaryDataExchanger();

    RLEBoundaryDataExchanger(const std::string& identifier, int tag, int rank);

    virtual ~RLEBoundaryDataExchanger();
};


#ifdef Parallel
#include "peano/heap/RLEBoundaryDataExchanger.cpph"
#endif

#endif
