// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_HEAP_AGGREGATION_BOUNDARY_DATA_EXCHANGER_H_
#define _PEANO_HEAP_AGGREGATION_BOUNDARY_DATA_EXCHANGER_H_

#include "peano/heap/BoundaryDataExchanger.h"


#include <vector>


namespace peano {
  namespace heap {
    template<class Data, class SendReceiveTaskType, class VectorContainer = std::vector<Data> >
    class AggregationBoundaryDataExchanger;

    namespace tests {
      class AggregationBoundaryDataExchangerTest;
    }
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
 *
 * <h2> Internal data format </h2>
 *
 * The very first entry in the array sent out is the number of (encoded)
 * messages. Per message we then have one entry which describes the number of
 * entries for this message, and then the actual message data.
 */
template<class Data, class SendReceiveTaskType, class VectorContainer>
class peano::heap::AggregationBoundaryDataExchanger: public peano::heap::BoundaryDataExchanger<Data,SendReceiveTaskType,VectorContainer> {
  private:
    /**
     * Logging device.
     */
    static tarch::logging::Log _log;

    typedef BoundaryDataExchanger<Data, SendReceiveTaskType, VectorContainer> Base;

    int _numberOfSentMessages;

    std::vector<Data>    _aggregatedSendData;

    static constexpr int NumberOfCharsToEncodeTotalMessageCount = 4;

    friend class tests::AggregationBoundaryDataExchangerTest;

    void setAggregatedMessageHeader();
    int  getNumberOfMessages( const SendReceiveTaskType& receivedTask );

  protected:
    /**
     * As each message immediately is sent, the internal field
     * _sendTasks.size() holds this value. Each task there equals one sent
     * message.
     */
    virtual int getNumberOfSentMessages() const;

    /**
     * All messages now have gone out, so we have actually aggregated all
     * output data into our local buffer _aggregatedSendData. Now we can
     * wrap this data into a send task, enqueue it locally into the set
     * of send tasks (which always has at most cardinality one with this
     * boundary exchanger) and trigger the MPI commands.
     *
     * The routine sends out data if and only if the send buffer is not
     * empty. That means, the number of elements in the aggregated buffer
     * has to be bigger than NumberOfCharsToEncodeTotalMessageCount.
     * Otherwise, nothing is sent out. The first
     * NumberOfCharsToEncodeTotalMessageCount chars are used to encode the
     * number of messages. For this, we need a little bit of bit shuffling.
     * Originally, I used only the first element of the buffer to store the
     * number of messages. This is a poor idea if we work with chars. There are
     * obviously many situations, where a buffer holds more than 255 messages.
     *
     * To make the whole bit encoding work, we interpret the input data all
     * as bitfields.
     */
    virtual void postprocessFinishedToSendData();

    /**
     * nop
     */
    virtual void postprocessStartToSendData();

    /**
     * A meta data message came in. We receive it into a temporary task message
     * and then decompose the incoming messages into the many messages it did
     * stem from in the first place. While we do so, we have to ensure that all
     * these reconstructed messages are marked to have length 0. Otherwise,
     * receiveDanglingMessages() would try to invoke MPI_Test on them.
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

    #ifdef Asserts
    /**
     * We concatenate all data in one buffer but kick out any validation data
     * such as the level from which the data comes from. So the abstract
     * superclass cannot validate whether data matches to grid entities.
     */
    bool heapStoresVertexSpatialDataForValidation() const override {
      return false;
    }
    #endif

    bool probeMPIQueues() const override {
      return true;
    };
};


#ifdef Parallel
#include "peano/heap/AggregationBoundaryDataExchanger.cpph"
#endif

#endif
