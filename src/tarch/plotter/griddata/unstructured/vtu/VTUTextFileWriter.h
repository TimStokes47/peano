//// This file is part of the Peano project. For conditions of distribution and
//// use, please see the copyright notice at www.peano-framework.org
//#ifndef _TARCH_PLOTTER_GRIDDATA_UNSTRUCTURED_VTU_VTUTEXTFILEWRITER_H_
//#define _TARCH_PLOTTER_GRIDDATA_UNSTRUCTURED_VTU_VTUTEXTFILEWRITER_H_
//
//
//#include "tarch/logging/Log.h"
//#include "tarch/plotter/griddata/unstructured/UnstructuredGridWriter.h"
//
//
//namespace tarch {
//  namespace plotter {
//    namespace griddata {
//      namespace unstructured {
//        namespace vtu {
//          class VTUTextFileWriter;
//        }
//      }
//    }
//  }
//}
//
//
///**
// * VTU Writer
// *
// * Output for vtu files (paraview) as text files. For usage notes, please
// * consult superclass.
// *
// * @author Robert Guder, Michael Lieb
// */
//class tarch::plotter::griddata::unstructured::vtu::VTUTextFileWriter:
//  public tarch::plotter::griddata::unstructured::UnstructuredGridWriter {
//  private:
//    /**
//     * Logging device.
//     */
//    static tarch::logging::Log _log;
//
//    static const std::string HEADER;
//
//    bool _writtenToFile;
//
//    /**
//     * precision
//     */
//    const int _precision;
//
//    /**
//     * either "float" or "double" depending on _precision
//     */
//    const std::string _doubleOrFloat;
//
//    /**
//     * Total number of vertices
//     */
//    int _numberOfVertices;
//
//    /**
//     * Total number of cells
//     */
//    int _numberOfCells;
//
//    /**
//     * Total number of cell entries. See _cellListEntries.
//     */
//    int _numberOfCellEntries;
//
//
//    std::string _vertexDescription;
//    std::string _cellTypeDescription;
//    std::string _vertexDataDescription;
//    std::string _cellDataDescription;
//
//    /**
//     * We need/use this attribute only in parallel mode.
//     */
//    std::string _vertexDataDescriptionParallel;
//
//    /**
//     * We need/use this attribute only in parallel mode.
//     */
//    std::string _cellDataDescriptionParallel;
//
//    std::string _offsets;
//    std::string _connectivity;
//
//    void validateDataWriterIdentifier( const std::string& identifier ) const;
//
//
//    static std::string setDoubleOrFloatString(const int precision);
//
//  public:
//    VTUTextFileWriter(int precision=6 /*, int numberOfParallelRanksWriting = 1 */ );
//    virtual ~VTUTextFileWriter();
//
//    virtual bool isOpen();
//
//    /**
//     * Stream the whole thing out to a file given the name filename.
//     * Basically, this operation is similar to startToWriteNewTimeStep() but it
//     * does not reopen any streams. Afterwards, the operation writes a meta file
//     * (now filename really is the filename) that references to all the other
//     * written snapshots.
//     */
//    virtual void writeToFile( const std::string& filename );
//
//    void startToWriteNewTimeStep();
//
//    virtual void clear();
//
//    virtual VertexWriter*      createVertexWriter();
//    virtual CellWriter*        createCellWriter();
//    virtual CellDataWriter*    createCellDataWriter( const std::string& identifier, int recordsPerCell );
//    virtual VertexDataWriter*  createVertexDataWriter( const std::string& identifier, int recordsPerVertex );
//
//    /**
//     * This is the vertex writer you have to create to plot the vertices.
//     * Besides the pure syntax management, the writer also provides a number
//     * generator which provides you with a unique id for each vertex.
//     *
//     * Please ensure that you call close() on the vertex writer before you
//     * close the underlying VTUTextFileWriter.
//     */
//    class VertexWriter:
//  public tarch::plotter::griddata::unstructured::UnstructuredGridWriter::VertexWriter {
//  private:
//    /**
//     * The father class is a friend. There are no other friends.
//     */
//    friend class VTUTextFileWriter;
//
//    /**
//     * Counter for the vertices written. Holds the maximum index.
//     */
//    int _currentVertexNumber;
//
//    /**
//     * Underlying VTU writer.
//     */
//    VTUTextFileWriter& _myWriter;
//
//    /**
//     * Output stream
//     */
//    std::ostringstream _out;
//
//    VertexWriter(VTUTextFileWriter& writer);
//  public:
//    virtual ~VertexWriter();
//
//    virtual int plotVertex(const tarch::la::Vector<2,double>& position);
//    virtual int plotVertex(const tarch::la::Vector<3,double>& position);
//
//    virtual void close();
//  };
//
//  /**
//   * Writes the element data.
//   */
//  class CellWriter:
//  public tarch::plotter::griddata::unstructured::UnstructuredGridWriter::CellWriter {
//  private:
//    /**
//     * The father class is a friend. There are no other friends.
//     */
//    friend class VTUTextFileWriter;
//
//    /**
//     * Counter for the elements written. Holds the maximum index.
//     */
//    int _currentCellNumber;
//
//    /**
//     * Underlying VTU writer.
//     */
//    VTUTextFileWriter& _myWriter;
//
//    /**
//     * The tag CELLS in a vtu file requires the number of total entries in the
//     * following list of cell-interconnection (for a triangle, an entry could
//     * look like this: "3 1 2 4", which states that the triangle has 3 links to
//     * vetrices with indices 1, 2 and 4. this makes up four entries). For an
//     * unstructured mesh, the element type is not fixed and, hence, the total
//     * amount of list entries must be counted by summing up the contributions
//     * of each element, when adding the element.
//     */
//    int _cellListEntries;
//
//
//    int _offset;
//
//    /**
//     * Output stream
//     */
//    std::ostringstream _cellConnectivityOut;
//
//    /**
//     * Output stream
//     */
//    std::ostringstream _cellOffsetsOut;
//
//
//
//    /**
//     * Output stream
//     */
//    std::ostringstream _cellTypeOut;
//
//    CellWriter(VTUTextFileWriter& writer);
//  public:
//    virtual ~CellWriter();
//
//    virtual int plotHexahedron(int vertexIndex[8]);
//
//    virtual int plotQuadrangle(int vertexIndex[4]);
//
//    virtual int plotLine(int vertexIndex[2]);
//
//    virtual int plotTriangle(int vertexIndex[3]);
//
//    virtual int plotPoint(int vertexIndex);
//
//    virtual void close();
//  };
//
//  class CellDataWriter:
//  public tarch::plotter::griddata::Writer::CellDataWriter {
//  private:
//    /**
//     * The father class is a friend. There are no other friends.
//     */
//    friend class VTUTextFileWriter;
//
//    /**
//     *
//     */
//    int _lastWriteCommandCellNumber;
//
//    /**
//     * Underlying VTU writer.
//     */
//    VTUTextFileWriter& _myWriter;
//
//    /**
//     * Output stream
//     */
//    std::ostringstream _out;
//  #ifdef Parallel
//    /**
//     * Output stream
//     */
//    std::ostringstream _outPVTU;
//
//  #endif
//    int _recordsPerCell;
//
//    double _minValue;
//    double _maxValue;
//    CellDataWriter(const std::string& dataIdentifier, VTUTextFileWriter& writer, int recordsPerCell);
//  public:
//    virtual ~CellDataWriter();
//
//    virtual void close();
//
//    virtual void plotCell( int index, double value );
//    virtual void plotCell( int index, const tarch::la::Vector<2,double>& value );
//    virtual void plotCell( int index, const tarch::la::Vector<3,double>& value );
//
//    virtual double getMinValue() const;
//    virtual double getMaxValue() const;
//  };
//
//  class VertexDataWriter:
//  public tarch::plotter::griddata::Writer::VertexDataWriter {
//  private:
//    /**
//     * The father class is a friend. There are no other friends.
//     */
//    friend class VTUTextFileWriter;
//
//    /**
//     *
//     */
//    int _lastWriteCommandVertexNumber;
//
//    /**
//     * Underlying VTU writer.
//     */
//    VTUTextFileWriter& _myWriter;
//
//    /**
//     * Output stream
//     */
//    std::ostringstream _out;
//
//    #ifdef Parallel
//    /**
//     * Output stream
//     */
//    std::ostringstream _outPVTU;
//    #endif
//
//    int _recordsPerVertex;
//
//    double _minValue;
//    double _maxValue;
//
//    VertexDataWriter(const std::string& dataIdentifier, VTUTextFileWriter& writer, int recordsPerVertex);
//
//    #ifdef Asserts
//    /**
//     * There is no reason to store a data identifier. However, we augment
//     * the assertions with the identifier to make it a little bit easier to
//     * find data inconsistencies.
//     */
//    std::string  _dataIdentifier;
//    #endif
//  public:
//    virtual ~VertexDataWriter();
//
//    virtual void close();
//
//    virtual void plotVertex( int index, double value );
//    virtual void plotVertex( int index, const tarch::la::Vector<2,double>& value );
//    virtual void plotVertex( int index, const tarch::la::Vector<3,double>& value );
//
//    virtual double getMinValue() const;
//    virtual double getMaxValue() const;
//  };
//};
//
//#endif
