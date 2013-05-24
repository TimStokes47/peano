// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_DATA_TRAVERSAL_AUTOTUNING_ORACLE_FOR_ONE_PHASE_DUMMY_H_
#define _PEANO_DATA_TRAVERSAL_AUTOTUNING_ORACLE_FOR_ONE_PHASE_DUMMY_H_


#include "tarch/logging/Log.h"
#include "peano/datatraversal/autotuning/OracleForOnePhase.h"
#include "tarch/timing/Measurement.h"
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
    const bool                                 _measureAlsoSerialProgramParts;
    std::map<int, tarch::timing::Measurement>  _executionTime;
    const MethodTrace                          _methodTrace;
    const int                                  _splitTheTree;
    const bool                                 _pipelineDescendProcessing;
    const bool                                 _pipelineAscendProcessing;

    int                                        _smallestGrainSize;

    int                                        _lastProblemSize;

    const int                                  _smallestGrainSize1DForCellEvents;
    const int                                  _smallestGrainSize1DForVertexEvents;
    const int                                  _smallestGrainSizeForLoadStoreSplits;
  public:
    /**
     * Dummy oracle
     *
     * @param splitTheTree (0=no, 1=yes and parallelise, 2=yes, but do not parallelise any events on the regular subgrid
     * @param smallestGrainSize1DForCellEvents    Magic number. (Sub-)Grids smaller than this size (in 1d) should not be parallelised.
     * @param smallestGrainSize1DForVertexEvents  Magic number.
     * @param smallestGrainSizeForLoadStoreSplits Magic number.
     *
     */
    OracleForOnePhaseDummy(
      bool useMultithreading             = true,
      bool measureAlsoSerialProgramParts = false,
      int  splitTheTree                  = 1,
      bool pipelineDescendProcessing     = false,
      bool pipelineAscendProcessing      = false,
      int  smallestGrainSize1DForCellEvents    = 12,
      int  smallestGrainSize1DForVertexEvents  = 3*3*3 + 1,
      int  smallestGrainSizeForLoadStoreSplits = THREE_POWER_D,
      const MethodTrace& methodTrace     = NumberOfDifferentMethodsCalling
    );

    virtual ~OracleForOnePhaseDummy();

    virtual std::pair<int,bool> parallelise(int problemSize);
    virtual void parallelSectionHasTerminated(double elapsedCalendarTime);
    virtual void plotStatistics() const;

    virtual void informAboutElapsedTimeOfLastTraversal(double elapsedTime);

    /**
     * For this oracle type, the adapter number is completely irrelevant.
     */
    virtual OracleForOnePhase* createNewOracle(int adapterNumber, const MethodTrace& methodTrace) const;

    std::string toString() const;
};


#endif
