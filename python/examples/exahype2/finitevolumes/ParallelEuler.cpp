#include "ParallelEuler.h"


tarch::logging::Log  examples::exahype2::finitevolumes::ParallelEuler::_log( "examples::exahype2::finitevolumes::ParallelEuler" );


::exahype2::RefinementCommand examples::exahype2::finitevolumes::ParallelEuler::refinementCriterion(
  double Q[5],
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h,
  double                                       t
) {
  if (tarch::la::equals(t,0.0) and tarch::la::max(h)>1.0/3.0 ) {
    return ::exahype2::RefinementCommand::Refine;
  }
  else return ::exahype2::RefinementCommand::Keep;
}


void examples::exahype2::finitevolumes::ParallelEuler::adjustSolution(
  double Q[5],
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h,
  double                                       t
) {
  if (tarch::la::equals(t,0.0) ) {
    // initial conditions
    //bool isInTheCentre = ( tarch::la::norm2( x-tarch::la::Vector<Dimensions,double>(0.5) ) < 0.05 );
    bool isInTheCentre = x(0)<=0.5;
    Q[0] = 0.1;  // rho
    Q[1] = 0;    // velocities
    Q[2] = 0;
    Q[3] = 0;
    Q[4] = isInTheCentre ? 1.0 : 0.0; // inner energy
  }
  else {
    // other stuff
  }
}



void examples::exahype2::finitevolumes::ParallelEuler::eigenvalues(
  double                                       Q[5],
  const tarch::la::Vector<Dimensions,double>&  faceCentre,
  const tarch::la::Vector<Dimensions,double>&  volumeH,
  double                                       t,
  int                                          normal,
  double                                       lambda[5]
) {
  assertion(normal>=0);
  assertion(normal<Dimensions);
  constexpr double gamma = 1.4;
  const double irho = 1./Q[0];
  #if Dimensions==3
  const double p = (gamma-1) * (Q[4] - 0.5*irho*(Q[1]*Q[1]+Q[2]*Q[2]+Q[3]*Q[3]));
  #else
  const double p = (gamma-1) * (Q[4] - 0.5*irho*(Q[1]*Q[1]+Q[2]*Q[2]));
  #endif

  const double u_n = Q[normal + 1] * irho;
  assertion10( gamma * p * irho>=0.0, gamma, p, irho, faceCentre, normal, Q[0], Q[1], Q[2], Q[3], Q[4] );
  const double c   = std::sqrt(gamma * p * irho);

  lambda[0]  = u_n - c;
  lambda[1]  = u_n;
  lambda[2]  = u_n;
  lambda[3]  = u_n;
  lambda[4]  = u_n + c;

  assertion4( lambda[0]==lambda[0], u_n, c, faceCentre, normal );
  assertion4( lambda[1]==lambda[1], u_n, c, faceCentre, normal );
  assertion4( lambda[2]==lambda[2], u_n, c, faceCentre, normal );
  assertion4( lambda[3]==lambda[3], u_n, c, faceCentre, normal );
  assertion4( lambda[4]==lambda[4], u_n, c, faceCentre, normal );
}


void examples::exahype2::finitevolumes::ParallelEuler::flux(
  double                                       Q[5],
  const tarch::la::Vector<Dimensions,double>&  faceCentre,
  const tarch::la::Vector<Dimensions,double>&  volumeH,
  double                                       t,
  int                                          normal,
  double                                       F[5]
) {
  assertion(normal>=0);
  assertion(normal<Dimensions);
  assertion9( Q[0]==Q[0], Q[0], Q[1], Q[2], Q[3], Q[4], faceCentre, volumeH, t, normal );
  assertion9( Q[1]==Q[1], Q[0], Q[1], Q[2], Q[3], Q[4], faceCentre, volumeH, t, normal );
  assertion9( Q[2]==Q[2], Q[0], Q[1], Q[2], Q[3], Q[4], faceCentre, volumeH, t, normal );
  assertion9( Q[3]==Q[3], Q[0], Q[1], Q[2], Q[3], Q[4], faceCentre, volumeH, t, normal );
  assertion9( Q[4]==Q[4], Q[0], Q[1], Q[2], Q[3], Q[4], faceCentre, volumeH, t, normal );

  assertion9( Q[0]>1e-12, Q[0], Q[1], Q[2], Q[3], Q[4], faceCentre, volumeH, t, normal );
  constexpr double gamma = 1.4;
  const double irho = 1./Q[0];
  #if Dimensions==3
  const double p = (gamma-1) * (Q[4] - 0.5*irho*(Q[1]*Q[1]+Q[2]*Q[2]+Q[3]*Q[3]));
  #else
  const double p = (gamma-1) * (Q[4] - 0.5*irho*(Q[1]*Q[1]+Q[2]*Q[2]));
  #endif

  switch (normal) {
    case 0:
        {
          F[0] = Q[1];
          F[1] = irho*Q[1]*Q[1] + p;
          F[2] = irho*Q[2]*Q[1];
          F[3] = irho*Q[3]*Q[1];
          F[4] = irho*(Q[4]+p)*Q[1];
        }
        break;
    case 1:
        {
          F[0] = Q[2];
          F[1] = irho*Q[1]*Q[2];
          F[2] = irho*Q[2]*Q[2] + p;
          F[3] = irho*Q[3]*Q[2];
          F[4] = irho*(Q[4]+p)*Q[2];
        }
        break;
    case 2:
        {
          F[0] = Q[3];
          F[1] = irho*Q[1]*Q[3];
          F[2] = irho*Q[2]*Q[3];
          F[3] = irho*Q[3]*Q[3] + p;
          F[4] = irho*(Q[4]+p)*Q[3];
        }
        break;
  }

  assertion( F[0]==F[0] );
  assertion( F[1]==F[1] );
  assertion( F[2]==F[2] );
  assertion( F[3]==F[3] );
  assertion( F[4]==F[4] );
}


void examples::exahype2::finitevolumes::ParallelEuler::boundaryConditions(
  double                                       Qinside[5],
  double                                       Qoutside[5],
  const tarch::la::Vector<Dimensions,double>&  faceCentre,
  const tarch::la::Vector<Dimensions,double>&  volumeH,
  double                                       t,
  int                                          normal
) {
  logTraceInWith4Arguments( "boundaryConditions(...)", faceCentre, volumeH, t, normal );
  assertion4( Qinside[0]>1e-12, faceCentre, volumeH, t, normal );
  Qoutside[0] = Qinside[0];
  Qoutside[1] = Qinside[1];
  Qoutside[2] = Qinside[2];
  Qoutside[3] = Qinside[3];
  Qoutside[4] = Qinside[4];
  logTraceOutWith4Arguments( "boundaryConditions(...)", faceCentre, volumeH, t, normal );
}


void examples::exahype2::finitevolumes::ParallelEuler::finishTimeStep() {
  AbstractParallelEuler::finishTimeStep();
  //_loadBalancer.finishTimeStep();
}
