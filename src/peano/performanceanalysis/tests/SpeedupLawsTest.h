// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef PEANO_PERFORMANCE_ANALYSIS_TESTS_SPEEDUP_LAWS_H_
#define PEANO_PERFORMANCE_ANALYSIS_TESTS_SPEEDUP_LAWS_H_

#include "peano/utils/Globals.h"
#include "tarch/tests/TestCase.h"

namespace peano {
    namespace performanceanalysis {
      namespace tests {
        class SpeedupLawsTest;
      }
    }
}

class peano::performanceanalysis::tests::SpeedupLawsTest: public tarch::tests::TestCase {
  private:
   /**
    * Logging device
    */
//   static tarch::logging::Log _log;

   /**
    * This test divides a 4x4 patch of vertices into the appropriate ranges, using a grain size of 1.
    * Thus, the result should be to have 16 ranges, each holding one vertex.
    */
   void testAmdahl1();

   double getAmdahlPrediction(double t1, double f, double c);
 public:
   SpeedupLawsTest();
   virtual ~SpeedupLawsTest();

   virtual void run();
};

#endif
