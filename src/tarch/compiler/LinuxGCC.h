// Default variant of CompilerSpecificSettings.h for IBM's XLC compiler.
// For a detailed description of the semantics of the settings, please
// consult LinuxIntel.h which is the most elaborate documentation.

//#define UseTestSpecificCompilerSettings
//#define CompilerCLANG
//#define CompilerICC
//#define VectorisationAlignment 16
#define CompilerHasProcStat
#define CompilerHasUTSName
#define CompilerHasTimespec
#define CompilerHasSysinfo
//#define CompilerDefinesMPIMaxNameString
//#define DaStGenPackedPadding 1      // 32 bit version
// #define DaStGenPackedPadding 2   // 64 bit version
#define SpecialiseVectorTemplatesForIntegers

#ifndef noMultipleThreadsMayTriggerMPICalls
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
#ifndef BroadcastToIdleNodesBlocking
 #define BroadcastToIdleNodesBlocking                   0
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

#define LittleEndian


#if !defined(MPIProgressionReliesOnMPITest) &&  !defined(noMPIProgressionReliesOnMPITest)
#define MPIProgressionReliesOnMPITest
#endif


#if !defined(UseTBBsParallelForAndReduce) &&  !defined(noUseTBBsParallelForAndReduce)
#define UseTBBsParallelForAndReduce
#endif


