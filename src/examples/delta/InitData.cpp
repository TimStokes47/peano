#include "InitData.h"
#include "CellData.h"

#include "peano4/utils/Loop.h"


#include "tarch/plotter/griddata/blockstructured/PeanoTextPatchFileWriter.h"
#include "tarch/multicore/Lock.h"



examples::delta::InitData::InitData() {
}


examples::delta::InitData::~InitData() {
}


void examples::delta::InitData::beginTraversal() {
}


void examples::delta::InitData::endTraversal() {
}


void examples::delta::InitData::createCell(
  const tarch::la::Vector<Dimensions,double>&  center,
  const tarch::la::Vector<Dimensions,double>&  h,
  CellData&                                    data,
  CellData&                                    coarseData
)  {
  for (int i=0; i<CellData::DoFsPerCell; i++) {
	data.value[i] = 0.0;
  }
}


void examples::delta::InitData::destroyCell(
  const tarch::la::Vector<Dimensions,double>&  center,
  const tarch::la::Vector<Dimensions,double>&  h,
  CellData&                                    data,
  CellData&                                    coarseData
)  {}


void examples::delta::InitData::touchCellFirstTime(
  const tarch::la::Vector<Dimensions,double>&  center,
  const tarch::la::Vector<Dimensions,double>&  h,
  CellData&                                    data,
  CellData&                                    coarseData,
  peano4::datamanagement::CellMarker           marker
)  {
}


void examples::delta::InitData::touchCellLastTime(
  const tarch::la::Vector<Dimensions,double>&  center,
  const tarch::la::Vector<Dimensions,double>&  h,
  CellData&                                    data,
  CellData&                                    coarseData,
  peano4::datamanagement::CellMarker           marker
)  {
}

