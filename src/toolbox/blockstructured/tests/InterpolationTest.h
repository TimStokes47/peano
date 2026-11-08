// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#pragma once

#include "tarch/tests/TestCase.h"
#include "tarch/logging/Log.h"

namespace toolbox {
  namespace blockstructured {
    namespace tests {
      class InterpolationTest;
    }
  }
}


class toolbox::blockstructured::tests::InterpolationTest: public tarch::tests::TestCase {
  private:

    /**
     * Tests this averaging
     *
     * The test results from a real test with the Euler equations and four
     * unknowns per coordinate axis. The input is as follows:
     *
<pre>

parameter tarch::la::DynamicMatrix::vectorToString(fineGridFaceEulerQUpdate.value,4*2*5): (entries=40,
{
 0.100047, 0.000933957, 0,0,0.00493399,
 0.100045, 0.000870917, 0,0,0.0045799,
 0.100039, 0.000801622, 0,0,0.00422816,
 0.100045, 0.00081321,  0,0,0.0042177,
 0.100047, 0.000933957, 0,0,0.00493399,
 0.100045, 0.000870917, 2.71051e-20,0,0.0045799,
 0.100039, 0.000801622, 0,0,0.00422816,
 0.100045,0.00081321,0,0,0.0042177
})

</pre>
     *
     */
    void testRestrictHaloLayer_AoS_averaging();

    void testRestrictCellForBreakingDam();

    /**
     * Trivial interpolation test for constant data
     */
    void testInterpolateCellDataAssociatedToVolumesIntoOverlappingCell_linear();

    /**
     * Interpolate onto hanging faces and restrict from them for overlaps of
     * one, two and three
     *
     * - Both interpolation schemes have to reproduce a constant coarse face
     *   on both halves of the hanging face.
     * - The linear interpolation has to reproduce a field which is linear
     *   along the face, except next to the ends of the face, where it
     *   extrapolates constantly.
     * - Restricting a constant fine face over all 3^(d-1) subfaces has to
     *   yield this constant on the fine half of the coarse face and leave
     *   the other half untouched.
     */
    void testHangingFacesForAnyOverlap();

    /**
     * Interpolate a coarse patch into its 3^d children
     *
     * The piecewise constant interpolation has to copy the value of the
     * coarse volume which contains a fine volume's centre. The d-linear
     * interpolation has to reproduce a linear field, except next to the
     * boundary of the coarse patch, where it extrapolates constantly.
     */
    void testCellInterpolation();

  public:

    /**
     * Cosntructor.
     */
    InterpolationTest();

    /**
     * Destructor, empty.
     */
    virtual ~InterpolationTest() = default;

    /**
     * This routine is triggered by the TestCaseCollection
     */
    virtual void run() override;
};

