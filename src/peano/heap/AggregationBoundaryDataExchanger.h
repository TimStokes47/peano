// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_HEAP_AGGREGATION_BOUNDARY_DATA_EXCHANGER_H_
#define _PEANO_HEAP_AGGREGATION_BOUNDARY_DATA_EXCHANGER_H_

#include "peano/heap/BoundaryDataExchanger.h"


namespace peano {
  namespace heap {
    template<class Data, bool CreateCopiesOfSentData, class SendReceiveTaskType, class VectorContainer = std::vector<Data> >
    class AggregationBoundaryDataExchanger;
  }
}


/**
 * The aggregation boundary data exchanger is usually only used for integers
 * and chars. It does not send out data step by step but collects all data in
 * one big buffer and sends it out at the end of the sweep. This way, we
 * eliminate the option for Peano to transfer data in the background. However,
 * we also eliminate any overhead created by lots of small messages.
 *
 * As we rely a lot on quite manual data modifications, this particular data
 * exchanger works only with the specialised heaps such as the CharHeap and the
 * DoubleHeap.
 */
template<class Data, bool CreateCopiesOfSentData, class SendReceiveTaskType, class VectorContainer>
class peano::heap::AggregationBoundaryDataExchanger: public peano::heap::BoundaryDataExchanger<Data,SendReceiveTaskType,VectorContainer> {
  private:
    /**
     * Logging device.
     */
    static tarch::logging::Log _log;

    typedef BoundaryDataExchanger<Data, SendReceiveTaskType, VectorContainer> Base;

    int _numberOfSentMessages;

    std::vector<Data>    _aggregatedSendData;
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
     *
     * @see BoundaryDataExchanger::handleAndQueueReceivedTask()
     */
    virtual void handleAndQueueReceivedTask( const SendReceiveTaskType& receivedTask );

    /**
     * We look whether there is already a
     */
    virtual void handleAndQueueSendTask( const SendReceiveTaskType& sendTask, const Data* const data );

    virtual bool dataExchangerCommunicatesInBackground() const;
  public:
    AggregationBoundaryDataExchanger();

    AggregationBoundaryDataExchanger(const std::string& identifier, int metaDataTag, int dataTag, int rank);

    virtual ~AggregationBoundaryDataExchanger();
};


#ifdef Parallel
#include "peano/heap/AggregationBoundaryDataExchanger.cpph"
#endif

#endif
