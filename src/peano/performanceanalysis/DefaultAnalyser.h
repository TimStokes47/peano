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
 * Analyser Interface
 *
 * @author Roland Wittmann, Tobias Weinzierl
 */
class peano::performanceanalysis::DefaultAnalyser: public peano::performanceanalysis::Analyser {
  private:
    static tarch::logging::Log     _log;

    tarch::timing::Watch           _traversalWatch;
    tarch::timing::Watch           _actualDomainTraversalWatch;
/*
    tarch::timing::Watch           _synchronousHeapWatch;
    tarch::timing::Watch           _asynchronousHeapWatch;
*/
 public:
    DefaultAnalyser();
    virtual ~DefaultAnalyser();

    virtual void beginIteration();

    virtual void endIteration();

    virtual void addWorker(
      int                                 workerRank,
      int                                 level,
      const tarch::la::Vector<3,double>&  boundingBoxOffset,
      const tarch::la::Vector<3,double>&  boundingBoxSize
    );

    virtual void removeWorker(
      int                                 workerRank,
      int                                 level,
      const tarch::la::Vector<3,double>&  boundingBoxOffset,
      const tarch::la::Vector<3,double>&  boundingBoxSize
    );

    virtual void addWorker(
      int                                 workerRank,
      int                                 level,
      const tarch::la::Vector<2,double>&  boundingBoxOffset,
      const tarch::la::Vector<2,double>&  boundingBoxSize
    );

    virtual void removeWorker(
      int                                 workerRank,
      int                                 level,
      const tarch::la::Vector<2,double>&  boundingBoxOffset,
      const tarch::la::Vector<2,double>&  boundingBoxSize
    );

    virtual void dataWasNotReceivedInBackground( int fromRank, int tag, int cardinality, int pageSize );
    virtual void dataWasNotReceivedFromWorker( int fromRank, double calendarTime );
    virtual void beginToReleaseSynchronousHeapData();
    virtual void endToReleaseSynchronousHeapData();
    virtual void beginToPrepareAsynchronousHeapDataExchange();
    virtual void endToPrepareAsynchronousHeapDataExchange();
    virtual void endReleaseOfJoinData();
    virtual void endReleaseOfBoundaryData();
    virtual void enterCentralElementOfEnclosingSpacetree();
    virtual void leaveCentralElementOfEnclosingSpacetree();
};


#endif
