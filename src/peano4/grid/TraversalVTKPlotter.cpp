#include "TraversalVTKPlotter.h"
#include "GridControlEvent.h"

#include "peano4/utils/Loop.h"

#include "tarch/multicore/BooleanSemaphore.h"
#include "tarch/multicore/Lock.h"
#include "tarch/multicore/Core.h"

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
  _coreWriter(nullptr),
  _timeSeriesWriter() {
  _writer            = new tarch::plotter::griddata::unstructured::vtk::VTUTextFileWriter();
  _vertexWriter      = _writer->createVertexWriter();
  _cellWriter        = _writer->createCellWriter();
  _spacetreeIdWriter = _writer->createCellDataWriter( "tree-id", 1 );
  _coreWriter        = _writer->createCellDataWriter( "core-number", 1 );
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
    _coreWriter->close();

    delete _vertexWriter;
    delete _cellWriter;
    delete _spacetreeIdWriter;
    delete _coreWriter;

    _vertexWriter      = nullptr;
    _cellWriter        = nullptr;
    _spacetreeIdWriter = nullptr;
    _coreWriter        = nullptr;

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
  bool                                         isRefined,
  int                                          treeId
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

    _spacetreeIdWriter->plotCell(cellIndex,treeId);
    _coreWriter->plotCell(cellIndex,tarch::multicore::Core::getInstance().getCoreNumber());
  }
}


void peano4::grid::TraversalVTKPlotter::updateMetaFile(int spacetreeId) {
  static tarch::multicore::BooleanSemaphore semaphore;
  tarch::multicore::Lock lock(semaphore);

  std::string newFile = _filename + "-" + std::to_string(spacetreeId) + "-" + std::to_string( _counter );
  _clonedSpacetreeIds.push_back( newFile );
  _writer->writeMetaDataFileForParallelSnapshot(
     _filename + "-" + std::to_string( _counter ),
    _clonedSpacetreeIds
  );
}


peano4::grid::TraversalObserver*  peano4::grid::TraversalVTKPlotter::clone(int spacetreeId) {
  if (_spacetreeId==-1) {
	updateMetaFile( spacetreeId );
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


std::vector< peano4::grid::GridControlEvent > peano4::grid::TraversalVTKPlotter::getGridControlEvents() {
  return std::vector< peano4::grid::GridControlEvent >();
}
