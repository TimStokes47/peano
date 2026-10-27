// This file is part of the ExaGRyPE project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#include "UnitTests.h"

#include "tarch/tests/TreeTestCaseCollection.h"

#include "exagrype/ccz4/tests/FlatSpacetimeTest.h"


tarch::tests::TestCase* exagrype::getUnitTests() {
  tarch::tests::TreeTestCaseCollection* result = new tarch::tests::TreeTestCaseCollection();

  result->addTestCase(new exagrype::ccz4::tests::FlatSpacetimeTest());

  return result;
}
