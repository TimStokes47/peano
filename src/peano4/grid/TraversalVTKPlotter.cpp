#include "TraversalVTKPlotter.h"

#include "peano4/utils/Loop.h"

#include <string>


tarch::logging::Log  peano4::grid::TraversalVTKPlotter::_log( "peano4::grid::TraversalVTKPlotter" );


peano4::grid::TraversalVTKPlotter::TraversalVTKPlotter( const std::string& filename, int treeId, int counter ):
  _filename(filename),
  _spacetreeId(treeId),
  _counter(counter),
  _writer(nullptr),
  _vertexWriter(nullptr),
  _cellWriter(nullptr),
  _timeSeriesWriter() {
  _writer = new tarch::plotter::griddata::unstructured::vtk::VTUTextFileWriter();
  _vertexWriter = _writer->createVertexWriter();
  _cellWriter   = _writer->createCellWriter();
}


peano4::grid::TraversalVTKPlotter::~TraversalVTKPlotter() {
  endTraversal();
}


void peano4::grid::TraversalVTKPlotter::beginTraversal() {
}


void peano4::grid::TraversalVTKPlotter::endTraversal() {
  logInfo( "beginTraversal(...)", "end traversal" );

  if (_writer!=nullptr) {
    _vertexWriter->close();
    _cellWriter->close();

    delete _vertexWriter;
    delete _cellWriter;

    _vertexWriter = nullptr;
    _cellWriter   = nullptr;

    std::string currentFile = _filename;

    // we run serially
    if (_spacetreeId==-1) {
      currentFile += "-" + std::to_string( _counter );
    }
    else {
      currentFile += "-" + std::to_string(_spacetreeId) + "-" + std::to_string( _counter );
    }
    _writer->writeToFile( currentFile );
    delete _writer;
    _writer = nullptr;
  }
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


peano4::grid::TraversalObserver*  peano4::grid::TraversalVTKPlotter::clone(int spacetreeId) {
  // This is the main plotter. In the parallel case, I will have to check whether I'm on the
  // global rank as well.
  if (_spacetreeId==-1) {
	std::string newFile = _filename + "-" + std::to_string(spacetreeId) + "-" + std::to_string( _counter );
    _clonedSpacetreeIds.push_back( newFile );

    _writer->writeMetaDataFileForParallelSnapshot(
      _filename + "-" + std::to_string( _counter ),
	  _clonedSpacetreeIds
	);
  }
  else {
	assertionMsg( false, "clone() should not be called for particular spacetree plotter" );
  }


  return new peano4::grid::TraversalVTKPlotter(
    _filename,
	spacetreeId,
	_counter
  );
}


void peano4::grid::TraversalVTKPlotter::startNewSnapshot() {
  _counter++;

  _timeSeriesWriter.addSnapshot( _filename + "-" + std::to_string( _counter ), _counter );
  _timeSeriesWriter.writeFile( _filename );

  _clonedSpacetreeIds.clear();
}
