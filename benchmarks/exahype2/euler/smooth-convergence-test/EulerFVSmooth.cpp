#include "EulerFVSmooth.h"

void benchmarks::exahype2::euler::smooth_convergence_test::EulerFVSmooth::initPointWisely(
  const tarch::la::Vector<Dimensions, double>& x, double* __restrict__ Q
) {
  logTraceInWith1Argument( "initPointWisely(...)", x );
  constexpr double gamma = 1.4;
  constexpr double p = 1.0;
  Q[0] = 1.0 + 0.2 * std::cos((2.0 * x[0]) * 3.1415);
  Q[1] = 1.0;
  Q[2] = 0.0;
  Q[3] = p / (gamma - 1) + 0.5 / Q[0] * (Q[1] * Q[1]);
  logTraceOut( "initPointWisely(...)" );
}

/*
void benchmarks::exahype2::euler::smooth_convergence_test::EulerFVSmooth::boundaryConditions(
  const double * __restrict__                  Qinside, // Qinside[4+0]
  double * __restrict__                        Qoutside, // Qoutside[4+0]
  const tarch::la::Vector<Dimensions,double>&  faceCentre,
  const tarch::la::Vector<Dimensions,double>&  volumeH,
  double                                       t,
  int                                          normal
) {
  logTraceInWith4Arguments( "boundaryConditions(...)", faceCentre, volumeH, t, normal );
  Qoutside[0] = Qinside[0];
  Qoutside[1] = Qinside[1];
  Qoutside[2] = Qinside[2];
  Qoutside[3] = Qinside[3];
  logTraceOut( "boundaryConditions(...)" );
}
*/

void benchmarks::exahype2::euler::smooth_convergence_test::EulerFVSmooth::flux(
  const double* __restrict__ Q, const tarch::la::Vector<Dimensions, double>& x, int normal, double* __restrict__ F
) {
  const double irho  = 1.0 / Q[0];
  constexpr double gamma = 1.4;
  const double p = (gamma - 1.0) * (Q[3] - 0.5 * irho * (Q[1] * Q[1] + Q[2] * Q[2]));

  switch (normal) {
    case 0:
      F[0] = Q[1];
      F[1] = irho * Q[1] * Q[1] + p;
      F[2] = irho * Q[1] * Q[2];
      F[3] = irho * Q[1] * (Q[3] + p);
      break;
    case 1:
      F[0] = Q[2];
      F[1] = irho * Q[2] * Q[1];
      F[2] = irho * Q[2] * Q[2] + p;
      F[3] = irho * Q[2] * (Q[3] + p);
      break;
  }
}

void benchmarks::exahype2::euler::smooth_convergence_test::EulerFVSmooth::maxEigenvalue(
  const double* __restrict__ Q, const tarch::la::Vector<Dimensions, double>& x, int normal, double* __restrict__ lambdaMax
) {
  logTraceInWith2Arguments( "maxEigenvalue(...)", x, normal );
  const double irho = 1.0 / Q[0];
  const double gamma = 1.4;
  const double p = (gamma - 1) * (Q[3] - 0.5 * irho * (Q[1] * Q[1] + Q[2] * Q[2]));
  const double c = std::sqrt(gamma * p * irho);

  // Eigenvalues of the Euler flux Jacobian in direction normal are u_n +/- c,
  // where u_n is the velocity (momentum/density), not the raw momentum
  const double u = Q[1 + normal] * irho;

  const double result = std::max(std::abs(u + c), std::abs(u - c));
  lambdaMax[0] = result;
  logTraceOutWith1Argument("maxEigenvalue(...)", result);
}
