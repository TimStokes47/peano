#include "ahf-postprocess-options.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace exahype2::ccz4::ahf {

namespace {

/** Remove a trailing @p extension from @p path, if it has one. */
std::string withoutExtension(const std::string& path, const std::string& extension) {
  if (path.size() > extension.size()
      && path.compare(path.size() - extension.size(), extension.size(), extension) == 0) {
    return path.substr(0, path.size() - extension.size());
  }
  return path;
}

std::vector<std::string> splitAtCommas(const std::string& text) {
  std::vector<std::string> parts;
  std::stringstream stream(text);
  std::string part;
  while (std::getline(stream, part, ',')) {
    parts.push_back(part);
  }
  if (!text.empty() && text.back() == ',') {
    parts.push_back("");
  }
  return parts;
}

/** A finite number that takes up all of @p text. */
double parseNumber(const std::string& option, const std::string& text) {
  std::size_t used = 0;
  double value = std::numeric_limits<double>::quiet_NaN();
  try {
    value = std::stod(text, &used);
  } catch (const std::exception&) {
    used = 0;
  }
  if (used == 0 || used != text.size() || !std::isfinite(value)) {
    throw std::invalid_argument(option + " expects a number, not '" + text + "'");
  }
  return value;
}

double parsePositiveNumber(const std::string& option, const std::string& text) {
  const double value = parseNumber(option, text);
  if (value <= 0.0) {
    throw std::invalid_argument(option + " expects a positive number, not '" + text + "'");
  }
  return value;
}

int parsePositiveInteger(const std::string& option, const std::string& text) {
  std::size_t used = 0;
  int value = 0;
  try {
    value = std::stoi(text, &used);
  } catch (const std::exception&) {
    used = 0;
  }
  if (used == 0 || used != text.size() || value <= 0) {
    throw std::invalid_argument(option + " expects a positive integer, not '" + text + "'");
  }
  return value;
}

PostprocessMapping parseMapping(const std::string& text) {
  if (text == "names") {
    return PostprocessMapping::Names;
  }
  if (text == "exagrype") {
    return PostprocessMapping::ExaGRyPE;
  }
  if (text == "legacy") {
    return PostprocessMapping::LegacyCCZ4;
  }
  throw std::invalid_argument(
    "--mapping expects names, exagrype or legacy, not '" + text + "'");
}

ReferencePoint parseCentre(const std::string& option, const std::string& text) {
  const auto parts = splitAtCommas(text);
  if (parts.size() != 3) {
    throw std::invalid_argument(option + " expects x,y,z, not '" + text + "'");
  }
  return {parseNumber(option, parts[0]), parseNumber(option, parts[1]),
          parseNumber(option, parts[2])};
}

/** The smallest gap between distinct values of @p values, sorted in place. */
double smallestGap(std::vector<double>& values) {
  std::sort(values.begin(), values.end());
  const double scale = std::max({values.back() - values.front(), std::abs(values.front()),
                                 std::abs(values.back())});
  // Centres on the same plane differ by rounding: render.py stores the points
  // in single precision, about 1e-7 relative. A real cell size is many orders
  // of magnitude above this threshold.
  const double sameValue = 1.0e-5 * scale;
  double gap = std::numeric_limits<double>::infinity();
  for (std::size_t i = 1; i < values.size(); ++i) {
    const double difference = values[i] - values[i - 1];
    if (difference > sameValue) {
      gap = std::min(gap, difference);
    }
  }
  return gap;
}

}  // namespace

PostprocessOptions ParsePostprocessArguments(const std::vector<std::string>& arguments) {
  PostprocessOptions options;
  const std::vector<std::string> optionsWithValue{
    "--input", "--output", "--mapping", "--centre", "--center", "--radius", "--lmax",
    "--support-radius", "--tolerance"};
  for (std::size_t i = 0; i < arguments.size(); ++i) {
    const std::string& argument = arguments[i];
    if (argument == "--help" || argument == "-h") {
      options.help = true;
      continue;
    }
    if (std::find(optionsWithValue.begin(), optionsWithValue.end(), argument)
        == optionsWithValue.end()) {
      throw std::invalid_argument(argument + " is not an option that ahf-postprocess knows");
    }
    if (i + 1 >= arguments.size()) {
      throw std::invalid_argument(argument + " needs a value");
    }
    const std::string& value = arguments[++i];
    if (argument == "--input") {
      options.inputFile = value;
    } else if (argument == "--output") {
      options.outputPrefix = withoutExtension(withoutExtension(value, ".vtp"), ".txt");
    } else if (argument == "--mapping") {
      options.mapping = parseMapping(value);
    } else if (argument == "--centre" || argument == "--center") {
      options.centre = parseCentre(argument, value);
    } else if (argument == "--radius") {
      for (const std::string& radius : splitAtCommas(value)) {
        options.initialRadii.push_back(parsePositiveNumber(argument, radius));
      }
    } else if (argument == "--lmax") {
      options.lMax = parsePositiveInteger(argument, value);
    } else if (argument == "--support-radius") {
      options.supportRadius = parsePositiveNumber(argument, value);
    } else if (argument == "--tolerance") {
      options.tolerance = parsePositiveNumber(argument, value);
    }
  }

  if (options.help) {
    return options;
  }
  if (options.inputFile.empty()) {
    throw std::invalid_argument("--input is required");
  }
  if (options.initialRadii.empty()) {
    throw std::invalid_argument("--radius is required");
  }
  if (options.outputPrefix.empty()) {
    throw std::invalid_argument("--output must not be empty");
  }
  return options;
}

std::string PostprocessUsage(const std::string& executable) {
  return "Usage: " + executable + " --input <snapshot.vtu|snapshot.pvtu> --radius <r>[,<r>...]\n"
    "         [--output <prefix>] [--centre <x,y,z>] [--mapping names|exagrype|legacy]\n"
    "         [--lmax <n>] [--support-radius <h>] [--tolerance <t>]\n"
    "\n"
    "Finds one apparent horizon in a VTK snapshot (convert a Peano patch file with\n"
    "src/tarch/plotter/api/render.py --filter-fine-grid). Starting from a sphere of\n"
    "each initial radius around the centre in turn, it stops at the first search\n"
    "that converges.\n"
    "\n"
    "  --input           snapshot to read (.vtu, or a .pvtu series)\n"
    "  --radius          initial radii, tried in order; repeat or separate by commas\n"
    "  --output          writes <prefix>.txt (summary) and <prefix>.vtp (surface);\n"
    "                    default apparent-horizon\n"
    "  --centre          centre of the initial spheres; default 0,0,0\n"
    "  --mapping         where the fields are: by component name (names, default),\n"
    "                    exagrype.api layout (exagrype), legacy ccz4.py (legacy)\n"
    "  --lmax            highest spherical-harmonic degree; default 4\n"
    "  --support-radius  interpolation support; default three times the finest\n"
    "                    cell spacing\n"
    "  --tolerance       convergence threshold on the spectral residual; default 1e-8\n"
    "\n"
    "Exit status: 0 if a horizon was found, 1 if not, 2 on an error.\n";
}

std::optional<SourceVariableMapping> PostprocessSourceMapping(PostprocessMapping mapping) {
  switch (mapping) {
    case PostprocessMapping::ExaGRyPE:
      return ExaGRyPESourceMapping();
    case PostprocessMapping::LegacyCCZ4:
      return LegacyCCZ4SourceMapping();
    case PostprocessMapping::Names:
      break;
  }
  return std::nullopt;
}

double FinestGridSpacing(const GridData& gridData) {
  double spacing = std::numeric_limits<double>::infinity();
  std::vector<double> values(gridData.coordinates.size());
  for (double GridCoordinate::*axis : {&GridCoordinate::x, &GridCoordinate::y, &GridCoordinate::z}) {
    if (values.size() < 2) {
      break;
    }
    std::transform(gridData.coordinates.begin(), gridData.coordinates.end(), values.begin(),
                   [axis](const GridCoordinate& coordinate) { return coordinate.*axis; });
    spacing = std::min(spacing, smallestGap(values));
  }
  if (!std::isfinite(spacing)) {
    throw std::invalid_argument(
      "FinestGridSpacing: no axis has two distinct cell-centre coordinates");
  }
  return spacing;
}

AHSearchOptions MakePostprocessSearchOptions(const PostprocessOptions& options,
                                             double supportRadius) {
  AHSearchOptions search;
  for (const double radius : options.initialRadii) {
    search.attempts.push_back({.referencePoint = options.centre, .initialRadius = radius});
  }
  search.lMax = options.lMax;
  search.interpolation = {
    .method = CartesianToSurfaceMethod::RadiusWeightedPolynomial,
    .polynomialDegree = 3,
    .supportRadius = supportRadius,
    .rankToleranceMultiplier = 64.0,
    .allowOrderReduction = false
  };
  search.solver.tolerance = options.tolerance;
  return search;
}

}  // namespace exahype2::ccz4::ahf
