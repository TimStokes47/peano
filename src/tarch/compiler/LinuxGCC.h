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


#if !defined(noMultipleThreadsMayTriggerMPICalls) && !defined(MultipleThreadsMayTriggerMPICalls)
#define MultipleThreadsMayTriggerMPICalls
#endif


#if !defined(UseManualInlining) &&  !defined(noUseManualInlining)
#define UseManualInlining
#endif


/**
 * I basically need these settings only in the plotters
 */
#define LittleEndian

#if !defined(noMPI2) && !defined(MPI2)
#define MPI2
#endif
