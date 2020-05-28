// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_GRID_TESTS_GRID_CONTROL_EVENT_TEST_H_
#define _PEANO4_GRID_TESTS_GRID_CONTROL_EVENT_TEST_H_

#include "tarch/tests/TestCase.h"
#include "tarch/logging/Log.h"


namespace peano4 {
  namespace grid {
    namespace tests {
      class GridControlEventTest;
    }
  }
}


/**
 * The GridControlEvent is a generated type, so we don't really test
 * this type. We test the routines around it which are held in grid.h.
 */
class peano4::grid::tests::GridControlEventTest: public tarch::tests::TestCase {
  private:
    /**
     * Logging device
     */
    static tarch::logging::Log _log;

    void testConsolidate();

  public:
    GridControlEventTest();
    void run() override;
};


#endif
