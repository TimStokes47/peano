#include "peano/datatraversal/autotuning/OracleForOnePhase.h"
#include "tarch/Assertions.h"


std::string peano::datatraversal::autotuning::toString( const MethodTrace& methodTrace ) {
  switch (methodTrace) {
    case LoadVertices:
      return "load-vertices";
    case LoadVerticesOnRegularStationaryGrid:
      return "load-vertices-on-regular-stationary-grid";
    case LoadVerticesOnIrregularStationaryGrid:
      return "load-vertices-on-irregular-stationary-grid";
    case LoadVerticesOnStationaryGridWithParallelBoundary:
      return "load-vertices-on-stationary-with-parallel-boundary";
    case StoreVertices:
      return "store-vertices";
    case StoreVerticesOnIrregularStationaryGrid:
      return "store-vertices-on-irregular-stationary-grid";
    case StoreVerticesOnRegularStationaryGrid:
      return "store-vertices-on-regular-stationary-grid";
    case CallEnterCellAndLoadSubCellsWithinTraverse:
      return "enter-cell-and-load-sub-cells";
    case CallLeaveCellAndStoreSubCellsWithinTraverse:
      return "leave-cell-and-store-sub-cells";
    case CallEnterCellAndInitialiseEnumeratorsOnRegularStationaryGrid:
      return "call-enter-cell-and-initialise-enumerators-on-regular-stationary-grid";
    case CallTouchFirstTimeOnRegularStationaryGrid:
      return "call-touch-first-time-on-regular-stationary-grid";
    case CallTouchLastTimeOnRegularStationaryGrid:
      return "call-touch-last-time-on-regular-stationary-grid";
    case CallEnterCellOnRegularStationaryGrid:
      return "call-enter-cell-on-regular-stationary-grid";
    case CallLeaveCellOnRegularStationaryGrid:
      return "call-leave-cell-on-regular-stationary-grid";
    case PipelineAscendTask:
      return "pipeline-ascend-task";
    case PipelineDescendTask:
      return "pipeline-descend-task";
    case SplitLoadVerticesTaskOnRegularStationaryGrid:
      return "split-load-vertices-task-on-regular-stationary-grid";
    case SplitStoreVerticesTaskOnRegularStationaryGrid:
      return "split-store-vertices-task-on-regular-stationary-grid";
    case AscendOnRegularStationaryGrid:
      return "ascend-on-regular-stationary-grid";
    case DescendOnRegularStationaryGrid:
      return "descend-on-regular-stationary-grid";
    case NumberOfDifferentMethodsCalling:
      return "<error>";
  }

  return "<error>";
}


peano::datatraversal::autotuning::MethodTrace peano::datatraversal::autotuning::toMethodTrace(int value) {
  assertion( value>=0 );
  assertion( value<NumberOfDifferentMethodsCalling );
  switch (value) {
    case static_cast<int>(LoadVertices):
      return LoadVertices;
    case static_cast<int>(LoadVerticesOnRegularStationaryGrid):
      return LoadVerticesOnRegularStationaryGrid;
    case static_cast<int>(LoadVerticesOnIrregularStationaryGrid):
      return LoadVerticesOnIrregularStationaryGrid;
    case static_cast<int>(LoadVerticesOnStationaryGridWithParallelBoundary):
      return LoadVerticesOnStationaryGridWithParallelBoundary;
    case static_cast<int>(StoreVertices):
      return StoreVertices;
    case static_cast<int>(StoreVerticesOnRegularStationaryGrid):
      return StoreVerticesOnRegularStationaryGrid;
    case static_cast<int>(StoreVerticesOnIrregularStationaryGrid):
      return StoreVerticesOnIrregularStationaryGrid;
    case static_cast<int>(CallEnterCellAndLoadSubCellsWithinTraverse):
      return CallEnterCellAndLoadSubCellsWithinTraverse;
    case static_cast<int>(CallLeaveCellAndStoreSubCellsWithinTraverse):
      return CallLeaveCellAndStoreSubCellsWithinTraverse;
    case static_cast<int>(CallEnterCellAndInitialiseEnumeratorsOnRegularStationaryGrid):
      return CallEnterCellAndInitialiseEnumeratorsOnRegularStationaryGrid;
    case static_cast<int>(CallTouchFirstTimeOnRegularStationaryGrid):
      return CallTouchFirstTimeOnRegularStationaryGrid;
    case static_cast<int>(CallTouchLastTimeOnRegularStationaryGrid):
      return CallTouchLastTimeOnRegularStationaryGrid;
    case static_cast<int>(CallEnterCellOnRegularStationaryGrid):
      return CallEnterCellOnRegularStationaryGrid;
    case static_cast<int>(CallLeaveCellOnRegularStationaryGrid):
      return CallLeaveCellOnRegularStationaryGrid;
    case static_cast<int>(PipelineAscendTask):
      return PipelineAscendTask;
    case static_cast<int>(PipelineDescendTask):
      return PipelineDescendTask;
    case static_cast<int>(SplitLoadVerticesTaskOnRegularStationaryGrid):
      return SplitLoadVerticesTaskOnRegularStationaryGrid;
    case static_cast<int>(SplitStoreVerticesTaskOnRegularStationaryGrid):
      return SplitStoreVerticesTaskOnRegularStationaryGrid;
    case static_cast<int>(AscendOnRegularStationaryGrid):
      return AscendOnRegularStationaryGrid;
    case static_cast<int>(DescendOnRegularStationaryGrid):
      return DescendOnRegularStationaryGrid;
    case static_cast<int>(NumberOfDifferentMethodsCalling):
      return NumberOfDifferentMethodsCalling;
  }

  return NumberOfDifferentMethodsCalling;
}
