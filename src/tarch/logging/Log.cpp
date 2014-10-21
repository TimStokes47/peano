#include "tarch/logging/Log.h"
#include "tarch/logging/CommandLineLogger.h"
#include "tarch/compiler/CompilerSpecificSettings.h"
#include "tarch/Assertions.h"

/**
 * For the machine name. If it doesn't work, switch it off in the file
 * CompilerSpecificSettings.h.
 */
#ifdef CompilerHasUTSName
#include <sys/utsname.h>
#endif

#ifdef Parallel
#include "tarch/parallel/Node.h"
#endif

#include <time.h>


#include "tarch/compiler/CompilerSpecificSettings.h"

#if !defined(SharedTBB) && !defined(SharedOMP) && defined(CompilerHasTimespec)
namespace tarch {
  namespace logging {
    /**
     * Forward declaration
     *
     * We need that dummy if and only if we are not doing shared memory stuff. If
     * we do shared memory, we rely on the shared memory libs' operations to get
     * the right time instead of this timespec struct.
     */
    struct timespec ts;
  }
}
#endif


tarch::logging::Log::Log(const std::string& className):
  _className( className ) {
}


tarch::logging::Log::~Log() {
}


#if defined(Debug) && !defined(LogOff)
void tarch::logging::Log::debug(const std::string& methodName, const std::string& message) const {
  UsedLogService::getInstance().debug(getTimeStampSeconds(),getTimeStampHumanReadable(),getMachineInformation(),getTraceInformation(methodName),message);
}
#endif


#if !defined(LogOff)
void tarch::logging::Log::info(const std::string& methodName, const std::string& message) const {
  UsedLogService::getInstance().info(getTimeStampSeconds(),getTimeStampHumanReadable(),getMachineInformation(),getTraceInformation(methodName),message);
}

void tarch::logging::Log::warning(const std::string& methodName, const std::string& message) const {
  UsedLogService::getInstance().warning(getTimeStampSeconds(),getTimeStampHumanReadable(),getMachineInformation(),getTraceInformation(methodName),message);
}

void tarch::logging::Log::error(const std::string& methodName, const std::string& message) const {
  UsedLogService::getInstance().error(getTimeStampSeconds(),getTimeStampHumanReadable(),getMachineInformation(),getTraceInformation(methodName),message);
}

void tarch::logging::Log::indent( bool indent, const std::string& trace, const std::string& message ) const {
  #if defined(Debug)
  UsedLogService::getInstance().indent( indent, trace, message );
  #endif
}
#endif


std::string tarch::logging::Log::getMachineInformation() const {
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
  if (tarch::parallel::Node::getInstance().isInitialised()) {
    message << "rank:" << tarch::parallel::Node::getInstance().getRank();
  }
  else {
    message << "rank:not-initialised-yet";
  }
  #else
  message << "rank:0";
  #endif

  return message.str();
}


double tarch::logging::Log::getTimeStampSeconds() const {
  #if defined(CompilerHasTimespec)
  if( clock_gettime(CLOCK_REALTIME, &ts) == 0 ) {
     return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-09;
  }
  else {
    return 0.0;
  }
  #else
  return 0.0;
  #endif
}


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
}


std::string tarch::logging::Log::getTraceInformation( const std::string& methodName ) const {
  return _className + "::" + methodName;
}
