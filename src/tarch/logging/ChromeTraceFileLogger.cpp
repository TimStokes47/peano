#include "tarch/logging/ChromeTraceFileLogger.h"

#include "tarch/Assertions.h"

#include "tarch/multicore/Lock.h"
#include "tarch/multicore/MulticoreDefinitions.h"

#include <sstream>
#include <fstream>
#include <stdlib.h>

#include "../mpi/Rank.h"



tarch::logging::Log tarch::logging::ChromeTraceFileLogger::_log( "tarch::logging::ChromeTraceFileLogger" );


tarch::logging::ChromeTraceFileLogger::ChromeTraceFileLogger():
  _outputStream(nullptr),
  _quitOnError(false) {
  configureOutputStreams();

  #if PeanoDebug>=1
  addFilterListEntry( FilterListEntry( "debug", false )  );
  #else
  addFilterListEntry( FilterListEntry( "debug", true )  );
  #endif
  addFilterListEntry( FilterListEntry( "info", false )  );
}


tarch::logging::ChromeTraceFileLogger& tarch::logging::ChromeTraceFileLogger::getInstance() {
  static ChromeTraceFileLogger singleton;
  return singleton;
}


std::ostream& tarch::logging::ChromeTraceFileLogger::out() {
  if (_outputStream==nullptr) {
    return std::cout;
  }
  else {
    return *_outputStream;
  }
}


void tarch::logging::ChromeTraceFileLogger::configureOutputStreams() {
  out().setf( std::ios_base::scientific, std::ios_base::floatfield );
  out().precision(20);
  std::cerr.setf( std::ios_base::scientific, std::ios_base::floatfield );
  std::cerr.precision(20);
}


tarch::logging::ChromeTraceFileLogger& tarch::logging::ChromeTraceFileLogger::operator=(const ChromeTraceFileLogger& rhs) {
  return *this;
}


tarch::logging::ChromeTraceFileLogger::ChromeTraceFileLogger(const ChromeTraceFileLogger& param) {
}


tarch::logging::ChromeTraceFileLogger::~ChromeTraceFileLogger() {
  if (_outputStream!=nullptr) {
	*_outputStream << "]";
    delete _outputStream;
    _outputStream = nullptr;
  }
  else {
	std::cerr << "Warning: ChromeTraceFileLogger used, but not trace file set. Use singleton's setOuputFile()" << std::endl;
  }
}


std::string tarch::logging::ChromeTraceFileLogger::constructMessageString(
  std::string          messageType,
  double               timestamp,
  std::string          timestampHumanReadable,
  std::string          machineName,
  std::string          threadName,
  std::string          trace,
  const std::string&   message
) {
  std::string prefix = "";
//  for (unsigned int i=0; i<_indent; i++ ) prefix += " ";

  std::string result;

/*
  if ( getLogTimeStamp() ) {
    std::ostringstream timeStampString;
    timeStampString << timestamp;
    result += addSeparators(NumberOfStandardColumnSpaces,timeStampString.str() );
  }

  if ( getLogTimeStampHumanReadable() ) {
    result += addSeparators(NumberOfStandardColumnSpaces,timestampHumanReadable);
  }

  if ( getLogMachineName() ) {
    result += addSeparators(NumberOfStandardColumnSpaces,machineName);
  }

  if ( getLogThreadName() ) {
    result += addSeparators(NumberOfStandardColumnSpaces,threadName);
  }

  if ( getLogMessageType() ) {
    result += addSeparators(NumberOfStandardColumnSpaces,messageType);
  }

  if ( getLogTrace() ) {
    result += addSeparators(NumberOfTraceColumnSpaces,trace);
  }

  result += addSeparators(NumberOfStandardColumnSpaces,prefix + message);
*/

  result += "{";
  result += "\"cat\": \"trace\",";
  result += "\"name\": " + trace + ",";
  result += "\"name\": " + trace + ",";
  result += "};";
  result += "\n";

  return result;
}


void tarch::logging::ChromeTraceFileLogger::debug(double timestamp, const std::string& timestampHumanReadable, const std::string& machineName, const std::string& threadName, const std::string& trace, const std::string& message) {
  if (writeDebug(trace)) {
    #if !defined(PeanoDebug) || PeanoDebug<1
    assertion(false);
    #endif

    std::string outputMessage = constructMessageString(
      "debug",
      timestamp,
      timestampHumanReadable,
      machineName,
	  threadName,
      trace,
      message
    );

    tarch::multicore::Lock lockCout( _semaphore );
    out() << outputMessage;
    out().flush();
  }
}


void tarch::logging::ChromeTraceFileLogger::info(double timestamp, const std::string& timestampHumanReadable, const std::string& machineName, const std::string& threadName, const std::string& trace, const std::string& message) {
  if (writeInfo(trace)) {
    std::string outputMessage = constructMessageString(
      "info",
      timestamp,
      timestampHumanReadable,
      machineName,
	  threadName,
      trace,
      message
    );

    tarch::multicore::Lock lockCout( _semaphore );
    out() << outputMessage;
    if (_outputStream!=nullptr) {
      std::cout << outputMessage;
    }
  }
}


void tarch::logging::ChromeTraceFileLogger::warning(double timestamp, const std::string& timestampHumanReadable, const std::string& machineName, const std::string& threadName, const std::string& trace, const std::string& message) {
  if (writeWarning(trace)) {
    std::string outputMessage = constructMessageString(
      "warning",
      timestamp,
      timestampHumanReadable,
      machineName,
	  threadName,
      trace,
      message
    );

    tarch::multicore::Lock lockCout( _semaphore );
    out().flush();
    std::cerr << outputMessage;
    std::cerr.flush();
  }
}


void tarch::logging::ChromeTraceFileLogger::traceIn(double timestamp, const std::string& timestampHumanReadable, const std::string& machineName, const std::string& threadName, const std::string& trace, const std::string& message) {
  if (writeTrace(trace)) {

  }
}


void tarch::logging::ChromeTraceFileLogger::traceOut(double timestamp, const std::string& timestampHumanReadable, const std::string& machineName, const std::string& threadName, const std::string& trace, const std::string& message) {
  if (writeTrace(trace)) {

  }
}


void tarch::logging::ChromeTraceFileLogger::error(double timestamp, const std::string& timestampHumanReadable, const std::string& machineName, const std::string& threadName, const std::string& trace, const std::string& message) {
  if ( writeError(trace) ) {
    std::string outputMessage = constructMessageString(
      "error",
      timestamp,
      timestampHumanReadable,
      machineName,
	  threadName,
      trace,
      message
    );

    tarch::multicore::Lock lockCout( _semaphore );
    out().flush();
    std::cerr << outputMessage;
    std::cerr.flush();
  }

  if (_quitOnError) {
    exit(-1);
  }
}




void tarch::logging::ChromeTraceFileLogger::indent( bool indent, const std::string& trace, const std::string& message ) {
}


void tarch::logging::ChromeTraceFileLogger::setOutputFile( const std::string&  outputLogFileName ) {
  #ifdef Parallel
  std::ostringstream myOutputFileName;
  if (!outputLogFileName.empty()) {
    myOutputFileName << "rank-" << tarch::mpi::Rank::getInstance().getRank() << "-" << outputLogFileName;
  }
  #else
  myOutputFileName << outputLogFileName;
  #endif

  if (_outputStream!=nullptr) {
    _outputStream->flush();
    delete _outputStream;
    _outputStream            = nullptr;
  }

  _outputStream = new std::ofstream( myOutputFileName.str().c_str() );

  *_outputStream << "[";
}


void tarch::logging::ChromeTraceFileLogger::setQuitOnError(bool value) {
  _quitOnError = value;
}

