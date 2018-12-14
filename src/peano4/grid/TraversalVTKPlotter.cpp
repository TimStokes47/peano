#include "TraversalVTKPlotter.h"

#include "peano4/utils/Loop.h"

#include "tarch/multicore/BooleanSemaphore.h"
#include "tarch/multicore/Lock.h"

#include <string>


tarch::logging::Log  peano4::grid::TraversalVTKPlotter::_log( "peano4::grid::TraversalVTKPlotter" );


peano4::grid::TraversalVTKPlotter::TraversalVTKPlotter( const std::string& filename, int treeId, int counter ):
  _filename(filename),
  _spacetreeId(treeId),
  _counter(counter),
  _writer(nullptr),
  _vertexWriter(nullptr),
  _cellWriter(nullptr),
  _spacetreeIdWriter(nullptr),
  _timeSeriesWriter() {
  _writer            = new tarch::plotter::griddata::unstructured::vtk::VTUTextFileWriter();
  _vertexWriter      = _writer->createVertexWriter();
  _cellWriter        = _writer->createCellWriter();
  _spacetreeIdWriter = _writer->createCellDataWriter( "unique-global-thread-id", 1 );
}


peano4::grid::TraversalVTKPlotter::~TraversalVTKPlotter() {
  endTraversal();
}


void peano4::grid::TraversalVTKPlotter::beginTraversal() {
}


void peano4::grid::TraversalVTKPlotter::endTraversal() {
  if (_writer!=nullptr) {
    _vertexWriter->close();
    _cellWriter->close();
    _spacetreeIdWriter->close();

    delete _vertexWriter;
    delete _cellWriter;
    delete _spacetreeIdWriter;

    _vertexWriter      = nullptr;
    _cellWriter        = nullptr;
    _spacetreeIdWriter = nullptr;

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

    logInfo( "endTraversal(...)", "wrote to file " << currentFile );
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

	int cellIndex = -1;
    #if Dimensions==2
    cellIndex = _cellWriter->plotQuadrangle(vertexIndices);
    #elif Dimensions==3
    #else
    #warning Noch net implementiert
    #endif

    _spacetreeIdWriter->plotCell(cellIndex,4);
  }
}


peano4::grid::TraversalObserver*  peano4::grid::TraversalVTKPlotter::clone(int spacetreeId) {
  if (_spacetreeId==-1) {
	static tarch::multicore::BooleanSemaphore semaphore;
	tarch::multicore::Lock lock(semaphore);

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
