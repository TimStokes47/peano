// Default variant of CompilerSpecificSettings.h for the Intel compiler running on Windows.
// For a detailed description of the semantics of the settings, please
// consult LinuxIntel.h which is the most elaborate documentation.


/**
 * The Intel compiler based upon Microsoft's Visual C++ does not support the
 * __align__ keyword. We thus disable this feature though you might try to
 * set it in PeanoOptimisations.h
 */
#define noManualAlignment
#define noManualInlining


#define UseTestSpecificCompilerSettings
//#define CompilerCLANG
#define CompilerICC
//#define CompilerHasProcStat
//#define CompilerHasUTSName
#define CompilerHasTimespec
//#define CompilerHasSysinfo
//#define CompilerDefinesMPIMaxNameString
//#define DaStGenPackedPadding 1      // 32 bit version
// #define DaStGenPackedPadding 2   // 64 bit version
//#define SpecialiseVectorTemplatesForIntegers


#if !defined(noMultipleThreadsMayTriggerMPICalls) && !defined(MultipleThreadsMayTriggerMPICalls)
#define MultipleThreadsMayTriggerMPICalls
#endif


#ifndef SendWorkerMasterMessagesBlocking
 #define SendWorkerMasterMessagesBlocking     0
#endif
#ifndef SendMasterWorkerMessagesBlocking
 #define SendMasterWorkerMessagesBlocking     0
#endif
#ifndef ReceiveMasterMessagesBlocking
 #define ReceiveMasterMessagesBlocking        0
#endif
#ifndef SendAndReceiveLoadBalancingMessagesBlocking
 #define SendAndReceiveLoadBalancingMessagesBlocking    0
#endif
#ifndef ReceiveIterationControlMessagesBlocking
 #define ReceiveIterationControlMessagesBlocking        0
#endif
#ifndef BroadcastToWorkingNodesBlocking
 #define BroadcastToWorkingNodesBlocking                0
#endif
#ifndef SendHeapMetaDataBlocking
 #define SendHeapMetaDataBlocking                       0
#endif
#ifndef SendAndReceiveHeapSynchronousDataBlocking
 #define SendAndReceiveHeapSynchronousDataBlocking      0
#endif


#if !defined(UseManualInlining) &&  !defined(noUseManualInlining)
#define UseManualInlining
#endif

#if !defined(MPIProgressionReliesOnMPITest) &&  !defined(noMPIProgressionReliesOnMPITest)
#define MPIProgressionReliesOnMPITest
#endif

#define LittleEndian

#define MPIProgressionReliesOnMPITest
#if !defined(UseTBBsParallelForAndReduce) &&  !defined(noUseTBBsParallelForAndReduce)
#define UseTBBsParallelForAndReduce
#endif


#if defined(MultipleThreadsMayTriggerMPICalls) && !defined(noMPIHeapUsesItsOwnThread) && !defined(MPIHeapUsesItsOwnThread)
#define MPIHeapUsesItsOwnThread
#endif

#if defined(MultipleThreadsMayTriggerMPICalls) && !defined(noMPIUsesItsOwnThread) && !defined(MPIUsesItsOwnThread)
#define MPIUsesItsOwnThread
#endif



#if !defined(BooleanSemaphoreUsesASpinLock) && !defined(noBooleanSemaphoreUsesASpinLock)
#define BooleanSemaphoreUsesASpinLock
#endif

#if !defined(JobQueueUsesSpinLockInsteadOfMutex) && !defined(noJobQueueUsesSpinLockInsteadOfMutex)
#define JobQueueUsesSpinLockInsteadOfMutex
#endif

#if !defined(JobQueueUsesStackOfBefilledQueues) && !defined(noJobQueueUsesStackOfBefilledQueues)
#define JobQueueUsesStackOfBefilledQueues
#endif
