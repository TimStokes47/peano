// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#pragma once


#include "tarch/tests/TestCase.h"


namespace peano4 {
  namespace datamanagement {
    namespace tests {
      class DataMovementTest;
    } // namespace tests
  }   // namespace datamanagement
} // namespace peano4


/**
 * Test the function templates of VertexDataMovement.h, FaceDataMovement.h
 * and CellDataMovement.h
 *
 * The tests use a small hand-written data type holding one integer, and
 * an peano4::maps::STDStackMap over peano4::stacks::STDVectorStack. They
 * build the traversal events by hand, i.e. they decide from which stack
 * each vertex, face or cell is taken and to which stack it is written,
 * and validate that the data ends up where the event says.
 */
class peano4::datamanagement::tests::DataMovementTest:
  public tarch::tests::TestCase {
private:
  /**
   * Load the 2^d vertices of a cell from the input stack, a temporary
   * stack and as new vertex, then store them to the output stack, a
   * temporary stack, or drop them. Repeat the store with a flag which
   * does not store persistently: the data on the output stack is then
   * dropped, the data on temporary stacks is not.
   */
  void testVertexLoadAndStore();

  /**
   * Same as testVertexLoadAndStore() for the 2d faces of a cell.
   */
  void testFaceLoadAndStore();

  /**
   * Load a cell from the input stack and store it to the output stack.
   * Load a new cell and drop it.
   */
  void testCellLoadAndStore();

  /**
   * Send vertex data subject to the send predicate, the fork and join
   * contexts, and the store flag. Receive vertex data subject to the
   * receive predicate and merge it.
   */
  void testVertexSendAndReceive();

  /**
   * Same as testVertexSendAndReceive() for cells.
   */
  void testCellSendAndReceive();

public:
  DataMovementTest();

  void run() override;
};
