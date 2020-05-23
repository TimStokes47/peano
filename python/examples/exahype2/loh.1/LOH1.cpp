#include "LOH1.h"


tarch::logging::Log   examples::exahype2::finitevolumes::LOH1::_log( "examples::exahype2::finitevolumes::LOH1" );



::exahype2::RefinementCommand examples::exahype2::finitevolumes::LOH1::refinementCriterion(
  double Q[9],
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h,
  double                                       t
) {
  logTraceInWith3Arguments( "refinementCriterion(...)", x, h, t );
  ::exahype2::RefinementCommand result = ::exahype2::RefinementCommand::Keep;
  if (tarch::la::equals(t,0.0) and tarch::la::max(h)>1.0/3.0 ) {
    result = ::exahype2::RefinementCommand::Refine;
  }
  logTraceOutWith1Argument( "refinementCriterion(...)", toString(result) );
  return result;
}


void examples::exahype2::finitevolumes::LOH1::adjustSolution(
  double Q[9],
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h,
  double                                       t
) {
  logTraceInWith3Arguments( "adjustSolution(...)", x, h, t );
  if (tarch::la::equals(t,0.0) ) {
    // initial conditions
  }
  else {
    // other stuff
  }
  logTraceOut( "adjustSolution(...)" );
}

void examples::exahype2::finitevolumes::LOH1::eigenvalues(
  double                                       Q[9],
  const tarch::la::Vector<Dimensions,double>&  faceCentre,
  const tarch::la::Vector<Dimensions,double>&  volumeH,
  double                                       t,
  int                                          normal,
  double                                       lambda[9]
) {
  //VariableShortcuts s;
  //double cp     = Q[s.cp];
  //double cs     = Q[s.cs];
  //
  //lambda[0] = -cp;
  //lambda[1] =  cp;
  //lambda[2] = -cs;
  //lambda[3] =  cp;
  //lambda[4] = 0.0;
  //lambda[5] = 0.0;
  //lambda[6] = 0.0;
  //lambda[7] = 0.0;
  //lambda[8] = 0.0;
  //lambda[9] = 0.0;
}


void examples::exahype2::finitevolumes::LOH1::boundaryConditions(
  double                                       Qinside[9],
  double                                       Qoutside[9],
  const tarch::la::Vector<Dimensions,double>&  faceCentre,
  const tarch::la::Vector<Dimensions,double>&  volumeH,
  double                                       t,
  int                                          normal
) {
  logTraceInWith4Arguments( "boundaryConditions(...)", faceCentre, volumeH, t, normal );
  // @todo implement
  logTraceOut( "boundaryConditions(...)" );
}

void examples::exahype2::finitevolumes::LOH1::flux(
  double                                       Q[9],
  const tarch::la::Vector<Dimensions,double>&  faceCentre,
  const tarch::la::Vector<Dimensions,double>&  volumeH,
  double                                       t,
  int                                          normal,
  double                                       F[9] ) {
  logTraceInWith4Arguments( "flux(...)", faceCentre, volumeH, t, normal );
  // @todo implement
  logTraceOut( "flux(...)" );
}

void examples::exahype2::finitevolumes::LOH1::nonconservativeProduct(
  double                                       Q[9],
  double                                       gradQ[9][Dimensions],
  double                                       BgradQ[9] ) {
  logTraceIn( "nonconservativeProduct(...)" );
  // @todo implement
  BgradQ[1] = Q[0];
  logTraceOut( "nonconservativeProduct(...)" );
}
