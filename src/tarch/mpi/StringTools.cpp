#include "StringTools.h"

#include "mpi.h"
#include "StringMessage.h"

#include <sstream>


std::string tarch::mpi::StringTools::convert(const tarch::mpi::StringMessage& value ) {
  std::ostringstream result;
  for (int i=0; i<value._length; i++) {
    result << static_cast<char>(value._data[i]);
  }
  return result.str();
}


tarch::mpi::StringMessage tarch::mpi::StringTools::convert(const std::string& value ) {
  tarch::mpi::StringMessage result;

  result._length = value.length();
  for (int i=0; i<value.length(); i++) {
    result.setData(i, static_cast<short int>(value.data()[i]));
  }

  return result;
}
