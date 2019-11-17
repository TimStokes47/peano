#include "tarch/logging/ChromeTraceFileLogger.h"

#include "tarch/Assertions.h"

#include "tarch/multicore/Lock.h"
#include "tarch/multicore/MulticoreDefinitions.h"

#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <algorithm>


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


void tarch::logging::ChromeTraceFileLogger::configureOutputStreams() {
  if (_outputStream!=nullptr) {
    _outputStream->setf( std::ios_base::scientific, std::ios_base::floatfield );
    _outputStream->precision(20);
  }
  std::cerr.setf( std::ios_base::scientific, std::ios_base::floatfield );
  std::cerr.precision(20);
  std::cout.setf( std::ios_base::scientific, std::ios_base::floatfield );
  std::cout.precision(20);
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




std::string tarch::logging::ChromeTraceFileLogger::addSeparators(std::string message) const {
  const int ColumnWidth = 8;
  if ( message.size() > 0 ) {
    while (message.size() < ColumnWidth) {
      message += " ";
    }
  }
  message += " ";

  return message;
}


std::string tarch::logging::ChromeTraceFileLogger::constructMessageString(
  std::string          messageType,
  int timestampMS, int rank, int threadId, const std::string& trace, const std::string& message
) {
  std::string result;

  result += addSeparators(std::to_string(timestampMS));
  result += addSeparators("rank:" + std::to_string(rank) );
  result += addSeparators("core:" + std::to_string(threadId) );
  result += addSeparators(messageType);
  result += message;
  result += "\n";

  return result;
}




std::string tarch::logging::ChromeTraceFileLogger::constructEventEntryInTraceFile(
  std::string          messageType,
  int timestampMS, int rank, int threadId, const std::string& trace, const std::string& message
) {
  std::ostringstream result;
  std::string args = message;
  std::replace( args.begin(), args.end(), '"', ' '); // replace all 'x' to 'y'
  result << "  {"
		 << " \"name\": \"" << trace << "\","
		 << " \"cat\": \"" << messageType << "\","
		 << " \"ph\": \"i\","
		 << " \"ts\": " << timestampMS << ","
		 << " \"pid\": " << rank << ","
		 << " \"tid\": " << threadId << ","
		 << " \"args\": { \"message\": \"" + args + "\"}"
		 " },\n";
  return result.str();
}


void tarch::logging::ChromeTraceFileLogger::debug(   int timestampMS, int rank, int threadId, const std::string& trace, const std::string& message) {
  if (writeDebug(trace)) {
    std::string eventEntry = constructEventEntryInTraceFile(
      "debug",
      timestampMS, rank, threadId, trace, message
    );
    tarch::multicore::Lock lockCout( _semaphore );
    if (_outputStream!=nullptr) {
      *_outputStream << eventEntry;
    }
  }
}


void tarch::logging::ChromeTraceFileLogger::info(   int timestampMS, int rank, int threadId, const std::string& trace, const std::string& message) {
  if (writeInfo(trace)) {
    std::string outputMessage = constructMessageString(
      "info",
	  timestampMS, rank, threadId, trace, message
    );
    std::string eventEntry = constructEventEntryInTraceFile(
      "info",
	  timestampMS, rank, threadId, trace, message
    );
    tarch::multicore::Lock lockCout( _semaphore );
    std::cout << outputMessage;
    if (_outputStream!=nullptr) {
      *_outputStream << eventEntry;
    }
  }
}


void tarch::logging::ChromeTraceFileLogger::warning(   int timestampMS, int rank, int threadId, const std::string& trace, const std::string& message) {
  if (writeWarning(trace)) {
    std::string outputMessage = constructMessageString(
      "warning",
	  timestampMS, rank, threadId, trace, message
    );
    std::string eventEntry = constructEventEntryInTraceFile(
      "warning",
	  timestampMS, rank, threadId, trace, message
    );
    tarch::multicore::Lock lockCout( _semaphore );
    std::cout << outputMessage;
    if (_outputStream!=nullptr) {
      *_outputStream << eventEntry;
    }
  }
}


void tarch::logging::ChromeTraceFileLogger::traceIn(   int timestampMS, int rank, int threadId, const std::string& trace, const std::string& message) {
  if (writeTrace(trace) and _outputStream!=nullptr) {
    std::string args = message;
	std::replace( args.begin(), args.end(), '"', ' '); // replace all 'x' to 'y'
    tarch::multicore::Lock lockCout( _semaphore );
	*_outputStream << "  {"
			 << " \"name\": \"" << trace << "\","
			 << " \"cat\": \"trace\","
			 << " \"ph\": \"b\","
			 << " \"ts\": " << timestampMS << ","
			 << " \"pid\": " << rank << ","
			 << " \"tid\": " << threadId << ","
			 << " \"args\": { \"params\": \"" + args + "\"}"
			 " },\n";
  }
}


void tarch::logging::ChromeTraceFileLogger::traceOut(   int timestampMS, int rank, int threadId, const std::string& trace, const std::string& message) {
  if (writeTrace(trace) and _outputStream!=nullptr) {
    std::string args = message;
	std::replace( args.begin(), args.end(), '"', ' '); // replace all 'x' to 'y'
    tarch::multicore::Lock lockCout( _semaphore );
	*_outputStream << "  {"
			 << " \"name\": \"" << trace << "\","
			 << " \"cat\": \"trace\","
			 << " \"ph\": \"e\","
			 << " \"ts\": " << timestampMS << ","
			 << " \"pid\": " << rank << ","
			 << " \"tid\": " << threadId << ","
			 << " \"args\": { \"result\": \"" + args + "\"}"
			 " },\n";
  }
}


void tarch::logging::ChromeTraceFileLogger::error(   int timestampMS, int rank, int threadId, const std::string& trace, const std::string& message) {
  if ( writeError(trace) ) {
    std::string outputMessage = constructMessageString(
      "error",
	  timestampMS, rank, threadId, trace, message
    );
    std::string eventEntry = constructEventEntryInTraceFile(
      "error",
	  timestampMS, rank, threadId, trace, message
    );
    tarch::multicore::Lock lockCout( _semaphore );
    std::cout << outputMessage;
    if (_outputStream!=nullptr) {
      *_outputStream << eventEntry;
    }
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

  configureOutputStreams();

  *_outputStream << "[\n";
}


void tarch::logging::ChromeTraceFileLogger::setQuitOnError(bool value) {
  _quitOnError = value;
}

