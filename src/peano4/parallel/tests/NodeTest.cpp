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


void peano4::parallel::tests::NodeTest::testTagCalculation() {
  int tagA = peano4::parallel::Node::getInstance().getGridDataExchangeTag(0,5,peano4::parallel::Node::ExchangeMode::HorizontalData);
  int tagB = peano4::parallel::Node::getInstance().getGridDataExchangeTag(5,0,peano4::parallel::Node::ExchangeMode::HorizontalData);
  validateWithParams2(tagA!=tagB, tagA, tagB);

  tagA = peano4::parallel::Node::getInstance().getGridDataExchangeTag(0,1,peano4::parallel::Node::ExchangeMode::HorizontalData);
  tagB = peano4::parallel::Node::getInstance().getGridDataExchangeTag(1,0,peano4::parallel::Node::ExchangeMode::HorizontalData);
  validateWithParams2(tagA!=tagB, tagA, tagB);
}


void peano4::parallel::tests::NodeTest::testGetPeriodicBoundaryNumber() {
  #if Dimensions==2
  tarch::la::Vector<TwoPowerD,int>  flags(1);
  flags(0) = peano4::grid::Spacetree::RankOfPeriodicBoundaryCondition;
  flags(1) = peano4::grid::Spacetree::RankOfPeriodicBoundaryCondition;

  std::bitset<2*Dimensions> result = peano4::parallel::Node::getPeriodicBoundaryNumber(flags);
  validateEqualsWithParams1( result.to_ulong(), 2, flags );
  #endif
}


void peano4::parallel::tests::NodeTest::run() {
  testMethod( testGetPeriodicBoundaryNumber );
  testMethod( testTagCalculation );
}


#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",on)
#endif
