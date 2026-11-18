#ifndef APPARENT_HORIZON_FINDER_AHF_POSTPROCESS_OPTIONS_H_
#define APPARENT_HORIZON_FINDER_AHF_POSTPROCESS_OPTIONS_H_

#include "ah-search.h"
#include "ahf-data.h"

#include <optional>
#include <string>
#include <vector>

namespace exahype2::ccz4::ahf {

/** How ahf-postprocess finds the AHF fields in a snapshot. */
enum class PostprocessMapping {
  /** By the component names of the cell data (ReadGridFromFile without a mapping). */
  Names,
  /** ExaGRyPESourceMapping(). */
  ExaGRyPE,
  /** LegacyCCZ4SourceMapping(). */
  LegacyCCZ4
};

/**
 * Command line of ahf-postprocess. It searches for one horizon, trying the
 * initial radii in order around one centre, and stops at the first that
 * converges.
 */
struct PostprocessOptions {
  std::string inputFile;
  /** Writes <outputPrefix>.txt (summary) and, on success, <outputPrefix>.vtp (surface). */
  std::string outputPrefix = "apparent-horizon";
  PostprocessMapping mapping = PostprocessMapping::Names;
  ReferencePoint centre{0.0, 0.0, 0.0};
  std::vector<double> initialRadii;
  int lMax = 4;
  /** Interpolation support radius; without one, three times the finest cell spacing. */
  std::optional<double> supportRadius;
  double tolerance = 1.0e-8;
  bool help = false;
};

/**
 * Parse the arguments that follow the program name.
 *
 * @throws std::invalid_argument for an unknown option, a missing or malformed
 *         value, or a missing --input or --radius (unless --help is given).
 */
PostprocessOptions ParsePostprocessArguments(const std::vector<std::string>& arguments);

/** The usage text printed by --help and after an argument error. */
std::string PostprocessUsage(const std::string& executable);

/** The explicit source mapping selected by @p mapping; nothing for Names. */
std::optional<SourceVariableMapping> PostprocessSourceMapping(PostprocessMapping mapping);

/**
 * The smallest distance between neighbouring cell centres along a coordinate
 * axis. On a Peano grid this is the cell size of the finest level present.
 *
 * @throws std::invalid_argument if no axis has two distinct coordinates.
 */
double FinestGridSpacing(const GridData& gridData);

/**
 * The search configuration: one attempt per initial radius around the centre,
 * radius-weighted cubic interpolation within @p supportRadius, and the given
 * lMax and tolerance.
 */
AHSearchOptions MakePostprocessSearchOptions(const PostprocessOptions& options,
                                             double supportRadius);

}  // namespace exahype2::ccz4::ahf

#endif  // APPARENT_HORIZON_FINDER_AHF_POSTPROCESS_OPTIONS_H_
