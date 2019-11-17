// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _TARCH_LOGGING_CHROME_TRACE_FILE_LOGGER_H_
#define _TARCH_LOGGING_CHROME_TRACE_FILE_LOGGER_H_


#ifdef Parallel
#include <mpi.h>
#endif
#include <iostream>
#include <fstream>
#include <set>
#include <stack>


#include "tarch/multicore/BooleanSemaphore.h"
#include "tarch/multicore/MulticoreDefinitions.h"
#include "tarch/logging/Log.h"


#include "LoggerWithFilter.h"


namespace tarch {
  namespace logging {
    class ChromeTraceFileLogger;
  }
}

/**
 * Chrome Trace File Logger
 *
 * To visualise the output files, either use Google's Chrome or the trace file
 * viewer from The Chromium Projects. In Ubuntu, simply install the Chromium
 * web browser. Once launched, type in chrome://tracing/ and press enter. After
 * that, you can load the trace file.
 *
 * @author  Tobias Weinzierl
 */
class tarch::logging::ChromeTraceFileLogger: public tarch::logging::LoggerWithFilter {
  private:
    static Log _log;

    tarch::multicore::BooleanSemaphore _semaphore;

    /**
     * Test for the column separator of a string output.
     */
    std::ostream*  _outputStream;
    bool           _quitOnError;


    /**
     * Declared private since assignment does not make sense for an output
     * class (output information mismatch).
     */
    ChromeTraceFileLogger& operator=(const ChromeTraceFileLogger& rhs);

    /**
     * Declared private since copying does not make sense for an output
     * class (output information mismatch).
     */
    ChromeTraceFileLogger(const ChromeTraceFileLogger& param);

    /**
     * Construct message string
     *
     * !!! Thread Safety
     *
     * The message string relies on the global field _indent. This one might
     * change throughout the execution of this method. However, I accept such a
     * behavior: Changing _indent throughout the message execution makes the
     * method add the wrong number of whitespaces in front of the message. That
     * is a 'bug' we can accept.
     */
    std::string constructMessageString(
      std::string          messageType,
      double               timestamp,
      std::string          timestampHumanReadable,
      std::string          machineName,
      std::string          threadName,
      std::string          trace,
      const std::string&   message
    );

    /**
     * Configures the output streams
     */
    void configureOutputStreams();

    /**
     * It's a singleton.
     */
    ChromeTraceFileLogger();

    std::ostream& out();
  public:
    ~ChromeTraceFileLogger();

    static ChromeTraceFileLogger& getInstance();


    void debug(      double timestamp, const std::string& timestampHumanReadable, const std::string& machineName, const std::string& threadName, const std::string& trace, const std::string& message);
    void info(       double timestamp, const std::string& timestampHumanReadable, const std::string& machineName, const std::string& threadName, const std::string& trace, const std::string& message);

    /**
     * Write Warning
     *
     * In the implementation, I call a flush on cout before I write to cerr.
     * Otherwise, the cerr messages might overtake cout. Before the operation
     * returns, it does a flush on cerr, too. Otherwise, the message might not
     * occur, i.e. the application might shut down before the message is flushed
     * to the terminal.
     */
    void warning(    double timestamp, const std::string& timestampHumanReadable, const std::string& machineName, const std::string& threadName, const std::string& trace, const std::string& message);

    /**
     * Write Error
     *
     * In the implementation, I call a flush on cout before I write to cerr.
     * Otherwise, the cerr messages might overtake cout. Before the operation
     * returns, it does a flush on cerr, too. Otherwise, the message might not
     * occur, i.e. the application might shut down before the message is flushed
     * to the terminal.
     */
    void error(      double timestamp, const std::string& timestampHumanReadable, const std::string& machineName, const std::string& threadName, const std::string& trace, const std::string& message);

    void traceIn(      double timestamp, const std::string& timestampHumanReadable, const std::string& machineName, const std::string& threadName, const std::string& trace, const std::string& message);
    void traceOut(      double timestamp, const std::string& timestampHumanReadable, const std::string& machineName, const std::string& threadName, const std::string& trace, const std::string& message);

    /**
     * Tells the logger to increment/decrement the indent.
     *
     * Implemented for compatibility reasons
     */
    void indent( bool indent, const std::string& trace, const std::string& message );

    /**
     * Is redundant, as you could use setLogFormat() instead. However, it is
     * tedious to specify all of these parameters if the only thing you want
     * is to establish an output file.
     */
    void setOutputFile( const std::string&  outputLogFileName );

    void setQuitOnError(bool value);
};

#endif
