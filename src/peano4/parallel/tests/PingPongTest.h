// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_PARALLEL_TESTS_PING_POINT_TEST_H_
#define _PEANO4_PARALLEL_TESTS_PING_POINT_TEST_H_

#include "tarch/tests/TestCaseFactory.h"
#include "tarch/logging/Log.h"


namespace peano4 {
  namespace parallel {
    namespace tests {
      class PingPongTest;
      declareTest(peano4::parallel::tests,PingPongTest)
    }
  }
}


/**
 * This is a ping pong test for our application-specific data types
 */
class peano4::parallel::tests::PingPongTest: public tarch::tests::TestCase {
  private:
    /**
     * Logging device
     */
    static tarch::logging::Log _log;

    void testBuiltInType();
    void testDaStGenType();
    void testDaStGenArray();
  public:
    void run() override;
};


#endif
