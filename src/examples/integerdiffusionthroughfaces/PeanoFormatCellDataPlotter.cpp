#include "PeanoFormatCellDataPlotter.h"
#include "CellData.h"

#include "peano4/utils/Loop.h"


#include "tarch/plotter/griddata/blockstructured/PeanoTextPatchFileWriter.h"


examples::integerdiffusionthroughfaces::PeanoFormatCellDataPlotter::PeanoFormatCellDataPlotter():
  _counter(0),
  _writer(nullptr),
  _dataWriter(nullptr) {
}


examples::integerdiffusionthroughfaces::PeanoFormatCellDataPlotter::~PeanoFormatCellDataPlotter() {
  assertion( _writer == nullptr);
  assertion( _dataWriter == nullptr);
}


void examples::integerdiffusionthroughfaces::PeanoFormatCellDataPlotter::beginTraversal() {
  _writer = new tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter(
    Dimensions,
	"marker",
	_counter>0  // bool append
  );
  _dataWriter   = _writer->createCellDataWriter( "cell-data", 1, 1 );
}


void examples::integerdiffusionthroughfaces::PeanoFormatCellDataPlotter::endTraversal() {
  assertion( _dataWriter!=nullptr );

  _dataWriter->close();

  std::ostringstream filename;
  filename << "marker-" << _counter;
  _writer->writeToFile( filename.str() );

  delete _dataWriter;
  delete _writer;

  _dataWriter = nullptr;
  _writer     = nullptr;

/*
  _timeSeriesWriter.addSnapshot( filename.str(), _counter, false );
  _timeSeriesWriter.writeFile( "data" );
*/

  _counter++;
}


void examples::integerdiffusionthroughfaces::PeanoFormatCellDataPlotter::createPersistentFace(
      const tarch::la::Vector<Dimensions,double>&  center,
      const tarch::la::Vector<Dimensions,double>&  h,
      const tarch::la::Vector<Dimensions,double>&  normal,
	  FaceData&                                    data
    ) {}

void examples::integerdiffusionthroughfaces::PeanoFormatCellDataPlotter::createHangingFace(
      const tarch::la::Vector<Dimensions,double>&  center,
      const tarch::la::Vector<Dimensions,double>&  h,
      const tarch::la::Vector<Dimensions,double>&  normal,
	  FaceData&                                    data
    ) {}


void examples::integerdiffusionthroughfaces::PeanoFormatCellDataPlotter::destroyPersistentFace(
      const tarch::la::Vector<Dimensions,double>&  center,
      const tarch::la::Vector<Dimensions,double>&  h,
      const tarch::la::Vector<Dimensions,double>&  normal,
	  FaceData&                                    data
    )  {}


void examples::integerdiffusionthroughfaces::PeanoFormatCellDataPlotter::destroyHangingFace(
      const tarch::la::Vector<Dimensions,double>&  center,
      const tarch::la::Vector<Dimensions,double>&  h,
      const tarch::la::Vector<Dimensions,double>&  normal,
	  FaceData&                                    data
    )  {}


void examples::integerdiffusionthroughfaces::PeanoFormatCellDataPlotter::touchFaceFirstTime(
      const tarch::la::Vector<Dimensions,double>&  center,
      const tarch::la::Vector<Dimensions,double>&  h,
      const tarch::la::Vector<Dimensions,double>&  normal,
	  FaceData&                                    data
    )  {}


void examples::integerdiffusionthroughfaces::PeanoFormatCellDataPlotter::touchFaceLastTime(
      const tarch::la::Vector<Dimensions,double>&  center,
      const tarch::la::Vector<Dimensions,double>&  h,
      const tarch::la::Vector<Dimensions,double>&  normal,
	  FaceData&                                    data
    )  {}


void examples::integerdiffusionthroughfaces::PeanoFormatCellDataPlotter::createCell(
      const tarch::la::Vector<Dimensions,double>&  center,
      const tarch::la::Vector<Dimensions,double>&  h,
	  CellData&                                    data,
	  Faces&                                       faces
    )  {}


void examples::integerdiffusionthroughfaces::PeanoFormatCellDataPlotter::destroyCell(
      const tarch::la::Vector<Dimensions,double>&  center,
      const tarch::la::Vector<Dimensions,double>&  h,
	  CellData&                                    data,
	  Faces&                                       faces
    )  {}


void examples::integerdiffusionthroughfaces::PeanoFormatCellDataPlotter::touchCellFirstTime(
  const tarch::la::Vector<Dimensions,double>&  center,
  const tarch::la::Vector<Dimensions,double>&  h,
  CellData&                                    data,
  Faces&                                       faces,
  peano4::datamanagement::CellMarker           marker
)  {
  int vertexIndices[TwoPowerD];

  std::pair<int,int> indices = _writer->plotPatch(
    center - h * 0.5,
	h
  );

  assertion( _dataWriter!=nullptr );
  _dataWriter->plotCell(indices.second,data.value);
}


void examples::integerdiffusionthroughfaces::PeanoFormatCellDataPlotter::touchCellLastTime(
  const tarch::la::Vector<Dimensions,double>&  center,
  const tarch::la::Vector<Dimensions,double>&  h,
  CellData&                                    data,
  Faces&                                       faces,
  peano4::datamanagement::CellMarker           marker
)  {}

