// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_PERFORMANCE_ANALYIS_DEFAULT_ANALYSER_H_
#define _PEANO_PERFORMANCE_ANALYIS_DEFAULT_ANALYSER_H_


#include "peano/performanceanalysis/Analyser.h"
#include "tarch/logging/Log.h"
#include "tarch/timing/Watch.h"


namespace peano {
  namespace performanceanalysis {
    class DefaultAnalyser;
  }
}



/**
 * Default Analyser
 *
 * This analyser pipes all data to the info output. It is usually used in
 * combination with the Python script, i.e. you run the code with this
 * analyser and afterwards postprocess the output with the script.
 *
 * @author Roland Wittmann, Tobias Weinzierl
 */
class peano::performanceanalysis::DefaultAnalyser: public peano::performanceanalysis::Analyser {
  private:
    static tarch::logging::Log     _log;


    /**
     * Total watch runs from one endIteration() to the next one.
     */
    tarch::timing::Watch           _totalWatch;

    /**
     * This one runs between beginIteration() and endIteration(). Cmp to _totalWatch.
     */
    tarch::timing::Watch           _traversalWatch;
    tarch::timing::Watch           _actualDomainTraversalWatch;
    tarch::timing::Watch           _waitForWorkerDataWatch;
    tarch::timing::Watch           _synchronousHeapWatch;
    tarch::timing::Watch           _asynchronousHeapWatch;
 public:
    DefaultAnalyser();
    virtual ~DefaultAnalyser();

    virtual void beginIteration();

    virtual void endIteration(double numberOfInnerLeafCells, double numberOfOuterLeafCells, double numberOfInnerCells, double numberOfOuterCells, double numberOfLocalCells, double numberOfLocalVertices);

    virtual void enterCentralElementOfEnclosingSpacetree();
    virtual void leaveCentralElementOfEnclosingSpacetree();

    virtual void addWorker(
      int                                 workerRank,
      int                                 level
    );

    virtual void removeWorker(
      int                                 workerRank,
      int                                 level
    );

    virtual void beginToReceiveDataFromWorker();
    virtual void endToReceiveDataFromWorker( int fromRank );

    virtual void dataWasNotReceivedInBackground( int fromRank, int tag, int cardinality, int pageSize );
    virtual void beginToReleaseSynchronousHeapData();
    virtual void endToReleaseSynchronousHeapData();
    virtual void beginToPrepareAsynchronousHeapDataExchange();
    virtual void endToPrepareAsynchronousHeapDataExchange();
    virtual void endReleaseOfJoinData();
    virtual void endReleaseOfBoundaryData();

    virtual void logWorkerMasterCommunication(int worker, bool exchangedData);
};


#endif
