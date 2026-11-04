// This file is part of the ExaGRyPE project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#include "FlatSpacetimeTest.h"

#include <cmath>
#include <vector>

#include "exagrype/ccz4/Kernels.h"
#include "exagrype/ccz4/SecondOrderAuxiliaryVariablesReconstruction.h"
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
  testMethod(testFirstDerivativeReconstructionIsExactForCubics);
  testMethod(testConstraintEnforcementOfEvolvedVariablesMatchesFullEnforcement);
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


void exagrype::ccz4::tests::FlatSpacetimeTest::testFirstDerivativeReconstructionIsExactForCubics() {
  constexpr int PrimaryVariables = 25;
  constexpr int Derivatives      = 34;
  constexpr int N                = 5;
  const tarch::la::Vector<Dimensions, double> patchSize(0.5);
  const double h = 0.5 / N;

  // Fields: lapse = x^3 + 2y^2 + 3z, phi = xyz + z^3, shift_0 = x^2 z, G_0 (xx) = y^3
  auto lapse  = [](double x, double y, double z) { return x * x * x + 2.0 * y * y + 3.0 * z; };
  auto phi    = [](double x, double y, double z) { return x * y * z + z * z * z; };
  auto shift0 = [](double x, double y, double z) { return x * x * z; };
  auto g0     = [](double x, double y, double z) { return y * y * y; };

  for (int haloSize = 2; haloSize <= 3; haloSize++) {
    const int volumesPerAxis = N + 2 * haloSize;
    std::vector<double> primaryVariables(volumesPerAxis * volumesPerAxis * volumesPerAxis * PrimaryVariables, 0.0);
    for (int z = 0; z < volumesPerAxis; z++)
    for (int y = 0; y < volumesPerAxis; y++)
    for (int x = 0; x < volumesPerAxis; x++) {
      const double px = (x - haloSize + 0.5) * h;
      const double py = (y - haloSize + 0.5) * h;
      const double pz = (z - haloSize + 0.5) * h;
      double* Q = &primaryVariables[(x + volumesPerAxis * (y + volumesPerAxis * z)) * PrimaryVariables];
      Q[0]  = g0(px, py, pz);
      Q[16] = lapse(px, py, pz);
      Q[17] = shift0(px, py, pz);
      Q[24] = phi(px, py, pz);
    }

    std::vector<double> derivatives(N * N * N * Derivatives, -1.0);
    exagrype::ccz4::reconstructFirstDerivativesFD4(primaryVariables.data(), patchSize, N, haloSize, derivatives.data());

    for (int z = 0; z < N; z++)
    for (int y = 0; y < N; y++)
    for (int x = 0; x < N; x++) {
      const double px = (x + 0.5) * h;
      const double py = (y + 0.5) * h;
      const double pz = (z + 0.5) * h;
      const double* d = &derivatives[(x + N * (y + N * z)) * Derivatives];
      // Entries are shifted by 25 compared to the 59 entries of CCZ4
      validateNumericalEqualsWithParams4(d[0], 3.0 * px * px, haloSize, x, y, z);
      validateNumericalEqualsWithParams4(d[1], 4.0 * py, haloSize, x, y, z);
      validateNumericalEqualsWithParams4(d[2], 3.0, haloSize, x, y, z);
      validateNumericalEqualsWithParams4(d[30], py * pz, haloSize, x, y, z);
      validateNumericalEqualsWithParams4(d[31], px * pz, haloSize, x, y, z);
      validateNumericalEqualsWithParams4(d[32], px * py + 3.0 * pz * pz, haloSize, x, y, z);
      // dx shift_0 = 28 - 25, dz shift_0 = 34 - 25
      validateNumericalEqualsWithParams4(d[3], 2.0 * px * pz, haloSize, x, y, z);
      validateNumericalEqualsWithParams4(d[9], px * px, haloSize, x, y, z);
      // dy G_xx = 43 - 25. The reconstruction yields D_kij = 1/2 d_k G_ij
      validateNumericalEqualsWithParams4(d[18], 1.5 * py * py, haloSize, x, y, z);
      // K0 is not touched
      validateNumericalEqualsWithParams4(d[33], -1.0, haloSize, x, y, z);
    }
  }
}


void exagrype::ccz4::tests::FlatSpacetimeTest::testConstraintEnforcementOfEvolvedVariablesMatchesFullEnforcement() {
  double full[Unknowns];
  initFlatSpacetime(full);
  // Perturb the metric, the extrinsic curvature, lapse, phi and the metric derivatives
  full[0] *= 1.3;
  full[3] *= 0.9;
  full[1] = 0.05;
  full[6] = 0.2;
  full[9] = -0.1;
  full[16] = 0.7;
  full[24] = 1.1;
  full[37] = 0.3;
  full[46] = -0.2;

  double evolved[Unknowns];
  for (int i = 0; i < Unknowns; i++) evolved[i] = full[i];

  exagrype::ccz4::enforceCCZ4constraints(full);
  exagrype::ccz4::enforceCCZ4constraintsOfEvolvedVariables(evolved);

  for (int i = 0; i < 25; i++) {
    validateNumericalEqualsWithParams1(evolved[i], full[i], i);
  }
  // The derivatives of the metric are untouched by the restricted version
  validateNumericalEqualsWithParams1(evolved[37], 0.3, 37);
  validateNumericalEqualsWithParams1(evolved[46], -0.2, 46);
}
