#include "ahf-postprocess-options.h"
#include "file-input.h"
#include "output.h"

#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

using namespace exahype2::ccz4::ahf;

namespace {

constexpr int HorizonNotFound = 1;
constexpr int Error = 2;

void PrintAttempts(const AHSearchResult& search) {
  for (std::size_t attempt = 0; attempt < search.attempts.size(); ++attempt) {
    const AHSearchAttemptResult& result = search.attempts[attempt];
    std::cout << "Attempt " << attempt << ": initial radius " << result.initialRadius << ", "
              << (result.succeeded ? "converged" : "failed") << " after "
              << result.result.solver.iterations << " iterations";
    if (!result.succeeded && !result.result.solver.message.empty()) {
      std::cout << " (" << result.result.solver.message << ")";
    }
    std::cout << "\n";
  }
}

void PrintHorizon(const AHFinderResult& horizon) {
  std::cout << "Apparent horizon found around (" << horizon.referencePoint.x << ", "
            << horizon.referencePoint.y << ", " << horizon.referencePoint.z << ")\n"
            << "  coordinate radius min/mean/max: " << horizon.minRadius << " / "
            << horizon.meanRadius << " / " << horizon.maxRadius << "\n"
            << "  spectral residual (max norm):   " << horizon.spectralResidualInfinityNorm << "\n"
            << "  max |expansion| on the surface: " << horizon.maxAbsExpansion << "\n";
}

}  // namespace

int main(int argc, char** argv) {
  const std::string executable = std::filesystem::path(argv[0]).filename().string();
  PostprocessOptions options;
  try {
    options = ParsePostprocessArguments(std::vector<std::string>(argv + 1, argv + argc));
  } catch (const std::invalid_argument& error) {
    std::cerr << executable << ": " << error.what() << "\n\n" << PostprocessUsage(executable);
    return Error;
  }
  if (options.help) {
    std::cout << PostprocessUsage(executable);
    return EXIT_SUCCESS;
  }

  try {
    GridData grid;
    const auto mapping = PostprocessSourceMapping(options.mapping);
    if (mapping) {
      ReadGridFromFile(options.inputFile, grid, *mapping);
    } else {
      ReadGridFromFile(options.inputFile, grid);
    }
    std::cout << "Read " << grid.numberOfPoints() << " cells from " << options.inputFile << "\n";

    double supportRadius = 0.0;
    if (options.supportRadius) {
      supportRadius = *options.supportRadius;
      std::cout << "Interpolation support radius " << supportRadius << "\n";
    } else {
      const double spacing = FinestGridSpacing(grid);
      supportRadius = 3.0 * spacing;
      std::cout << "Interpolation support radius " << supportRadius
                << " (three times the finest cell spacing, " << spacing << ")\n";
    }

    const AHOutputOptions output{
      .summaryFilename = options.outputPrefix + ".txt",
      .surfaceFilename = options.outputPrefix + ".vtp"
    };
    const AHSearchResult search =
      WriteAHToFile(grid, MakePostprocessSearchOptions(options, supportRadius), output);

    PrintAttempts(search);
    if (!search.converged || !search.horizon) {
      std::cout << "No apparent horizon found. Summary: " << output.summaryFilename << "\n";
      return HorizonNotFound;
    }
    PrintHorizon(*search.horizon);
    std::cout << "Summary: " << output.summaryFilename << "\n"
              << "Surface: " << output.surfaceFilename << "\n";
    return EXIT_SUCCESS;
  } catch (const std::exception& error) {
    std::cerr << executable << ": " << error.what() << "\n";
    return Error;
  }
}
