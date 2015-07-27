#include "peano/heap/tests/TestCase.h"

#include "peano/heap/CompressedFloatingPointNumbers.h"

#include "tarch/compiler/CompilerSpecificSettings.h"
#include "tarch/tests/TestCaseFactory.h"
registerTest(picard::tests::TestCase)


#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",off)
#endif

 
picard::tests::TestCase::TestCase():
  tarch::tests::TestCase( "picard::tests::TestCase" ) {
}


picard::tests::TestCase::~TestCase() {
}


void picard::tests::TestCase::run() {
  testMethod( testDecomposeWithLongInt );
  testMethod( testDecomposeWithInt );
  testMethod( testDecomposeWithChar );
  testMethod( testBatchDecomposition );
}


/**
 * @todo Gehoert in den Heap
 */
void picard::tests::TestCase::testDecomposeWithLongInt() {
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


void picard::tests::TestCase::testDecomposeWithInt() {
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


void picard::tests::TestCase::testDecomposeWithChar() {
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

void picard::tests::TestCase::testBatchDecomposition() {
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


#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",on)
#endif
