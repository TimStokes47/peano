// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _TARCH_MPI_
#define _TARCH_MPI_


#include "config.h"
#include "tarch/compiler/CompilerSpecificSettings.h"

#if !defined(CompilerDefinesMPIMaxNameString)
#define MPI_MAX_NAME_STRING            80
#endif


#ifdef Parallel
#include <mpi.h>
#else
typedef int    MPI_Datatype;
typedef int    MPI_Comm;
typedef int    MPI_Status;
typedef int    MPI_Request;
#endif


#include <string>


namespace tarch {

/**

 @namespace "mpi (Parallel Architecture)"
 
 <h2> Init MPI </h2>


 <h2> Regenerate all data types </h2>
 \code
 java -jar ~/git/DaStGen/DaStGen.jar --plugin PeanoSnippetGenerator --naming Peano4NameTranslator src/tarch/mpi/IntegerMessage.def src/tarch/mpi
 java -jar ~/git/DaStGen/DaStGen.jar --plugin PeanoSnippetGenerator --naming Peano4NameTranslator src/tarch/mpi/StringMessage.def src/tarch/mpi
 \endcode 
 */
  namespace mpi {
    class StringMessage;
    class StringTools;
  }
}


class tarch::mpi::StringTools {
  private:
    StringTools() {}
    ~StringTools() {}
  public:
    static std::string convert(const tarch::mpi::StringMessage& value );
    static tarch::mpi::StringMessage convert(const std::string& value );
};


#endif

