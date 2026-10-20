// This file is part of the ExaGRyPE project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#pragma once

#include "tarch/tests/TestCase.h"


namespace exagrype {
  namespace ccz4 {
    namespace tests {
      class FlatSpacetimeTest;
    } // namespace tests
  } // namespace ccz4
} // namespace exagrype


/**
 * Sanity checks of the CCZ4 kernels against flat (Minkowski) spacetime
 *
 * Flat spacetime with a unit lapse and zero shift is a stationary solution
 * of the CCZ4 equations. If we take exagrype::initialvalues::flat() and set
 * all gradients to zero, both the non-conservative product and the source
 * therefore have to vanish. The test furthermore checks the maximum
 * eigenvalue estimate for flat spacetime, and that the gauge wave initial
 * data yields the analytically known lapse.
 */
class exagrype::ccz4::tests::FlatSpacetimeTest: public tarch::tests::TestCase {
private:
  void testNonconservativeProductVanishes();
  void testSourceVanishes();
  void testMaxEigenvalueOfFlatSpacetime();
  void testGaugeWaveLapse();

public:
  FlatSpacetimeTest();

  virtual void run() override;
};
