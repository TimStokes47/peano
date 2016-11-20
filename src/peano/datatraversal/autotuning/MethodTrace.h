// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_DATA_TRAVERSAL_AUTOTUNING_METHOD_TRACE_H_
#define _PEANO_DATA_TRAVERSAL_AUTOTUNING_METHOD_TRACE_H_


#include <string>


namespace peano {
  namespace datatraversal {
    namespace autotuning {
      enum class MethodTrace {
        LoadVertices                                     = 0,
        LoadVerticesOnRegularStationaryGrid              = 1, // not unrolled
        LoadVerticesOnIrregularStationaryGrid            = 2,
        StoreVertices                                    = 3,
        StoreVerticesOnRegularStationaryGrid             = 4, // not unrolled
        StoreVerticesOnIrregularStationaryGrid           = 5,
        CallEnterCellAndLoadSubCellsWithinTraverse       = 6,
        CallLeaveCellAndStoreSubCellsWithinTraverse      = 7,
        CallEnterCellAndInitialiseEnumeratorsOnRegularStationaryGrid = 8,
        CallTouchFirstTimeOnRegularStationaryGrid        = 9,
        CallTouchLastTimeOnRegularStationaryGrid         = 10,
        CallEnterCellOnRegularStationaryGrid             = 11,
        CallLeaveCellOnRegularStationaryGrid             = 12,
        PipelineAscendTask                               = 13,
        PipelineDescendTask                              = 14,
        SplitLoadVerticesTaskOnRegularStationaryGrid     = 15,
        SplitStoreVerticesTaskOnRegularStationaryGrid    = 16,
        AscendOnRegularStationaryGrid                    = 17,
        DescendOnRegularStationaryGrid                   = 18,

        UserDefined0  = 19,
        UserDefined1  = 20,
        UserDefined2  = 21,
        UserDefined3  = 22,
        UserDefined4  = 23,
        UserDefined5  = 24,
        UserDefined6  = 25,
        UserDefined7  = 26,
        UserDefined8  = 27,
        UserDefined9  = 28,
        UserDefined10 = 29,
        UserDefined11 = 30,
        UserDefined12 = 31,

        NumberOfDifferentMethodsCalling                  = 32
      };

      std::string toString( const MethodTrace& methodTrace );
      MethodTrace toMethodTrace(int value);
}
  }
}



#endif
