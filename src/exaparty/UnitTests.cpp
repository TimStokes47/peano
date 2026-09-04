#include "UnitTests.h"

#include "tarch/tests/TreeTestCaseCollection.h"

tarch::tests::TestCase* exaparty::getUnitTests() {
  tarch::tests::TreeTestCaseCollection* result = new tarch::tests::TreeTestCaseCollection();

  return result;
}
