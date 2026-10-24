// This file is part of the ExaGRyPE project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#pragma once


#include <cmath>
#include "tarch/tarch.h"
#include "tarch/compiler/CompilerSpecificSettings.h"
#include "tarch/multicore/multicore.h"
#include "tarch/accelerator/accelerator.h"
#include "tarch/la/Vector.h"


namespace exagrype {
  namespace ccz4 {

      #if defined(GPUOffloadingOMP)
      #pragma omp declare target
      #endif
      /**
       * Non-conservative product of the first-order CCZ4 formulation.
       *
       * ncp(), source() and maxEigenvalue() have one implementation in
       * Kernels.cpp which serves both the CPU and OpenMP offloading. With
       * GPUOffloadingOMP, they are device functions (omp declare target),
       * declared here and defined in Kernels.cpp, i.e. they are not inlined
       * into the GPU kernels.
       *
       * Older LLVM OpenMP implementations failed on local arrays in target
       * functions, so Peano used to ship a separate array-free GPU variant
       * of these three functions (KernelsOpenMP.cpph). That copy had drifted
       * from the CPU version and has been removed.
       *
       * @ref source() discusses the remaining annotations.
       */
      #if defined(SharedOMP)
      #pragma omp declare simd
      #endif
      void ncp(double* BgradQ, const double* const Q, const double* const gradQSerialised, const int normal,
        const int CCZ4LapseType,
        const double CCZ4ds,
        const double CCZ4c,
        const double CCZ4e,
        const double CCZ4f,
        const double CCZ4bs,
        const double CCZ4sk,
        const double CCZ4xi,
        const double CCZ4mu,
        const double CCZ4SO
      );

      /**
       * The source term is one out of two terms that we use in our CCZ4
       * formulation. As it is used within the compute kernel, we also
       * want to use it on the GPU, and we want to vectorise over it
       * aggressively. To make this work, we have to inline the function
       * plus ensure that the interna of the function are not vectorised
       * before we inline the routine.
       *
       * So we first declare it as simd and then we also say explicitly
       * that it should be inlined. In my opinion, the inlining should
       * imply that we wanna use it as SIMD, but my guess is that the
       * simd statement ensures that the compiler doesn't prematurely
       * vectorise.
       *
       * @see ncp()
       */
      #if defined(SharedOMP)
      #pragma omp declare simd
      #endif
      void source(double* S, const double* const Q,
        const int CCZ4LapseType,
        const double CCZ4ds,
        const double CCZ4c,
        const double CCZ4e,
        const double CCZ4f,
        const double CCZ4bs,
        const double CCZ4sk,
        const double CCZ4xi,
        const double CCZ4itau,
        const double CCZ4eta,
        const double CCZ4k1,
        const double CCZ4k2,
        const double CCZ4k3,
        const double CCZ4SO
      );

      #if defined(SharedOMP)
      #pragma omp declare simd
      #endif
      void maxEigenvalue(
        const double* const Q,
        int                 normal,
        const double        CCZ4e,
        const double        CCZ4ds,
        const double        CCZ4GLMc,
        const double        CCZ4GLMd,
        double*             maxEigenvalue
      );
      #if defined(GPUOffloadingOMP)
      #pragma omp end declare target
      #endif

      /**
       * @todo Han
       */
      #if defined(SharedOMP)
      #pragma omp declare simd
      #endif
      KeywordToAvoidDuplicateSymbolsForInlinedFunctions void maxEigenvalue(
        const double* const Q,
        int                 normal,
        const double        CCZ4e,
        const double        CCZ4ds,
        const double        CCZ4GLMc,
        const double        CCZ4GLMd
      ) InlineMethod;

      /**
       * This is a postprocessing routine to monitor if the physical constraints
       * are fulfilled. Used in python script.
       */
      void admconstraints(double* constraints, const double* const Q, const double* const gradQSerialised);

      /**
       * A temporary test function, to output Hamilton constraint related term in theta, 1 terms: RPlusTwoNablaZNCP
       */
      void ThetaOutputNCP(double* NCPterm, const double* const Q, const double* const gradQSerialised, int normal);
      /**
       * A temporary test function, to output some testing values 
       * 0,1 entries: Hamilton constraint related term in theta, 2 terms: RPlusTwoNablaZSrc, pure Src
       * 2-10 entry: no-symmetry version of R_ij
       */
      void TestingOutput(double* terms, const double* const Q, const double* const gradQSerialised);

      /**
       * This function is for the calculation of psi4, a quantity related to gravitional wave. 
       * Not used yet. 
       */
      void Psi4Calc(double* Psi4, const double* const Q, const double* const gradQSerialised, double* coor);


      /**
       * A postprocessing routine which pushes the volume solution back into the
       * area of the CCZ4 constraints. This is a local operation that we can
       * invoke after each time step per volume.
       *
       * If works on the full set of unknowns of the first-order formulation
       */
      void enforceCCZ4constraints(double* __restrict__  newQ);

      /**
       * Anticipate Euler step and correct dQdt such that outcome does fulfill
       * constraints.
       */
      void enforceCCZ4constraints(
        const double* __restrict__  oldQ,
        double* __restrict__        dQdt,
        double                      timeStepSize
      );

      /**
       * Add the Kreiss-Oliger dissipation of a patch to its right-hand side
       *
       * The routine adds
       *
       * @f$ \frac{\sigma}{64 \Delta x_d} (Q_{i-3} - 6 Q_{i-2} + 15 Q_{i-1} - 20 Q_i + 15 Q_{i+1} - 6 Q_{i+2} + Q_{i+3}) @f$
       *
       * per axis d to the right-hand side of each grid cell i of a patch,
       * i.e. a sixth-order dissipation term which damps oscillations on the
       * scale of the grid cells. @f$ \Delta x_d @f$ is the patch size along
       * d divided by the number of grid cells per axis. The term is added to
       * the selected quantities only, all other entries of the right-hand
       * side remain unchanged.
       *
       * The routine is the dissipation which the legacy fourth-order Finite
       * Differences of ExaHyPE (exahype2::fd::fd4) built into their kernels.
       * The Finite Differences of exahype2.api.rkfd4 have no dissipation.
       * exagrype.api.FD4WithKODissipation invokes this routine after their
       * right-hand side kernel, within each Runge-Kutta stage.
       *
       * Both arrays are stored lexicographically in AoS format, i.e. all
       * quantities of one grid cell are stored together, and the first axis
       * runs fastest. The stencil reaches three grid cells into the halo,
       * i.e. haloSize has to be at least three.
       *
       * @param QWithHalo Input of the Runge-Kutta stage with its halo, i.e.
       *   (numberOfGridCellsPerPatchPerAxis+2*haloSize)^3 grid cells with
       *   unknowns+auxiliaryVariables entries each.
       * @param rightHandSide Right-hand side of the stage without halo, i.e.
       *   numberOfGridCellsPerPatchPerAxis^3 grid cells with unknowns entries
       *   each. The routine adds the dissipation to it.
       * @param patchSize Size of the patch, i.e. of the mesh cell.
       * @param quantities Indices of the unknowns which are subject to the
       *   dissipation. Each index has to be smaller than unknowns.
       * @param numberOfQuantities Number of entries of quantities.
       */
      void addKreissOligerDissipation(
        const double* __restrict__          QWithHalo,
        double* __restrict__                rightHandSide,
        const tarch::la::Vector<3, double>& patchSize,
        int                                 numberOfGridCellsPerPatchPerAxis,
        int                                 haloSize,
        int                                 unknowns,
        int                                 auxiliaryVariables,
        double                              sigma,
        const int*                          quantities,
        int                                 numberOfQuantities
      );
  }
}
