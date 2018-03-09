// Default variant of CompilerSpecificSettings.h for IBM's XLC compiler.
// For a detailed description of the semantics of the settings, please
// consult LinuxIntel.h which is the most elaborate documentation.


#define UseTestSpecificCompilerSettings


#ifndef noCompilerICC
  #define CompilerICC
#endif

#define CompilerHasProcStat

#define CompilerHasUTSName

#define CompilerHasTimespec

#define CompilerHasSysinfo

//#define CompilerDefinesMPIMaxNameString


//#define DaStGenPackedPadding 1      // 32 bit version
// #define DaStGenPackedPadding 2   // 64 bit version

//#define SpecialiseVectorTemplatesForIntegers

#ifndef noMultipleThreadsMayTriggerMPICalls
#define MultipleThreadsMayTriggerMPICalls
#endif


/**
 * These values are integer values which define which variant of MPI Sends and
 * Receives is actually used. A value smaller than 0 instructs the code to use
 * plain MPI_Recv and MPI_Send, i.e. blocking operation variants. If the value
 * equals 0, the code triggers a logically blocking operation through and
 * ISend or IRecv, respectively, and then enters immediately a loop that tests
 * whether it has completed before the non-blocking handles are released.
 * Within the loop, Peano both realises deadlock detection and it also calls
 *
 * tarch::parallel::Node::getInstance().receiveDanglingMessages();
 *
 * which is the built-in deadlock avoidance which tries to empty all dangling
 * MPI handles whenever possible.
 *
 * If you finally use a value of that is greater than 0, you retain the
 * Isend/Irecv variant with its deadlock/dangling features. Furthermore, the
 * code goes to sleep after each unsuccessful completion test for the specified
 * number of microseconds.
 */
#define SendWorkerMasterMessagesBlocking     0
#define SendMasterWorkerMessagesBlocking     0
#define ReceiveMasterMessagesBlocking        0
#define SendAndReceiveLoadBalancingMessagesBlocking    0
#define ReceiveIterationControlMessagesBlocking        0
#define BroadcastToIdleNodesBlocking                   0
#define BroadcastToWorkingNodesBlocking                0
#define SendHeapMetaDataBlocking                       0
#define SendAndReceiveHeapSynchronousDataBlocking      0



#ifndef noManualInlining
#define UseManualInlining
#endif

#define LittleEndian

#define MPIProgressionReliesOnMPITest
