#include "ParticleSetIterators.h"


tarch::multicore::BooleanSemaphore exaparty::kernels::internal::_multiscaleInteractionSemaphore;


exaparty::kernels::kernelrealisation::GenericRealisation                     exaparty::kernels::kernelrealisation::Generic;
exaparty::kernels::kernelrealisation::CoalescedMemoryRealisation             exaparty::kernels::kernelrealisation::CoalescedMemory;
exaparty::kernels::kernelrealisation::CoalescedMemoryLocalActiveRealisation  exaparty::kernels::kernelrealisation::CoalescedMemoryLocalActive;
exaparty::kernels::kernelrealisation::CoalescedMemoryActiveLocalRealisation  exaparty::kernels::kernelrealisation::CoalescedMemoryActiveLocal;
exaparty::kernels::kernelrealisation::EvaluatePredicateInPrologueRealisation exaparty::kernels::kernelrealisation::EvaluatePredicateInPrologue;
exaparty::kernels::kernelrealisation::EvaluatePredicateInPrologueLocalActiveRealisation  exaparty::kernels::kernelrealisation::EvaluatePredicateInPrologueLocalActive;
exaparty::kernels::kernelrealisation::EvaluatePredicateInPrologueActiveLocalRealisation  exaparty::kernels::kernelrealisation::EvaluatePredicateInPrologueActiveLocal;
exaparty::kernels::kernelrealisation::GenericOffloadRealisation              exaparty::kernels::kernelrealisation::GenericOffload;

