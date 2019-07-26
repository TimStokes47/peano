#include "NodeTest.h"
#include "../Node.h"

#include "tarch/la/Vector.h"

#include "peano4/grid/Spacetree.h"


#include "tarch/tests/TestCaseFactory.h"
registerTest(peano4::parallel::tests::NodeTest)


tarch::logging::Log peano4::parallel::tests::NodeTest::_log("peano4::parallel::tests::NodeTest");


#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",off)
#endif


void peano4::parallel::tests::NodeTest::testGetPeriodicBoundaryNumber() {
  tarch::la::Vector<TwoPowerD,int>  flags(1);
  flags(0) = peano4::grid::Spacetree::RankOfPeriodicBoundaryCondition;
  flags(1) = peano4::grid::Spacetree::RankOfPeriodicBoundaryCondition;

  validateEqualsWithParams1( peano4::parallel::Node::getPeriodicBoundaryNumber(flags), 1, flags );
}


void peano4::parallel::tests::NodeTest::run() {
  testMethod(  testGetPeriodicBoundaryNumber );
}


#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",on)
#endif
