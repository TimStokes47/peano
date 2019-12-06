// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _TARCH_MPI_STRING_TOOLS_H_
#define _TARCH_MPI_STRING_TOOLS_H_


#include "mpi.h"


namespace tarch {
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
