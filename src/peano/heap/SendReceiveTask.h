// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_HEAP_SEND_RECEIVE_TASK_H_
#define _PEANO_HEAP_SEND_RECEIVE_TASK_H_


#include "peano/heap/records/MetaInformation.h"


#ifdef Parallel
#include <mpi.h>
#endif


namespace peano {
  namespace heap {
    template<class Data>
    struct SendReceiveTask;
  }
}



/**
 * Wrapper for a send or receive task
 *
 * Holds the information for a send or receive task, i.e. it holds the MPI
 * request to check weather the task has been finished (both sends and
 * receives are realised as non-blocking MPI data exchange) as well as a
 * pointer to the allocated data where the copy of the data is
 * stored (send task) or is to be stored (receive task).
 */
template<class Data>
struct peano::heap::SendReceiveTask {
  static tarch::logging::Log _log;


  /**
   * We always use the plain meta information as record, i.e. we do not pack
   * anything here as the meta information usually is one integer only
   * anyway.
   */
  typedef peano::heap::records::MetaInformation          MetaInformation;

  #ifdef Parallel
  MPI_Request     _request;
  #endif

  MetaInformation _metaInformation;

  /**
   * Without semantics for send tasks but important for receive tasks as we
   * have to store from which rank the data arrived from.
   */
  int             _rank;

  #if defined(Parallel) && defined(ParallelExchangePackedRecordsInHeaps)
  typedef typename Data::Packed  MPIData;
  #else
  typedef Data    MPIData;
  #endif

  /**
   * Pointer to the actual data. If meta data marks a message without
   * content, this pointer is 0.
   */
  MPIData*        _data;

  #ifdef Asserts
  SendReceiveTask();
  #endif

  /**
   * Please note that you have to call delete[] on _data afterwards.
   */
  void wrapData(const std::vector<Data>& data);

  /**
   * @see triggerReceive() for implementation remarks.
   */
  void triggerSend(int tag);

  /**
   * If you use the task in combination with containers, please push or pop
   * data first and then call trigger. Trigger uses MPI and MPI uses memory
   * references. If you move the tasks around later on, MPI references invalid
   * memory addresses and data consistency is not given anymore.
   */
  void triggerReceive(int tag);

  std::vector<Data> unwrapDataAndFreeMemory();

  void setInvalid();

  /**
   * A task fits if it is
   *
   * - either invalid (see setInvalid())
   * - or position and level coincide.
   *
   * Fits should only be called in assert mode. However, some compiler seem to
   * translate it also if the function is not used at all. For them, I provide
   * a non-asserts version returning true all the time.
   */
  bool fits(
    const tarch::la::Vector<DIMENSIONS, double>&  position,
    int                                           level
  ) const;
};


#ifdef Parallel
#include "peano/heap/SendReceiveTask.cpph"
#endif


#endif
