// This file is part of the ExaGRyPE project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#include "ApplyTwoPunctures.h"

#ifdef IncludeTwoPunctures
void applications::exahype2::ccz4::ApplyTwoPunctures(
  double* __restrict__                         Q,
  const tarch::la::Vector<Dimensions, double>& X,
  double                                       t,
  TP::TwoPunctures*                            tp,
  bool                                         low_res
) {
  constexpr int nVars = 59;
  const double  coor[3] = {X[0], X[1], X[2]};
  double        LgradQ[3 * nVars];
  tarch::memset(Q, .0, nVars * sizeof(double));
  tp->Interpolate(coor, Q, low_res);
  // calculate corresponding soccz4 quantities
  TP_bindding::SOCCZ4Cal(Q);
  // calculate gradient for auxiliary variables
  TP_bindding::GradientCal(coor, Q, LgradQ, nVars, tp, low_res);
  // calculate the auxiliary variables
  TP_bindding::AuxiliaryCal(Q, LgradQ, nVars);
}
#endif
