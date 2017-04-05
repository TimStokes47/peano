// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _TARCH_PLOTTER_GRID_DATA_BLOCK_STRUCTURED_PEANO_PATCH_FILE_WRITER_H_
#define _TARCH_PLOTTER_GRID_DATA_BLOCK_STRUCTURED_PEANO_PATCH_FILE_WRITER_H_


#include "tarch/plotter/griddata/Writer.h"


namespace tarch {
  namespace plotter {
    namespace griddata {
      namespace blockstructured {
        class PeanoPatchFileWriter;
      }
    }
  }
}


class tarch::plotter::griddata::blockstructured::PeanoPatchFileWriter: public tarch::plotter::griddata::Writer {
  public:
    class CellDataWriter: public tarch::plotter::griddata::Writer::CellDataWriter {
       public:
         virtual ~CellDataWriter();

         /**
          * Write data for one cell.
          *
          * @param index Index of the cell. This index has to equal the index
          *              used for the cell within the VTKWriter class
          *              interface.
          * @param value Value for the cell.
          */
         void plotCell( int index, double value ) override;
         void plotCell( int index, const tarch::la::Vector<2,double>& value ) override;
         void plotCell( int index, const tarch::la::Vector<3,double>& value ) override;
         void plotCell( int index, double* values, int numberOfValues ) override;

         double getMinValue() const override;
         double getMaxValue() const override;

         void close() override;

         void assignRemainingCellsDefaultValues() override;
     };

     /**
      * A writer for scalar data on points (vertices).
      */
     class VertexDataWriter: public tarch::plotter::griddata::Writer::VertexDataWriter {
       public:
         ~VertexDataWriter();

         /**
          * Write data for one cell.
          *
          * @param index Index of the vertex. This index has to equal the index
          *              used for the cell within the VTKWriter class
          *              interface.
          * @param value Value for the cell.
          */
         void plotVertex( int index, double value ) override;
         void plotVertex( int index, const tarch::la::Vector<2,double>& value ) override;
         void plotVertex( int index, const tarch::la::Vector<3,double>& value ) override;
         void plotVertex( int index, double* values, int numberOfValues ) override;

         double getMinValue() const override;
         double getMaxValue() const override;

         void close() override;

         /**
          * @see close()
          */
         void assignRemainingVerticesDefaultValues() override;
     };

    PeanoPatchFileWriter();

    /**
     * Caller has to destroy this instance manually.
     */
    CellDataWriter*    createCellDataWriter( const std::string& identifier, int recordsPerCell ) override;

    /**
     * Caller has to destroy this instance manually.
     */
    VertexDataWriter*  createVertexDataWriter( const std::string& identifier, int recordsPerVertex ) override;


    /**
     * Add a patch to the output format with a given spec.
     *
     * @return A pair of indices. The first one is the first vertex to be
     *         written and the second one is the index of the first cell
     *         to be written.
     */
    std::pair<int,int> plotPatch(
      const tarch::la::Vector<2,double>& offset,
      const tarch::la::Vector<2,double>& size
    );

    /**
     * @return Write has been successful
     */
    bool writeToFile( const std::string& filename ) override;

    /**
     * @return Whether writer is ready to accept data.
     */
    bool isOpen() override;

    /**
     * Clear the writer, i.e. erase all the data. However, as the writer does
     * not track how many vertex and cell writers you've created, it's up to
     * you to ensure that none of these instances is left.
     */
    void clear() override;
};


#endif
