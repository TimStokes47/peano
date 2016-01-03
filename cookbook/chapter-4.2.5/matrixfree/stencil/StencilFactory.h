// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _MATRIXFREE_STENCIL_STENCIL_FACTORY_H_
#define _MATRIXFREE_STENCIL_STENCIL_FACTORY_H_


#include "matrixfree/stencil/Stencil.h"


namespace matrixfree {
  /**
   * Stencil factory methods
   *
   * Many components in the Peano framework switch from a stencil notation to
   * element-wise assembly matrices. This class provides some operations to
   * construct the stencils. To convert these stencil into element-wise assembly
   * matrices, use the class ElementMatrix.
   *
   * @author Tobias Weinzierl
   */
  namespace stencil {
    /**
     * Exchanges the coordinates of a stencil.
     *
     * Operations like this operation can be used to rotate a stencil, i.e. to
     * derive several stencils from one input stencil.
     */
     Stencil exchangeCoordinates( const Stencil& stencil, int coord0, int coord1 );

    /**
     * @return @f$ [\frac{1}{6}, \frac{2}{3}, \frac{1}{6}] @f$
     */
     tarch::la::Vector<3,double> get1DMassStencil();
    

    /**
     * Stencil is not scaled at all with any mesh width.
     *
     * @return @f$ [-1, 2, -1] @f$
     */
     tarch::la::Vector<3,double> get1DLaplaceStencil();

    /**
     * Stencil is not scaled at all with any mesh width.
     */
     tarch::la::Vector<3,double> get1DIdentity();
     tarch::la::Vector<3,double> get1DLeftIdentity();
     tarch::la::Vector<3,double> get1DRightIdentity();

    /**
     * Stencil is not scaled at all with any mesh width.
     */
     tarch::la::Vector<5,double> get1DLinearInterpolationStencil();

    /**
     * Stencil is not scaled at all with any mesh width.
     */
     tarch::la::Vector<3,double> get1DMeanValueStencil();

    /**
     * Stencil is not scaled at all with any mesh width.
     */
     tarch::la::Vector<3,double> get1DDownwindStencil();
     tarch::la::Vector<3,double> get1DUpwindStencil();
     tarch::la::Vector<3,double> get1DCentralDifferencesStencil();

    /**
     * Makes stencil-product of two stencils:
     *
     * a * b = [a_1, a_2, a_3] o [ b_1, b_2, B_3]
     *         [a_1*b_3 a_2*b_3 a_3*b_3]
     *       = [a_1*b_2 a_2*b_2 a_3*b_2]
     *         [a_1*b_1 a_2*b_1 a_3*b_1]
     */
     tarch::la::Vector<3*3,double> stencilProduct(
            const tarch::la::Vector<3,double>& a,
            const tarch::la::Vector<3,double>& b
            );
     tarch::la::Vector<5*5,double> stencilProduct(
            const tarch::la::Vector<5,double>& a,
            const tarch::la::Vector<5,double>& b
            );

    /**
     * Equals a * (b*c)
     */
     tarch::la::Vector<3*3*3,double> stencilProduct(
            const tarch::la::Vector<3,double>& a,
            const tarch::la::Vector<3,double>& b,
            const tarch::la::Vector<3,double>& c
            );
     tarch::la::Vector<5*5*5,double> stencilProduct(
            const tarch::la::Vector<5,double>& a,
            const tarch::la::Vector<5,double>& b,
            const tarch::la::Vector<5,double>& c
            );

     tarch::la::Vector<3*3*3*3,double> stencilProduct(
            const tarch::la::Vector<3,double>& a,
            const tarch::la::Vector<3,double>& b,
            const tarch::la::Vector<3,double>& c,
            const tarch::la::Vector<3,double>& d
            );
     tarch::la::Vector<5*5*5*5,double> stencilProduct(
            const tarch::la::Vector<5,double>& a,
            const tarch::la::Vector<5,double>& b,
            const tarch::la::Vector<5,double>& c,
            const tarch::la::Vector<5,double>& d
            );

     tarch::la::Vector<3*3*3*3*3,double> stencilProduct(
            const tarch::la::Vector<3,double>& a,
            const tarch::la::Vector<3,double>& b,
            const tarch::la::Vector<3,double>& c,
            const tarch::la::Vector<3,double>& d,
            const tarch::la::Vector<3,double>& e
            );
     tarch::la::Vector<5*5*5*5*5,double> stencilProduct(
            const tarch::la::Vector<5,double>& a,
            const tarch::la::Vector<5,double>& b,
            const tarch::la::Vector<5,double>& c,
            const tarch::la::Vector<5,double>& d,
            const tarch::la::Vector<5,double>& e
            );

    /**
     * Computes the Laplacian.
     *
     * The individual derivatives are scaled with the entries of scaling. For
     * the standard Laplacian, you can use the scaling 1.0. For a scalar
     * material parameter, either pass 1.0 and rescale the result or pass in a
     * vector where all entries hold the material parameter.
     */
     tarch::la::Vector<THREE_POWER_D,double>     getLaplacian(
            const tarch::la::Vector<DIMENSIONS,double>& h,
            const tarch::la::Vector<DIMENSIONS,double>& scaling = 1.0
            );
     tarch::la::Vector<THREE_POWER_D,std::complex<double> >  getLaplacian(
            const tarch::la::Vector<DIMENSIONS,std::complex<double> >& h,
            const tarch::la::Vector<DIMENSIONS,std::complex<double> >& scaling = std::complex<double>(1.0)
            );
     tarch::la::Vector<THREE_POWER_D,std::complex<double> >  getLaplacian(
            const tarch::la::Vector<DIMENSIONS, double >& h,
            const tarch::la::Vector<DIMENSIONS,std::complex<double> >& scaling
            );

    /**
     * Computes the mass matrix.
     *
     * The mass results results from an identity in a PDE that is interpreted
     * in the finite element sense. As such, it is scaled with h^d. And it is
     * not a nodal evaluation but a nine-point stencil (in 2d).
     */
     tarch::la::Vector<THREE_POWER_D,double>                 getMassMatrix(const tarch::la::Vector<DIMENSIONS,double>& h);
     tarch::la::Vector<THREE_POWER_D,std::complex<double> >  getMassMatrix(const tarch::la::Vector<DIMENSIONS,std::complex<double> >& h);


    /**
     * Computes the Helmholtz shift term matrix.
     *
     * The Helmholtz shift is the term that needs to be added to the Laplacian matrix in order to get the full Helmholtz matrix
     */
     tarch::la::Vector<THREE_POWER_D,std::complex<double> >  getHelmholtzShiftMatrix(
            const tarch::la::Vector<DIMENSIONS,double>& h,
            const std::complex<double>& phi
            );

     tarch::la::Vector<THREE_POWER_D,std::complex<double> >  getHelmholtzShiftMatrix(
            const tarch::la::Vector<DIMENSIONS,std::complex<double> >& h,
            const std::complex<double>& phi
            );


    /**
     * Computes the mass matrix.
     *
     * The mass matrix is returned in the finite element form, i.e.~the
     * entries result from an integral over a h. They are scaled with the
     * volume of h. As such it corresponds to the integral over a
     * characteristic function of one cell.
     */
     tarch::la::Vector<THREE_POWER_D,double>                 getIdentity(const tarch::la::Vector<DIMENSIONS,double>& h);
     tarch::la::Vector<THREE_POWER_D,std::complex<double> >  getIdentity(const tarch::la::Vector<DIMENSIONS,std::complex<double> >& h);

    /**
     * Computes the stencil for a d-linear interpolation.
     */
     tarch::la::Vector<FIVE_POWER_D,double> getDLinearInterpolation();
  }
}



#endif
