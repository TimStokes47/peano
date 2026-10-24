// This file is part of the ExaHyPE2 project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#pragma once


#include "peano4/datamanagement/FaceMarker.h"
#include "peano4/utils/Globals.h"
#include "tarch/la/Vector.h"

#include <bitset>
#include <functional>


namespace exahype2 {
  namespace blockstructured {
    /**
     * Advance the ghost cells of a face along the domain boundary through a
     * Sommerfeld (radiative, outgoing) boundary condition
     *
     *
     * # What Sommerfeld conditions are
     *
     * Many simulations cut a finite box out of an infinite domain. The
     * classic example is a black hole spacetime: The solution is defined
     * everywhere, but we can only afford to simulate @f$ [-L,L]^d @f$. Waves
     * that are emitted in the interior should leave this box as if the box
     * was not there. Dirichlet conditions, which hold the boundary at a
     * prescribed value, do not achieve this: Everything that travels
     * outward is reflected back into the domain.
     *
     * Sommerfeld conditions, also called radiative or outgoing boundary
     * conditions, assume that, close to the boundary, every unknown @f$ u
     * @f$ behaves like an outgoing spherical wave which decays towards a
     * far-field value @f$ u_\infty @f$:
     *
     * @f{eqnarray*}{
     *   u(r,t) = u_\infty + \frac{f(r - v t)}{r}.
     * @f}
     *
     * Here, @f$ v @f$ is the speed at which the field propagates, @f$ f @f$
     * an arbitrary profile, and @f$ r = |x-x_0| @f$ the distance from the
     * centre @f$ x_0 @f$ (origin) of the system, i.e. the point that emits
     * the waves. Differentiating the ansatz with respect to @f$ t @f$ and
     * @f$ r @f$ eliminates the unknown profile @f$ f @f$, and we obtain a
     * partial differential equation which the solution has to satisfy along
     * the boundary:
     *
     * @f{eqnarray*}{
     *   \partial_t u + v \partial_r u + \frac{v}{r} ( u - u_\infty ) = 0
     *   \qquad \text{with} \qquad
     *   \partial_r u = \frac{x_i - x_{0,i}}{r} \partial_i u.
     * @f}
     *
     * The condition is exact for a spherical wave with constant speed.
     * Everything else (non-spherical waves, waves that do not originate
     * from @f$ x_0 @f$, several speeds) is reflected partially. The
     * reflections become smaller the further the boundary is away from the
     * sources. Different to a Dirichlet condition, the boundary value is
     * not known a priori: It follows from the solution inside the domain,
     * and it has its own time evolution. A Sommerfeld boundary condition
     * hence is an evolution equation for the ghost cells.
     *
     *
     * # Discretisation
     *
     * This routine works on the grid cells (Finite Volumes or Finite
     * Difference points) of a block-structured patch. Along a boundary face
     * with normal @f$ n @f$, there are overlap layers of ghost cells
     * outside of the domain. We only discretise the derivative along the
     * normal, as a ghost cell has no neighbours outside of the domain along
     * the tangential directions which we could use. For a field which
     * depends on @f$ r @f$ only, @f$ \partial_n u = \frac{x_n - x_{0,n}}{r}
     * \partial_r u @f$. We invert this relation, i.e. we reconstruct the
     * radial derivative from the normal one:
     *
     * @f{eqnarray*}{
     *   \partial_r u \approx \frac{r}{|x_n - x_{0,n}|} \partial_n u.
     * @f}
     *
     * Along the coordinate axes, @f$ \partial_r u = \partial_n u @f$. In
     * the edges and corners of the domain, the factor grows up to
     * @f$ \sqrt{d} @f$. The alternative, i.e. to take the definition of
     * @f$ \partial_r u @f$ above and to drop the tangential derivatives,
     * would yield the factor @f$ |x_n - x_{0,n}|/r \leq 1 @f$ instead, and
     * would underestimate the radial derivative of a spherical wave.
     *
     * Let @f$ o @f$ be a ghost cell and @f$ i @f$ its neighbour one step
     * further inside, i.e. the first cell inside the domain for ghost layer
     * 0, and ghost layer @f$ l-1 @f$ for ghost layer @f$ l @f$. We
     * discretise the condition with a box scheme: It is centred between
     * @f$ o @f$ and @f$ i @f$ in space, and between @f$ t^n @f$ and @f$
     * t^{n+1} = t^n + \Delta t @f$ in time:
     *
     * @f{eqnarray*}{
     *   \frac{o^{n+1}-o^n+i^{n+1}-i^n}{\Delta t}
     *   + \sigma \left( o^{n+1}-i^{n+1}+o^n-i^n \right)
     *   + \frac{v}{r} \left( o^n+i^n-2u_\infty \right) = 0
     *   \qquad \text{with} \qquad
     *   \sigma = \frac{v}{h} \frac{r}{|x_n - x_{0,n}|}.
     * @f}
     *
     * @f$ h @f$ is the grid cell size along the normal. The damping term is
     * evaluated explicitly at the old time. Solved for the new ghost value,
     * this yields
     *
     * @f{eqnarray*}{
     *   o^{n+1} = \frac{
     *       (1 - \sigma \Delta t) o^n
     *     + (1 + \sigma \Delta t) i^n
     *     - (1 - \sigma \Delta t) i^{n+1}
     *     - \Delta t \frac{v}{r} \left( o^n+i^n-2u_\infty \right)
     *   }{ 1 + \sigma \Delta t }.
     * @f}
     *
     * The routine evaluates @f$ v @f$ via waveSpeed() for @f$ o^n @f$, @f$
     * u_\infty @f$ via farFieldSolution(), and @f$ r @f$ and @f$ x_n @f$ at
     * the centre of the ghost cell. It runs through the ghost layers from
     * the face outward. Therefore, @f$ i^{n+1} @f$ is the new interior
     * solution for layer 0, and the ghost value which the routine has just
     * computed for all further layers. The scheme is consistent for any
     * overlap, i.e. it serves the one ghost layer of a first-order Finite
     * Volume scheme as well as the three layers of fourth-order Finite
     * Differences.
     *
     * A state which equals @f$ u_\infty @f$ everywhere is preserved
     * exactly. If @f$ \Delta t = 0 @f$, the routine copies the old ghost
     * values, i.e. it sets @f$ o^{n+1} = o^n @f$. This is not what the
     * formula above yields in the limit: there, the jump of the interior
     * solution would be passed on to the ghost cell. See the discussion of
     * Runge-Kutta schemes below for why zero time step sizes arise.
     *
     * The auxiliary variables are not evolved. Each ghost cell takes the
     * auxiliary variables of @f$ i^{n+1} @f$, i.e. they are extrapolated
     * from the new interior solution.
     *
     *
     * # How to use the routine within a block-structured scheme
     *
     * Block-structured schemes such as exahype2.api.fv and exahype2.api.rkfd4
     * hold the halo data of their patches on the faces: A face carries
     * 2*overlap layers of grid cells along its normal. The left half belongs
     * to the cell left of the face, the right half to the cell right of it.
     * On the domain boundary, one of the two halves lies outside of the
     * domain. These are the ghost cells, and nobody but the boundary
     * treatment writes them. The layout of the face data is the one of
     * toolbox::blockstructured::serialiseVoxelIndexInOverlap().
     *
     * Different to @ref
     * exahype2::blockstructured::applyDirichletBoundaryConditions(), which only
     * needs a position and a time stamp, the Sommerfeld update needs two
     * versions of the face data:
     *
     * - faceDataOld at @f$ t^n @f$: both halves have to be valid, i.e. the
     *   interior half holds @f$ i^n @f$ and the ghost half @f$ o^n @f$.
     * - faceDataNew at @f$ t^{n+1} @f$: the interior half holds the new
     *   interior solution. The routine writes the ghost half.
     *
     * A scheme which projects its patches onto the faces into a temporary
     * buffer and then copies (rolls over) this buffer into the persistent
     * face data meets these requirements without any additional storage:
     * Directly before the roll over, the persistent face data still holds
     * the old solution, and the temporary buffer already holds the new
     * interior solution. The scheme hence calls the routine with
     * faceDataOld being the persistent face data and faceDataNew the
     * temporary buffer, and then rolls over both halves of the face. After
     * the roll over, the persistent ghost half holds @f$ o^{n+1} @f$, i.e.
     * it is ready for the next time step. A typical call from within the
     * generated touchFaceLastTime() reads
     *
     * @code
     * ::exahype2::blockstructured::applySommerfeldBoundaryConditions(
     *   [](const double* Q, const tarch::la::Vector<Dimensions,double>& x, int
     * normal) -> double { double lambdaMax;
     *     repositories::instanceOfMySolver.maxEigenvalue(Q, x, normal,
     * &lambdaMax); return lambdaMax;
     *   },
     *   [](const tarch::la::Vector<Dimensions,double>& x, double* Q) -> void {
     *     repositories::instanceOfMySolver.sommerfeldFarFieldSolution(x, Q);
     *   },
     *   marker,
     *   DomainOffset,
     *   DomainSize,
     *   PeriodicBC,
     *   NumberOfGridCellsPerAxis,
     *   Overlap,
     *   NumberOfUnknowns,
     *   NumberOfAuxiliaryVariables,
     *   {0.0, 0.0, 0.0},
     *   repositories::instanceOfMySolver.getMinTimeStepSize(),
     *   fineGridFaceQCurrentTimeStep.value,
     *   fineGridFaceQUpdate.value
     * );
     * // ... roll over fineGridFaceQUpdate into fineGridFaceQCurrentTimeStep
     * @endcode
     *
     * The routine can be called for any face. It leaves faces which are
     * not on the domain boundary or which lie along a periodic axis
     * unchanged, and it never writes into faceDataOld.
     *
     * A scheme has to take care of three further points:
     *
     * 1. <b>Initial data.</b> The ghost half of the persistent face data has
     *    to hold valid data before the first time step, as it is the
     *    @f$ o^n @f$ of the first update. A natural choice is to evaluate
     *    the initial condition at the centres of the ghost cells. This is
     *    exactly what @ref
     * exahype2::blockstructured::applyDirichletBoundaryConditions() does if it
     * is handed the initial condition as boundary condition.
     * 2. <b>Single-step schemes</b> such as explicit Euler in time call the
     *    routine once per time step with the time step size @f$ \Delta t
     *    @f$.
     * 3. <b>Runge-Kutta schemes</b> project the input @f$ Y_k @f$ of each
     *    stage onto the faces. The ghost cells of stage @f$ k @f$ should
     *    represent the boundary at the stage's time stamp @f$ t^n + c_k
     *    \Delta t @f$. Therefore, a scheme calls the routine before every
     *    roll over, and advances the ghost cells from one stage's time stamp
     *    to the next one, i.e. by @f$ (c_{k+1}-c_k) \Delta t @f$, and by
     *    @f$ (1-c_{s-1}) \Delta t @f$ after the last stage. Over a time
     *    step, these increments add up to @f$ \Delta t @f$. Stages with the
     *    same time stamp (classic RK4 has @f$ c = 0, \frac{1}{2},
     *    \frac{1}{2}, 1 @f$) yield an increment of zero. The stage inputs
     *    then differ, but they represent the same time, so the ghost cells
     *    remain unchanged. This is the reason why the routine copies the
     *    ghost cells for @f$ \Delta t = 0 @f$.
     *
     * The scheme does not need to know whether a face is on the domain
     * boundary: It may call the routine for all faces, and the routine
     * returns immediately for all faces within the domain.
     *
     *
     * # Choice of the wave speed and of the far field
     *
     * The wave speed is the speed at which the outgoing waves travel. The
     * routine evaluates it per ghost cell and uses one speed for all
     * unknowns. The maximum eigenvalue of the PDE along the normal is a
     * safe choice: It is the speed of the fastest wave. Some codes use a
     * known physical speed instead, such as the speed of light for most
     * fields of a spacetime. Both choices are up to the waveSpeed()
     * functor.
     *
     * The far-field solution @f$ u_\infty @f$ is the value the solution
     * approaches at infinity, e.g. the flat spacetime for a black hole. It
     * may depend on the position, but not on time.
     *
     *
     * # Limitations
     *
     * - The scheme only uses derivatives along the normal. Ghost cells in
     *   the edges and corners of the domain are never set, as the faces do
     *   not hold them.
     * - The condition assumes spherical waves around origin. All other
     *   waves are partially reflected.
     * - The time stepping of the ghost cells is first order for Runge-Kutta
     *   stages of higher order, as the box scheme only sees the interior
     *   solution at the stages' time stamps.
     * - The routine works on one resolution level. It is up to the scheme
     *   to provide valid data on all boundary faces it reads from.
     *
     *
     * # Differences to the legacy implementation
     *
     * The routine replaces the legacy exahype2::fd::applySommerfeldConditions()
     * (removed), which the legacy Python API (python/exahype2) injected
     * through the TemplateHandleBoundary_KernelCalls attribute of its
     * solvers. The legacy routine realised the same box scheme, but differed
     * in a few details:
     *
     * - It used the signed coordinate @f$ x_n @f$ within @f$ \sigma @f$. On
     *   lower boundary faces of a domain around the origin, @f$ \sigma
     *   @f$ hence was negative, i.e. the condition modelled an incoming
     *   wave. The update of the ghost cell then has an amplification factor
     *   of @f$ (1+|\sigma|\Delta t)/(1-|\sigma|\Delta t) > 1 @f$. We use the
     *   distance @f$ |x_n - x_{0,n}| @f$ instead.
     * - It measured @f$ r @f$ relative to the origin, but took @f$ x_n @f$
     *   from the absolute coordinate.
     * - It evaluated the wave speed at the face centre rather than at the
     *   ghost cell.
     * - It copied the inner data into the ghost cells of the old face data
     *   if the time stamp was zero or equalled a checkpoint time stamp, as
     *   the ghost cells had not been initialised. Here, it is the scheme's
     *   responsibility to initialise them.
     * - The auxiliary variables were taken from @f$ i^n @f$.
     *
     *
     * @param waveSpeed Functor returning the wave speed @f$ v @f$ for the
     *   unknowns and auxiliary variables Q of the old ghost cell at its
     *   centre x. normal is the axis of the face normal, i.e. a number
     *   between 0 and Dimensions-1.
     * @param farFieldSolution Functor writing the far-field solution
     *   @f$ u_\infty @f$ at position x into Q. Only the first
     *   numberOfUnknowns entries of Q are read, but Q is large enough to
     *   hold numberOfUnknowns+numberOfAuxiliaryVariables doubles.
     * @param marker Marker of the face, i.e. the face's centre, the size of
     *   the adjacent cells and the number of the face.
     * @param domainOffset Lower left corner of the computational domain.
     * @param domainSize Size of the computational domain.
     * @param periodicBC Periodic boundary conditions per axis. Faces along
     *   periodic axes are left unchanged.
     * @param numberOfGridCellsPerAxis Number of grid cells per axis within
     *   one patch.
     * @param overlap Number of grid cell layers per side of the face, i.e.
     *   the halo size.
     * @param numberOfUnknowns Number of unknowns per grid cell which are
     *   evolved by the Sommerfeld condition.
     * @param numberOfAuxiliaryVariables Number of auxiliary variables per
     *   grid cell. They follow the unknowns.
     * @param origin Centre @f$ x_0 @f$ of the system, i.e. the point the
     *   outgoing waves emanate from.
     * @param timeStepSize Time step size @f$ \Delta t @f$ by which the ghost
     *   cells are advanced. Zero copies the old ghost cells.
     * @param faceDataOld The face's overlap data at @f$ t^n @f$, i.e. both
     *   halves have to be valid. The routine does not modify it.
     * @param faceDataNew The face's overlap data at @f$ t^{n+1} @f$. The
     *   half inside the domain has to hold the new solution. The routine
     *   sets the ghost half.
     */
    void applySommerfeldBoundaryConditions(
      std::function<double(
        const double*                                Q,
        const tarch::la::Vector<Dimensions, double>& x,
        int                                          normal
      )> waveSpeed,
      std::function<
        void(const tarch::la::Vector<Dimensions, double>& x, double* Q)>
                                                   farFieldSolution,
      const peano4::datamanagement::FaceMarker&    marker,
      const tarch::la::Vector<Dimensions, double>& domainOffset,
      const tarch::la::Vector<Dimensions, double>& domainSize,
      const std::bitset<Dimensions>&               periodicBC,
      int                                          numberOfGridCellsPerAxis,
      int                                          overlap,
      int                                          numberOfUnknowns,
      int                                          numberOfAuxiliaryVariables,
      const tarch::la::Vector<Dimensions, double>& origin,
      double                                       timeStepSize,
      const double*                                faceDataOld,
      double*                                      faceDataNew
    );
  } // namespace blockstructured
} // namespace exahype2
