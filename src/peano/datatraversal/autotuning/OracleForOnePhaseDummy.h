// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_DATA_TRAVERSAL_AUTOTUNING_ORACLE_FOR_ONE_PHASE_DUMMY_H_
#define _PEANO_DATA_TRAVERSAL_AUTOTUNING_ORACLE_FOR_ONE_PHASE_DUMMY_H_


#include "tarch/logging/Log.h"
#include "peano/datatraversal/autotuning/OracleForOnePhase.h"
#include "tarch/timing/Measurement.h"
#include "tarch/la/ScalarOperations.h"
#include "peano/utils/Globals.h"


#include <map>


namespace peano {
  namespace datatraversal {
    namespace autotuning {
      class OracleForOnePhaseDummy;
    }
  }
}


/**
 * @author Tobias Weinzierl
 */
class peano::datatraversal::autotuning::OracleForOnePhaseDummy: public peano::datatraversal::autotuning::OracleForOnePhase {
  private:
    static tarch::logging::Log                 _log;

    const bool                                 _useMulticore;
    const bool                                 _measureRuntimes;
    std::map<int, tarch::timing::Measurement>  _executionTime;
    const int                                  _adapterNumber;
    const MethodTrace                          _methodTrace;
    const int                                  _splitTheTree;
    const bool                                 _pipelineDescendProcessing;
    const bool                                 _pipelineAscendProcessing;

    int                                        _grainSize;
    int                                        _smallestProblemSize;

    int                                        _lastProblemSize;

    const int                                  _smallestGrainSizeForAscendDescend;
    const int                                  _grainSizeForAscendDescend;

    const int                                  _smallestGrainSizeForEnterLeaveCell;
    const int                                  _grainSizeForEnterLeaveCell;

    const int                                  _smallestGrainSizeForTouchFirstLast;
    const int                                  _grainSizeForTouchFirstLast;

    const int                                  _smallestGrainSizeForSplitLoadStore;
    const int                                  _grainSizeForSplitLoadStore;
  public:
    /**
     * Dummy oracle
     *
     * The magic numbers in the default arguments are taken from the following
     * table:
     *
     *
|| dim || Phase/trace                        ||    p=2   ||    p=4    ||   p=6   ||    p=8    ||   p=16    || choice
|| 2   || ascend-regular-stationary          | 1640/4    |  1640/8,12 | 1640/12  | 1640/64    | 1640/3     || 1640/3
||     || call-enter-cell-regular-stationary |   20/1-8  |    20/2    |   20/2   |   20/2     |   20/2-6   ||   20/2
||     || touch-first-regular-stationary     | 6724/2-8  |  6724/64   | 6724/18  | 6724/12    | 6724/2-7   || 6724/64
||     || touch-last-regular-stationary      | 6724/2,8  |  6724/8    | 6724/12  | 6724/10    | 6724/3,4,5 || 6724/64
||     || split-load                         |  900/2,8  |   900/3,8  |  900/2,4 |  900/40,2  |  900/8,24  ||  900/8
||     || split-store                        |  900/~128 |   900/128  |  900/18  |  900/18    |  900/18    ||  900/8
     *
     *
     * @param splitTheTree (0=no, 1=yes and parallelise, 2=yes, but do not parallelise any events on the regular subgrid
     */
    OracleForOnePhaseDummy(
      bool useMultithreading                  = true,
      bool measureRuntimes                    = false,
      int  splitTheTree                       = 1,
      bool pipelineDescendProcessing          = false,
      bool pipelineAscendProcessing           = false,
      int  smallestGrainSizeForAscendDescend  = tarch::la::aPowI(DIMENSIONS,3*3*3*3/2),
      int  grainSizeForAscendDescend          = 3,
      int  smallestGrainSizeForEnterLeaveCell = tarch::la::aPowI(DIMENSIONS,9/2),
      int  grainSizeForEnterLeaveCell         = 2,
      int  smallestGrainSizeForTouchFirstLast = tarch::la::aPowI(DIMENSIONS,3*3*3*3+1),
      int  grainSizeForTouchFirstLast         = 64,
      int  smallestGrainSizeForSplitLoadStore = tarch::la::aPowI(DIMENSIONS,3*3*3),
      int  grainSizeForSplitLoadStore         = 8,
      int  adapterNumber                      = -1,
      const MethodTrace& methodTrace          = NumberOfDifferentMethodsCalling
    );

    virtual ~OracleForOnePhaseDummy();

    virtual std::pair<int,bool> parallelise(int problemSize);
    virtual void parallelSectionHasTerminated();
    virtual void plotStatistics() const;

    virtual void informAboutElapsedTimeOfLastTraversal(double elapsedTime);

    /**
     * For this oracle type, the adapter number is completely irrelevant.
     */
    virtual OracleForOnePhase* createNewOracle(int adapterNumber, const MethodTrace& methodTrace) const;

    std::string toString() const;
};


#endif
