#include "GridControlEventTest.h"
#include "../grid.h"
#include "../GridControlEvent.h"


tarch::logging::Log peano4::grid::tests::GridControlEventTest::_log("peano4::grid::tests::GridControlEventTest");


#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",off)
#endif


peano4::grid::tests::GridControlEventTest::GridControlEventTest():
  TestCase( "peano4::grid::tests::GridControlEventTest" ) {
}



void peano4::grid::tests::GridControlEventTest::testConsolidate() {
  #if Dimensions==2
  #endif
}


void peano4::grid::tests::GridControlEventTest::run() {
  testMethod( testConsolidate );
}


#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",on)
#endif
