// This file is part of the ExaHyPE2 project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#pragma once


#include "tarch/tests/TestCase.h"


namespace exahype2 {
  namespace blockstructured {
    namespace tests {
      class SommerfeldBoundaryConditionsTest;
    } // namespace tests
  }   // namespace blockstructured
} // namespace exahype2


/**
 * Test exahype2::blockstructured::applySommerfeldBoundaryConditions()
 *
 * All tests work over the domain @f$ [-1,1]^d @f$ around the origin with
 * patches of three grid cells per axis and two unknowns plus one auxiliary
 * variable per grid cell. Unless stated otherwise, the overlap is three,
 * i.e. the one of fourth-order Finite Differences. Entries the routine is
 * not supposed to touch are initialised with a marker value and have to
 * keep it.
 */
class exahype2::blockstructured::tests::SommerfeldBoundaryConditionsTest:
  public tarch::tests::TestCase {
private:
  /**
   * If the old and the new solution equal a constant far-field solution,
   * the ghost cells have to equal it, too, on every boundary face. The
   * auxiliary variable is copied from the new interior solution.
   */
  void testFarFieldIsPreserved();

  /**
   * One ghost cell on the upper boundary along the axis through the
   * origin, where the radial and the normal direction coincide. The new
   * ghost value has to satisfy the discrete box scheme documented in
   * applySommerfeldBoundaryConditions(), i.e. its residual has to vanish.
   * The other unknowns of the face are not on the axis and are not
   * checked.
   */
  void testBoxSchemeOnAxis();

  /**
   * The lower and the upper boundary face of a domain around the origin
   * are mirror images of each other. With mirrored data, they have to
   * yield the same ghost values. This test fails for a scheme that uses
   * the signed coordinate along the normal, as the removed legacy
   * exahype2::fd::applySommerfeldConditions() did.
   */
  void testLowerAndUpperFacesAreSymmetric();

  /**
   * A time step size of zero copies the old ghost cells.
   */
  void testZeroTimeStepSizeCopiesGhostCells();

  /**
   * Faces within the domain and boundary faces along periodic axes are
   * left unchanged.
   */
  void testInteriorAndPeriodicFacesAreSkipped();

public:
  SommerfeldBoundaryConditionsTest();

  void run() override;
};
