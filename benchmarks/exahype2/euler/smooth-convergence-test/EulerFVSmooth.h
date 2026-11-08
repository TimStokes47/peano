
// This file is part of the ExaHyPE2 project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#pragma once

#include "AbstractEulerFVSmooth.h"


namespace benchmarks {
  namespace exahype2 {
  namespace euler {
  namespace smooth_convergence_test {
  
  class EulerFVSmooth;

}}}}


class benchmarks::exahype2::euler::smooth_convergence_test::EulerFVSmooth: public benchmarks::exahype2::euler::smooth_convergence_test::AbstractEulerFVSmooth {
  public:
    EulerFVSmooth() = default;

    virtual void flux(
      const double* __restrict__ Q, const tarch::la::Vector<Dimensions, double>& x, int normal, double* __restrict__ F
    ) override;

    virtual void maxEigenvalue(
     const double* __restrict__ Q, const tarch::la::Vector<Dimensions, double>& x, int normal, double* __restrict__ lambdaMax
    ) override;

    /**
     * Smooth density wave advected in x, constant pressure - same
     * closed-form initial condition the legacy, cell-wise
     * initialCondition() used, just evaluated at the single point x
     * instead of a whole cell's centre.
     */
    virtual void initPointWisely(
      const tarch::la::Vector<Dimensions, double>& x, double* __restrict__ Q
    ) override;

    /**
     * Prescribe the exact solution outside of the domain: The density wave
     * is advected with constant velocity u, i.e. the solution at (x,t)
     * equals the initial condition at x-ut. With these Dirichlet
     * conditions, the numerical solution should converge towards the
     * same exact solution as with periodic boundary conditions.
     */
    virtual void dirichletBoundaryConditions(
      const tarch::la::Vector<Dimensions, double>& x, double t, double* __restrict__ Q
    ) override;
};

