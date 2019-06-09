#include "PeanoFormatCellDataPlotter.h"
#include "CellData.h"

#include "peano4/utils/Loop.h"


#include "tarch/plotter/griddata/blockstructured/PeanoTextPatchFileWriter.h"
#include "tarch/multicore/Lock.h"


tarch::multicore::BooleanSemaphore  examples::delta::PeanoFormatCellDataPlotter::_semaphore;
int                                 examples::delta::PeanoFormatCellDataPlotter::_instanceCounter(0);


examples::delta::PeanoFormatCellDataPlotter::PeanoFormatCellDataPlotter(const std::string&  fileNamePrefix, bool plotThroughoutDescent):
  _plotThroughoutDescent(plotThroughoutDescent),
  _fileNamePrefix(fileNamePrefix),
  _counter(0),
  _writer(nullptr),
  _dataWriter(nullptr) {
  assertionEquals(_instanceCounter,0);
}


examples::delta::PeanoFormatCellDataPlotter::~PeanoFormatCellDataPlotter() {
  assertion( _writer == nullptr);
  assertion( _dataWriter == nullptr);
}


void examples::delta::PeanoFormatCellDataPlotter::beginTraversal() {
  tarch::multicore::Lock lock(_semaphore);
  if (_instanceCounter==0) {
    assertion(_writer==nullptr);

 	_writer = new tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter(
      Dimensions,
      _fileNamePrefix,
      _counter>0  // bool append
    );
 	das kann so noch net sein!
    _dataWriter      = _writer->createCellDataWriter( "cell-data", 1, 1 );
  }
  _instanceCounter++;
}


void examples::delta::PeanoFormatCellDataPlotter::endTraversal() {
  tarch::multicore::Lock lock(_semaphore);
  _instanceCounter--;

  if (_instanceCounter==0) {
    assertion( _dataWriter!=nullptr );

    _dataWriter->close();

    std::ostringstream filename;
    filename << _fileNamePrefix << "-" << _counter;
    _writer->writeToFile( filename.str() );

    delete _dataWriter;
    delete _writer;

    _dataWriter    = nullptr;
    _writer        = nullptr;

    _counter++;
  }
}


void examples::delta::PeanoFormatCellDataPlotter::createCell(
  const tarch::la::Vector<Dimensions,double>&  center,
  const tarch::la::Vector<Dimensions,double>&  h,
  CellData&                                    data,
  CellData&                                    coarseData
)  {}


void examples::delta::PeanoFormatCellDataPlotter::destroyCell(
  const tarch::la::Vector<Dimensions,double>&  center,
  const tarch::la::Vector<Dimensions,double>&  h,
  CellData&                                    data,
  CellData&                                    coarseData
)  {}


void examples::delta::PeanoFormatCellDataPlotter::plotCell(
  const tarch::la::Vector<Dimensions,double>&  center,
  const tarch::la::Vector<Dimensions,double>&  h,
  CellData&                                    data
) {
  int vertexIndices[TwoPowerD];

  std::pair<int,int> indices = _writer->plotPatch(
    center - h * 0.5,
	h
  );

  assertion( _dataWriter!=nullptr );
  // @todo
  _dataWriter->plotCell(indices.second,data.value);
}


void examples::delta::PeanoFormatCellDataPlotter::touchCellFirstTime(
  const tarch::la::Vector<Dimensions,double>&  center,
  const tarch::la::Vector<Dimensions,double>&  h,
  CellData&                                    data,
  CellData&                                    coarseData,
  peano4::datamanagement::CellMarker           marker
)  {
  if (_plotThroughoutDescent) {
    plotCell(center,h,data);
  }
}


void examples::delta::PeanoFormatCellDataPlotter::touchCellLastTime(
  const tarch::la::Vector<Dimensions,double>&  center,
  const tarch::la::Vector<Dimensions,double>&  h,
  CellData&                                    data,
  CellData&                                    coarseData,
  peano4::datamanagement::CellMarker           marker
)  {
  if (not _plotThroughoutDescent) {
    plotCell(center,h,data);
  }
}

