// This file is part of the ExaGRyPE project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#pragma once

#include "tarch/la/Vector.h"

#include "peano4/utils/Globals.h"

#ifdef IncludeTwoPunctures
#include "libtwopunctures/TP_bindding.h"

namespace applications {
  namespace exahype2 {
    namespace ccz4 {
      /**
       * Initial conditions for a (binary) black hole from TwoPunctures
       *
       * TwoPunctures needs GSL, which Peano's build does not support yet.
       * Therefore, this routine is not part of the ExaGRyPE library (see
       * exagrype::initialvalues for the analytical initial conditions), but
       * stays with the application next to libtwopunctures. It is only
       * available if the code is translated with -DIncludeTwoPunctures.
       */
      void ApplyTwoPunctures(
        double* __restrict__                         Q,
        const tarch::la::Vector<Dimensions, double>& X,
        double                                       t,
        TP::TwoPunctures*                            tp,
        bool                                         low_res
      );
    } // namespace ccz4
  } // namespace exahype2
} // namespace applications
#endif
