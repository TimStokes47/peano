// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_DATA_TRAVERSAL_AUTOTUNING_ORACLE_FOR_ONE_PHASE_H_
#define _PEANO_DATA_TRAVERSAL_AUTOTUNING_ORACLE_FOR_ONE_PHASE_H_


#include <map>
#include <string>


namespace peano {
  namespace datatraversal {
    namespace autotuning {
      class OracleForOnePhase;

      enum MethodTrace {
        LoadVertices                                     = 0,
        LoadVerticesOnRegularStationaryGrid              = 1, // not unrolled
        LoadVerticesOnIrregularStationaryGrid            = 2,
        LoadVerticesOnStationaryGridWithParallelBoundary = 3,
        StoreVertices                                    = 4,
        StoreVerticesOnRegularStationaryGrid             = 5, // not unrolled
        StoreVerticesOnIrregularStationaryGrid           = 6,
        SetCounter                                       = 7,
        CallEnterCellAndLoadSubCellsWithinTraverse       = 8,
        CallLeaveCellAndStoreSubCellsWithinTraverse      = 9,
        CallEnterCellAndInitialiseEnumeratorsOnRegularStationaryGrid = 10,
        CallTouchFirstTimeOnRegularStationaryGrid        = 11,
        CallTouchLastTimeOnRegularStationaryGrid         = 12,
        CallEnterCellOnRegularStationaryGrid             = 13,
        CallLeaveCellOnRegularStationaryGrid             = 14,
        PipelineAscendTask                               = 15,
        PipelineDescendTask                              = 16,
        SplitLoadVerticesTaskOnRegularStationaryGrid     = 17,
        SplitStoreVerticesTaskOnRegularStationaryGrid    = 18,
        AscendOnRegularStationaryGrid                    = 19,
        DescendOnRegularStationaryGrid                   = 20,
        NumberOfDifferentMethodsCalling                  = 21
      };

      /**
       * The repository management needs a couple of adapters itself. So, if you
       * speak about adapter no 5 given you by the repository statistics, it is
       * actually the 8th adapter.
       */
      const int NumberOfPredefinedAdapters = 4;

      std::string toString( const MethodTrace& methodTrace );
      MethodTrace toMethodTrace(int value);
    }
  }
}



/**
 * Abstract superclass of all oracles
 *
 * !!! Copy constructor
 *
 * The oracle singleton never works with the original oracle. Instead, it
 * clones its oracle strategy for each new phase (see createNewOracle()).
 *
 * @author Svetlana Nogina, Tobias Weinzierl
 */
class peano::datatraversal::autotuning::OracleForOnePhase {
  public:
    virtual ~OracleForOnePhase() {}

    /**
     * This operation is not const, as the oracle might insert further computations.
     *
     * @see peano::datatraversal::autotuning::Oracle::parallelise()
     *
     * @return Tuple with grain size plus flag indicating weather you wanna be informed about runtime
     */
    virtual std::pair<int,bool> parallelise(int problemSize) = 0;

    /**
     * Informs oracle that the parallel code fraction has terminated. The last
     * fraction is the one, parallelise() has been called for before. There's
     * never more than one section running in parallel.
     */
    virtual void parallelSectionHasTerminated(double elapsedCalendarTime) = 0;

    /**
     * Plot something to info log device.
     */
    virtual void plotStatistics() const = 0;

    /**
     * This operation is called by the oracle (management) on all oracles. Can
     * be used to adopt oracle behaviour to global runtime.
     */
    virtual void informAboutElapsedTimeOfLastTraversal(double elapsedTime) = 0;

    /**
     * Clone this oracle. This operation is used by the singleton whenever a
     * piece of code asks for parallelisation that never asked before.
     *
     * @param adapterNumber Number of your adapter. Have a closer look to your
     *        repository's state if you want to find out which adapters are
     *        mapped to which state. You can even use the toString() operation
     *        there to map this parameter to a string. Sometimes, I use the
     *        term phase as an alias. See NumberOfPredefinedAdapters.
     */
    virtual OracleForOnePhase* createNewOracle(int adapterNumber, const MethodTrace& methodTrace) const = 0;
};


#endif
