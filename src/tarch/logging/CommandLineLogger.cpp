#include "tarch/logging/CommandLineLogger.h"

#include "tarch/Assertions.h"

#include "tarch/multicore/Lock.h"
#include "tarch/multicore/multicore.h"

#include "LogFilter.h"

#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <chrono>

#include "../mpi/Rank.h"



tarch::logging::Log tarch::logging::CommandLineLogger::_log( "tarch::logging::CommandLineLogger" );


std::string::size_type tarch::logging::CommandLineLogger::_indent = 0;


const std::string::size_type tarch::logging::CommandLineLogger::NumberOfIndentSpaces             = 2;
const std::string::size_type tarch::logging::CommandLineLogger::NumberOfTraceColumnSpaces        = 55;
const std::string::size_type tarch::logging::CommandLineLogger::NumberOfStandardColumnSpaces     = 12;
const int                    tarch::logging::CommandLineLogger::DigitsInFilenamesIterationNumer  = 5;


tarch::logging::CommandLineLogger::CommandLineLogger():
  _outputStream(nullptr),
  _hasWrittenToOuputStream(false),
  _iterationCounter(0),
  _quitOnError(false) {
  configureOutputStreams();
  setLogColumnSeparator();
  setLogTimeStamp();
  setLogTimeStampHumanReadable();
  setLogThreadName();
  setLogMachineName();
  setLogMessageType();
  setLogTrace();
}


tarch::logging::CommandLineLogger& tarch::logging::CommandLineLogger::getInstance() {
  static CommandLineLogger singleton;
  return singleton;
}


std::ostream& tarch::logging::CommandLineLogger::out() {
  if (_outputStream==nullptr) {
    return std::cout;
  }
  else {
    _hasWrittenToOuputStream = true;
    return *_outputStream;
  }
}


void tarch::logging::CommandLineLogger::configureOutputStreams() {
  out().setf( std::ios_base::scientific, std::ios_base::floatfield );
  out().precision(20);
  std::cerr.setf( std::ios_base::scientific, std::ios_base::floatfield );
  std::cerr.precision(20);
}


tarch::logging::CommandLineLogger& tarch::logging::CommandLineLogger::operator=(const CommandLineLogger& rhs) {
  return *this;
}


tarch::logging::CommandLineLogger::CommandLineLogger(const CommandLineLogger& param) {
}


tarch::logging::CommandLineLogger::~CommandLineLogger() {
  close();
}


std::string tarch::logging::CommandLineLogger::addSeparators(std::string::size_type spaces, std::string message) const {
  if ( message.size() > 0 ) {
    while (message.size() < spaces) {
      message += " ";
    }
    message = getLogColumnSeparator() + message;
  }

  return message;
}


void tarch::logging::CommandLineLogger::closeOutputStreamAndReopenNewOne() {
  _iterationCounter ++;
  reopenOutputStream();
}


std::string tarch::logging::CommandLineLogger::getTimeStampHumanReadable( long int timestampMS ) {
  long int hours = timestampMS / 3600000;
  timestampMS = timestampMS - 3600000 * hours;

  //60000 milliseconds in a minute
  long int minutes = timestampMS / 60000;
  timestampMS = timestampMS - 60000 * minutes;

  //1000 milliseconds in a second
  long int secones = timestampMS / 1000;

  std::stringstream result;
  result << hours << ":" << minutes << ":" << secones;
  return result.str();
}


std::string tarch::logging::CommandLineLogger::constructMessageString(
  std::string          messageType,
  long int timestampMS, int rank, int threadId, const std::string& trace, const std::string& message
) {
  std::string prefix = "";
  for (unsigned int i=0; i<_indent; i++ ) prefix += " ";

  std::string result;

  if ( getLogTimeStamp() ) {
    std::ostringstream timeStampString;
    timeStampString << timestampMS;
    result += addSeparators(NumberOfStandardColumnSpaces,timeStampString.str() );
  }

  if ( getLogTimeStampHumanReadable() ) {
    result += addSeparators(NumberOfStandardColumnSpaces,getTimeStampHumanReadable(timestampMS));
  }

  if ( getLogMachineName() ) {
    result += addSeparators(NumberOfStandardColumnSpaces,"rank:" + std::to_string(rank));
  }

  if ( getLogThreadName() ) {
    result += addSeparators(NumberOfStandardColumnSpaces,"core:" + std::to_string(threadId));
  }

  if ( getLogMessageType() ) {
    result += addSeparators(NumberOfStandardColumnSpaces,messageType);
  }

  if ( getLogTrace() ) {
    result += addSeparators(NumberOfTraceColumnSpaces,trace);
  }

  result += addSeparators(NumberOfStandardColumnSpaces,prefix + message);

  result += "\n";

  return result;
}


void tarch::logging::CommandLineLogger::debug(long int timestampMS, int rank, int threadId, const std::string& trace, const std::string& message) {
  #if !defined(PeanoDebug) || PeanoDebug<1
  assertion(false);
  #endif

  std::string outputMessage = constructMessageString(
    LogFilter::FilterListEntry::TargetDebug,
    timestampMS, rank, threadId, trace, message
  );

  tarch::multicore::Lock lockCout( _semaphore );
  out() << outputMessage;
  out().flush();
}


void tarch::logging::CommandLineLogger::info(long int timestampMS, int rank, int threadId, const std::string& trace, const std::string& message) {
  std::string outputMessage = constructMessageString(
    LogFilter::FilterListEntry::TargetInfo,
    timestampMS, rank, threadId, trace, message
  );

  tarch::multicore::Lock lockCout( _semaphore );
  out() << outputMessage;
  if (_outputStream!=nullptr) {
    std::cout << outputMessage;
  }
}


void tarch::logging::CommandLineLogger::warning(long int timestampMS, int rank, int threadId, const std::string& trace, const std::string& message) {
    std::string outputMessage = constructMessageString(
      "warning",
	  timestampMS, rank, threadId, trace, message
    );

    tarch::multicore::Lock lockCout( _semaphore );
    out() << outputMessage;
    out().flush();
    std::cerr << outputMessage;
    std::cerr.flush();
}


void tarch::logging::CommandLineLogger::error(long int timestampMS, int rank, int threadId, const std::string& trace, const std::string& message) {
    std::string outputMessage = constructMessageString(
      "error",
	  timestampMS, rank, threadId, trace, message
    );

    tarch::multicore::Lock lockCout( _semaphore );
    out() << outputMessage;
    out().flush();
    std::cerr << outputMessage;
    std::cerr.flush();

  if (_quitOnError) {
    exit(-1);
  }
}


void tarch::logging::CommandLineLogger::traceIn(long int timestampMS, int rank, int threadId, const std::string& trace, const std::string& message) {
    std::string outputMessage = constructMessageString(
      "trace",
	  timestampMS, rank, threadId, trace, message
    );

    tarch::multicore::Lock lockCout( _semaphore );
    out() << outputMessage;
    out().flush();
}


void tarch::logging::CommandLineLogger::traceOut(long int timestampMS, int rank, int threadId, const std::string& trace, const std::string& message) {
    std::string outputMessage = constructMessageString(
      "trace",
      timestampMS, rank, threadId, trace, message
    );

    tarch::multicore::Lock lockCout( _semaphore );
    out() << outputMessage;
    out().flush();
}


void tarch::logging::CommandLineLogger::indent( bool indent, const std::string& trace, const std::string& message ) {
  #if PeanoDebug>=1
  tarch::multicore::Lock lockCout( _semaphore );
  if (indent) {
    _indent+=NumberOfIndentSpaces;
     #if !defined(SharedMemoryParallelisation)
    _indentTraces.push(trace);
     #endif
  }
  else {
    #if !defined(SharedMemoryParallelisation)
    assertionEquals2(
      _indentTraces.top(),
      trace,
      message,
      indent
    );
    _indentTraces.pop();
    assertion5(
      _indent >= NumberOfIndentSpaces,
      _indent, NumberOfIndentSpaces,
      "more logTraceOut calls than logTraceIn calls invoked before",
      trace, message
    );
    #endif
    _indent-=NumberOfIndentSpaces;
  }
  #endif
}


void tarch::logging::CommandLineLogger::reopenOutputStream() {
  tarch::multicore::Lock lock( _semaphore );

  if (_outputStream!=nullptr && _hasWrittenToOuputStream) {
    _outputStream->flush();
    delete _outputStream;
    _outputStream            = nullptr;
    _hasWrittenToOuputStream = false;
  }

  if (!_outputFileName.empty() && _outputStream==nullptr) {
    std::ostringstream fileName;
    if (_iterationCounter>0) {
      int leadingZeros = 1;
      for (int i=0; i<DigitsInFilenamesIterationNumer-1; i++) {
        leadingZeros*=10;
      }
      fileName << "grid-traversal-" ;
      while (_iterationCounter < leadingZeros) {
        fileName << "0";
        leadingZeros /= 10;
      }
      fileName << _iterationCounter << "-";
    }
    fileName << _outputFileName;
    _outputStream = new std::ofstream( fileName.str().c_str() );
  }
}


void tarch::logging::CommandLineLogger::setOutputFile( const std::string&  outputLogFileName ) {
  #ifdef Parallel
  if (!outputLogFileName.empty()) {
    std::ostringstream myOutputFileName;
    myOutputFileName << "rank-" << tarch::mpi::Rank::getInstance().getRank() << "-" << outputLogFileName;
    _outputFileName = myOutputFileName.str();
  }
  else {
    _outputFileName = outputLogFileName;
  }
  #else
  _outputFileName = outputLogFileName;
  #endif

  reopenOutputStream();
}


void tarch::logging::CommandLineLogger::setLogFormat(
  const std::string& columnSeparator,
  bool logTimeStamp,
  bool logTimeStampHumanReadable,
  bool logMachineName,
  bool logThreadName,
  bool logMessageType,
  bool logTrace,
  const std::string&  outputLogFileName
) {
  _logColumnSeparator        = columnSeparator;
  _logTimeStamp              = logTimeStamp;
  _logTimeStampHumanReadable = logTimeStampHumanReadable;
  _logMachineName            = logMachineName;
  _logThreadName             = logThreadName;
  _logMessageType            = logMessageType;
  _logTrace                  = logTrace;

  setOutputFile( outputLogFileName );
}


std::string tarch::logging::CommandLineLogger::getLogColumnSeparator() const {
  return _logColumnSeparator;
}


bool tarch::logging::CommandLineLogger::getLogTimeStamp() const {
  return _logTimeStamp;
}


bool tarch::logging::CommandLineLogger::getLogTimeStampHumanReadable() const {
  return _logTimeStampHumanReadable;
}


bool tarch::logging::CommandLineLogger::getLogMachineName() const {
  return _logMachineName;
}


bool tarch::logging::CommandLineLogger::getLogThreadName() const {
  return _logThreadName;
}


bool tarch::logging::CommandLineLogger::getLogMessageType() const {
  return _logMessageType;
}


bool tarch::logging::CommandLineLogger::getLogTrace() const {
  return _logTrace;
}


void tarch::logging::CommandLineLogger::setLogColumnSeparator( const std::string& separator ) {
  _logColumnSeparator = separator;
}


void tarch::logging::CommandLineLogger::setLogTimeStamp( bool value ) {
  _logTimeStamp = value;
}


void tarch::logging::CommandLineLogger::setLogTimeStampHumanReadable( bool value ) {
  _logTimeStampHumanReadable = value;
}


void tarch::logging::CommandLineLogger::setLogMachineName( bool value ) {
  _logMachineName = value;
}


void tarch::logging::CommandLineLogger::setLogThreadName( bool value ) {
  _logThreadName = value;
}


void tarch::logging::CommandLineLogger::setLogMessageType( bool value ) {
  _logMessageType  = value;
}


void tarch::logging::CommandLineLogger::setLogTrace( bool value ) {
  _logTrace = value;
}


void tarch::logging::CommandLineLogger::setQuitOnError(bool value) {
  _quitOnError = value;
}


void tarch::logging::CommandLineLogger::close() {
  std::cout.flush();
  std::cerr.flush();
  if (_outputStream!=nullptr) {
    delete _outputStream;
    _outputStream = nullptr;
  }
}
