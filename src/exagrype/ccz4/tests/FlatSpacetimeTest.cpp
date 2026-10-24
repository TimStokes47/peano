// This file is part of the ExaGRyPE project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#include "FlatSpacetimeTest.h"

#include <cmath>

#include "exagrype/ccz4/Kernels.h"
#include "exagrype/initialvalues/InitialValues.h"
#include "tarch/tests/TestMacros.h"


namespace {
  constexpr int Unknowns = 59;

  constexpr int    CCZ4LapseType = 1;
  constexpr int    CCZ4SO        = 1;
  constexpr double CCZ4ds        = 1.0;
  constexpr double CCZ4c         = 1.0;
  constexpr double CCZ4e         = 1.0;
  constexpr double CCZ4f         = 0.75;
  constexpr double CCZ4bs        = 1.0;
  constexpr double CCZ4sk        = 1.0;
  constexpr double CCZ4xi        = 1.0;
  constexpr double CCZ4mu        = 0.2;
  constexpr double CCZ4itau      = 1.0;
  constexpr double CCZ4eta       = 1.0;
  constexpr double CCZ4k1        = 0.1;
  constexpr double CCZ4k2        = 0.0;
  constexpr double CCZ4k3        = 0.5;
  constexpr double CCZ4GLMc      = 1.2;
  constexpr double CCZ4GLMd      = 2.0;

  void initFlatSpacetime(double* Q) {
    exagrype::initialvalues::flat(Q, tarch::la::Vector<Dimensions, double>(0.0), 0.0);
  }
} // namespace


exagrype::ccz4::tests::FlatSpacetimeTest::FlatSpacetimeTest():
  TestCase("exagrype::ccz4::tests::FlatSpacetimeTest") {}


void exagrype::ccz4::tests::FlatSpacetimeTest::run() {
  testMethod(testNonconservativeProductVanishes);
  testMethod(testSourceVanishes);
  testMethod(testMaxEigenvalueOfFlatSpacetime);
  testMethod(testGaugeWaveLapse);
}


void exagrype::ccz4::tests::FlatSpacetimeTest::testNonconservativeProductVanishes() {
  double Q[Unknowns];
  double gradQ[3 * Unknowns] = {0.0};
  initFlatSpacetime(Q);

  for (int normal = 0; normal < 3; normal++) {
    double BgradQ[Unknowns];
    exagrype::ccz4::ncp(BgradQ, Q, gradQ, normal, CCZ4LapseType, CCZ4ds, CCZ4c, CCZ4e, CCZ4f, CCZ4bs, CCZ4sk, CCZ4xi, CCZ4mu, CCZ4SO);
    for (int i = 0; i < Unknowns; i++) {
      validateNumericalEqualsWithParams2(BgradQ[i], 0.0, normal, i);
    }
  }
}


void exagrype::ccz4::tests::FlatSpacetimeTest::testSourceVanishes() {
  double Q[Unknowns];
  double S[Unknowns];
  initFlatSpacetime(Q);

  exagrype::ccz4::source(S, Q, CCZ4LapseType, CCZ4ds, CCZ4c, CCZ4e, CCZ4f, CCZ4bs, CCZ4sk, CCZ4xi, CCZ4itau, CCZ4eta, CCZ4k1, CCZ4k2, CCZ4k3, CCZ4SO);
  for (int i = 0; i < Unknowns; i++) {
    validateNumericalEqualsWithParams1(S[i], 0.0, i);
  }
}


void exagrype::ccz4::tests::FlatSpacetimeTest::testMaxEigenvalueOfFlatSpacetime() {
  double Q[Unknowns];
  initFlatSpacetime(Q);

  // With unit lapse, unit conformal factor, unit metric and zero shift, the
  // estimate is the largest of sqrt(2), e, ds, GLMc and GLMd, i.e. GLMd=2.
  for (int normal = 0; normal < 3; normal++) {
    double lambda = -1.0;
    exagrype::ccz4::maxEigenvalue(Q, normal, CCZ4e, CCZ4ds, CCZ4GLMc, CCZ4GLMd, &lambda);
    validateNumericalEqualsWithParams1(lambda, 2.0, normal);
  }
}


void exagrype::ccz4::tests::FlatSpacetimeTest::testGaugeWaveLapse() {
  // The gauge wave has H = 1 - A sin(2 pi (x - t)) with amplitude A=0.1, and
  // the lapse is sqrt(H). At x=0.25 and t=0, we hence expect sqrt(0.9).
  double                                Q[Unknowns];
  tarch::la::Vector<Dimensions, double> x(0.0);
  x(0) = 0.25;
  exagrype::initialvalues::gaugeWave(Q, x, 0.0);
  validateNumericalEquals(Q[16], std::sqrt(0.9));
}
