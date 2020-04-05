#include "{CLASSNAME}.h"



::exahype2::RefinementControl {FULL_QUALIFIED_CLASSNAME}::refinementCriterion(
  double Q[{NUMBER_OF_UNKNOWNS}],
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h,
  const tarch::la::Vector<Dimensions,double>&  t
) {{
  if (tarch::la::equals(t,0.0) and tarch::la::max(h)>1.0/3.0 ) {{
    return ::exahype2::RefinementControl::Refine;
  }}
  else return ::exahype2::RefinementControl::Keep;
}}
