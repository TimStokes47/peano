// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _TARCH_MPIANALYIS_ANALYSER_H_
#define _TARCH_MPIANALYIS_ANALYSER_H_


#include "tarch/la/Vector.h"


namespace tarch {
  namespace mpianalysis {
    class Analyser;
  }
}



/**
 * Analyser Interface
 *
 * @author Roland Wittmann, Tobias Weinzierl
 */
class tarch::mpianalysis::Analyser {
  public:
    virtual ~Analyser() {};

    /**
     * Indicates start of a new global iteration run
     *
     *
     * The grid informs the analyser when it start to traverse. At the end of
     * the traversal, it invokes endIteration(). The time in-between
     * endIteration() and beginIteration() consequently is idle time.
     */
    virtual void beginIteration() = 0;

    /**
     * Counterpart of beginIteration()
     */
    virtual void endIteration() = 0;

    /**
     * Inform analysis device about a worker and its associated domain
     * information. This information is 3d though it might be a 2d
     * simulation only. In this case, the third entry should equals 0
     * due to the overloaded function below.
     */
    virtual void addWorker(
      int                                 workerRank,
      int                                 level,
      const tarch::la::Vector<3,double>&  boundingBoxOffset,
      const tarch::la::Vector<3,double>&  boundingBoxSize
    ) = 0;

    virtual void removeWorker(
      int                                 workerRank,
      int                                 level,
      const tarch::la::Vector<3,double>&  boundingBoxOffset,
      const tarch::la::Vector<3,double>&  boundingBoxSize
    ) = 0;

    virtual void addWorker(
      int                                 workerRank,
      int                                 level,
      const tarch::la::Vector<2,double>&  boundingBoxOffset,
      const tarch::la::Vector<2,double>&  boundingBoxSize
    ) = 0;

    virtual void removeWorker(
      int                                 workerRank,
      int                                 level,
      const tarch::la::Vector<2,double>&  boundingBoxOffset,
      const tarch::la::Vector<2,double>&  boundingBoxSize
    ) = 0;

    /**
     * Inform analyser what an MPI tag is used for.
     */
    virtual void tagIsUsedFor( int tag, const std::string& communicationTypeIdentifier ) = 0;

    /**
     * Inform analyser that there was data that should have been received in
     * background but that wasn't there, when we needed it.
     *
     * Peano tries to receive all data from neighbours, i.e. boundary data, in
     * the background. One idea is that once the traversal is done, already
     * all/most of the records for the subsequent traversal are already
     * available. If this is not the case, the buffers at least send a
     * notification to the analysis.
     *
     * @param fromRank     From which rank was data expected.
     * @param tag          On which tag was the data expected.
     * @param cardinality  How much data (i.e. how many integeres, e.g.) were expected.
     * @param pageSize     In which batches should this data arrive.
     */
    virtual void dataWasNotReceivedInBackground( int fromRank, int tag, int cardinality, int pageSize ) = 0;

    virtual void dataWasNotReceivedFromWorker( int fromRank, double calendarTime ) = 0;

    /**
     * Log information about mpi resources.
     *
     * This operation is invoked by node pools, i.e. by mpi ranks managing
     * resources. They call this operation upon state changes. See
     * NodePool::logStatistics().
     */
    virtual void logNodePoolStatistics(int registeredWorkers, int idleWorkers) = 0;

    virtual void beginToReleaseSynchronousHeapData() = 0;
    virtual void endToReleaseSynchronousHeapData() = 0;
    virtual void beginToPrepareAsynchronousHeapDataExchange() = 0;
    virtual void endToPrepareAsynchronousHeapDataExchange() = 0;
    virtual void endReleaseOfJoinData() = 0;
    virtual void endReleaseOfBoundaryData() = 0;
};


#endif
