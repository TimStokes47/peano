// This file is part of the ExaHyPE2 project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#pragma once

#include <functional>

#include "peano4/utils/Globals.h"
#include "tarch/la/Vector.h"
#include "tarch/multicore/multicore.h"

namespace exahype2::fv::rusanov {
  /**
   * Volumetric source as used by exahype2.api, i.e. by exahype2.api.VolumetricSource
   *
   * A volumetric source depends on the position and the time stamp only,
   * but neither on the mesh size nor on the time step size.
   */
  typedef std::function<void(
    const double* __restrict__                   Q,
    const tarch::la::Vector<Dimensions, double>& x,
    double                                       t,
    double* __restrict__                         S
  )>
    VolumetricSourceFunctor;


  typedef std::function<void(
    const double* __restrict__ Q,
    const tarch::la::Vector<Dimensions, double>& x,
    int                                          normal,
    double* __restrict__ F
  )>
    FluxFunctor;


  typedef std::function<void(
    const double* __restrict__ Q,
    const double* __restrict__ deltaQ,
    const tarch::la::Vector<Dimensions, double>& x,
    const tarch::la::Vector<Dimensions, double>& h,
    double                                       t,
    double                                       dt,
    int                                          normal,
    double* __restrict__ BTimesDeltaQ
  )>
    NonconservativeProductFunctor;

    
  typedef std::function<
    void(const double* __restrict__ Q, 
      const tarch::la::Vector<Dimensions, double>& x, 
      int normal, 
      double* __restrict__ maxEigenvalue)>
    MaxEigenvalueFunctor;
} // namespace exahype2::fv::rusanov
