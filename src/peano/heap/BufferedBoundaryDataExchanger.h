// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_HEAP_BUFFERED_BOUNDARY_DATA_EXCHANGER_H_
#define _PEANO_HEAP_BUFFERED_BOUNDARY_DATA_EXCHANGER_H_

#include "peano/heap/BoundaryDataExchanger.h"


namespace peano {
  namespace heap {
    template<class Data>
    class BufferedBoundaryDataExchanger;
  }
}



/**
 * Buffered Boundary Data Exchanger
 *
 * Very simple boundary exchanger that locally buffers all data before it then
 * sends away all the data in one big block at the end of the iteration.
 * Circumnavigates all overheads with asynchronous mpi and network latency.
 *
 * !!! Implementation
 *
 * We use three instead of two messages.
 *
 * - The buffer sends away a meta data message that does holds the total number
 *   of sends.
 * - This message is followed by an integer array holding the sizes of the
 *   individual messages. This is the first part of buffered.
 * - One long message follows that holds all the actual data.
 *
 * @author Tobias Weinzierl, Roland Wittmann
 */
template<class Data>
class peano::heap::BufferedBoundaryDataExchanger: public peano::heap::BoundaryDataExchanger<Data> {
  private:
    /**
     * Logging device.
     */
    static tarch::logging::Log _log;

    /**
     * Holds the size of the individual messages. The very last entry is the
     * total sum of Data entries sent.
     */
    std::vector<int>    _sizeOfMessages;


    #if defined(Parallel) && defined(ParallelExchangePackedRecordsInHeaps)
    typedef typename Data::Packed  MPIData;
    #else
    typedef Data    MPIData;
    #endif

    std::vector<MPIData>                    _concatenatedSentData;

    typedef BoundaryDataExchanger<Data> Base;
  protected:
    /**
     * Number of sent messages
     *
     * This operation is basically the size of _sizeOfMessages. No messages
     * have been sent jet if that one is not released, but if we would send,
     * the one message going out basically comprises _sizeOfMessages.size()
     * different messages.
     */
    virtual int getNumberOfSentMessages() const;

    /**
     * @todo raus
     */
    virtual void postprocessFinishedToSendData();

    /**
     * nop
     */
    virtual void postprocessStartToSendData();

    virtual void handleAndQueueReceivedTask( const SendReceiveTask<Data>& receivedTask );

    virtual void handleAndQueueSendTask( const SendReceiveTask<Data>& sendTask, const std::vector<Data>& data );
  public:
    BufferedBoundaryDataExchanger();

    BufferedBoundaryDataExchanger(const std::string& identifier, int tag, int rank);

    /**
     * Destructor
     *
     * !!! Assertions
     *
     * Logically, one could add a statement alike
     *
     * assertion4( _sizeOfMessages.empty(), ...
     *
     * However, the boundary exchangers are held by a map, and the C++ maps
     * tend to copy around things. It hence might happen that a full buffer
     * is copied to a different location and the original instance then is
     * destroyed. And this initial instance still thinks that it has data.
     * I could write a copy constructor to make this clean and safe (a copy
     * constructor erasing the data in the preimage of the copy), but I'm too
     * lazy to do so right now.
     */
    virtual ~BufferedBoundaryDataExchanger();
};


#ifdef Parallel
#include "peano/heap/BufferedBoundaryDataExchanger.cpph"
#endif

#endif
