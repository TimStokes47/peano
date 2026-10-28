#include "CheckpointReader.h"

#include "tarch/multicore/Lock.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <limits>
#include <set>
#include <sstream>


tarch::logging::Log exahype2::CheckpointReader::_log(
  "exahype2::CheckpointReader"
);

tarch::multicore::BooleanSemaphore exahype2::CheckpointReader::_semaphore;
std::map<std::string, std::unique_ptr<exahype2::CheckpointReader>>
  exahype2::CheckpointReader::_instances;


namespace {
  /**
   * Line without leading and trailing white spaces
   */
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


exahype2::CheckpointReader& exahype2::CheckpointReader::getInstance(
  const std::string&                           indexFileName,
  double                                       timeStamp,
  int                                          haloWidth,
  const tarch::la::Vector<Dimensions, double>& domainOffset,
  const tarch::la::Vector<Dimensions, double>& domainSize
) {
  tarch::multicore::Lock lock(_semaphore);
  if (_instances.count(indexFileName) == 0) {
    _instances[indexFileName] = std::make_unique<CheckpointReader>(
      indexFileName,
      timeStamp,
      haloWidth,
      domainOffset,
      domainSize
    );
  }
  return *_instances[indexFileName];
}


void exahype2::CheckpointReader::release(const std::string& indexFileName) {
  tarch::multicore::Lock lock(_semaphore);
  _instances.erase(indexFileName);
}


exahype2::CheckpointReader::CheckpointReader(
  const std::string&                           indexFileName,
  double                                       timeStamp,
  int                                          haloWidth,
  const tarch::la::Vector<Dimensions, double>& domainOffset,
  const tarch::la::Vector<Dimensions, double>& domainSize
):
  _haloWidth(haloWidth),
  _domainOffset(domainOffset),
  _domainSize(domainSize),
  _timeStamp(0.0) {
  for (const auto& fileName : readIndexFile(indexFileName, timeStamp)) {
    readPatchFile(fileName);
  }
  createRefinementEvents();
  logInfo(
    "CheckpointReader(...)",
    "read "
      << _patchData.size() << " patches of checkpoint " << indexFileName
      << " at time stamp " << _timeStamp << ", which yield "
      << _refinementEvents.size() << " refinement events"
  );
}


std::vector<std::string> exahype2::CheckpointReader::readIndexFile(
  const std::string& indexFileName,
  double             timeStamp
) {
  std::ifstream in(indexFileName);
  if (not in.good()) {
    logError("readIndexFile(...)", "cannot open checkpoint " << indexFileName);
    std::abort();
  }

  const std::filesystem::path directory = std::filesystem::path(indexFileName)
                                            .parent_path();

  // Time stamp and files of all datasets which include at least one file
  std::vector<std::pair<double, std::vector<std::string>>> datasets;
  double                   currentTimeStamp = 0.0;
  std::vector<std::string> currentFiles;
  std::string              line;
  while (std::getline(in, line)) {
    line = trim(line);
    if (line == "begin dataset") {
      currentTimeStamp = 0.0;
      currentFiles.clear();
    } else if (startsWith(line, "timestamp")) {
      currentTimeStamp = readNumbers(line).at(0);
    } else if (startsWith(line, "include")) {
      const auto first = line.find('"');
      const auto last  = line.rfind('"');
      currentFiles.push_back(
        (directory / line.substr(first + 1, last - first - 1)).string()
      );
    } else if (line == "end dataset" and not currentFiles.empty()) {
      datasets.push_back({currentTimeStamp, currentFiles});
    }
  }

  if (datasets.empty()) {
    logError(
      "readIndexFile(...)",
      "checkpoint " << indexFileName << " holds no dataset"
    );
    std::abort();
  }

  int selected = datasets.size() - 1;
  if (timeStamp >= 0.0) {
    for (int i = 0; i < static_cast<int>(datasets.size()); i++) {
      if (std::abs(datasets[i].first - timeStamp) < std::abs(datasets[selected].first - timeStamp)) {
        selected = i;
      }
    }
  }

  _timeStamp = datasets[selected].first;
  return datasets[selected].second;
}


void exahype2::CheckpointReader::readPatchFile(const std::string& fileName) {
  std::ifstream in(fileName);
  if (not in.good()) {
    logError("readPatchFile(...)", "cannot open checkpoint file " << fileName);
    std::abort();
  }

  int                 entriesPerAxis = 0;
  std::vector<double> offset;
  std::vector<double> size;
  std::string         line;
  while (std::getline(in, line)) {
    line = trim(line);
    if (startsWith(line, "dimensions")) {
      if (static_cast<int>(readNumbers(line).at(0)) != Dimensions) {
        logError(
          "readPatchFile(...)",
          "checkpoint file "
            << fileName << " has not been written by a " << Dimensions
            << "d code"
        );
        std::abort();
      }
    } else if (startsWith(line, "number-of-dofs-per-axis")) {
      entriesPerAxis = static_cast<int>(readNumbers(line).at(0));
    } else if (line == "begin meta-data") {
      std::string metaData;
      while (std::getline(in, line) and trim(line) != "end meta-data") {
        metaData += (metaData.empty() ? "" : "\n") + trim(line);
      }
      if (_metaData.empty()) {
        _metaData = metaData;
      }
    } else if (startsWith(line, "offset")) {
      offset = readNumbers(line);
    } else if (startsWith(line, "size")) {
      size = readNumbers(line);
    } else if (startsWith(line, "begin cell-values")) {
      std::getline(in, line);
      std::vector<double> values = readNumbers("values " + line);

      // The patch covers the cell plus the halo. The cell spans the
      // entriesPerAxis-2*haloWidth entries in the middle.
      tarch::la::Vector<Dimensions, double> cellOffset;
      tarch::la::Vector<Dimensions, double> cellH;
      for (int d = 0; d < Dimensions; d++) {
        const double entryWidth = size.at(d) / entriesPerAxis;
        cellOffset(d)           = offset.at(d) + _haloWidth * entryWidth;
        cellH(d) = (entriesPerAxis - 2 * _haloWidth) * entryWidth;
      }

      const Key key = toKey(cellOffset, cellH);
      if (_patchIndex.count(key) == 0) {
        _patchIndex[key] = _patchData.size();
        _patchData.push_back(std::move(values));
      }
    }
  }
}


exahype2::CheckpointReader::Key exahype2::CheckpointReader::toKey(
  const tarch::la::Vector<Dimensions, double>& cellOffset,
  const tarch::la::Vector<Dimensions, double>& cellH
) const {
  Key result;
  result[Dimensions] = std::llround(
    std::log(_domainSize(0) / cellH(0)) / std::log(3.0)
  );
  for (int d = 0; d < Dimensions; d++) {
    result[d] = std::llround((cellOffset(d) - _domainOffset(d)) / cellH(d));
  }
  return result;
}


void exahype2::CheckpointReader::createRefinementEvents() {
  std::set<Key> parents;
  for (const auto& [key, index] : _patchIndex) {
    const long long level = key[Dimensions];
    if (level < 2) {
      continue;
    }
    Key parent;
    parent[Dimensions] = level - 1;
    for (int d = 0; d < Dimensions; d++) {
      parent[d] = key[d] / 3;
    }
    parents.insert(parent);
  }

  for (const auto& parent : parents) {
    tarch::la::Vector<Dimensions, double> parentH;
    tarch::la::Vector<Dimensions, double> middleThirdOffset;
    for (int d = 0; d < Dimensions; d++) {
      parentH(d
      ) = _domainSize(d)
          * std::pow(3.0, -static_cast<double>(parent[Dimensions]));
      middleThirdOffset(d
      ) = _domainOffset(d) + parent[d] * parentH(d) + parentH(d) / 3.0;
    }
    _refinementEvents.push_back(peano4::grid::GridControlEvent(
      peano4::grid::GridControlEvent::RefinementControl::Refine,
      middleThirdOffset,
      parentH / 3.0,
      parentH
    ));
  }
}


const std::vector<peano4::grid::GridControlEvent>& exahype2::CheckpointReader::
  getRefinementEvents() const {
  return _refinementEvents;
}


bool exahype2::CheckpointReader::readPatch(
  const tarch::la::Vector<Dimensions, double>& cellCentre,
  const tarch::la::Vector<Dimensions, double>& cellH,
  int                                          numberOfEntries,
  double*                                      patchWithHalo
) const {
  const auto entry = _patchIndex.find(toKey(cellCentre - 0.5 * cellH, cellH));
  if (entry == _patchIndex.end()) {
    return false;
  }
  const std::vector<double>& values = _patchData[entry->second];
  if (static_cast<int>(values.size()) != numberOfEntries) {
    logError(
      "readPatch(...)",
      "patch of cell at "
        << cellCentre << " holds " << values.size()
        << " values in the checkpoint, but the solver expects "
        << numberOfEntries
        << ". Has the checkpoint been written by another solver or patch size?"
    );
    std::abort();
  }
  std::copy(values.begin(), values.end(), patchWithHalo);
  return true;
}


const std::string& exahype2::CheckpointReader::getMetaData() const {
  return _metaData;
}


double exahype2::CheckpointReader::getTimeStamp() const { return _timeStamp; }


int exahype2::CheckpointReader::getNumberOfPatches() const {
  return _patchData.size();
}
