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

    /**
     * From a debugging session:
     *
     <pre>

 69568402     00:00:00     rank:0       core:0       info         peano4::grid::consolidate(...)                                (refinementControl=Refine,offset=[-0.0333333,-0.0333333],width=[0.4,0.4],h=[0.111111,0.111111])
 69600708     00:00:00     rank:0       core:0       info         peano4::grid::consolidate(...)                                (refinementControl=Refine,offset=[0.3,-0.0333333],width=[0.4,0.4],h=[0.111111,0.111111])
 69630098     00:00:00     rank:0       core:0       info         peano4::grid::consolidate(...)                                (refinementControl=Refine,offset=[0.633333,-0.0333333],width=[0.4,0.4],h=[0.111111,0.111111])
 69658414     00:00:00     rank:0       core:0       info         peano4::grid::consolidate(...)                                (refinementControl=Refine,offset=[0.633333,0.3],width=[0.4,0.4],h=[0.111111,0.111111])
 69739708     00:00:00     rank:0       core:0       info         peano4::grid::consolidate(...)                                (refinementControl=Refine,offset=[0.3,0.3],width=[0.4,0.4],h=[0.111111,0.111111])
 69769194     00:00:00     rank:0       core:0       info         peano4::grid::consolidate(...)                                (refinementControl=Refine,offset=[-0.0333333,0.3],width=[0.4,0.4],h=[0.111111,0.111111])
 69797019     00:00:00     rank:0       core:0       info         peano4::grid::consolidate(...)                                (refinementControl=Refine,offset=[-0.0333333,0.633333],width=[0.4,0.4],h=[0.111111,0.111111])
 69824642     00:00:00     rank:0       core:0       info         peano4::grid::consolidate(...)                                (refinementControl=Refine,offset=[0.3,0.633333],width=[0.4,0.4],h=[0.111111,0.111111])
 69852630     00:00:00     rank:0       core:0       info         peano4::grid::consolidate(...)                                (refinementControl=Refine,offset=[0.633333,0.633333],width=[0.4,0.4],h=[0.111111,0.111111])
 69894892     00:00:00     rank:0       core:0       info         peano4::grid::consolidate(...)                                1: (refinementControl=Refine,offset=[0.633333,0.3],width=[0.4,0.733333],h=[0.111111,0.111111])

     </pre>
     */
    void testMerge();

  public:
    GridControlEventTest();
    void run() override;
};


#endif
