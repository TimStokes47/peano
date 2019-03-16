#ifdef defined(SharedTBB)
#include "JobProcessingService.h"

#include "tarch/multicore/Jobs.h"


#ifdef MPIWaitsProcessJobs
#include "tarch/services/ServiceFactory.h"
registerService(tarch::multicore::tbb::JobProcessingService)
#endif


tarch::multicore::tbb::JobProcessingService::JobProcessingService() {
}


tarch::multicore::tbb::JobProcessingService& tarch::multicore::tbb::JobProcessingService::getInstance() {
  static tarch::multicore::tbb::JobProcessingService singleton;
  return singleton;
}


void tarch::multicore::tbb::JobProcessingService::receiveDanglingMessages() {
  tarch::multicore::jobs::processBackgroundJobs();
  tarch::multicore::jobs::processHighBandwidthJobs();
}

#endif
