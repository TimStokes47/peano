#include "tarch/logging/Log.h"
#include "tarch/compiler/CompilerSpecificSettings.h"
#include "tarch/Assertions.h"
#include "tarch/multicore/Core.h"

#include "config.h"


#include "tarch/logging/LogFilter.h"
#include "tarch/logging/CommandLineLogger.h"
#include "tarch/logging/ChromeTraceFileLogger.h"


/**
 * For the machine name. If it doesn't work, switch it off in the file
 * CompilerSpecificSettings.h.
 */
#ifdef CompilerHasUTSName
#include <sys/utsname.h>
#endif

#include "tarch/mpi/Rank.h"

#include <time.h>


#include "tarch/compiler/CompilerSpecificSettings.h"


tarch::logging::Log::Log(const std::string& className):
  _className( className ),
  _hasQueriedFilter(false),
  _logTrace(true),
  _logDebug(true),
  _logInfo(true) {
  _startupTime = std::chrono::system_clock::now();
}


tarch::logging::Log::~Log() {
}


#if PeanoDebug>=4
void tarch::logging::Log::debug(const std::string& methodName, const std::string& message) {
  if (not _hasQueriedFilter) {
	_hasQueriedFilter = true;
	_logDebug = LogFilter::getInstance().writeDebug( _className );
  }

  if (_logDebug) {
    UsedLogService::getInstance().debug(getTimeStamp(),tarch::mpi::Rank::getInstance().getRank(),tarch::multicore::Core::getInstance().getCoreNumber(),getTraceInformation(methodName),message);
  }
}
#endif


void tarch::logging::Log::info(const std::string& methodName, const std::string& message) {
  if (not _hasQueriedFilter) {
	_hasQueriedFilter = true;
	_logInfo = LogFilter::getInstance().writeInfo( _className );
  }
  if (_logInfo)
    UsedLogService::getInstance().info(getTimeStamp(),tarch::mpi::Rank::getInstance().getRank(),tarch::multicore::Core::getInstance().getCoreNumber(),getTraceInformation(methodName),message);
}


void tarch::logging::Log::warning(const std::string& methodName, const std::string& message) {
  UsedLogService::getInstance().warning(getTimeStamp(),tarch::mpi::Rank::getInstance().getRank(),tarch::multicore::Core::getInstance().getCoreNumber(),getTraceInformation(methodName),message);
}


void tarch::logging::Log::error(const std::string& methodName, const std::string& message) {
  UsedLogService::getInstance().error(getTimeStamp(),tarch::mpi::Rank::getInstance().getRank(),tarch::multicore::Core::getInstance().getCoreNumber(),getTraceInformation(methodName),message);
}


#if PeanoDebug>=1
void tarch::logging::Log::traceIn(const std::string& methodName, const std::string& message) {
  if (not _hasQueriedFilter) {
	_hasQueriedFilter = true;
	_logTrace = LogFilter::getInstance().writeTrace( _className );
  }
  if (_logTrace)
    UsedLogService::getInstance().traceIn(getTimeStamp(),tarch::mpi::Rank::getInstance().getRank(),tarch::multicore::Core::getInstance().getCoreNumber(),getTraceInformation(methodName),message);
}


void tarch::logging::Log::traceOut(const std::string& methodName, const std::string& message) {
  if (_logTrace)
    UsedLogService::getInstance().traceOut(getTimeStamp(),tarch::mpi::Rank::getInstance().getRank(),tarch::multicore::Core::getInstance().getCoreNumber(),getTraceInformation(methodName),message);
}
#else
void tarch::logging::Log::traceIn(const std::string& methodName, const std::string& message) {
}


void tarch::logging::Log::traceOut(const std::string& methodName, const std::string& message) {
}
#endif


void tarch::logging::Log::indent( bool indent, const std::string& trace, const std::string& message ) const {
  UsedLogService::getInstance().indent( indent, trace, message );
}


std::string tarch::logging::Log::getMachineInformation() {
  std::ostringstream message;

  #ifdef CompilerHasUTSName
  utsname* utsdata = new utsname();
  assertion( utsdata!=NULL );
  uname(utsdata);

  message << "[" << utsdata->nodename << "]";

  #ifdef Parallel
  message << ",";
  #endif

  delete utsdata;
  #endif

  #ifdef Parallel
  if (tarch::mpi::Rank::getInstance().isInitialised()) {
    message << "rank:" << tarch::mpi::Rank::getInstance().getRank();
  }
  else {
    message << "rank:not-initialised-yet";
  }
  #else
  message << "rank:0";
  #endif

  return message.str();
}


void tarch::logging::Log::flushBeforeAssertion() {
  UsedLogService::getInstance().close();
}


long int tarch::logging::Log::getTimeStamp() const {
/*
  #ifdef SharedOMP
    double currentTS       = omp_get_wtime();
    return currentTS - _startupTime;
  #elif defined(SharedTBB)
    tbb::tick_count currentTS       = tbb::tick_count::now();
    return (currentTS - _startupTime).seconds();
  #elif defined(__APPLE__)
    static mach_timebase_info_data_t s_timebase_info;
    if (s_timebase_info.denom == 0) mach_timebase_info(&s_timebase_info);
    return (double)((mach_absolute_time() - _startupTime) * (s_timebase_info.numer) / s_timebase_info.denom) * 1e-09;
*/
/*
  #if defined(CompilerHasTimespec)
    struct timespec ts;
    if( clock_gettime(CLOCK_REALTIME, &ts) == 0 ) {
       const double currentTS = (double)ts.tv_sec + (double)ts.tv_nsec * 1e-09;
       return currentTS - _startupTime;
    }
    else {
      return 0;
    }
  #else
    return 0;
  #endif
*/
  std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
  long int result = std::chrono::duration_cast<std::chrono::nanoseconds>(now - _startupTime).count();
  assertion1(result>=0,result);
  return result;
}

/*
std::string tarch::logging::Log::getTimeStampHumanReadable() const {
  // calender time: create struct and get time from system
  time_t* timeStamp = new time_t();
  assertion( timeStamp!=NULL );
  time(timeStamp);

  // Break down time into hour, seconds, ...
  // Note that time is only a substructure of timeStamp. Therefore the pointer
  // to time may not be deleted.
  tm*     time      = localtime(timeStamp);
  assertion( time!=NULL );

  std::ostringstream message;

  // write all information
  if (time->tm_hour<10) {
    message << "0";
  }
  message << time->tm_hour << ":";

  if (time->tm_min<10) {
    message << "0";
  }
  message << time->tm_min << ":";

  if (time->tm_sec<10) {
    message << "0";
  }
  message << time->tm_sec;

  delete timeStamp;

  return message.str();
}*/


std::string tarch::logging::Log::getTraceInformation( const std::string& methodName ) const {
  return _className + "::" + methodName;
}

