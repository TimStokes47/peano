// This file is part of the ExaHyPE2 project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _EXAHYPE2_FV_RUSANOV_H_
#define _EXAHYPE2_FV_RUSANOV_H_



#include "tarch/la/Vector.h"
#include "peano4/utils/Globals.h"


#include <functional>


namespace exahype2 {
  namespace fv {
    void applyRusanovToPatch(
      std::function< void(
        double                                       Q[],
        const tarch::la::Vector<Dimensions,double>&  faceCentre,
        const tarch::la::Vector<Dimensions,double>&  volumeH,
        const tarch::la::Vector<Dimensions,double>&  t,
        const tarch::la::Vector<Dimensions,double>&  dt,
        int                                          normal,
        double                                       F[]
      ) >   flux,
      std::function< void(
        double                                       Q[],
        const tarch::la::Vector<Dimensions,double>&  faceCentre,
        const tarch::la::Vector<Dimensions,double>&  volumeH,
        const tarch::la::Vector<Dimensions,double>&  t,
        const tarch::la::Vector<Dimensions,double>&  dt,
        int                                          normal,
        double                                       lambdas[]
      ) >   eigenvalues,
      const tarch::la::Vector<Dimensions,double>&  patchCentre,
      const tarch::la::Vector<Dimensions,double>&  patchSize,
      double                                       t,
      double                                       dt,
      const tarch::la::Vector<Dimensions,int>&     numberOfVolumesInPatch,
      int                                          haloSize,
      int                                          unknowns,
      double                                       Qin[],
      double                                       Qout[]
    );
  }
}


#endif
