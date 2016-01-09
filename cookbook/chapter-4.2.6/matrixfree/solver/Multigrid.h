// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _MATRIXFREE_SOLVER_MULTIGRID_H_
#define _MATRIXFREE_SOLVER_MULTIGRID_H_

#include "tarch/logging/Log.h"
#include "tarch/la/Vector.h"
#include "tarch/la/Matrix.h"
#include "tarch/la/GramSchmidt.h"
#include "peano/utils/Globals.h"
#include "peano/utils/Loop.h"
#include "peano/grid/VertexEnumerator.h"
#include "matrixfree/stencil/ElementMatrix.h"

namespace matrixfree {
  namespace solver{
     class Multigrid;
  }
}

/**
 * Basis utility class for multigrid
 *
 * @author Marion Weinzierl, Tobias Weinzierl
 */
class matrixfree::solver::Multigrid {
  private:
    static tarch::logging::Log _log;

    /**
     * Stencil for d-linear interpolation
     *
     * The class provides a number of standard routines for permanently
     * reoccuring problems such as d-linear interpolation. For this, it
     * internally holds a d-linear interpolation stencil.
     */
    tarch::la::Vector<FIVE_POWER_D,double>  _dLinearInterpolation;

    /**
     * The class keeps track of the number of stencil evaluations.
     */
    int                                     _numberOfStencilEvaluations;

  public:
    Multigrid();
    Multigrid(const Multigrid& masterThread);
    virtual ~Multigrid();

    void mergeWithWorkerThread(const Multigrid& workerThread);

    int getNumberOfStencilUpdates() const;

    /**
     * Should be called in beginIteration() when you intend to use the stencil counting
     */
    void clearNumberOfStencilUpdates();

    template <int StencilSize1d, int StencilSize>
    static tarch::la::Vector<THREE_POWER_D,double> getGalerkinMultigridOperatorForDLinearInterpolationAndRestriction( int fineGridSubdivisionFactor, const tarch::la::Vector<StencilSize,double>& fineGridStencil );

    template <int StencilSize1d, int StencilSize>
    static tarch::la::Vector<THREE_POWER_D,double> getGalerkinMultigridOperatorForDLinearInterpolationAndInjection( int fineGridSubdivisionFactor, const tarch::la::Vector<StencilSize,double>& fineGridStencil );

    static tarch::la::Vector<THREE_POWER_D,double> getGalerkinMultigridOperatorForDLinearInterpolationAndRestriction( int fineGridSubdivisionFactor, const tarch::la::Vector<THREE_POWER_D,double>& fineGridStencil );
    static tarch::la::Vector<THREE_POWER_D,double> getGalerkinMultigridOperatorForDLinearInterpolationAndInjection( int fineGridSubdivisionFactor, const tarch::la::Vector<THREE_POWER_D,double>& fineGridStencil );

    /**
     * Compute position relative to one coarse grid vertex.
     *
     * @image html StencilRelativeToCoarseGrid.png
     *
     * @param coarseGridVertexPosition Index of current coarse grid vertex. It is a
     *        d-dimensional integer vector that holds only 0 and 1. (0,0) in
     *        d=2, e.g., is the bottom left vertex, (1,1) is the top right one.
     * @param fineGridVertexPosition Fine grid position of the fine grid
     *        vertex within a @f$ 4^d @f$ patch. It is an integer vector of
     *        length d that holds entries of @f$ \{ 0,1,2,3 \} @f$.
     * @param entryOfCoarseGridStencil Is an out parameter and afterwards
     *        contains a d-dimensional integer entry out of @f$ \{ 0,1,2,3,4 \} @f$
     *        that tells you for a 5-point stencil which entry affects the fine
     *        grid vertex.
     * @param coarseGridStencilInfluencesFineGridVertex Tells you whether the
     *        5-point stencil on the coarse grid influences the fine grid vertex
     *        at all. If the fine grid vertex, e.g., is (3,0) and the coarse grid
     *        vertex is (0,1), then this flag is false.
     */
    static void getPositionIn5PowDStencilRelativeToKthCoarseVertex(
      const tarch::la::Vector<DIMENSIONS,int>&       coarseGridVertexPosition,
      const tarch::la::Vector<DIMENSIONS,int>&       fineGridVertexPosition,
      tarch::la::Vector<DIMENSIONS,int>&             entryOfCoarseGridStencil,
      int&                                           indexOfCoarseGridStencil,
      bool&                                          coarseGridStencilInfluencesFineGridVertex
    );

    /**
     * Simple d-linear Interpolation
     *
     * I moved this to the multigrid solver, as it is a standard routine
     * required all the time - for example by most visualisation environments.
     * It requires the @f$ 2^d @f$ coarse grid values and the vertex's position
     * within a @f$ 3^d @f$ patch. Also, the method expects a scaling factor to
     * avoid that someone forgets to scale appropriately.
     *
     * If you interpolate scalar data such as the solution, the scaling
     * typically is 1.0. If you interpolate FEM data such as the residual, the
     * scaling has to reflect the integral of the supports, i.e. if you have
     * the Laplacian, the scaling is 1.0/static_cast<double>(THREE_POWER_D).
     *
     * !!! Update stencil evaluation counter
     *
     * Done by computeContributionWeightOfInterGridTransfer().
     *
     * @param coarseGridValues         The @f$ 2^d @f$ coarse grid values to be interpolated.
     * @param fineGridPositionOfVertex Position of the vertex within the @f$ 3^d @f$ patch, i.e. an integer vector whose entries are somewhere between 0 and 4.
     */
    double getDLinearInterpolatedValue(
      const tarch::la::Vector<TWO_POWER_D,double>&           coarseGridValues,
      const tarch::la::Vector<DIMENSIONS,int>&               fineGridPositionOfVertex
    );


    std::complex<double> getDLinearInterpolatedValue(
      const tarch::la::Vector<TWO_POWER_D,std::complex<double> >&  coarseGridValues,
      const tarch::la::Vector<DIMENSIONS,int>&                     fineGridPositionOfVertex
    );


    tarch::la::Vector<TWO_POWER_D,double > getDLinearInterpolationWeights(
      const tarch::la::Vector<DIMENSIONS,int>&                     fineGridPositionOfVertex
    );


    /**
     * !!! Compute Transfer Contribution Weight for one Individual Fine Grid Vertex
     *
     * If you wanna compute @f$ Pu_\mbox{coarse} @f$ or @f$ Ru_\mbox{fine} @f$ (for
     * example in a a touchVertex FirstTime() event) for one individual fine
     * grid vertex, you can do this in @f$ 2^d @f$ steps: For each coarse grid
     * vertex
     * - you either compute its contribution/weight to the fine grid value and
     *   scale it with the corresponding coarse grid vertex's value
     *   (prolongation) or
     * - you compute its contribution/weight for the fine grid vertex and
     *   scale it with the fine grid vertex's value (restriction)
     * then you add this value to the fine grid vertex or the corresponding
     * coarse grid value, respectively.
     *
     * To split it up into @f$ 2^d @f$ steps does make sense,
     * as the P stencil of each vertex might be different. This operation
     * implements how to get the weight between fine grid vertex and one
     * coarse grid vertex.
     *
     * @image html Multigrid.png
     *
     * !! Usage (compute Pu, i.e. prolongation, for one individual fine grid vertex)
     *
     * - Create temporary value. It is typically a double, and it represents
     *   the new fine grid value.
     * - Run over all @f$ 2^d @f$ (typically a dfor2 loop):
     *   - We will now compute the contribution of one coarse grid vertex to
     *     our fine grid vertex.
     *   - Take the coarse grid inter-grid transfer stencil. It is a
     *     @f$ 5^d @f$ vector.
     *   - Pass the stencil, the @f$ (0,1)^d @f$ position of the current
     *     coarse grid vertex relative to the current fine grid vertex, and the
     *     fine grid vertex position within the @f$ 4^d @f$ patch to this
     *     operation. You get a weight.
     *   - Multiply this weight with the coarse grid vertex's value that is to
     *     be interpolated.
     *   - Add the result to your temporary variable.
     * - The temporary variable now holds the contributions from all
     *   @f$ 2^d @f$ coarse grid values. Set it on the fine grid vertex.
     *
     * Usage:
     * \code
  double interpolatedValue = 0.0;
  dfor2(k) // run over the coarse grid vertices (0,0), (1,0), (0,1),
           // and (1,1) of this one coarse cell if d=2. If d!=2 it
           // does a couple of iterations more. k is an integer vector.
    tarch::la::Vector<FIVE_POWER_D,double> coarseGridStencil = coming from your coarse grid vertex k;
    double                                 coarseGridValue   = coming from your coarse grid vertex k;
    interpolatedValue +=
      coarseGridValue *
      _multigrid.computeContributionWeightOfInterGridTransfer(
        k,
        coarseGridStencil,
        fineGridPositionOfVertexWithinPatch
      );
  enddforx
  fineGridVertex.setXXX( interpolatedValue );
       \endcode
     *
     *
     * !! Usage (compute Ru, i.e. restriction, for one individual fine grid vertex)
     *
     * - Create temporary value. It is typically a double, and it represents
     *   the new fine grid value.
     * - Run over all @f$ 2^d @f$ (typically a dfor2 loop):
     *   - We will now compute the contribution of one coarse grid vertex to
     *     our fine grid vertex.
     *   - Take the coarse grid inter-grid transfer stencil. It is a
     *     @f$ 5^d @f$ vector.
     *   - Pass the stencil, the @f$ (0,1)^d @f$ position of the current
     *     coarse grid vertex relative to the current fine grid vertex, and the
     *     fine grid vertex position within the @f$ 4^d @f$ patch to this
     *     operation. You get a weight.
     *   - Multiply this weight with the fine grid vertex's value that is to
     *     be interpolated.
     *   - Add the result to the current coarse grid value.
     *
     * Usage:
     * \code
  dfor2(k) // run over the coarse grid vertices (0,0), (1,0), (0,1),
           // and (1,1) of this one coarse cell if d=2. If d!=2 it
           // does a couple of iterations more. k is an integer vector.
    tarch::la::Vector<FIVE_POWER_D,double> coarseGridStencil = coming from your coarse grid vertex k;
    double contributionToCurrentCoarseGridVertex =
      fineGridValue *
      _multigrid.computeContributionWeightOfInterGridTransfer(
        k,
        coarseGridStencil,
        fineGridPositionOfVertexWithinPatch
      );
  enddforx
       \endcode
     *
     *
     * The operation is not const as it has to update the internal counters.
     */
    double computeContributionWeightOfInterGridTransfer(
      const tarch::la::Vector<DIMENSIONS,int>&       currentCoarseGridVertex,
      const tarch::la::Vector<FIVE_POWER_D,double>&  currentCoarseGridVertexsInterGridTransferOperator,
      const tarch::la::Vector<DIMENSIONS,int>&       fineGridPositionOfVertex
    );


    /**
     * Inject values from fine grid onto coarse grid
     *
     * Typical application snippet:
     * \code
  VertexOperations::writeU(
    coarseGridVerticesEnumerator,
    coarseGridVertices,
    _multigrid.injectFineGridValues( VertexOperations::readAllU( fineGridVerticesEnumerator, fineGridVertices ) )
  );
       \endcode
     *
     * Many codes cannot use this operation as illustrated above as they
     * rely on touchVertexLastTime() to perform some operations before they
     * restrict. And this operation clearly is tied to ascend() which is called
     * before touchVertexLastTime(). In such a case, you have to induce all
     * vertex data manually vertex-by-vertex. Please consult in particular
     * SingleLevelEnumerator in this case. It comes along with some static
     * operations that are of great use for injection.
     */
    tarch::la::Vector<TWO_POWER_D,double> injectFineGridValues(
      const tarch::la::Vector<FOUR_POWER_D,double>&  fineGridValues
    );


    tarch::la::Vector<TWO_POWER_D,std::complex<double> > injectFineGridValues(
      const tarch::la::Vector<FOUR_POWER_D,std::complex<double> >&  fineGridValues
    );

    /**
     * Prolongs the four values along a hole @f$3^d@f$ patch.
     */
    tarch::la::Vector<FOUR_POWER_D,double > dLinearInterpolation(
      const tarch::la::Vector<TWO_POWER_D,double >&  coarseGridValues
    );

    tarch::la::Vector<FOUR_POWER_D,std::complex<double> > dLinearInterpolation(
      const tarch::la::Vector<TWO_POWER_D,std::complex<double> >&  coarseGridValues
    );

    /*
     * May be const, as it does not directly apply the matrix. If you apply the
     * matrix manually, please be aware that the internal flop counter is not
     * accurate anymore.
     */
    tarch::la::Matrix<FOUR_POWER_D, TWO_POWER_D, double> calculateP(
      const tarch::la::Vector<TWO_POWER_D_TIMES_FIVE_POWER_D, double>&  coarsePStencils
    ) const;

    /**
     * Special case where the @f$ 2^d @f$ P operators of all involved vertices are the same.
     */
    tarch::la::Matrix<FOUR_POWER_D, TWO_POWER_D, double> calculateP(
      const tarch::la::Vector<FIVE_POWER_D, double>&  coarsePStencil
    ) const;


    /**
     * Again a special case of the previous one where P is d-linear.
     */
    tarch::la::Matrix<FOUR_POWER_D, TWO_POWER_D, double> calculateP() const;


    /**
     * Calculates the P weights only for one particular vertex
     */
    tarch::la::Vector<TWO_POWER_D, double> calculateP(
      const tarch::la::Vector<TWO_POWER_D_TIMES_FIVE_POWER_D, double>&  coarsePStencils,
      const tarch::la::Vector<DIMENSIONS,int>&                          fineGridVertexPosition
    ) const;

    tarch::la::Vector<TWO_POWER_D, double> calculateP(
      const tarch::la::Vector<FIVE_POWER_D, double>&                    coarsePStencil,
      const tarch::la::Vector<DIMENSIONS,int>&                          fineGridVertexPosition
    ) const;

    tarch::la::Vector<TWO_POWER_D, double> calculateP(
      const tarch::la::Vector<DIMENSIONS,int>&                          fineGridVertexPosition
    ) const;
};


#include "matrixfree/solver/Multigrid.cpph"

#endif
