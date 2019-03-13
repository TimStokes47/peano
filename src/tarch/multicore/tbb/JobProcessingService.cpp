#include "JobProcessingService.h"

#include "tarch/multicore/Jobs.h"


#include "tarch/services/ServiceFactory.h"
registerService(tarch::multicore::tbb::JobProcessingService)


tarch::multicore::tbb::JobProcessingService::JobProcessingService() {
}


tarch::multicore::tbb::JobProcessingService& tarch::multicore::tbb::JobProcessingService::getInstance() {
  static tarch::multicore::tbb::JobProcessingService singleton;
  return singleton;
}


void tarch::multicore::tbb::JobProcessingService::receiveDanglingMessages() {
  tarch::multicore::jobs::processBackgroundJobs();
  tarch::multicore::jobs::processHighPriorityJobs();
  tarch::multicore::jobs::processHighBandwidthJobs();
}
