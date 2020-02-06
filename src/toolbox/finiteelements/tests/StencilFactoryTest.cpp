#include "StencilFactoryTest.h"
#include "../StencilFactory.h"

#include "peano4/utils/Globals.h"
#include "peano4/utils/Loop.h"
#include "tarch/la/Vector.h"


#include "tarch/tests/TestCaseFactory.h"
registerTest(toolbox::finiteelements::tests::StencilFactoryTest)


tarch::logging::Log toolbox::finiteelements::tests::StencilFactoryTest::_log( "toolbox::finiteelements::tests::StencilFactoryTest" );


#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",off)
#endif


void toolbox::finiteelements::tests::StencilFactoryTest::testIntegrationWithN1() {
	// @todo Haven't implemented this one yet
  validate(false);
}


void toolbox::finiteelements::tests::StencilFactoryTest::run() {
  testMethod( testIntegrationWithN1 );
}


#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",on)
#endif
