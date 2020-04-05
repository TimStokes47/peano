#include "RefinementControl.h"


exahype2::RefinementControl operator&&( exahype2::RefinementControl lhs, exahype2::RefinementControl rhs) {
  if (
    lhs == exahype2::RefinementControl::Refine or rhs == exahype2::RefinementControl::Refine
  ) {
	return exahype2::RefinementControl::Refine;
  }
  else if (
    lhs == exahype2::RefinementControl::Keep or rhs == exahype2::RefinementControl::Keep
  ) {
    return exahype2::RefinementControl::Keep;
  }
  else {
	return exahype2::RefinementControl::Coarsen;
  }
}

