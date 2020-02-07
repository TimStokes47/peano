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

 To compile with multicore support, we currently offer three variants:
 
 -DSharedOMP       OpenMP    Not maintained at the moment due to a lack of time
 -DSharedTBB       TBB       Default
 -SharedTBBInvade  TBB       Invasive variant of TBB (collaboration with Martin Schreiber, Exceter)

 
 If you wanna distinguish in your code between multicore and no-multicore variants, 
 please use
\code
#include "tarch/multicore/MulticoreDefinitions.h"
\endcode
and  
\code
#if defined(SharedMemoryParallelisation)
\endcode
 With the symbol $SharedMemoryParallelisation$, you make your code independent of 
 OpenMP or TBB.  

 */
  namespace multicore {
  }
}

