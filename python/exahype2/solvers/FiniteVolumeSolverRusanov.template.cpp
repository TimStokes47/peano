#include "{CLASSNAME}.h"



::exahype2::RefinementCommand {FULL_QUALIFIED_CLASSNAME}::refinementCriterion(
  double Q[{NUMBER_OF_UNKNOWNS}],
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h,
  const tarch::la::Vector<Dimensions,double>&  t
) {{
  if (tarch::la::equals(t,0.0) and tarch::la::max(h)>1.0/3.0 ) {{
    return ::exahype2::RefinementCommand::Refine;
  }}
  else return ::exahype2::RefinementCommand::Keep;
}}


void {FULL_QUALIFIED_CLASSNAME}::adjustSolution(
  double Q[{NUMBER_OF_UNKNOWNS}],
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h,
  const tarch::la::Vector<Dimensions,double>&  t
) {{
  if (tarch::la::equals(t,0.0) ) {{
    // initial conditions
  }}
  else {{
    // other stuff
  }}
}}



void {FULL_QUALIFIED_CLASSNAME}::eigenvalues(
  double                                       Q[{NUMBER_OF_UNKNOWNS}],
  const tarch::la::Vector<Dimensions,double>&  faceCentre,
  const tarch::la::Vector<Dimensions,double>&  volumeH,
  const tarch::la::Vector<Dimensions,double>&  t,
  int                                          normal,
  double                                       lambda[{NUMBER_OF_UNKNOWNS}]
) {{
  // @todo implement
}}


void {FULL_QUALIFIED_CLASSNAME}::flux(
  double                                       Q[{NUMBER_OF_UNKNOWNS}],
  const tarch::la::Vector<Dimensions,double>&  faceCentre,
  const tarch::la::Vector<Dimensions,double>&  volumeH,
  const tarch::la::Vector<Dimensions,double>&  t,
  int                                          normal,
  double                                       F[{NUMBER_OF_UNKNOWNS}]
) {{
	  // @todo implement
}}


void {FULL_QUALIFIED_CLASSNAME}::boundaryConditions(
  double                                       Qinside[{NUMBER_OF_UNKNOWNS}],
  double                                       Qoutside[{NUMBER_OF_UNKNOWNS}],
  const tarch::la::Vector<Dimensions,double>&  faceCentre,
  const tarch::la::Vector<Dimensions,double>&  volumeH,
  const tarch::la::Vector<Dimensions,double>&  t,
  int                                          normal
) {{
  // @todo implement
}}

