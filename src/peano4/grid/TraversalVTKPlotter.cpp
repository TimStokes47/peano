#include "TraversalVTKPlotter.h"
#include "GridControlEvent.h"
#include "GridTraversalEvent.h"

#include "peano4/utils/Loop.h"

#include "tarch/multicore/BooleanSemaphore.h"
#include "tarch/multicore/Lock.h"
#include "tarch/multicore/Core.h"

#include <string>



tarch::logging::Log  peano4::grid::TraversalVTKPlotter::_log( "peano4::grid::TraversalVTKPlotter" );


int peano4::grid::TraversalVTKPlotter::_counter(0);


peano4::grid::TraversalVTKPlotter::TraversalVTKPlotter( const std::string& filename, int treeId ):
  _filename(filename),
  _spacetreeId(treeId),
  _writer(nullptr),
  _vertexWriter(nullptr),
  _cellWriter(nullptr),
  _spacetreeIdWriter(nullptr),
  _coreWriter(nullptr),
  _timeSeriesWriter() {
  openFile();
}


peano4::grid::TraversalVTKPlotter::~TraversalVTKPlotter() {
  closeFile();
}


void peano4::grid::TraversalVTKPlotter::openFile() {
  if (_writer==nullptr) {
    _writer            = new tarch::plotter::griddata::unstructured::vtk::VTUTextFileWriter();
    _vertexWriter      = _writer->createVertexWriter();
    _cellWriter        = _writer->createCellWriter();
    _spacetreeIdWriter = _writer->createCellDataWriter( "tree-id", 1 );
    _coreWriter        = _writer->createCellDataWriter( "core-number", 1 );
  }
}


void peano4::grid::TraversalVTKPlotter::beginTraversal(
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h
) {
}


void peano4::grid::TraversalVTKPlotter::endTraversal(
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h
) {
}


void peano4::grid::TraversalVTKPlotter::closeFile() {
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
  const GridTraversalEvent&  event
) {
  if (not event.getIsRefined()) {
    int vertexIndices[TwoPowerD];

    dfor2(k)
      assertion( _vertexWriter!=nullptr );
      vertexIndices[kScalar] = _vertexWriter->plotVertex(
        event.getX() + tarch::la::multiplyComponents( k.convertScalar<double>(), event.getH() ) - event.getH() * 0.5
      );
    enddforx

	assertion( _cellWriter!=nullptr );
	int cellIndex = -1;
    #if Dimensions==2
    cellIndex = _cellWriter->plotQuadrangle(vertexIndices);
    #elif Dimensions==3
    cellIndex = _cellWriter->plotHexahedron(vertexIndices);
    #else
    logError( "enterCell(...)", "supports only 2d and 3d" );
    #endif

    assertion( _spacetreeIdWriter!=nullptr );
    assertion( _coreWriter!=nullptr );
    _spacetreeIdWriter->plotCell(cellIndex,_spacetreeId);
    _coreWriter->plotCell(cellIndex,tarch::multicore::Core::getInstance().getCoreNumber());
  }
}



void peano4::grid::TraversalVTKPlotter::leaveCell(
  const GridTraversalEvent&  event
) {
}


void peano4::grid::TraversalVTKPlotter::updateMetaFile(int spacetreeId) {
  static tarch::multicore::BooleanSemaphore semaphore;
  tarch::multicore::Lock lock(semaphore);

  std::string newFile = _filename + "-" + std::to_string(spacetreeId) + "-" + std::to_string( _counter );
  _clonedSpacetreeIds.push_back( newFile );
  assertion1( _writer!=nullptr, _spacetreeId );
  _writer->writeMetaDataFileForParallelSnapshot(
     _filename + "-" + std::to_string( _counter ),
    _clonedSpacetreeIds
  );
}


peano4::grid::TraversalObserver*  peano4::grid::TraversalVTKPlotter::clone(int spacetreeId) {
  peano4::grid::TraversalVTKPlotter* result = new peano4::grid::TraversalVTKPlotter(
    _filename,
	spacetreeId
  );

  if (_spacetreeId!=-1) {
	assertionMsg( false, "clone() should not be called for particular spacetree plotter" );
  }
  else {
    updateMetaFile(spacetreeId);
  }

  return result;
}


void peano4::grid::TraversalVTKPlotter::startNewSnapshot(bool isParallelRun) {
  _counter++;

  _timeSeriesWriter.addSnapshot( _filename + "-" + std::to_string( _counter ), _counter, isParallelRun );
  _timeSeriesWriter.writeFile( _filename );

  _clonedSpacetreeIds.clear();
}


std::vector< peano4::grid::GridControlEvent > peano4::grid::TraversalVTKPlotter::getGridControlEvents() {
  return std::vector< peano4::grid::GridControlEvent >();
}
