// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _TARCH_MPIANALYIS_ANALYSIS_H_
#define _TARCH_MPIANALYIS_ANALYSIS_H_


#include "tarch/mpianalysis/Analyser.h"


namespace tarch {
  namespace mpianalysis {
    class Analysis;
  }
}



/**
 * Analyser Singleton
 *
 * This type/component results from the Peano project however is of more
 * general nature. It yields performance analysis and load balancing data with
 * respect to mpi. By default, the tarch forwards such information to the
 * logging facilities. However, you can also redirect them to your own types
 * to connect to postprocessing tools, e.g.
 *
 * Analysis is a plain singleton that holds an instance of Analyser. While
 * there is a default Analyser forwarding data to the info log level, you can
 * switch the device yourself.
 *
 * @author Roland Wittmann, Tobias Weinzierl
 */
class tarch::mpianalysis::Analysis: public tarch::mpianalysis::Analyser {
  private:
    Analysis();

    Analyser*  _device;

    void deleteDevice();
  public:
    ~Analysis();

    static Analysis& getInstance();

    /**
     * Set new device. The Analysis object becomes responsible for the pointer,
     * i.e. will delete it automatically at shutdown (or if you set another
     * device again).
     */
    void setDevice( Analyser* device );


    /**
     * =========================================================================
     * Everything below is inherited and forwards the call to the current device.
     * =========================================================================
     */


    /**
     * The grid informs the analyser when it start to traverse. At the end of
     * the traversal, it invokes endIteration(). The time in-between
     * endIteration() and beginIteration() consequently is idle time.
     */
    virtual void beginIteration();

    /**
     * @see beginIteration()
     */
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

    virtual void tagIsUsedFor( int tag, const std::string& communicationTypeIdentifier );

    /**
     * Peano tries to receive all data from neighbours, i.e. boundary data, in
     * the background. One idea is that once the traversal is done, already
     * all/most of the records for the subsequent traversal are already
     * available. If this is not the case, the buffers at least send a
     * notification to the analysis.
     */
    virtual void dataWasNotReceivedInBackground( int fromRank, int tag, int cardinality, int pageSize );

    virtual void dataWasNotReceivedFromWorker( int fromRank, double calendarTime );

    virtual void logNodePoolStatistics(int registeredWorkers, int idleWorkers);
};


#endif
