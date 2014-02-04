// Default variant of CompilerSpecificSettings.h for Linux with GCC.

/**
 * Switch off Optimisation
 *
 * Some compilers (icc 10.x, e.g.) run into problems compiling the test cases
 * as they run out of memory. In this case, one can use these two defined
 * within the implementation files. Unfortunately, the corresponding pragmas
 * are not supported by all compilers (gcc 4.x.x, e.g.). Therefore, I
 * encapsulated them within my own defines.
 *
 * To make define work, see the documentation of the test super class.
 */
//#define UseTestSpecificCompilerSettings



/**
 * Switch on the CLX Compiler
 *
 * Using this compiler switch makes the code pass the CLX compiler.
 *
 * !!! Usage of keyword typename
 * Some compilers (on BlueGene/P, e.g.) require the implementations of templates
 * to use the typename keyword permanenlty. In contrast, they do not allow the
 * header to use typename everywhere. GCC is completely different, i.e. it
 * sometimes complains if there's a typename used in the implementation without
 * the typename keyword in the header. The Intel compiler is less restrictive
 * and accepts both variants.
 *
 * This keyword controls the typename usage. It has to be set on the BlueGene.
 * It has to be commented out if one uses the gcc.
 */
//#define CompilerCLX

/**
 * Switch on the Intel Compiler
 *
 */
//#define CompilerICC

// @todo wieder raus
//#define CompilerHasSSE
//#define VectorisationAlignment 16


/**
 * Defines whether the statement
 *   sprintf(work, "/proc/%d/stat", (int)pid);
 * does work. Typically on all UNIX systems.
 */
#define CompilerHasProcStat

/**
 * UTSName header
 *
 * For the logging of the machine, we need the header $<sys/utsname.h>$. This
 * header however is not available on windows machines, i.e. if you are on
 * windows, comment this statement out.
 */
#define CompilerHasUTSName


#define CompilerHasTimespec


/**
 * Some compiler/MPI combinations define the constant
 * MPI_MAX_NAME_STRING. In this case, please comment this flag out.
 */
//#define CompilerDefinesMPIMaxNameString


/**
 * Padding for Packed Records
 *
 * Usually, there's no need to set these values manually. However, if you prefer,
 * you can change the padding of the packed records.
 */
//#define DaStGenPackedPadding 1      // 32 bit version
// #define DaStGenPackedPadding 2   // 64 bit version



/**
 * Some platforms profit from a specialisation of the linear algebra vectors of
 * small sizes (typically 2 and 3) when the specialisation explicitly loads the
 * unknowns into register variables.
 */
#define SpecialiseVectorTemplatesForIntegers

//#ifndef noMultipleThreadsMayTriggerMPICalls
//#define MultipleThreadsMayTriggerMPICalls
//#endif


/**
 * Peano relies on synchronous and asynchronous messages. Synchronous messages
 * are sent up and down the tree throughout the traversal or are used to
 * communicate with the load balancing rank, e.g. By default, Peano also uses
 * non-blocking methods for these messages and tries to receive dangling
 * messages after the send has been triggered until it has completed. This way,
 * Peano avoids deadlocks. However, you can observe a signficiant speedup if
 * you switch from a non-blocking realisation (false as argument here) to plain
 * MPI_Send or MPI_Recv, respectively.
 */
#define SendSynchronousMessagesBlocking false
