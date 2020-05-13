// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org


#include "config.h"

#if defined(SharedOMP) || defined(SharedTBB) || defined(SharedCPP)
  #define SharedMemoryParallelisation
#endif

#if defined(TBBInvade) && !defined(SharedTBB)
#error TBBInvade used without SharedTBB
#endif



namespace tarch {


  /**
 
 \namespace "tarch::multicore"

 <h1> Peano 4's multithreading layer </h1>

 To compile with multicore support, you have to invoke the configure script with
 the option --with-multithreading=value where value is

 - cpp. This adds support through C++14 threads.
 - tbb. This adds support through Intel's Threading Building Blocks. If you use
   this option, you first have to ensure that your CXXFLAGS and LDFLAGS point to
   the right include or library, respectively, directories. LDFLAGS also has to
   compromise either -ltbb or -tbb_debug.
 - openmp. This adds OpenMP support. We currently develop against OpenMP 4.x
   though some of our routines use OpenMP target and thus are developed against
   OpenMP 5.

 <h2> Writing your own code with multithreading features </h2>

 If you wanna distinguish in your code between multicore and no-multicore variants, 
 please use
\code
#include "tarch/multicore/multicore.h"
\endcode
and  
\code
#if defined(SharedMemoryParallelisation)
\endcode
 With the symbol SharedMemoryParallelisation, you make your code independent of
 OpenMP, TBB or C++ threading.

 Our vision is that each code should be totally independent of the multithreading
 implementation chosen. Indeed, Peano 4 itself does not contain any direct
 multithreading library calls. It solely relies on the classes and functions from
 this namespace.

 */
  namespace multicore {
  }
}

