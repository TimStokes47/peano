#include "InitData.h"
#include "CellData.h"

#include "peano4/utils/Loop.h"


#include "tarch/multicore/Lock.h"
#include "delta/ContactPoint.h"
#include "delta/io/vtk.h"
#include "delta/contactdetection/filter.h"
#include "delta/contactdetection/sphere.h"
#include "delta/math.h"
#include "delta/primitives/Cube.h"


tarch::logging::Log examples::delta::InitData::_log( "examples::delta::InitData" );


examples::delta::InitData::InitData():
  _primitive( nullptr ) {

  _primitive = new ::delta::primitives::Cylinder(
    0.5, 0.5, 0.5, // centre
    0.2, // radius
    0.2,0.8, // min/max Z
    0.1 // h
  );

/*
  _primitive = new ::delta::primitives::Cube(
    0.5, 0.5, 0.5, // centre
    0.2            // h
  );
*/

  ::delta::io::writeVTK(
    _primitive->getNumberOfTriangles(),
    _primitive->getXCoordinates(),
    _primitive->getYCoordinates(),
    _primitive->getZCoordinates(),
    "geometry.vtk"
  );
}


examples::delta::InitData::~InitData() {
  delete _primitive;
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
  // @todo gleich Parallel
  dfor(i,CellData::DoFsPerAxis) {
    tarch::la::Vector<Dimensions,double> x = center + offset + tarch::la::multiplyComponents(i.convertScalar<double>(),patchH);

    const double epsilon = 0.4;
    std::vector<::delta::ContactPoint> contactPoints =
      ::delta::contactdetection::filter(
        ::delta::contactdetection::sphereToTriangle(
          x(0),x(1),x(2),
          0.0, // radius. This one is degenerated
          _primitive->getNumberOfTriangles(),
          _primitive->getXCoordinates(),
          _primitive->getYCoordinates(),
          _primitive->getZCoordinates(),
          epsilon // epsilon
        ),
        epsilon
      );

    if (contactPoints.empty()) {
      data.valueX[currentEntry] = 0.0;
      data.valueY[currentEntry] = 0.0;
      data.valueZ[currentEntry] = 0.0;
    }
    else {
      //data.valueX[currentEntry] = contactPoints[0].distance;
      const double distance = contactPoints[0].distance;
      data.valueX[currentEntry] = distance * contactPoints[0].normal[0];
      data.valueY[currentEntry] = distance * contactPoints[0].normal[1];
      data.valueZ[currentEntry] = distance * contactPoints[0].normal[2];
      logDebug( "createCell(...)", "contact for point " << x << ": " << contactPoints[0].toString() );
    }

    currentEntry++;
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

