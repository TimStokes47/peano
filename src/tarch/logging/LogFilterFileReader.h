// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _TARCH_LOGGING_LOG_FILTER_FILE_READER_H_
#define _TARCH_LOGGING_LOG_FILTER_FILE_READER_H_


#include <string>


#include "tarch/logging/Log.h"


namespace tarch {
  namespace logging {
    class LogFilterFileReader;
  }
}


/**
 * Very simple utility class offering a couple of static operations to parse
 * different filter files for the command line logger.
 *
 * @author Tobias Weinzierl
 */
class tarch::logging::LogFilterFileReader {
  private:
    static tarch::logging::Log  _log;

    static bool interpretTokens( const std::string& levelToken, const std::string& classNameToken, const std::string& rankToken, const std::string& onOffToken );
  public:
    /**
     * Tries to parse a simple text file where each line contains an entry
\code
info   my::full::qualified::class::name    -1     black
debug  my::full::qualified::class::name    43     white
\endcode
     *
     * If the file does not exist or the parse did not succeed, the operation
     * writes an error and returns false. Otherwise, it returns true.
     */
    static bool parsePlainTextFile( const std::string& filename );
};

#endif
