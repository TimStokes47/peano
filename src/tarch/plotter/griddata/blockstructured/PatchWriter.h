// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _TARCH_PLOTTER_GRID_DATA_BLOCK_STRUCTURED_PATCH_WRITER_H_
#define _TARCH_PLOTTER_GRID_DATA_BLOCK_STRUCTURED_PATCH_WRITER_H_

#include "tarch/plotter/griddata/Writer.h"
#include "tarch/multicore/BooleanSemaphore.h"

#include "tarch/multicore/BooleanSemaphore.h"


namespace tarch {
  namespace plotter {
    namespace griddata {
      namespace blockstructured {
        class PatchWriter;
      }
    }
  }
}


/**
 * Abstract base class for a patch plotter
 *
 * Patch plotters are either wrappers around existing plotters or real
 * plotters. This is their abstract interface. They are different to
 * the
 *
 * @author Kristof Unterweger, Tobias Weinzierl
 */
class tarch::plotter::griddata::blockstructured::PatchWriter:
  public tarch::plotter::griddata::Writer {
  public:
    /**
     * Writes patches
     */
    class SinglePatchWriter {
      private:
        static tarch::multicore::BooleanSemaphore _semaphore;

      public:
        virtual ~SinglePatchWriter() {}

        /**
         * @param offset
         * @param size
         * @param cells
         *
         * @return Pair of indices. The first index is the index of very first
         *         vertex in the patch, the second index is the index of the
         *         very first cell.
         */
        virtual std::pair<int,int> plotPatch(
          const tarch::la::Vector<2,double>& offset,
          const tarch::la::Vector<2,double>& size,
          const tarch::la::Vector<2,int>&    cells
        ) = 0;

        virtual std::pair<int,int> plotPatch(
          const tarch::la::Vector<3,double>& offset,
          const tarch::la::Vector<3,double>& size,
          const tarch::la::Vector<3,int>&    cells
        ) = 0;

        /**
         * Write data
         *
         * This operation works if and only if the passed data are the only
         * unknowns plotted for this grid. If you wanna visualise several
         * unknowns, you have to use a different plot wrapper or visualise
         * your data manually.
         *
         * Simple wrapper around plotPatch that takes a data writer as well
         * as a data array and pipes the data directly into the patch.
         *
         * This operation is thread-safe.
         *
         * Padding describes the distance between two entries along each
         * coordinate axis in bytes. An example: You have floats on the heap
         * with 12x12 patches that are surrounded by a ghost layer of size
         * two. If you wanna plot only the interior, the padding looks as
         * follows:
         * \code
    tarch::la::Vector<2,int> padding;
    padding(0) = sizeof(DataHeap::HeapData);
    padding(1) = sizeof(DataHeap::HeapData)*12;
           \endcode
         * and you hand in the pointer of the 14th element of the patch.
         *
         * @param cellValues  Pointer to first data value to be plotted.
         */
        template <typename T>
        void plotPatch(
          const tarch::la::Vector<2,double>& offset,
          const tarch::la::Vector<2,double>& size,
          const tarch::la::Vector<2,int>&    cells,
          T const * const                    cellValues,
          const tarch::la::Vector<2,int>&    paddingInBytes,
          CellDataWriter&                    writer
        );

        template <typename T>
        void plotPatch(
          const tarch::la::Vector<2,double>& offset,
          const tarch::la::Vector<2,double>& size,
          const tarch::la::Vector<2,int>&    cells,
          T const * const                    cellValuesA,
          T const * const                    cellValuesB,
          T const * const                    cellValuesC,
          const tarch::la::Vector<2,int>&    paddingInBytes,
          CellDataWriter&                    writerA,
          CellDataWriter&                    writerB,
          CellDataWriter&                    writerC
        );

        /**
         * Pleaes close the patch writer before you close any data writer.
         */
        virtual void close() = 0;
    };

    /**
     * Caller has to destroy this instance manually. Do not create more than
     * one vertex writer.
     */
    virtual SinglePatchWriter*   createSinglePatchWriter() = 0;

    virtual ~PatchWriter();
};


#include "tarch/plotter/griddata/blockstructured/PatchWriter.cpph"


#endif
