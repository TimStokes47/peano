#include "PeanoBinaryPatchFileReader.h"

#include <cstdlib>
#include <iostream>


void tarch::reader::PeanoBinaryPatchFileReader::readValues(
  std::istream&        in,
  int                  numberOfValues,
  std::vector<double>& values
) const {
  values.assign(numberOfValues, 0.0);
  in.read(
    reinterpret_cast<char*>(values.data()),
    static_cast<std::streamsize>(numberOfValues) * sizeof(double)
  );
  if (not in.good()) {
    std::cerr << "PeanoBinaryPatchFileReader: file ended within the quantities of a patch" << std::endl;
    std::abort();
  }

  // Line break behind the raw data
  std::string rest;
  std::getline(in, rest);
}


tarch::reader::PeanoTextPatchFileReader::Format tarch::reader::PeanoBinaryPatchFileReader::getFormat() const {
  return Format::Binary;
}
