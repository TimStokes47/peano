#include "tarch/multicore/tests/dForRangeTest.h"
#include "tarch/multicore/dForRange.h"


#include "tarch/tests/TestCaseFactory.h"
registerTest(tarch::multicore::tests::dForRangeTest)


#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",off)
#endif


tarch::multicore::tests::dForRangeTest::dForRangeTest():
  TestCase( "tarch::multicore::tests::dForRangeTest" ) {
}


tarch::multicore::tests::dForRangeTest::~dForRangeTest() {
}


void tarch::multicore::tests::dForRangeTest::run() {
  testMethod( test2D10x10Range1 );
  testMethod( test2D10x10Range12 );
  testMethod( test2D10x10Range23 );
  testMethod( test2D10x10Range40 );
  testMethod( test2D10x10Range80 );
}


void tarch::multicore::tests::dForRangeTest::setUp() {
}


void tarch::multicore::tests::dForRangeTest::test2D10x10Range1() {
/*
  #ifdef Dim2
  tarch::la::Vector<DIMENSIONS,int>  range(10);

  tarch::multicore::dForRange testRange1(range,1);

  validate( testRange1.is_divisible() );

  tarch::multicore::dForRange::Split split;

  tarch::multicore::dForRange testRange1_1(testRange1, split);

  validateWithParams2( testRange1.is_divisible(), testRange1.toString(), testRange1_1.toString() );
  validateWithParams2( !testRange1.empty(), testRange1.toString(), testRange1_1.toString() );
  validateEqualsWithParams2( testRange1.getOffset()(0), 5, testRange1.toString(), testRange1_1.toString() );
  validateEqualsWithParams2( testRange1.getOffset()(1), 0, testRange1.toString(), testRange1_1.toString() );
  validateEqualsWithParams2( testRange1.getRange()(0),  5, testRange1.toString(), testRange1_1.toString() );
  validateEqualsWithParams2( testRange1.getRange()(1), 10, testRange1.toString(), testRange1_1.toString() );

  validateWithParams2( testRange1_1.is_divisible(), testRange1.toString(), testRange1_1.toString() );
  validateWithParams2( !testRange1_1.empty(), testRange1.toString(), testRange1_1.toString() );
  validateEqualsWithParams2( testRange1_1.getOffset()(0), 0, testRange1.toString(), testRange1_1.toString() );
  validateEqualsWithParams2( testRange1_1.getOffset()(1), 0, testRange1.toString(), testRange1_1.toString() );
  validateEqualsWithParams2( testRange1_1.getRange()(0),  5, testRange1.toString(), testRange1_1.toString() );
  validateEqualsWithParams2( testRange1_1.getRange()(1), 10, testRange1.toString(), testRange1_1.toString() );

  #endif
*/
}


void tarch::multicore::tests::dForRangeTest::test2D10x10Range12() {
/*
  #ifdef Dim2
  tarch::la::Vector<DIMENSIONS,int>  range(10);

  tarch::multicore::dForRange testRange12(range,12);

  validate( testRange12.is_divisible() );

  tarch::multicore::dForRange::Split split;

  tarch::multicore::dForRange testRange12_1(testRange12, split);

  validateWithParams2( testRange12.is_divisible(), testRange12.toString(), testRange12_1.toString() );
  validateWithParams2( !testRange12.empty(), testRange12.toString(), testRange12_1.toString() );
  validateEqualsWithParams2( testRange12.getOffset()(0), 5, testRange12.toString(), testRange12_1.toString() );
  validateEqualsWithParams2( testRange12.getOffset()(1), 0, testRange12.toString(), testRange12_1.toString() );
  validateEqualsWithParams2( testRange12.getRange()(0),  5, testRange12.toString(), testRange12_1.toString() );
  validateEqualsWithParams2( testRange12.getRange()(1), 10, testRange12.toString(), testRange12_1.toString() );

  validateWithParams2( testRange12_1.is_divisible(), testRange12.toString(), testRange12_1.toString() );
  validateWithParams2( !testRange12_1.empty(), testRange12.toString(), testRange12_1.toString() );
  validateEqualsWithParams2( testRange12_1.getOffset()(0), 0, testRange12.toString(), testRange12_1.toString() );
  validateEqualsWithParams2( testRange12_1.getOffset()(1), 0, testRange12.toString(), testRange12_1.toString() );
  validateEqualsWithParams2( testRange12_1.getRange()(0),  5, testRange12.toString(), testRange12_1.toString() );
  validateEqualsWithParams2( testRange12_1.getRange()(1), 10, testRange12.toString(), testRange12_1.toString() );

  #endif
*/
}


void tarch::multicore::tests::dForRangeTest::test2D10x10Range23() {
/*
  #ifdef Dim2
  tarch::la::Vector<DIMENSIONS,int>  range(10);

  tarch::multicore::dForRange testRange23(range,23);

  validate( testRange23.is_divisible() );

  tarch::multicore::dForRange::Split split;

  tarch::multicore::dForRange testRange23_1(testRange23, split);

  validateWithParams2( testRange23.is_divisible(), testRange23.toString(), testRange23_1.toString() );
  validateWithParams2( !testRange23.empty(), testRange23.toString(), testRange23_1.toString() );
  validateEqualsWithParams2( testRange23.getOffset()(0), 5, testRange23.toString(), testRange23_1.toString() );
  validateEqualsWithParams2( testRange23.getOffset()(1), 0, testRange23.toString(), testRange23_1.toString() );
  validateEqualsWithParams2( testRange23.getRange()(0),  5, testRange23.toString(), testRange23_1.toString() );
  validateEqualsWithParams2( testRange23.getRange()(1), 10, testRange23.toString(), testRange23_1.toString() );

  validateWithParams2( testRange23_1.is_divisible(), testRange23.toString(), testRange23_1.toString() );
  validateWithParams2( !testRange23_1.empty(), testRange23.toString(), testRange23_1.toString() );
  validateEqualsWithParams2( testRange23_1.getOffset()(0), 0, testRange23.toString(), testRange23_1.toString() );
  validateEqualsWithParams2( testRange23_1.getOffset()(1), 0, testRange23.toString(), testRange23_1.toString() );
  validateEqualsWithParams2( testRange23_1.getRange()(0),  5, testRange23.toString(), testRange23_1.toString() );
  validateEqualsWithParams2( testRange23_1.getRange()(1), 10, testRange23.toString(), testRange23_1.toString() );

  #endif
*/
}


void tarch::multicore::tests::dForRangeTest::test2D10x10Range40() {
/*
  #ifdef Dim2
  tarch::la::Vector<DIMENSIONS,int>  range(10);

  tarch::multicore::dForRange testRange40(range,40);

  validate( testRange40.is_divisible() );

  tarch::multicore::dForRange::Split split;

  tarch::multicore::dForRange testRange40_1(testRange40, split);

  validateWithParams2( testRange40.is_divisible(), testRange40.toString(), testRange40_1.toString() );
  validateWithParams2( !testRange40.empty(), testRange40.toString(), testRange40_1.toString() );
  validateEqualsWithParams2( testRange40.getOffset()(0), 5, testRange40.toString(), testRange40_1.toString() );
  validateEqualsWithParams2( testRange40.getOffset()(1), 0, testRange40.toString(), testRange40_1.toString() );
  validateEqualsWithParams2( testRange40.getRange()(0),  5, testRange40.toString(), testRange40_1.toString() );
  validateEqualsWithParams2( testRange40.getRange()(1), 10, testRange40.toString(), testRange40_1.toString() );

  validateWithParams2( testRange40_1.is_divisible(), testRange40.toString(), testRange40_1.toString() );
  validateWithParams2( !testRange40_1.empty(), testRange40.toString(), testRange40_1.toString() );
  validateEqualsWithParams2( testRange40_1.getOffset()(0), 0, testRange40.toString(), testRange40_1.toString() );
  validateEqualsWithParams2( testRange40_1.getOffset()(1), 0, testRange40.toString(), testRange40_1.toString() );
  validateEqualsWithParams2( testRange40_1.getRange()(0),  5, testRange40.toString(), testRange40_1.toString() );
  validateEqualsWithParams2( testRange40_1.getRange()(1), 10, testRange40.toString(), testRange40_1.toString() );

  tarch::multicore::dForRange testRange40_2(testRange40, split);
  tarch::multicore::dForRange testRange40_3(testRange40_1, split);

  validateWithParams2( !testRange40_2.is_divisible(), testRange40.toString(), testRange40_1.toString() );
  validateWithParams2( !testRange40_2.empty(), testRange40.toString(), testRange40_1.toString() );
  validateEqualsWithParams2( testRange40_2.getOffset()(0), 5, testRange40.toString(), testRange40_2.toString() );
  validateEqualsWithParams2( testRange40_2.getOffset()(1), 0, testRange40.toString(), testRange40_2.toString() );
  validateEqualsWithParams2( testRange40_2.getRange()(0),  5, testRange40.toString(), testRange40_2.toString() );
  validateEqualsWithParams2( testRange40_2.getRange()(1),  5, testRange40.toString(), testRange40_2.toString() );

  validateWithParams2( !testRange40.is_divisible(), testRange40.toString(), testRange40_1.toString() );
  validateWithParams2( !testRange40.empty(), testRange40.toString(), testRange40_1.toString() );
  validateEqualsWithParams2( testRange40.getOffset()(0), 5, testRange40.toString(), testRange40_2.toString() );
  validateEqualsWithParams2( testRange40.getOffset()(1), 5, testRange40.toString(), testRange40_2.toString() );
  validateEqualsWithParams2( testRange40.getRange()(0),  5, testRange40.toString(), testRange40_2.toString() );
  validateEqualsWithParams2( testRange40.getRange()(1),  5, testRange40.toString(), testRange40_2.toString() );
  #endif
*/
}


void tarch::multicore::tests::dForRangeTest::test2D10x10Range80() {
/*
  #ifdef Dim2
  tarch::la::Vector<DIMENSIONS,int>  range(10);

  tarch::multicore::dForRange testRange80(range,80);

  validate( testRange80.is_divisible() );

  tarch::multicore::dForRange::Split split;

  tarch::multicore::dForRange testRange80_1(testRange80, split);

  validateWithParams2( !testRange80.is_divisible(), testRange80.toString(), testRange80_1.toString() );
  validateWithParams2( !testRange80.empty(), testRange80.toString(), testRange80_1.toString() );
  validateEqualsWithParams2( testRange80.getOffset()(0), 5, testRange80.toString(), testRange80_1.toString() );
  validateEqualsWithParams2( testRange80.getOffset()(1), 0, testRange80.toString(), testRange80_1.toString() );
  validateEqualsWithParams2( testRange80.getRange()(0),  5, testRange80.toString(), testRange80_1.toString() );
  validateEqualsWithParams2( testRange80.getRange()(1), 10, testRange80.toString(), testRange80_1.toString() );

  validateWithParams2( !testRange80_1.is_divisible(), testRange80.toString(), testRange80_1.toString() );
  validateWithParams2( !testRange80_1.empty(), testRange80.toString(), testRange80_1.toString() );
  validateEqualsWithParams2( testRange80_1.getOffset()(0), 0, testRange80.toString(), testRange80_1.toString() );
  validateEqualsWithParams2( testRange80_1.getOffset()(1), 0, testRange80.toString(), testRange80_1.toString() );
  validateEqualsWithParams2( testRange80_1.getRange()(0),  5, testRange80.toString(), testRange80_1.toString() );
  validateEqualsWithParams2( testRange80_1.getRange()(1), 10, testRange80.toString(), testRange80_1.toString() );
  #endif
*/
}

#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",on)
#endif
