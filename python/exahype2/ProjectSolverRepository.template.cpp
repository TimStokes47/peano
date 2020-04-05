#include "SolverRepository.h"



{FULL_QUALIFIED_CLASSNAME}::{CLASSNAME}() {{

}}


{FULL_QUALIFIED_CLASSNAME}::~{CLASSNAME}() {{
  for (auto& p: _solver) {{
	delete p;
  }}
}}


{FULL_QUALIFIED_CLASSNAME}& {FULL_QUALIFIED_CLASSNAME}::getInstance() {{
  static {FULL_QUALIFIED_CLASSNAME} singleton;
  return singleton;
}}


int {FULL_QUALIFIED_CLASSNAME}::getNumberOfSolvers() const {{
  return _solver.size();
}}


exahype2::Solver& {FULL_QUALIFIED_CLASSNAME}::getSolver(int number) {{
  return _solver.at(number);
}}


#endif
