// This file is part of the ExaGRyPE project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#pragma once

#include "tarch/tests/TestCase.h"


namespace exagrype {
  /**
   * Collection of all ExaGRyPE unit tests
   *
   * ExaGRyPE is built for 3d only, so this routine is only available in
   * 3d builds. Please destroy the result after usage.
   */
  tarch::tests::TestCase* getUnitTests();
}
