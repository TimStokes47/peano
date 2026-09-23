
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

    virtual void initPointWisely(
      const tarch::la::Vector<Dimensions, double>& x, double* __restrict__ Q
    ) override;

/*
    virtual void boundaryConditions(
      const double * __restrict__ Qinside, // Qinside[4+0]
      double * __restrict__ Qoutside, // Qoutside[4+0]
      const tarch::la::Vector<Dimensions,double>&  faceCentre,
      const tarch::la::Vector<Dimensions,double>&  volumeH,
      double                                       t,
      int                                          normal
    )  override;
*/
};

