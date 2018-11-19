// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_UTILS_PEANO_OPTIMISATIONS
#define _PEANO4_UTILS_PEANO_OPTIMISATIONS


/**
 * This header provides multiple ways to tailor Peano algorithmically. However,
 * it does not provide ifdefs to tailor Peano to the chosen compiler. If you
 * wanna adopt your settings to your compiler, MPI installation, whatever,
 * please consult the compiler-specific settings.
 */
#include "tarch/compiler/CompilerSpecificSettings.h"


/**
 * Store only the part of the objects that is marked as persistent. Reduces
 * size (memory consumption) of the grid data types.
 */
#ifndef noStoreOnlyPersistentAttributes
#define StoreOnlyPersistentAttributes
#endif


/**
 * Peano data packing enabled: less data per node/cell
 * Only in very rare cases, this flag might have negative influence on runtime.
 * You may even use it in Debug mode, as it is no compiler optimisation flag.
 */
#ifndef noPackedRecords
#define PackedRecords
#endif


/**
 * Switch on manual alignment of vectors
 */
// @todo raus -> Wenn dann in Compiler Stuff
#define noUseManualAlignment

#ifdef noUseManualAlignment
  #if defined(VectorisationAlignment)
    #warning Specified VectorisationAlignment though manual alignment was switched off due to -DnoUseManualAlignment
  #endif
#elif !defined(VectorisationAlignment)
  #warning No alignment specified by compiler though UseManualAlignment is switched on
#endif


/**
 * Some MPI implementations cannot do data transfers in the background of
 * computations but need the code to call MPI_Test over and over again to
 * realise the data transfer.
 *
 * Flag only plays a role if you are heavily relying on heaps. For the standard
 * boundary data exchange, the testing is always implicitly enabled.
 *
 * It seems however that for most applications switching on this feature is not
 * a good idea.
 */
#if !defined(MPIProgressionReliesOnMPITest) &&  !defined(noMPIProgressionReliesOnMPITest)
#define noMPIProgressionReliesOnMPITest
#endif

/**
 * We usually do all the heap data exchange via non-blocking calls, i.e. all
 * meta data (how many records are exchanged) is communicated immediately while
 * the actual data transfer might linger in the background. For debugging and
 * optimisation, i.e. to free MPI from too many dangling requests, you can
 * alter this behaviour and make each heap send/receive a blocking
 * send/receive.
 *
 * Heap data is usually sent out in pairs: There's a meta data message
 * containing (among other data) the information how many records are
 * exchanged. And then next we transfer the actual data. So whenever we
 * receive a meta data message, we know that there will be real data, too.
 * With this flag, one can select whether this very data is to be received
 * blocking or non-blocking. By default, I try to do this real data exchange
 * in the background as I assume that this data is massive.
 */
#if !defined(noNonblockingHeapDataReceives) and !defined(NonblockingHeapDataReceives)
  #define NonblockingHeapDataReceives
#endif

/**
 * The boundary data exchange (grid data) relies on probing the MPI queues: We
 * check from time to time whether there are messages pending. If there's one,
 * we issue a receive. This receive either can be blocking or non-blocking. The
 * latter is the default as it implies that computation and communication can
 * overlap. There might however be cases where the non-blocking management
 * overhead renders a blocking data exchange advantageous.
 *
 * For most of the existing Peano applications, this flag did not make any
 * difference.
 */
#if !defined(noNonblockingBoundaryDataReceives) and !defined(NonblockingBoundaryDataReceives)
  #define NonblockingBoundaryDataReceives
#endif

#endif
