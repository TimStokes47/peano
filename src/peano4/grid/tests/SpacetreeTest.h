// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_GRID_TESTS_SPACETREE_TEST_H_
#define _PEANO4_GRID_TESTS_SPACETREE_TEST_H_

#include "tarch/tests/TestCase.h"
#include "tarch/logging/Log.h"


namespace peano4 {
  namespace grid {
    namespace tests {
      class SpacetreeTest;
    }
  }
}


class peano4::grid::tests::SpacetreeTest: public tarch::tests::TestCase {
  private:
    /**
     * Logging device
     */
    static tarch::logging::Log _log;

    void testRestrictToCoarseGrid();
    void testCreateLeaveCellTraversalEvent();
  public:
    void run() override;
};


#endif
