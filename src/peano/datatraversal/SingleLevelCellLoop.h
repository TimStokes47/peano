// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_DATA_TRAVERSAL_SINGLE_LEVEL_CELL_LOOP_H_
#define _PEANO_DATA_TRAVERSAL_SINGLE_LEVEL_CELL_LOOP_H_


#include "peano/datatraversal/ActionSet.h"
#include "peano/datatraversal/ActionSetTraversal.h"


namespace peano {
    namespace datatraversal {
      template <class LoopBody>
      class SingleLevelCellLoop;
    }
}


/**
 * Loop over set of cells on a single level
 *
 * Realises either a @f$ 2^d @f$ red-black Gauss-Seidel-type traversal of the
 * Cartesian grid or a plain parallel loop as well as a serial version.
 *
 * @author Tobias Weinzierl
 */

template <class LoopBody>
class peano::datatraversal::SingleLevelCellLoop {
  private:
    void runSequentially(
      LoopBody&                                 loopBody,
      const tarch::la::Vector<DIMENSIONS,int>&  range
    );

    void runParallelWithColouring(
      LoopBody&                                 loopBody,
      const tarch::la::Vector<DIMENSIONS,int>&  range,
      int                                       grainSize,
      bool                                      useSixPowerDColouring
    );

    void runParallelWithoutColouring(
      LoopBody&                                 loopBody,
      const tarch::la::Vector<DIMENSIONS,int>&  range,
      int                                       grainSize
    );

    class dForLoopInstance {
       private:
         LoopBody                           _loopBody;
         tarch::la::Vector<DIMENSIONS,int>  _offset;
         /**
          * Should be set to 2 for @f$ 2^d @f$ colouring.
          */
         const int                                _padding;

       public:
         dForLoopInstance( const LoopBody& loopBody, const tarch::la::Vector<DIMENSIONS,int>& offset, int padding );

         #if defined(SharedTBB)
         typedef tbb::split   SplitFlag;
         #else
         typedef int          SplitFlag;
         #endif

         /**
          * Copy Constructor
          *
          * TBB requires the copy constructor for loops to accept an additional
          * argument to be able to distinguish it from the standard copy
          * constructor. As a consequence, the code does not compile anymore
          * without tbb. I thus wrap the TBB type with a typedef of my own.
          */
         dForLoopInstance( const dForLoopInstance& loopBody, SplitFlag );

         /**
          * Process range
          *
          * Could, at first glance, be const as the method copies the loop body anyway. The
          * operation first copies the loop body. This process can run
          * in parallel, as the copy process may not modify the original
          * loop body instance. When the operation has terminated, it calls the
          * loop body copy's updateGlobalValues(). Usually, the copy holds a
          * reference to the original data. A reference not used until this
          * final operation is called. The final operation then commits changes
          * to the original data set. This operation hence is not const.
          * Consequently, the whole operator may not be const.
          */
         void operator() (const dForRange& range);

         /**
          * Empty Join
          *
          * Instead, we use the destructor. See remarks on TBB realisation for
          * encapsulating class. If you do not use TBB's parallel_reduce, we can
          * throw away this operation.
          */
         void join(const dForLoopInstance&  with);

         void setOffset(const tarch::la::Vector<DIMENSIONS,int>&  offset);
     };

  public:
    enum ParallelisationStrategy {
      Serial,
      NoColouring,
      TwoPowerDColouring,
      SixPowerDColouring
    };

    static std::string toString(ParallelisationStrategy strategy);

    /**
     * Run in Red-black Manner Through All Cells of One Level
     *
     * Please note that the actual problem size of a single level cell
     * traversal is only @f$ 2^{-d} @f$ of the whole problem size as we are 
     * using red-black colouring.
     */
    inline SingleLevelCellLoop(
      const tarch::la::Vector<DIMENSIONS,int>&  range,
      LoopBody&                                 loopBody,
      int                                       grainSize,
      ParallelisationStrategy                   parallelisationStrategy
    );

    /**
     * Standard constructor
     */
    SingleLevelCellLoop();
};



#include "peano/datatraversal/SingleLevelCellLoop.cpph"

#endif
