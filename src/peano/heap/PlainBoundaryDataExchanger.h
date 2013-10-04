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
    virtual int getNumberOfSentMessages() const;

    virtual void postprocessFinishedToSendData();

    virtual void postprocessStartToSendData();

    virtual void handleAndQueueReceivedTask( const SendReceiveTask<Data>& receivedTask );

    virtual void handleAndQueueSendTask( const SendReceiveTask<Data>& sendTask, const std::vector<Data>& data );
  public:
    PlainBoundaryDataExchanger();

    PlainBoundaryDataExchanger(const std::string& identifier, int tag, int rank);

    virtual ~PlainBoundaryDataExchanger();
};


#ifdef Parallel
#include "peano/heap/PlainBoundaryDataExchanger.cpph"
#endif

#endif
