#include "ParticleSpecies.h"

#include <limits>

#include "tarch/Assertions.h"
#include "tarch/mpi/Rank.h"
#include "tarch/multicore/Lock.h"
#include "tarch/services/ServiceRepository.h"

tarch::multicore::BooleanSemaphore exaparty::ParticleSpecies::_semaphore;
tarch::logging::Log                exaparty::ParticleSpecies::_log("exaparty::ParticleSpecies");
