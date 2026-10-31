#include "PeanoTextPatchFileReader.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>


namespace {
  [[noreturn]] void fail(const std::string& message) {
    std::cerr << "tarch::reader::PeanoTextPatchFileReader: " << message << std::endl;
    std::abort();
  }

  std::string trim(const std::string& line) {
    const auto first = line.find_first_not_of(" \t\r");
    if (first == std::string::npos) {
      return "";
    }
    const auto last = line.find_last_not_of(" \t\r");
    return line.substr(first, last - first + 1);
  }

  bool startsWith(const std::string& line, const std::string& prefix) {
    return line.rfind(prefix, 0) == 0;
  }

  /**
   * The numbers behind the keyword of a line such as "offset 0.1 0.2"
   */
  std::vector<double> readNumbers(const std::string& line) {
    std::istringstream  in(line);
    std::string         keyword;
    std::vector<double> result;
    in >> keyword;
    std::string token;
    while (in >> token) {
      result.push_back(std::strtod(token.c_str(), nullptr));
    }
    return result;
  }
} // namespace


tarch::reader::PeanoTextPatchFileReader::Header tarch::reader::PeanoTextPatchFileReader::readHeader(
  const std::string& fileName
) {
  std::ifstream in(fileName, std::ios::binary);
  if (not in.good()) {
    fail("cannot open file " + fileName);
  }

  Header      result;
  std::string line;
  while (std::getline(in, line)) {
    line = trim(line);
    if (startsWith(line, "format")) {
      std::istringstream tokens(line);
      std::string        keyword, value;
      tokens >> keyword >> value;
      if (value == "Binary") {
        result.format = Format::Binary;
      } else if (value == "ASCII") {
        result.format = Format::ASCII;
      } else {
        fail("file " + fileName + " has unknown format " + value);
      }
    } else if (startsWith(line, "dimensions")) {
      result.dimensions = static_cast<int>(readNumbers(line).at(0));
      return result;
    } else if (startsWith(line, "begin")) {
      break;
    }
  }
  return result;
}


tarch::reader::PeanoTextPatchFileReader::Format tarch::reader::PeanoTextPatchFileReader::getFormat() const {
  return Format::ASCII;
}


void tarch::reader::PeanoTextPatchFileReader::readValues(
  std::istream&        in,
  [[maybe_unused]] int numberOfValues,
  std::vector<double>& values
) const {
  std::string line;
  std::getline(in, line);
  values = readNumbers("values " + line);
}


tarch::reader::PeanoTextPatchFileReader::Content tarch::reader::PeanoTextPatchFileReader::read(
  const std::string& fileName
) const {
  Content result;
  result.header = readHeader(fileName);
  if (result.header.format != getFormat()) {
    fail("file " + fileName + " has a format which this reader does not support");
  }
  if (result.header.dimensions < 2 or result.header.dimensions > 3) {
    fail("file " + fileName + " does not hold a valid dimensions entry");
  }

  std::ifstream in(fileName, std::ios::binary);

  int                 entriesPerAxis = 0;
  int                 unknowns       = 0;
  std::vector<double> offset;
  std::vector<double> size;
  std::string         line;
  while (std::getline(in, line)) {
    line = trim(line);
    if (startsWith(line, "number-of-dofs-per-axis")) {
      entriesPerAxis = static_cast<int>(readNumbers(line).at(0));
    } else if (startsWith(line, "number-of-unknowns")) {
      unknowns = static_cast<int>(readNumbers(line).at(0));
    } else if (line == "begin meta-data") {
      std::string metaData;
      while (std::getline(in, line) and trim(line) != "end meta-data") {
        metaData += (metaData.empty() ? "" : "\n") + trim(line);
      }
      if (result.metaData.empty()) {
        result.metaData = metaData;
      }
    } else if (startsWith(line, "offset")) {
      offset = readNumbers(line);
    } else if (startsWith(line, "size")) {
      size = readNumbers(line);
    } else if (startsWith(line, "begin cell-values")) {
      int numberOfValues = unknowns;
      for (int d = 0; d < result.header.dimensions; d++) {
        numberOfValues *= entriesPerAxis;
      }

      Patch patch;
      patch.offset.assign(offset.begin(), offset.begin() + result.header.dimensions);
      patch.size.assign(size.begin(), size.begin() + result.header.dimensions);
      patch.numberOfDoFsPerAxis = entriesPerAxis;
      patch.numberOfUnknowns    = unknowns;
      readValues(in, numberOfValues, patch.values);
      result.patches.push_back(std::move(patch));
    }
  }
  return result;
}
