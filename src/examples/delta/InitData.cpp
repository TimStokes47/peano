#include "InitData.h"
#include "CellData.h"

#include "peano4/utils/Loop.h"


#include "tarch/multicore/Lock.h"
#include "delta/ContactPoint.h"
#include "delta/io/vtk.h"
#include "delta/contactdetection/filter.h"
#include "delta/contactdetection/sphere.h"


examples::delta::InitData::InitData():
  _primitive(
    0.5, 0.5, 0.5, // centre
    0.2, // radius
    0.2,0.8, // min/max Z
    0.001 // h
  ) {

  ::delta::io::writeVTK(
    _primitive.getNumberOfTriangles(),
    _primitive.getXCoordinates(),
    _primitive.getYCoordinates(),
    _primitive.getZCoordinates(),
    "geometry.vtk"
  );
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
  tarch::la::Vector<Dimensions,double> patchH = (1.0 / CellData::DoFsPerAxis) * h;
  tarch::la::Vector<Dimensions,double> offset = -0.5 * h + 0.5 * patchH;

  int currentEntry = 0;
  dfor(i,CellData::DoFsPerAxis) {
    tarch::la::Vector<Dimensions,double> x = center + offset + tarch::la::multiplyComponents(i.convertScalar<double>(),patchH);

    std::vector<::delta::ContactPoint> contactPoints = ::delta::contactdetection::sphereToTriangle(
      x(0),x(1),x(2),
      0.0, // radius. This one is degenerated
      _primitive.getNumberOfTriangles(),
      _primitive.getXCoordinates(),
      _primitive.getYCoordinates(),
      _primitive.getZCoordinates(),
      0.4 // epsilon
    );



    data.valueX[currentEntry] = 1.0;
    currentEntry++;
  }

/*
  ::delta::io::writeVTK(
    _primitive.getNumberOfTriangles(),
    _primitive.getXCoordinates(),
    _primitive.getYCoordinates(),
    _primitive.getZCoordinates(),
    "geometry.vtk"
  );
*/

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

