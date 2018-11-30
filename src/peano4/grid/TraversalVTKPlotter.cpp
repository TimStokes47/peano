#include "TraversalVTKPlotter.h"

#include "peano4/utils/Loop.h"


tarch::logging::Log  peano4::grid::TraversalVTKPlotter::_log( "peano4::grid::TraversalVTKPlotter" );


peano4::grid::TraversalVTKPlotter::TraversalVTKPlotter( const std::string& filename ):
  _filename(filename),
  _counter(0),
  _writer(nullptr),
  _vertexWriter(nullptr),
  _cellWriter(nullptr),
  _timeSeriesWriter() {
}


void peano4::grid::TraversalVTKPlotter::beginTraversal() {
  _writer = new tarch::plotter::griddata::unstructured::vtk::VTUTextFileWriter();
  _vertexWriter = _writer->createVertexWriter();
  _cellWriter   = _writer->createCellWriter();
}


void peano4::grid::TraversalVTKPlotter::endTraversal() {
  logInfo( "beginTraversal(...)", "end traversal" );

  _vertexWriter->close();
  _cellWriter->close();

  delete _vertexWriter;
  delete _cellWriter;

  _vertexWriter = nullptr;
  _cellWriter   = nullptr;

  std::string currentFile = _filename + "_" + std::to_string( _counter );
  _writer->writeToFile( currentFile );
  delete _writer;
  _writer = nullptr;

  _timeSeriesWriter.addSnapshot( currentFile, _counter );
  _timeSeriesWriter.writeFile( _filename );

  _counter++;
}


void peano4::grid::TraversalVTKPlotter::enterCell(
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h,
  bool                                         isRefined
) {
  if (not isRefined) {
    int vertexIndices[TwoPowerD];

    dfor2(k)
      vertexIndices[kScalar] = _vertexWriter->plotVertex(
        x + tarch::la::multiplyComponents( k.convertScalar<double>(), h )
      );
    enddforx

    #if Dimensions==2
    _cellWriter->plotQuadrangle(vertexIndices);
    #elif Dimensions==3
    #else
    #warning Noch net implementiert
    #endif
  }
}
