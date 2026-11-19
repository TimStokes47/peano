#ifndef APPARENT_HORIZON_FINDER_AH_FINDER_RESULT_H_
#define APPARENT_HORIZON_FINDER_AH_FINDER_RESULT_H_

#include "ahf-data.h"
#include "harmonics.h"
#include "interpolation.h"
#include "spectral-solver.h"

#include <limits>

namespace exahype2::ccz4::ahf {

/**
 * User-level result of one apparent-horizon solve.
 *
 * horizon uses the numerical SpectralBasis collocation ordering. The retained
 * lMax and final coefficients are sufficient for a later writer to construct
 * a separate, denser visualisation grid without rerunning Newton.
 *
 * The radii are coordinate radii about referencePoint. meanRadius is the plain
 * average of h over the collocation points, which do not sample the sphere
 * evenly; meanCoordinateRadius is the solid-angle mean a_00 / sqrt(4 pi). They
 * agree for a coordinate sphere. area, arealRadius and irreducibleMass use the
 * physical metric interpolated from the grid onto the surface.
 */
struct AHFinderResult {
  SpectralNewtonResult solver;
  ReferencePoint referencePoint{0.0, 0.0, 0.0};
  int lMax = 0;
  AHGuess horizon;

  double minRadius = std::numeric_limits<double>::quiet_NaN();
  double meanRadius = std::numeric_limits<double>::quiet_NaN();
  double maxRadius = std::numeric_limits<double>::quiet_NaN();
  double meanCoordinateRadius = std::numeric_limits<double>::quiet_NaN();
  double area = std::numeric_limits<double>::quiet_NaN();
  double arealRadius = std::numeric_limits<double>::quiet_NaN();
  double irreducibleMass = std::numeric_limits<double>::quiet_NaN();
  double spectralResidualInfinityNorm =
    std::numeric_limits<double>::quiet_NaN();
  double maxAbsExpansion = std::numeric_limits<double>::quiet_NaN();

  bool surfaceAvailable = false;
};

/**
 * Reconstruct the final numerical surface and its diagnostics without running
 * Newton again. Reconstruction failure is represented by surfaceAvailable ==
 * false; the complete solver result and reference point are still preserved.
 */
AHFinderResult BuildAHFinderResult(
  const SpectralBasis& basis,
  const CartesianPointInterpolator& interpolator,
  const ReferencePoint& referencePoint,
  const SpectralNewtonResult& solverResult
);

}  // namespace exahype2::ccz4::ahf

#endif  // APPARENT_HORIZON_FINDER_AH_FINDER_RESULT_H_
