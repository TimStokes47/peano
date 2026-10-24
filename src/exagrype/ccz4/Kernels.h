// This file is part of the ExaGRyPE project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#pragma once


#include <cmath>
#include "tarch/tarch.h"
#include "tarch/compiler/CompilerSpecificSettings.h"
#include "tarch/multicore/multicore.h"
#include "tarch/accelerator/accelerator.h"


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
  }
}
