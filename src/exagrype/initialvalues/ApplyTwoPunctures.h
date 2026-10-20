// This file is part of the ExaGRyPE project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#pragma once

#include "config.h"

#include "tarch/la/Vector.h"

#include "peano4/utils/Globals.h"

#if defined(IncludeTwoPunctures) and not defined(UseGSL)
#error "TwoPunctures initial data requires GSL: configure Peano with --with-gsl (or -DWITH_GSL=ON)"
#endif

#ifdef UseGSL
#include "exagrype/twopunctures/TP_bindding.h"

namespace exagrype {
  namespace initialvalues {
    /**
     * Initial conditions for a (binary) black hole from TwoPunctures
     *
     * TwoPunctures (namespaces TP and TP_bindding in src/exagrype/twopunctures)
     * is taken from the EinsteinToolkit and needs GSL. Therefore, it is only
     * part of the ExaGRyPE library if Peano is configured with --with-gsl
     * (-DWITH_GSL=ON for CMake), which defines UseGSL.
     *
     * Applications that use this routine typically guard their TwoPunctures
     * code with IncludeTwoPunctures, so they can switch between scenarios
     * per build. If an application sets IncludeTwoPunctures but Peano lacks
     * GSL, this header aborts the translation with a meaningful error.
     */
    void ApplyTwoPunctures(
      double* __restrict__                         Q,
      const tarch::la::Vector<Dimensions, double>& X,
      double                                       t,
      TP::TwoPunctures*                            tp,
      bool                                         low_res
    );
  } // namespace initialvalues
} // namespace exagrype
#endif
