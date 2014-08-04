// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_HEAP_PLAIN_BOUNDARY_DATA_EXCHANGER_H_
#define _PEANO_HEAP_PLAIN_BOUNDARY_DATA_EXCHANGER_H_

#include "peano/heap/BoundaryDataExchanger.h"


namespace peano {
  namespace heap {
    template<class Data>
    class PlainBoundaryDataExchanger;
  }
}


template<class Data>
class peano::heap::PlainBoundaryDataExchanger: public peano::heap::BoundaryDataExchanger<Data> {
  private:
    /**
     * Logging device.
     */
    static tarch::logging::Log _log;

    typedef BoundaryDataExchanger<Data> Base;
  protected:
    /**
     * As each message immediately is sent, the internal field
     * _sendTasks.size() holds this value. Each task there equals one sent
     * message.
     */
    virtual int getNumberOfSentMessages() const;

    /**
     * nop
     */
    virtual void postprocessFinishedToSendData();

    /**
     * nop
     */
    virtual void postprocessStartToSendData();

    /**
     * A meta data message came in. Insert into local queue and then trigger
     * receive is necessary.
     */
    virtual void handleAndQueueReceivedTask( const SendReceiveTask<Data>& receivedTask );

    /**
     * Insert send task into local data structure and wrap and send out data
     * if necessary.
     */
    virtual void handleAndQueueSendTask( const SendReceiveTask<Data>& sendTask, const std::vector<Data>& data );

    virtual bool dataExchangerCommunicatesInBackground() const;
  public:
    PlainBoundaryDataExchanger();

    PlainBoundaryDataExchanger(const std::string& identifier, int tag, int rank);

    virtual ~PlainBoundaryDataExchanger();
};


#ifdef Parallel
#include "peano/heap/PlainBoundaryDataExchanger.cpph"
#endif

#endif
