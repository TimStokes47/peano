#include "../require-asserts.h"

#include "../../apparent-horizon-finder.h"
#include "../temporary-directory.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace exahype2::ccz4::ahf::tests {
namespace {

GridData MakeGrid() {
  GridData grid;
  for (int ix = -2; ix <= 2; ++ix) {
    for (int iy = -2; iy <= 2; ++iy) {
      for (int iz = -2; iz <= 2; ++iz) {
        grid.coordinates.push_back({double(ix), double(iy), double(iz)});
        const std::array<double, NumberOfCompactFields> row{
          1.0, 0.0, 0.0, 1.0, 0.0, 1.0,
          1.0, 0.0, 0.0, 1.0, 0.0, 1.0,
          3.0, 1.0
        };
        grid.values.insert(grid.values.end(), row.begin(), row.end());
      }
    }
  }
  return grid;
}

AHBinaryMergerOptions Options(double x, double radius = 0.2) {
  AHBinaryMergerOptions options;
  options.components = {{
    {{.id = "plus", .attempts = {{{x, 0.0, 0.0}, radius}}}, 1.0},
    {{.id = "minus", .attempts = {{{-x, 0.0, 0.0}, radius}}}, 3.0}
  }};
  options.lMax = 2;
  options.interpolation = {
    .method = CartesianToSurfaceMethod::UnweightedPolynomial,
    .numberOfNeighbours = 64,
    .polynomialDegree = 1,
    .allowOrderReduction = false
  };
  return options;
}

std::string ReadText(const std::filesystem::path& path) {
  std::ifstream input(path);
  std::ostringstream text;
  text << input.rdbuf();
  return text.str();
}

void TestSeparatedPlan() {
  const AHBinaryMergerPlan plan = BuildBinaryMergerPlan(Options(1.0));
  assert(!plan.commonSearchEnabled);
  assert(std::abs(plan.separation - 2.0) < 1.0e-14);
  assert(std::abs(plan.facingRadii[0] - 0.2) < 1.0e-14);
  assert(plan.search.targets.size() == 2);
  assert(plan.search.targets[0].id == "plus");
  assert(plan.search.targets[1].id == "minus");
  assert(*plan.search.targets[0].massScale == 1.0);
  assert(*plan.search.targets[1].massScale == 3.0);
}

void TestOverlappingPlan() {
  AHBinaryMergerOptions options = Options(0.125);
  options.commonRadiusFactors = {1.0, 1.5};
  const AHBinaryMergerPlan plan = CheckCommonHorizonOfBBH(options);
  assert(plan.commonSearchEnabled);
  assert(std::abs(plan.separation - 0.25) < 1.0e-14);
  assert(std::abs(plan.facingRadii[0] + plan.facingRadii[1] - 0.4) < 1.0e-14);
  assert(std::abs(plan.commonMass - 4.0) < 1.0e-14);
  assert(std::abs(plan.commonReferencePoint.x + 0.0625) < 1.0e-14);
  assert(std::abs(plan.enclosingInitialRadius - 0.3875) < 1.0e-14);
  assert(plan.search.targets.size() == 3);
  assert(plan.search.targets[0].id == "common");
  assert(plan.search.targets[0].kind == AHHorizonKind::Common);
  assert(*plan.search.targets[0].massScale == 4.0);
  assert(plan.search.targets[0].attempts.size() == 2);
  assert(plan.search.targets[1].id == "plus");
  assert(plan.search.targets[2].id == "minus");
}

void TestDistortedFacingRadius() {
  AHBinaryMergerOptions options = Options(0.5, 0.1);
  const SpectralBasis basis = BuildSpectralBasis(options.lMax);
  std::vector<double> radii(basis.nPoints);
  for (std::size_t p = 0; p < radii.size(); ++p) {
    radii[p] = 0.45 + 0.05 * std::cos(basis.rho[p]) *
      std::cos(basis.sigma[p]);
  }
  options.components[0].target.attempts[0].initialCoefficients =
    MakeDistortedInitialCoefficients(basis, radii);
  options.components[1].target.attempts[0].initialRadius = 0.65;
  const AHBinaryMergerPlan plan = BuildBinaryMergerPlan(options);
  assert(plan.commonSearchEnabled);
  assert(plan.facingRadii[0] > 0.39 && plan.facingRadii[0] < 0.41);
  assert(std::abs(plan.facingRadii[1] - 0.65) < 1.0e-14);
}

double Length(double x, double y, double z) { return std::sqrt(x * x + y * y + z * z); }

// Off-axis, unequal masses: the separation, the facing radius of a distorted
// surface and the mass-weighted centre use all three coordinates.
void TestOffAxisPlan() {
  const ReferencePoint a{0.1, 0.3, -0.2};
  const ReferencePoint b{-0.2, -0.2, 0.3};
  const double dx = b.x - a.x, dy = b.y - a.y, dz = b.z - a.z;
  const double distance = Length(dx, dy, dz);

  AHBinaryMergerOptions options = Options(1.0);
  options.components[0].target.attempts = {{a, 0.3}};
  options.components[1].target.attempts = {{b, 0.3}};
  // The first surface bulges towards the second: r = 0.3 + 0.05 (n . d/|d|).
  const SpectralBasis basis = BuildSpectralBasis(options.lMax);
  std::vector<double> radii(basis.nPoints);
  for (std::size_t p = 0; p < radii.size(); ++p) {
    const double c = std::cos(basis.sigma[p]);
    const double n[3] = {c * std::cos(basis.rho[p]), c * std::sin(basis.rho[p]),
                         std::sin(basis.sigma[p])};
    radii[p] = 0.3 + 0.05 * (n[0] * dx + n[1] * dy + n[2] * dz) / distance;
  }
  options.components[0].target.attempts[0].initialCoefficients =
    MakeDistortedInitialCoefficients(basis, radii);

  const AHBinaryMergerPlan plan = BuildBinaryMergerPlan(options);
  assert(std::abs(plan.separation - distance) < 1.0e-14);
  assert(std::abs(plan.facingRadii[0] - 0.35) < 1.0e-12);
  assert(std::abs(plan.facingRadii[1] - 0.3) < 1.0e-14);
  assert(std::abs(plan.commonMass - 4.0) < 1.0e-14);
  const ReferencePoint centre{(a.x + 3.0 * b.x) / 4.0, (a.y + 3.0 * b.y) / 4.0,
                              (a.z + 3.0 * b.z) / 4.0};
  assert(std::abs(plan.commonReferencePoint.x - centre.x) < 1.0e-14);
  assert(std::abs(plan.commonReferencePoint.y - centre.y) < 1.0e-14);
  assert(std::abs(plan.commonReferencePoint.z - centre.z) < 1.0e-14);
  const SynthesizedField first =
    Synthesize(basis, options.components[0].target.attempts[0].initialCoefficients);
  const double firstMaximum = *std::max_element(first.h.begin(), first.h.end());
  const double enclosing = std::max(
    Length(centre.x - a.x, centre.y - a.y, centre.z - a.z) + firstMaximum,
    Length(centre.x - b.x, centre.y - b.y, centre.z - b.z) + 0.3);
  assert(std::abs(plan.enclosingInitialRadius - enclosing) < 1.0e-14);
}

void TestValidation() {
  AHBinaryMergerOptions invalid = Options(1.0);
  invalid.components[0].mass = 0.0;
  bool threw = false;
  try { (void)BuildBinaryMergerPlan(invalid); }
  catch (const std::invalid_argument&) { threw = true; }
  assert(threw);

  invalid = Options(1.0);
  invalid.components[1].target.id = "plus";
  threw = false;
  try { (void)BuildBinaryMergerPlan(invalid); }
  catch (const std::invalid_argument&) { threw = true; }
  assert(threw);
}

void TestExecutionAndOutput() {
  AHBinaryMergerOptions options = Options(0.1, 1.1);
  options.components[1].mass = 1.0;
  options.commonRadiusFactors = {1.0};
  options.solver.maxIterations = 0;
  const TemporaryDirectory temporary("ahf-merger-orchestration-tests");
  const auto& directory = temporary.path();
  const auto summary = directory / "ahf-merger-summary.txt";
  const auto prefix = directory / "ahf-merger";
  std::filesystem::remove(summary);

  const AHBinaryMergerResult result = WriteAHToFile(
    MakeGrid(), options,
    {.summaryFilename = summary.string(),
     .horizonFilenamePrefix = prefix.string(),
     .visualization = {9, 6}});
  assert(result.plan.commonSearchEnabled);
  assert(result.horizons.targets.size() == 3);
  assert(result.horizons.targets[0].id == "common");
  assert(!result.horizons.allTargetsFound);
  assert(!result.horizons.anyHorizonFound);
  const std::string summaryText = ReadText(summary);
  assert(summaryText.find("merger.commonSearchEnabled = true") !=
         std::string::npos);
  assert(summaryText.find("merger.commonMass = 2") != std::string::npos);
  assert(summaryText.find("targets.count = 3") != std::string::npos);
  assert(!std::filesystem::exists(prefix.string() + "-0-common.vtp"));
  std::filesystem::remove(summary);
}

}  // namespace
}  // namespace exahype2::ccz4::ahf::tests

int main() {
  using namespace exahype2::ccz4::ahf::tests;
  TestSeparatedPlan();
  TestOverlappingPlan();
  TestDistortedFacingRadius();
  TestOffAxisPlan();
  TestValidation();
  TestExecutionAndOutput();
}
