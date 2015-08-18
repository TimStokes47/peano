#include "peano/heap/tests/CompressedFloatingPointNumbersTest.h"

#include "peano/heap/CompressedFloatingPointNumbers.h"

#include "tarch/compiler/CompilerSpecificSettings.h"
#include "tarch/tests/TestCaseFactory.h"
registerTest(peano::heap::tests::CompressedFloatingPointNumbersTest)


#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",off)
#endif

 
peano::heap::tests::CompressedFloatingPointNumbersTest::CompressedFloatingPointNumbersTest():
  tarch::tests::TestCase( "peano::heap::tests::CompressedFloatingPointNumbersTest" ) {
}


peano::heap::tests::CompressedFloatingPointNumbersTest::~CompressedFloatingPointNumbersTest() {
}


void peano::heap::tests::CompressedFloatingPointNumbersTest::run() {
  testMethod( testDecomposeWithLongInt );
  testMethod( testDecomposeWithInt );
  testMethod( testDecomposeWithChar );
  testMethod( testBatchDecomposition );

  testMethod( testDecompose1 );
  testMethod( testErrorComputation );
}


/**
 * @todo Gehoert in den Heap
 */
void peano::heap::tests::CompressedFloatingPointNumbersTest::testDecomposeWithLongInt() {
  double       value;
  double       reconstructedValue;
  char         exponent;
  long int     mantissa;

  // Casts here are used to display char as number (otherwise you might get line breaks, e.g.
  value = 1.0;
  peano::heap::decompose( value, exponent, mantissa );
  reconstructedValue = peano::heap::compose(exponent,mantissa);
  //validateEqualsWithParams3( static_cast<int>(exponent),1,value,static_cast<int>(exponent),mantissa );
  //validateEqualsWithParams3( mantissa,1,value,static_cast<int>(exponent),mantissa );
  validateEqualsWithParams2( reconstructedValue, value, static_cast<int>(exponent),mantissa );

  value = 0.5;
  peano::heap::decompose( value, exponent, mantissa );
  reconstructedValue = peano::heap::compose(exponent,mantissa);
  //validateEqualsWithParams3( static_cast<int>(exponent),1,value,static_cast<int>(exponent),mantissa );
  //validateEqualsWithParams3( mantissa,1,value,static_cast<int>(exponent),mantissa );
  validateEqualsWithParams2( reconstructedValue, value, static_cast<int>(exponent),mantissa );

  value = 0.1;
  peano::heap::decompose( value, exponent, mantissa );
  reconstructedValue = peano::heap::compose(exponent,mantissa);
  //validateEqualsWithParams3( static_cast<int>(exponent),1,value,static_cast<int>(exponent),mantissa );
  //validateEqualsWithParams3( mantissa,1,value,static_cast<int>(exponent),mantissa );
  validateEqualsWithParams2( reconstructedValue, value, static_cast<int>(exponent),mantissa );

  value = 0.0;
  peano::heap::decompose( value, exponent, mantissa );
  reconstructedValue = peano::heap::compose(exponent,mantissa);
  //validateEqualsWithParams3( static_cast<int>(exponent),1,value,static_cast<int>(exponent),mantissa );
  //validateEqualsWithParams3( mantissa,1,value,static_cast<int>(exponent),mantissa );
  validateEqualsWithParams2( reconstructedValue, value, static_cast<int>(exponent),mantissa );

  value = 2.345678e12;
  peano::heap::decompose( value, exponent, mantissa );
  reconstructedValue = peano::heap::compose(exponent,mantissa);
  //validateEqualsWithParams3( static_cast<int>(exponent),1,value,static_cast<int>(exponent),mantissa );
  //validateEqualsWithParams3( mantissa,1,value,static_cast<int>(exponent),mantissa );
  validateEqualsWithParams2( reconstructedValue, value, static_cast<int>(exponent),mantissa );

  value = -2.345678e12;
  peano::heap::decompose( value, exponent, mantissa );
  reconstructedValue = peano::heap::compose(exponent,mantissa);
  //validateEqualsWithParams3( static_cast<int>(exponent),1,value,static_cast<int>(exponent),mantissa );
  //validateEqualsWithParams3( mantissa,1,value,static_cast<int>(exponent),mantissa );
  validateEqualsWithParams2( reconstructedValue, value, static_cast<int>(exponent),mantissa );

  value = 2.345678e-12;
  peano::heap::decompose( value, exponent, mantissa );
  reconstructedValue = peano::heap::compose(exponent,mantissa);
  //validateEqualsWithParams3( static_cast<int>(exponent),1,value,static_cast<int>(exponent),mantissa );
  //validateEqualsWithParams3( mantissa,1,value,static_cast<int>(exponent),mantissa );
  validateEqualsWithParams2( reconstructedValue, value, static_cast<int>(exponent),mantissa );
}


void peano::heap::tests::CompressedFloatingPointNumbersTest::testDecomposeWithInt() {
  double       value;
  double       reconstructedValue;
  char         exponent;
  int          mantissa;

  // Casts here are used to display char as number (otherwise you might get line breaks, e.g.
  value = 1.0;
  peano::heap::decompose( value, exponent, mantissa );
  reconstructedValue = peano::heap::compose(exponent,mantissa);
  //validateEqualsWithParams3( static_cast<int>(exponent),1,value,static_cast<int>(exponent),mantissa );
  //validateEqualsWithParams3( mantissa,1,value,static_cast<int>(exponent),mantissa );
  validateEqualsWithParams2( reconstructedValue, value, static_cast<int>(exponent),mantissa );

  value = 0.5;
  peano::heap::decompose( value, exponent, mantissa );
  reconstructedValue = peano::heap::compose(exponent,mantissa);
  //validateEqualsWithParams3( static_cast<int>(exponent),1,value,static_cast<int>(exponent),mantissa );
  //validateEqualsWithParams3( mantissa,1,value,static_cast<int>(exponent),mantissa );
  validateEqualsWithParams2( reconstructedValue, value, static_cast<int>(exponent),mantissa );

  value = 0.1;
  peano::heap::decompose( value, exponent, mantissa );
  reconstructedValue = peano::heap::compose(exponent,mantissa);
  //validateEqualsWithParams3( static_cast<int>(exponent),1,value,static_cast<int>(exponent),mantissa );
  //validateEqualsWithParams3( mantissa,1,value,static_cast<int>(exponent),mantissa );
  validateNumericalEqualsWithParams2( reconstructedValue, value, static_cast<int>(exponent),mantissa );

  value = 0.0;
  peano::heap::decompose( value, exponent, mantissa );
  reconstructedValue = peano::heap::compose(exponent,mantissa);
  //validateEqualsWithParams3( static_cast<int>(exponent),1,value,static_cast<int>(exponent),mantissa );
  //validateEqualsWithParams3( mantissa,1,value,static_cast<int>(exponent),mantissa );
  validateNumericalEqualsWithParams2( reconstructedValue, value, static_cast<int>(exponent),mantissa );

  value = 2.345678e12;
  peano::heap::decompose( value, exponent, mantissa );
  reconstructedValue = peano::heap::compose(exponent,mantissa);
  //validateEqualsWithParams3( static_cast<int>(exponent),1,value,static_cast<int>(exponent),mantissa );
  //validateEqualsWithParams3( mantissa,1,value,static_cast<int>(exponent),mantissa );
  validateWithParams4( std::abs(reconstructedValue-value)<1e3, static_cast<int>(exponent),mantissa,reconstructedValue,value );

  value = -2.345678e12;
  peano::heap::decompose( value, exponent, mantissa );
  reconstructedValue = peano::heap::compose(exponent,mantissa);
  //validateEqualsWithParams3( static_cast<int>(exponent),1,value,static_cast<int>(exponent),mantissa );
  //validateEqualsWithParams3( mantissa,1,value,static_cast<int>(exponent),mantissa );
  validateWithParams4( std::abs(reconstructedValue-value)<1e3, static_cast<int>(exponent),mantissa,reconstructedValue,value );

  value = 2.345678e-12;
  peano::heap::decompose( value, exponent, mantissa );
  reconstructedValue = peano::heap::compose(exponent,mantissa);
  //validateEqualsWithParams3( static_cast<int>(exponent),1,value,static_cast<int>(exponent),mantissa );
  //validateEqualsWithParams3( mantissa,1,value,static_cast<int>(exponent),mantissa );
  validateWithParams4( std::abs(reconstructedValue-value)<1e-12, static_cast<int>(exponent),mantissa,reconstructedValue,value );
}


void peano::heap::tests::CompressedFloatingPointNumbersTest::testDecomposeWithChar() {
  double       value;
  double       reconstructedValue;
  char         exponent;
  char         mantissa;

  // Casts here are used to display char as number (otherwise you might get line breaks, e.g.
  value = 1.0;
  peano::heap::decompose( value, exponent, mantissa );
  reconstructedValue = peano::heap::compose(exponent,mantissa);
  //validateEqualsWithParams3( static_cast<int>(exponent),1,value,static_cast<int>(exponent),mantissa );
  //validateEqualsWithParams3( mantissa,1,value,static_cast<int>(exponent),mantissa );
  validateWithParams4( std::abs(reconstructedValue-value)<1e-12, static_cast<int>(exponent),mantissa,reconstructedValue,value );

  value = 0.5;
  peano::heap::decompose( value, exponent, mantissa );
  reconstructedValue = peano::heap::compose(exponent,mantissa);
  //validateEqualsWithParams3( static_cast<int>(exponent),1,value,static_cast<int>(exponent),mantissa );
  //validateEqualsWithParams3( mantissa,1,value,static_cast<int>(exponent),mantissa );
  validateWithParams4( std::abs(reconstructedValue-value)<1e-12, static_cast<int>(exponent),mantissa,reconstructedValue,value );

  value = 0.1;
  peano::heap::decompose( value, exponent, mantissa );
  reconstructedValue = peano::heap::compose(exponent,mantissa);
  //validateEqualsWithParams3( static_cast<int>(exponent),1,value,static_cast<int>(exponent),mantissa );
  //validateEqualsWithParams3( mantissa,1,value,static_cast<int>(exponent),mantissa );
  validateWithParams4( std::abs(reconstructedValue-value)<1e-2, static_cast<int>(exponent),mantissa,reconstructedValue,value );

  value = 0.0;
  peano::heap::decompose( value, exponent, mantissa );
  reconstructedValue = peano::heap::compose(exponent,mantissa);
  //validateEqualsWithParams3( static_cast<int>(exponent),1,value,static_cast<int>(exponent),mantissa );
  //validateEqualsWithParams3( mantissa,1,value,static_cast<int>(exponent),mantissa );
  validateWithParams4( std::abs(reconstructedValue-value)<1e-12, static_cast<int>(exponent),mantissa,reconstructedValue,value );

  value = 2.345678e12;
  peano::heap::decompose( value, exponent, mantissa );
  reconstructedValue = peano::heap::compose(exponent,mantissa);
  //validateEqualsWithParams3( static_cast<int>(exponent),1,value,static_cast<int>(exponent),mantissa );
  //validateEqualsWithParams3( mantissa,1,value,static_cast<int>(exponent),mantissa );
  validateWithParams4( std::abs(reconstructedValue-value)<1e10, static_cast<int>(exponent),mantissa,reconstructedValue,value );

  value = -2.345678e12;
  peano::heap::decompose( value, exponent, mantissa );
  reconstructedValue = peano::heap::compose(exponent,mantissa);
  //validateEqualsWithParams3( static_cast<int>(exponent),1,value,static_cast<int>(exponent),mantissa );
  //validateEqualsWithParams3( mantissa,1,value,static_cast<int>(exponent),mantissa );
  validateWithParams4( std::abs(reconstructedValue-value)<1e10, static_cast<int>(exponent),mantissa,reconstructedValue,value );

  value = 2.345678e-12;
  peano::heap::decompose( value, exponent, mantissa );
  reconstructedValue = peano::heap::compose(exponent,mantissa);
  //validateEqualsWithParams3( static_cast<int>(exponent),1,value,static_cast<int>(exponent),mantissa );
  //validateEqualsWithParams3( mantissa,1,value,static_cast<int>(exponent),mantissa );
  validateWithParams4( std::abs(reconstructedValue-value)<1e-12, static_cast<int>(exponent),mantissa,reconstructedValue,value );
}


void peano::heap::tests::CompressedFloatingPointNumbersTest::testBatchDecomposition() {
  double       value;

  char          exponent[8];
  long int      mantissa[8];
  double        error[8];


  value = 0.0;
  peano::heap::decompose( value, exponent, mantissa,error );
  validateWithParams4( std::abs(peano::heap::compose(exponent[0],mantissa[0])-value)<1e-12, static_cast<int>(exponent[0]),mantissa[0],error[0],peano::heap::compose(exponent[0],mantissa[0]) );
  validateWithParams4( std::abs(peano::heap::compose(exponent[1],mantissa[1])-value)<1e-12, static_cast<int>(exponent[1]),mantissa[1],error[1],peano::heap::compose(exponent[1],mantissa[1]) );
  validateWithParams4( std::abs(peano::heap::compose(exponent[2],mantissa[2])-value)<1e-12, static_cast<int>(exponent[2]),mantissa[2],error[2],peano::heap::compose(exponent[2],mantissa[2]) );
  validateWithParams4( std::abs(peano::heap::compose(exponent[3],mantissa[3])-value)<1e-12, static_cast<int>(exponent[3]),mantissa[3],error[3],peano::heap::compose(exponent[3],mantissa[3]) );
  validateWithParams4( std::abs(peano::heap::compose(exponent[4],mantissa[4])-value)<1e-12, static_cast<int>(exponent[4]),mantissa[4],error[4],peano::heap::compose(exponent[4],mantissa[4]) );
  validateWithParams4( std::abs(peano::heap::compose(exponent[5],mantissa[5])-value)<1e-12, static_cast<int>(exponent[5]),mantissa[5],error[5],peano::heap::compose(exponent[5],mantissa[5]) );
  validateWithParams4( std::abs(peano::heap::compose(exponent[6],mantissa[6])-value)<1e-12, static_cast<int>(exponent[6]),mantissa[6],error[6],peano::heap::compose(exponent[6],mantissa[6]) );
  validateWithParams4( std::abs(peano::heap::compose(exponent[7],mantissa[7])-value)<1e-12, static_cast<int>(exponent[7]),mantissa[7],error[7],peano::heap::compose(exponent[7],mantissa[7]) );


  value = 0.1;
  peano::heap::decompose( value, exponent, mantissa,error );
  validateWithParams4( std::abs(peano::heap::compose(exponent[0],mantissa[0])-value)<1e-2,  static_cast<int>(exponent[0]),mantissa[0],error[0],peano::heap::compose(exponent[0],mantissa[0]) );
  validateWithParams4( std::abs(peano::heap::compose(exponent[1],mantissa[1])-value)<1e-2,  static_cast<int>(exponent[1]),mantissa[1],error[1],peano::heap::compose(exponent[1],mantissa[1]) );
  validateWithParams4( std::abs(peano::heap::compose(exponent[2],mantissa[2])-value)<1e-2,  static_cast<int>(exponent[2]),mantissa[2],error[2],peano::heap::compose(exponent[2],mantissa[2]) );
  validateWithParams4( std::abs(peano::heap::compose(exponent[3],mantissa[3])-value)<1e-2,  static_cast<int>(exponent[3]),mantissa[3],error[3],peano::heap::compose(exponent[3],mantissa[3]) );
  validateWithParams4( std::abs(peano::heap::compose(exponent[4],mantissa[4])-value)<1e-12, static_cast<int>(exponent[4]),mantissa[4],error[4],peano::heap::compose(exponent[4],mantissa[4]) );
  validateWithParams4( std::abs(peano::heap::compose(exponent[5],mantissa[5])-value)<1e-12, static_cast<int>(exponent[5]),mantissa[5],error[5],peano::heap::compose(exponent[5],mantissa[5]) );
  validateWithParams4( std::abs(peano::heap::compose(exponent[6],mantissa[6])-value)<1e-12, static_cast<int>(exponent[6]),mantissa[6],error[6],peano::heap::compose(exponent[6],mantissa[6]) );
  validateWithParams4( std::abs(peano::heap::compose(exponent[7],mantissa[7])-value)<1e-12, static_cast<int>(exponent[7]),mantissa[7],error[7],peano::heap::compose(exponent[7],mantissa[7]) );


  value = 0.01;
  peano::heap::decompose( value, exponent, mantissa,error );
  validateWithParams4( std::abs(peano::heap::compose(exponent[0],mantissa[0])-value)<1e-2,  static_cast<int>(exponent[0]),mantissa[0],error[0],peano::heap::compose(exponent[0],mantissa[0]) );
  validateWithParams4( std::abs(peano::heap::compose(exponent[1],mantissa[1])-value)<1e-2,  static_cast<int>(exponent[1]),mantissa[1],error[1],peano::heap::compose(exponent[1],mantissa[1]) );
  validateWithParams4( std::abs(peano::heap::compose(exponent[2],mantissa[2])-value)<1e-2,  static_cast<int>(exponent[2]),mantissa[2],error[2],peano::heap::compose(exponent[2],mantissa[2]) );
  validateWithParams4( std::abs(peano::heap::compose(exponent[3],mantissa[3])-value)<1e-2,  static_cast<int>(exponent[3]),mantissa[3],error[3],peano::heap::compose(exponent[3],mantissa[3]) );
  validateWithParams4( std::abs(peano::heap::compose(exponent[4],mantissa[4])-value)<1e-12, static_cast<int>(exponent[4]),mantissa[4],error[4],peano::heap::compose(exponent[4],mantissa[4]) );
  validateWithParams4( std::abs(peano::heap::compose(exponent[5],mantissa[5])-value)<1e-12, static_cast<int>(exponent[5]),mantissa[5],error[5],peano::heap::compose(exponent[5],mantissa[5]) );
  validateWithParams4( std::abs(peano::heap::compose(exponent[6],mantissa[6])-value)<1e-12, static_cast<int>(exponent[6]),mantissa[6],error[6],peano::heap::compose(exponent[6],mantissa[6]) );
  validateWithParams4( std::abs(peano::heap::compose(exponent[7],mantissa[7])-value)<1e-12, static_cast<int>(exponent[7]),mantissa[7],error[7],peano::heap::compose(exponent[7],mantissa[7]) );


  value = 0.001;
  peano::heap::decompose( value, exponent, mantissa,error );
  validateWithParams4( std::abs(peano::heap::compose(exponent[0],mantissa[0])-value)<1e-3,  static_cast<int>(exponent[0]),mantissa[0],error[0],peano::heap::compose(exponent[0],mantissa[0]) );
  validateWithParams4( std::abs(peano::heap::compose(exponent[1],mantissa[1])-value)<1e-3,  static_cast<int>(exponent[1]),mantissa[1],error[1],peano::heap::compose(exponent[1],mantissa[1]) );
  validateWithParams4( std::abs(peano::heap::compose(exponent[2],mantissa[2])-value)<1e-3,  static_cast<int>(exponent[2]),mantissa[2],error[2],peano::heap::compose(exponent[2],mantissa[2]) );
  validateWithParams4( std::abs(peano::heap::compose(exponent[3],mantissa[3])-value)<1e-12, static_cast<int>(exponent[3]),mantissa[3],error[3],peano::heap::compose(exponent[3],mantissa[3]) );
  validateWithParams4( std::abs(peano::heap::compose(exponent[4],mantissa[4])-value)<1e-12, static_cast<int>(exponent[4]),mantissa[4],error[4],peano::heap::compose(exponent[4],mantissa[4]) );
  validateWithParams4( std::abs(peano::heap::compose(exponent[5],mantissa[5])-value)<1e-12, static_cast<int>(exponent[5]),mantissa[5],error[5],peano::heap::compose(exponent[5],mantissa[5]) );
  validateWithParams4( std::abs(peano::heap::compose(exponent[6],mantissa[6])-value)<1e-12, static_cast<int>(exponent[6]),mantissa[6],error[6],peano::heap::compose(exponent[6],mantissa[6]) );
  validateWithParams4( std::abs(peano::heap::compose(exponent[7],mantissa[7])-value)<1e-12, static_cast<int>(exponent[7]),mantissa[7],error[7],peano::heap::compose(exponent[7],mantissa[7]) );

}


void peano::heap::tests::CompressedFloatingPointNumbersTest::testDecompose1() {
  double       value;

  char          exponent[8];
  long int      mantissa[8];
  double        error[8];


  value = 0.920446;
  peano::heap::decompose( value, exponent, mantissa,error );
  validateWithParams6(std::abs(peano::heap::compose(exponent[0],mantissa[0])-value)<2e-3, static_cast<int>(exponent[0]),mantissa[0],error[0],peano::heap::compose(exponent[0],mantissa[0]),value,peano::heap::compose(exponent[0],mantissa[0])-value);
  validateWithParams6(std::abs(peano::heap::compose(exponent[1],mantissa[1])-value)<2e-5, static_cast<int>(exponent[1]),mantissa[1],error[1],peano::heap::compose(exponent[1],mantissa[1]),value,peano::heap::compose(exponent[1],mantissa[1])-value);
  validateWithParams6(std::abs(peano::heap::compose(exponent[2],mantissa[2])-value)<4e-8, static_cast<int>(exponent[2]),mantissa[2],error[2],peano::heap::compose(exponent[2],mantissa[2]),value,peano::heap::compose(exponent[2],mantissa[2])-value);

  // [0.00830992,0.0877622,0.0873153

/*
  assertion in file ../picard/mappings/GlueTogether.cpp, line 103 failed: std::abs(validationP->_persistentRecords._x(0) - newParticle._persistentRecords._x(0)) < picard::mappings::TearApartTask::_particleErrorTolerance
  parameter p->toString(): (xBPS:[2,2,3],vBPS:[2,2,2],massBPS:2,cutOffRadiusBPS:2,internalEnergyBPS:2,rhoBPS:2,rhoDhBPS:2,wcountBPS:2,wcountDhBPS:2,divVBPS:2,curlBPS:[2,2,2],cBPS:2,uBPS:2,POrho2BPS:2)
  parameter validationP->toString(): (x:[0.00830992,0.0877622,0.0873153],maxDx:1.79769e+308,movedParticle:New,v:[0,0,0],mass:0.001,cutOffRadius:0.11255,internalEnergy:1.5e-05,rho:6.91205e-310,rhoDh:4.94066e-324,wcount:1.67982e-322,wcountDh:6.95299e-310,divV:0,curl:[0,0,0],c:1.74816e-316,u:6.95299e-310,POrho2:2.2302e-317)
  parameter newParticle.toString(): (x:[0.13323,0.0878547,0.0873159],maxDx:0,movedParticle:undefined,v:[0,0,0],mass:0.001,cutOffRadius:0.11255,internalEnergy:1.5e-05,rho:6.91205e-310,rhoDh:4.94066e-324,wcount:1.67982e-322,wcountDh:6.95299e-310,divV:0,curl:[0,0,0],c:1.74816e-316,u:6.95299e-310,POrho2:2.2302e-317)
  parameter referenceParticleForConversion.toString(): (x:[0.0468039,0.0463508,0.0461382],maxDx:0,movedParticle:New,v:[0,0,0],mass:0.001,cutOffRadius:0.11255,internalEnergy:1.5e-05,rho:6.91205e-310,rhoDh:4.94066e-324,wcount:1.67982e-322,wcountDh:6.95299e-310,divV:0,curl:[0,0,0],c:1.74816e-316,u:6.95299e-310,POrho2:2.2302e-317)
  parameter hierarchicalParticle.toString(): (x:[0.0864258,0.0415039,0.0411777],maxDx:6.95299e-310,movedParticle:New,v:[0,0,0],mass:0,cutOffRadius:4.16334e-17,internalEnergy:0,rho:0,rhoDh:0,wcount:0,wcountDh:0,divV:0,curl:[0,0,0],c:0,u:0,POrho2:0)
  parameter fineGridVertex.toString(): (pidtVertexIndex:61,heapEntryOfReferenceParticles:62,heapEntryOfPackingTaskFlag:1,heapEntryOfValidationParticles:63,isHangingNode:0,refinementControl:Unrefined,adjacentCellsHeight:0,adjacentCellsHeightOfPreviousIteration:0,numberOfAdjacentRefinedCells:0,insideOutsideDomain:Boundary,x:[0,0,0],level:2)
  src-parco2015: ../picard/mappings/GlueTogether.cpp:103: void picard::mappings::GlueTogether::touchVertexFirstTime(picard::Vertex&, const tarch::la::Vector<3, double>&, const tarch::la::Vector<3, double>&, picard::Vertex*, const peano::grid::VertexEnumerator&, picard::Cell&, const tarch::la::Vector<3, int>&): Assertion `false' failed.
*/

  value = 0.00830992 - 0.0468039;
  peano::heap::decompose( value, exponent, mantissa, error );
  validateWithParams6(error[0]<1e-4, static_cast<int>(exponent[0]),mantissa[0],error[0],peano::heap::compose(exponent[0],mantissa[0]),value,peano::heap::compose(exponent[0],mantissa[0])-value);
  // bps=2 i.e. two chars sufficient

  char*     pMantissa = reinterpret_cast<char*>( &(mantissa[0]) );

  long int  reconstructedMantissa;
  char*     pReconstructedMantissa = reinterpret_cast<char*>( &reconstructedMantissa );
  pReconstructedMantissa[0] = *pMantissa;

  const double gluedValue    = peano::heap::compose(exponent[0],reconstructedMantissa,1);
  validateWithParams5(std::abs(gluedValue-value)<1e-4, value, gluedValue, mantissa[0], (int)(exponent[0]), reconstructedMantissa );
}


void peano::heap::tests::CompressedFloatingPointNumbersTest::testErrorComputation() {
  double       value;

  char          exponent[8];
  long int      mantissa[8];
  double        error[8];

  value = 0.920446;
  peano::heap::decompose( value, exponent, mantissa,error );
  validateNumericalEquals( std::abs(peano::heap::compose(exponent[0],mantissa[0])-value), error[0] );
}


#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",on)
#endif
