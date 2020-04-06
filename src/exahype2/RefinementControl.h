// This file is part of the ExaHyPE2 project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _EXAHYPE2_REFINEMENT_CONTROL_H_
#define _EXAHYPE2_REFINEMENT_CONTROL_H_


namespace exahype2 {
  enum class RefinementControl {
    Keep, Refine, Coarsen
  };
}


/**
 * If one of the flags says refine, then refine. If one of the two flags says keep and
 * noone says refine, return keep. Coarsen if and only if both say coarsen.
 */
exahype2::RefinementControl operator&&( exahype2::RefinementControl lhs, exahype2::RefinementControl rhs);


#endif
