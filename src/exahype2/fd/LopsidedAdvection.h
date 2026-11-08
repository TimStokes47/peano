// This file is part of the ExaHyPE2 project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#pragma once


#include "tarch/la/Vector.h"

#include "peano4/utils/Globals.h"


namespace exahype2 {
  namespace fd {
    /**
     * Add advection terms discretised through lopsided stencils to the right-hand side of a patch
     *
     * Many PDEs carry advection terms
     *
     * @f$ \partial _t Q_q = \ldots + \sum _d v_d \partial _d Q_q @f$
     *
     * with a velocity v which is part of the solution itself, such as the
     * shift in the Einstein equations. Central differences treat these terms
     * like any other derivative. Numerical relativity codes discretise them
     * with lopsided (upwind-biased) fourth-order stencils instead,
     *
     * @f$ \partial _d Q_i \approx \frac{-3 Q_{i-1} - 10 Q_i + 18 Q_{i+1} - 6 Q_{i+2} + Q_{i+3}}{12 \Delta x_d} @f$
     *
     * for @f$ v_d > 0 @f$, and the mirrored stencil for @f$ v_d \leq 0 @f$.
     * For a positive velocity, the solution travels towards smaller
     * coordinates, i.e. its information comes from the right, and the
     * stencil reaches further into this direction. The stencils are
     * fourth-order accurate.
     *
     * The routine adds @f$ v_d \partial _d Q_q @f$ for each axis d and each
     * selected quantity q to the right-hand side of each grid cell of a
     * patch. The PDE terms of the solver must hence not contain these
     * advection terms themselves. The velocity is taken from the grid cell
     * itself.
     *
     * exahype2.api.rkfd4.CentralDifferencesWithRungeKutta invokes this
     * routine after its right-hand side kernel within each Runge-Kutta
     * stage, if the user selects the kernel
     * CentralDifferencesWithLopsidedAdvection.
     *
     * Both arrays are stored lexicographically in AoS format, i.e. all
     * entries of one grid cell are stored together, and the first axis
     * runs fastest. The stencils reach three grid cells into the halo, i.e.
     * haloSize has to be at least three.
     *
     * @param QWithHalo Input of the Runge-Kutta stage with its halo, i.e.
     *   (numberOfGridCellsPerPatchPerAxis+2*haloSize)^d grid cells with
     *   unknowns+auxiliaryVariables entries each.
     * @param rightHandSide Right-hand side of the stage without halo, i.e.
     *   numberOfGridCellsPerPatchPerAxis^d grid cells with unknowns entries
     *   each. The routine adds the advection terms to it.
     * @param patchSize Size of the patch, i.e. of the mesh cell.
     * @param velocity Indices of the Dimensions entries of Q which hold the
     *   velocity components. They may also refer to auxiliary variables.
     * @param quantities Indices of the unknowns which are advected. Each
     *   index has to be smaller than unknowns.
     * @param numberOfQuantities Number of entries of quantities.
     */
    void addLopsidedAdvection(
      const double* __restrict__                         QWithHalo,
      double* __restrict__                               rightHandSide,
      const tarch::la::Vector<Dimensions, double>&       patchSize,
      int                                                numberOfGridCellsPerPatchPerAxis,
      int                                                haloSize,
      int                                                unknowns,
      int                                                auxiliaryVariables,
      const int*                                         velocity,
      const int*                                         quantities,
      int                                                numberOfQuantities
    );
  }
}
