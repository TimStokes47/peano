#include "peano/performanceanalysis/tests/SpeedupLawsTest.h"

#include "peano/performanceanalysis/SpeedupLaws.h"


#include "tarch/tests/TestCaseFactory.h"
registerTest(peano::performanceanalysis::tests::SpeedupLawsTest)


//tarch::logging::Log peano::performanceanalysis::tests::SpeedupLawsTest::_log( "peano::performanceanalysis::tests::SpeedupLawsTest" );


#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",off)
#endif


peano::performanceanalysis::tests::SpeedupLawsTest::SpeedupLawsTest() {
}


peano::performanceanalysis::tests::SpeedupLawsTest::~SpeedupLawsTest() {
}


void peano::performanceanalysis::tests::SpeedupLawsTest::run() {
  testMethod( testAmdahl1 );
}


double peano::performanceanalysis::tests::SpeedupLawsTest::getAmdahlPrediction(double t1, double f, double c) {
  assertion3(t1>0.0,t1,f,c);
  assertion3(f>=0.0,t1,f,c);
  assertion3(f<=1.0,t1,f,c);
  assertion3(c>=1,t1,f,c);
  return t1 * f + (1-f) * t1 / c;
}


void peano::performanceanalysis::tests::SpeedupLawsTest::testAmdahl1() {
  const double t1 = 4.0;
  const double f  = 0.1;

  SpeedupLaws law(0.1);

  for (int i=0; i<SpeedupLaws::Entries; i++) {
    validateNumericalEquals( getAmdahlPrediction(t1,f,1), t1 );
    law.addMeasurement( 1, getAmdahlPrediction(t1,f,1) );
    law.addMeasurement( 2, getAmdahlPrediction(t1,f,2) );
    law.addMeasurement( 3, getAmdahlPrediction(t1,f,3) );
    law.addMeasurement( 4, getAmdahlPrediction(t1,f,4) );
    law.addMeasurement( 5, getAmdahlPrediction(t1,f,5) );
  }

  law.relaxAmdahlsLaw();

  validateEqualsWithParams1(law.getSerialCodeFraction(),f,  law.toString());
  validateEqualsWithParams1(law.getSerialTime(),        t1, law.toString());
}


#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",on)
#endif
