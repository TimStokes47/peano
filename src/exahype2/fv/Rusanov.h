// This file is part of the ExaHyPE2 project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _EXAHYPE2_FV_RUSANOV_H_
#define _EXAHYPE2_FV_RUSANOV_H_



#include "tarch/la/Vector.h"
#include "peano4/utils/Globals.h"


#include <functional>


namespace exahype2 {
  namespace fv {
    /**
     * Apply simple Rusanov with dimensional splitting
     *
     * We run over the mesh and compute the fluxes through every single face as
     * a 1d problem according to the Rusanov flux
     * @f$
         F(x) = F( \frac{1}{2} (Q_L+Q_R) ) - \lambda_{\mbox{max}} (Q_R - Q_L)
       @f$
     *
     * where @f$ \lambda_{\mbox{max}} @f$  is the maximum over the eigenvalues both of @f$ Q_R @f$  and @f$ Q_L @f$.
     *
     * The outcome of F is added to the solution in Q. The reconstructed array is not
     * touched. So if you find out later that this time step has been too aggressive,
     * then you have to manually backup the patch beforehand.
     *
     * Please consult the guidebook for some documentation of this routine. Some
     * background is given in its ExaHyPE chapter, but the really interesting
     * details are discussed within the Finite Volume chapter.
     */
    void applyRusanovToPatch(
      std::function< void(
        double                                       Q[],
        const tarch::la::Vector<Dimensions,double>&  faceCentre,
        const tarch::la::Vector<Dimensions,double>&  volumeH,
        double                                       t,
        double                                       dt,
        int                                          normal,
        double                                       F[]
      ) >   flux,
      std::function< void(
        double                                       Q[],
        const tarch::la::Vector<Dimensions,double>&  faceCentre,
        const tarch::la::Vector<Dimensions,double>&  volumeH,
        double                                       t,
        double                                       dt,
        int                                          normal,
        double                                       lambdas[]
      ) >   eigenvalues,
      const tarch::la::Vector<Dimensions,double>&  patchCentre,
      const tarch::la::Vector<Dimensions,double>&  patchSize,
      double                                       t,
      double                                       dt,
      int                                          numberOfVolumesPerAxisInPatch,
      int                                          unknowns,
      double                                       Qin[],
      double                                       Qout[]
    );

    void applyRusanovToPatch_FaceLoops2d(
      std::function< void(
        double                                       Q[],
        const tarch::la::Vector<Dimensions,double>&  faceCentre,
        const tarch::la::Vector<Dimensions,double>&  volumeH,
        double                                       t,
        double                                       dt,
        int                                          normal,
        double                                       F[]
      ) >   flux,
      std::function< void(
        double                                       Q[],
        const tarch::la::Vector<Dimensions,double>&  faceCentre,
        const tarch::la::Vector<Dimensions,double>&  volumeH,
        double                                       t,
        double                                       dt,
        int                                          normal,
        double                                       lambdas[]
      ) >   eigenvalues,
      const tarch::la::Vector<Dimensions,double>&  patchCentre,
      const tarch::la::Vector<Dimensions,double>&  patchSize,
      double                                       t,
      double                                       dt,
      int                                          numberOfVolumesPerAxisInPatch,
      int                                          unknowns,
      double                                       Qin[],
      double                                       Qout[]
    );
  }
}


#endif
