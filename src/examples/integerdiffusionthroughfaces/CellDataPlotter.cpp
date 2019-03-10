#include "CellDataPlotter.h"
#include "CellData.h"


#include "peano4/utils/Loop.h"


examples::integerdiffusionthroughfaces::CellDataPlotter::CellDataPlotter():
  _counter(0),
  _writer(nullptr),
  _vertexWriter(nullptr),
  _cellWriter(nullptr),
  _dataWriter(nullptr),
  _timeSeriesWriter() {
}


examples::integerdiffusionthroughfaces::CellDataPlotter::~CellDataPlotter() {
  assertion( _writer == nullptr);
  assertion( _vertexWriter == nullptr);
  assertion( _cellWriter == nullptr);
  assertion( _dataWriter == nullptr);
}


void examples::integerdiffusionthroughfaces::CellDataPlotter::beginTraversal() {
  _writer       = new tarch::plotter::griddata::unstructured::vtk::VTUTextFileWriter();
  _vertexWriter = _writer->createVertexWriter();
  _cellWriter   = _writer->createCellWriter();
  _dataWriter   = _writer->createCellDataWriter( "cell-data", 1 );
}


void examples::integerdiffusionthroughfaces::CellDataPlotter::endTraversal() {
  assertion( _vertexWriter!=nullptr );
  assertion( _cellWriter!=nullptr );
  assertion( _dataWriter!=nullptr );

  _vertexWriter->close();
  _cellWriter->close();
  _dataWriter->close();

  std::ostringstream filename;
  filename << "data-" << _counter;
  _writer->writeToFile( filename.str() );

  delete _vertexWriter;
  delete _cellWriter;
  delete _dataWriter;
  delete _writer;

  _vertexWriter = 0;
  _cellWriter = 0;
  _dataWriter = 0;
  _writer = 0;

  _timeSeriesWriter.addSnapshot( filename.str(), _counter, false );
  _timeSeriesWriter.writeFile( "data" );

  _counter++;
}


void examples::integerdiffusionthroughfaces::CellDataPlotter::createPersistentFace(
      const tarch::la::Vector<Dimensions,double>&  center,
      const tarch::la::Vector<Dimensions,double>&  h,
      const tarch::la::Vector<Dimensions,double>&  normal,
	  FaceData&                                    data
    ) {}

void examples::integerdiffusionthroughfaces::CellDataPlotter::createHangingFace(
      const tarch::la::Vector<Dimensions,double>&  center,
      const tarch::la::Vector<Dimensions,double>&  h,
      const tarch::la::Vector<Dimensions,double>&  normal,
	  FaceData&                                    data
    ) {}


void examples::integerdiffusionthroughfaces::CellDataPlotter::destroyPersistentFace(
      const tarch::la::Vector<Dimensions,double>&  center,
      const tarch::la::Vector<Dimensions,double>&  h,
      const tarch::la::Vector<Dimensions,double>&  normal,
	  FaceData&                                    data
    )  {}


void examples::integerdiffusionthroughfaces::CellDataPlotter::destroyHangingFace(
      const tarch::la::Vector<Dimensions,double>&  center,
      const tarch::la::Vector<Dimensions,double>&  h,
      const tarch::la::Vector<Dimensions,double>&  normal,
	  FaceData&                                    data
    )  {}


void examples::integerdiffusionthroughfaces::CellDataPlotter::touchFaceFirstTime(
      const tarch::la::Vector<Dimensions,double>&  center,
      const tarch::la::Vector<Dimensions,double>&  h,
      const tarch::la::Vector<Dimensions,double>&  normal,
	  FaceData&                                    data
    )  {}


void examples::integerdiffusionthroughfaces::CellDataPlotter::touchFaceLastTime(
      const tarch::la::Vector<Dimensions,double>&  center,
      const tarch::la::Vector<Dimensions,double>&  h,
      const tarch::la::Vector<Dimensions,double>&  normal,
	  FaceData&                                    data
    )  {}


void examples::integerdiffusionthroughfaces::CellDataPlotter::createCell(
      const tarch::la::Vector<Dimensions,double>&  center,
      const tarch::la::Vector<Dimensions,double>&  h,
	  CellData&                                    data,
	  Faces&                                       faces
    )  {}


void examples::integerdiffusionthroughfaces::CellDataPlotter::destroyCell(
      const tarch::la::Vector<Dimensions,double>&  center,
      const tarch::la::Vector<Dimensions,double>&  h,
	  CellData&                                    data,
	  Faces&                                       faces
    )  {}


void examples::integerdiffusionthroughfaces::CellDataPlotter::touchCellFirstTime(
  const tarch::la::Vector<Dimensions,double>&  center,
  const tarch::la::Vector<Dimensions,double>&  h,
  CellData&                                    data,
  Faces&                                       faces
)  {
	// @todo Ueberlegen.
//	  if (not event.getIsRefined()) {
  int vertexIndices[TwoPowerD];

  dfor2(k)
    assertion( _vertexWriter!=nullptr );
    vertexIndices[kScalar] = _vertexWriter->plotVertex(
      center + tarch::la::multiplyComponents( k.convertScalar<double>(), h ) - h * 0.5
    );
  enddforx

  assertion( _cellWriter!=nullptr );
  int cellIndex = -1;
  #if Dimensions==2
  cellIndex = _cellWriter->plotQuadrangle(vertexIndices);
  #elif Dimensions==3
  cellIndex = _cellWriter->plotHexahedron(vertexIndices);
  #else
  assertionMsg( false, "supports only 2d and 3d" );
  #endif

  assertion( _dataWriter!=nullptr );
  _dataWriter->plotCell(cellIndex,data.value);
}


void examples::integerdiffusionthroughfaces::CellDataPlotter::touchCellLastTime(
      const tarch::la::Vector<Dimensions,double>&  center,
      const tarch::la::Vector<Dimensions,double>&  h,
	  CellData&                                    data,
	  Faces&                                       faces
    )  {}

