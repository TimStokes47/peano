// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#include "DataMovementTest.h"

// STDStackMap.h uses peano4::parallel::Node without including it, so Node.h
// has to come first. clang-format would sort it behind STDStackMap.h.
// clang-format off
#include "peano4/parallel/Node.h"
// clang-format on

#include "peano4/datamanagement/CellDataMovement.h"
#include "peano4/datamanagement/FaceDataMovement.h"
#include "peano4/datamanagement/VertexDataMovement.h"
#include "peano4/grid/GridTraversalEvent.h"
#include "peano4/grid/LoadStoreComputeFlag.h"
#include "peano4/grid/PeanoCurve.h"
#include "peano4/grid/TraversalObserver.h"
#include "peano4/maps/STDStackMap.h"
#include "peano4/stacks/STDVectorStack.h"
#include "peano4/utils/Globals.h"
#include "tarch/la/Vector.h"
#include "tarch/tests/TestMacros.h"

#include <sstream>
#include <string>


namespace {
  using Context = peano4::grid::TraversalObserver::SendReceiveContext;
  using Flag    = peano4::grid::LoadStoreComputeFlag;

  constexpr int Tree           = 0;
  constexpr int CallStack      = peano4::grid::PeanoCurve::CallStack;
  constexpr int InputStack     = 1;
  constexpr int OutputStack    = 2;
  constexpr int TemporaryStack = 3;
  constexpr int OtherStack     = 4;
  constexpr int Create         = peano4::grid::TraversalObserver::
    CreateOrDestroyPersistentGridEntity;

  /**
   * Minimal data type which offers what the data movement routines use
   */
  struct TestData {
    int value  = -1;
    int merges = 0;
    tarch::la::Vector<Dimensions, double>
      debugX = tarch::la::Vector<Dimensions, double>(0.0);
    tarch::la::Vector<Dimensions, double>
      debugH = tarch::la::Vector<Dimensions, double>(0.0);

    // Required by peano4::stacks::STDVectorStack::pushBlock(), as for generated
    // data types
    enum ObjectConstruction { NoData };

    TestData() = default;
    explicit TestData(ObjectConstruction) {}
    explicit TestData(int value_):
      value(value_) {}

    void setDebugX(const tarch::la::Vector<Dimensions, double>& x) {
      debugX = x;
    }
    void setDebugH(const tarch::la::Vector<Dimensions, double>& h) {
      debugH = h;
    }
    tarch::la::Vector<Dimensions, double> getDebugX() const { return debugX; }
    tarch::la::Vector<Dimensions, double> getDebugH() const { return debugH; }

    std::string toString() const {
      std::ostringstream msg;
      msg << "(value=" << value << ",merges=" << merges << ")";
      return msg.str();
    }

    template <typename Marker>
    void merge(Context, const TestData& neighbour, const Marker&, int) {
      value += neighbour.value;
      merges++;
    }
  };

  /**
   * Stack map which frees its stacks, as STDStackMap insists on clear()
   * before it is destroyed
   */
  struct Stacks:
    public peano4::maps::STDStackMap<peano4::stacks::STDVectorStack<TestData>> {
    ~Stacks() { clear(); }
  };

  peano4::grid::GridTraversalEvent createEvent() {
    peano4::grid::GridTraversalEvent event;
    event.setX(tarch::la::Vector<Dimensions, double>(0.5));
    event.setH(tarch::la::Vector<Dimensions, double>(1.0));
    return event;
  }

  /**
   * Functor returning the same flag for any marker
   */
  auto constantFlag(Flag flag) {
    return [flag](const auto&) -> Flag { return flag; };
  }

  constexpr Flag StoreFlag = Flag::
    LoadFromInputStream_ProvideToCalculations_StoreToOutputStream;
  constexpr Flag DiscardFlag = Flag::
    LoadFromInputStream_ProvideToCalculations_Discard;

  /**
   * Data with value and the debug information of the given position
   */
  template <typename Marker>
  TestData createData(int value, const Marker& marker, int position) {
    TestData data(value);
    data.setDebugX(marker.x(position));
    data.setDebugH(marker.h());
    return data;
  }

  int popValue(Stacks& stacks, int stack) {
    return stacks.getForPop(Tree, stack)->pop().value;
  }

  bool isEmpty(Stacks& stacks, int stack) {
    return stacks.getForPush(Tree, stack)->empty();
  }
} // namespace


peano4::datamanagement::tests::DataMovementTest::DataMovementTest():
  TestCase("peano4::datamanagement::tests::DataMovementTest") {}


void peano4::datamanagement::tests::DataMovementTest::run() {
  testMethod(testVertexLoadAndStore);
  testMethod(testFaceLoadAndStore);
  testMethod(testCellLoadAndStore);
  testMethod(testVertexSendAndReceive);
  testMethod(testCellSendAndReceive);
}


void peano4::datamanagement::tests::DataMovementTest::testVertexLoadAndStore() {
  for (Flag storeFlag : {StoreFlag, DiscardFlag}) {
    Stacks                                     stacks;
    peano4::grid::GridTraversalEvent           event = createEvent();
    const peano4::datamanagement::VertexMarker marker(event);

    // Vertex 0 is new, odd vertices come from the input stack, the others
    // from a temporary stack. The stacks are popped in the order of the
    // vertices, so we push in reverse order.
    for (int i = 0; i < TwoPowerD; i++) {
      event.setVertexDataFrom(
        i,
        i == 0 ? Create : (i % 2 == 1 ? InputStack : TemporaryStack)
      );
      event.setVertexDataTo(i, i);
    }
    for (int i = TwoPowerD - 1; i > 0; i--) {
      stacks.getForPush(Tree, i % 2 == 1 ? InputStack : TemporaryStack)
        ->push(createData(100 + i, marker, i));
    }

    peano4::datamanagement::loadVertexData<
      TestData>(stacks, Tree, event, marker, "test", constantFlag(StoreFlag));

    validate(isEmpty(stacks, InputStack));
    validate(isEmpty(stacks, TemporaryStack));
    for (int i = 0; i < TwoPowerD; i++) {
      const int value = stacks.getForPop(Tree, CallStack)
                          ->top(TwoPowerD - 1 - i)
                          .value;
      const int expected = i == 0 ? -1 : 100 + i;
      validateEqualsWithParams1(value, expected, i);
    }

    // Vertex 0 is destroyed, odd vertices go to the output stack, the others
    // to a temporary stack
    for (int i = 0; i < TwoPowerD; i++) {
      event.setVertexDataFrom(i, i);
      event.setVertexDataTo(
        i,
        i == 0 ? Create : (i % 2 == 1 ? OutputStack : OtherStack)
      );
    }

    peano4::datamanagement::storeVertexData<
      TestData>(stacks, Tree, event, marker, "test", constantFlag(storeFlag));

    validate(isEmpty(stacks, CallStack));
    for (int i = TwoPowerD - 1; i > 0; i--) {
      if (i % 2 == 0) {
        validateEqualsWithParams1(popValue(stacks, OtherStack), 100 + i, i);
      } else if (storeFlag == StoreFlag) {
        validateEqualsWithParams1(popValue(stacks, OutputStack), 100 + i, i);
      }
    }
    validate(isEmpty(stacks, OutputStack));
    validate(isEmpty(stacks, OtherStack));
  }
}


void peano4::datamanagement::tests::DataMovementTest::testFaceLoadAndStore() {
  for (Flag storeFlag : {StoreFlag, DiscardFlag}) {
    Stacks                                   stacks;
    peano4::grid::GridTraversalEvent         event = createEvent();
    const peano4::datamanagement::FaceMarker marker(event);

    for (int i = 0; i < TwoTimesD; i++) {
      event.setFaceDataFrom(
        i,
        i == 0 ? Create : (i % 2 == 1 ? InputStack : TemporaryStack)
      );
      event.setFaceDataTo(i, i);
    }
    for (int i = TwoTimesD - 1; i > 0; i--) {
      stacks.getForPush(Tree, i % 2 == 1 ? InputStack : TemporaryStack)
        ->push(createData(200 + i, marker, i));
    }

    peano4::datamanagement::loadFaceData<
      TestData>(stacks, Tree, event, marker, "test", constantFlag(StoreFlag));

    validate(isEmpty(stacks, InputStack));
    validate(isEmpty(stacks, TemporaryStack));
    for (int i = 0; i < TwoTimesD; i++) {
      const int value = stacks.getForPop(Tree, CallStack)
                          ->top(TwoTimesD - 1 - i)
                          .value;
      const int expected = i == 0 ? -1 : 200 + i;
      validateEqualsWithParams1(value, expected, i);
    }

    for (int i = 0; i < TwoTimesD; i++) {
      event.setFaceDataFrom(i, i);
      event.setFaceDataTo(
        i,
        i == 0 ? Create : (i % 2 == 1 ? OutputStack : OtherStack)
      );
    }

    peano4::datamanagement::storeFaceData<
      TestData>(stacks, Tree, event, marker, "test", constantFlag(storeFlag));

    validate(isEmpty(stacks, CallStack));
    for (int i = TwoTimesD - 1; i > 0; i--) {
      if (i % 2 == 0) {
        validateEqualsWithParams1(popValue(stacks, OtherStack), 200 + i, i);
      } else if (storeFlag == StoreFlag) {
        validateEqualsWithParams1(popValue(stacks, OutputStack), 200 + i, i);
      }
    }
    validate(isEmpty(stacks, OutputStack));
    validate(isEmpty(stacks, OtherStack));
  }
}


void peano4::datamanagement::tests::DataMovementTest::testCellLoadAndStore() {
  Stacks                                   stacks;
  peano4::grid::GridTraversalEvent         event = createEvent();
  const peano4::datamanagement::CellMarker marker(event);

  // Existing cell: from the input stack to the output stack
  TestData cell(300);
  cell.setDebugX(marker.x());
  cell.setDebugH(marker.h());
  stacks.getForPush(Tree, InputStack)->push(cell);

  event.setCellData(InputStack);
  peano4::datamanagement::loadCellData<
    TestData>(stacks, Tree, event, marker, "test", constantFlag(StoreFlag));
  validate(isEmpty(stacks, InputStack));
  validateEquals(stacks.getForPop(Tree, CallStack)->top().value, 300);

  event.setCellData(OutputStack);
  peano4::datamanagement::storeCellData<
    TestData>(stacks, Tree, event, marker, "test", constantFlag(StoreFlag));
  validate(isEmpty(stacks, CallStack));
  validateEquals(popValue(stacks, OutputStack), 300);

  // New cell which is destroyed straightaway
  event.setCellData(Create);
  peano4::datamanagement::loadCellData<
    TestData>(stacks, Tree, event, marker, "test", constantFlag(StoreFlag));
  validateEquals(stacks.getForPop(Tree, CallStack)->top().value, -1);
  peano4::datamanagement::storeCellData<
    TestData>(stacks, Tree, event, marker, "test", constantFlag(StoreFlag));
  validate(isEmpty(stacks, CallStack));
  validate(isEmpty(stacks, OutputStack));
}


void peano4::datamanagement::tests::DataMovementTest::testVertexSendAndReceive(
) {
  Stacks                                     stacks;
  peano4::grid::GridTraversalEvent           event = createEvent();
  const peano4::datamanagement::VertexMarker marker(event);

  // Vertex at position p holds value p+1
  for (int p = 0; p < TwoPowerD; p++) {
    stacks.getForPush(Tree, CallStack)->push(TestData(p + 1));
  }

  auto sendIfValueGreaterOne =
    [](const TestData& data, const peano4::datamanagement::VertexMarker&)
    -> bool { return data.value > 1; };

  // Send subject to the predicate
  peano4::datamanagement::sendVertexData<TestData>(
    stacks,
    Tree,
    1,
    OtherStack,
    Context::BoundaryExchange,
    marker,
    constantFlag(StoreFlag),
    sendIfValueGreaterOne
  );
  validateEquals(popValue(stacks, OtherStack), 2);
  peano4::datamanagement::sendVertexData<TestData>(
    stacks,
    Tree,
    0,
    OtherStack,
    Context::BoundaryExchange,
    marker,
    constantFlag(StoreFlag),
    sendIfValueGreaterOne
  );
  validate(isEmpty(stacks, OtherStack));

  // Forks and joins send regardless of the predicate, but only persistent data
  peano4::datamanagement::sendVertexData<TestData>(
    stacks,
    Tree,
    0,
    OtherStack,
    Context::ForkDomain,
    marker,
    constantFlag(StoreFlag),
    sendIfValueGreaterOne
  );
  validateEquals(popValue(stacks, OtherStack), 1);
  peano4::datamanagement::sendVertexData<TestData>(
    stacks,
    Tree,
    0,
    OtherStack,
    Context::JoinDomain,
    marker,
    constantFlag(DiscardFlag),
    sendIfValueGreaterOne
  );
  validate(isEmpty(stacks, OtherStack));

  // Receive and merge subject to the predicate
  auto receive =
    [](TestData&, const peano4::datamanagement::VertexMarker&) -> bool {
    return true;
  };
  auto doNotReceive =
    [](TestData&, const peano4::datamanagement::VertexMarker&) -> bool {
    return false;
  };

  stacks.getForPush(Tree, TemporaryStack)->push(TestData(10));
  peano4::datamanagement::receiveAndMergeVertexData<TestData>(
    stacks,
    Tree,
    1,
    TemporaryStack,
    Context::BoundaryExchange,
    event,
    marker,
    constantFlag(StoreFlag),
    doNotReceive
  );
  validate(not isEmpty(stacks, TemporaryStack));
  peano4::datamanagement::receiveAndMergeVertexData<TestData>(
    stacks,
    Tree,
    1,
    TemporaryStack,
    Context::BoundaryExchange,
    event,
    marker,
    constantFlag(StoreFlag),
    receive
  );
  validate(isEmpty(stacks, TemporaryStack));
  validateEquals(
    stacks.getForPop(Tree, CallStack)->top(TwoPowerD - 1 - 1).value,
    12
  );
  validateEquals(
    stacks.getForPop(Tree, CallStack)->top(TwoPowerD - 1 - 1).merges,
    1
  );
  validateEquals(
    stacks.getForPop(Tree, CallStack)->top(TwoPowerD - 1).value,
    1
  );
}


void peano4::datamanagement::tests::DataMovementTest::testCellSendAndReceive() {
  Stacks                                   stacks;
  peano4::grid::GridTraversalEvent         event = createEvent();
  const peano4::datamanagement::CellMarker marker(event);

  stacks.getForPush(Tree, CallStack)->push(TestData(5));

  auto send =
    [](const TestData&, const peano4::datamanagement::CellMarker&) -> bool {
    return true;
  };
  auto doNotSend =
    [](const TestData&, const peano4::datamanagement::CellMarker&) -> bool {
    return false;
  };

  peano4::datamanagement::sendCellData<TestData>(
    stacks,
    Tree,
    OtherStack,
    Context::BoundaryExchange,
    marker,
    constantFlag(StoreFlag),
    doNotSend
  );
  validate(isEmpty(stacks, OtherStack));
  peano4::datamanagement::sendCellData<TestData>(
    stacks,
    Tree,
    OtherStack,
    Context::BoundaryExchange,
    marker,
    constantFlag(StoreFlag),
    send
  );
  validateEquals(popValue(stacks, OtherStack), 5);

  auto receive =
    [](TestData&, const peano4::datamanagement::CellMarker&) -> bool {
    return true;
  };
  stacks.getForPush(Tree, TemporaryStack)->push(TestData(7));
  peano4::datamanagement::receiveAndMergeCellData<TestData>(
    stacks,
    Tree,
    TemporaryStack,
    Context::BoundaryExchange,
    event,
    marker,
    constantFlag(DiscardFlag),
    receive
  );
  validate(isEmpty(stacks, TemporaryStack));
  validateEquals(stacks.getForPop(Tree, CallStack)->top().value, 12);
}
