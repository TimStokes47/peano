
// This file is part of the ExaGRyPE project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#pragma once


#include "tarch/tests/TestCase.h"


namespace exagrype {
  namespace ccz4 {
    namespace tests {
      class KernelTest;
    }
  }
}


/**
 * Compare the CCZ4 kernels against GRChombo (apple with apple test)
 *
 * This test is work in progress: All validations are commented out, and the
 * test is not registered in exagrype::getUnitTests(). It used to live in
 * ExaHyPE 2 as exahype2::fd::tests::CCZ4KernelTest.
 */
class exagrype::ccz4::tests::KernelTest : public tarch::tests::TestCase {
  private:
    void AppleWithAppleTest();

    void prepareFieldData(
      double** g, 
      double& phi, 
      double** K, 
      double& trK,
      double& Theta,
      double* Gamma, 
      double& alpha, 
      double* beta,
      double* b,
      double x, double y, double z
    );

  public:
    KernelTest();

    /**
     * This routine is triggered by the TestCaseCollection
     */
    virtual void run();
};

