#include "tarch.h"
#include "tarch/logging/Log.h"
#include "tarch/compiler/CompilerSpecificSettings.h"

#include <iostream>

#if defined(CompilerHasProcStat)
#include <unistd.h>
#endif

#include <cstdlib>
#include <cstring>


int tarch::getMemoryUsage(MemoryUsageFormat format) {
  static tarch::logging::Log _log( "tarch" );

  #if defined(CompilerHasProcStat)
  char   work[4096];
  FILE*  f;
  char*  pCh;

  pid_t pid = getpid();

  sprintf(work, "/proc/%d/stat", (int)pid);
  f = fopen(work, "r");

  if (f == NULL) {
    logError("getMemoryUsage()", "can't open file " << work );
    return(0);
  }
  if(fgets(work, sizeof(work), f) == NULL) {
    logError("getMemoryUsage","Error while reading from file");
  }
  fclose(f);
  strtok(work, " ");

  for (int i = 1; i < 23; i++) {
    pCh = strtok(NULL, " ");
  }

  std::size_t rawInput = atol(pCh);
  #else
  std::size_t rawInput = 0;
  #endif

  int result = 0;
  switch (format) {
    case MemoryUsageFormat::MByte:
      {
        long megaByte = 1024 * 1024;
        long usageMB ((rawInput + (megaByte/2)) / megaByte );
        const long maxInteger = std::numeric_limits<int>::max();
        if (usageMB>maxInteger) {
          logError( "getMemoryUsageKB()", "cannot cast result to return value: " << usageMB );
        }
        result = static_cast<int>(usageMB);
      }
  }
  return result;
}
