#include "PeanoBinaryPatchFileWriter.h"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <limits>

#include "tarch/Assertions.h"
#include "tarch/mpi/Lock.h"
#include "tarch/mpi/Rank.h"

tarch::logging::Log tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::_log("tarch::plotter::"
                                                                                              "griddata::"
                                                                                              "blockstructured::"
                                                                                              "PeanoBinaryPatchFileWriter"
);

const std::string tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::HEADER
  = "# \n"
    "# Peano patch file \n"
    "# Version 0.2 \n"
    "# \n";

namespace {
  const std::string Token_BeginDataSet = "begin dataset";
  const std::string Token_EndDataSet   = "end dataset";
  const std::string Token_TimeStamp    = "timestamp";
} // namespace

void tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::createBackupOfMetaFile() {
  std::ifstream inFile(_indexFile);
  std::ofstream outFile(_indexFile + ".bak");
  for (std::string line; std::getline(inFile, line); /**/) {
    outFile << line << std::endl;
  }
}

double tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::getLatestTimeStepInIndexFile() const {
  double result = -1.0;

  std::ifstream            ifs(_indexFile);
  std::vector<std::string> lines;
  for (std::string line; std::getline(ifs, line); /**/) {
    lines.push_back(line);
  }
  ifs.close();

  for (auto line : lines) {
    if (line.find(Token_TimeStamp) != std::string::npos) {
      std::string timeStampToken = line.substr(line.find(Token_TimeStamp) + Token_TimeStamp.length());
      #if defined(__cpp_exceptions)
      try {
        double timeStamp = std::stod(timeStampToken);
        result           = std::max(result, timeStamp);
      } catch (std::exception&) {
        logError(
          "getLatestTimeStepInIndexFile()", "file " << _indexFile << " contained invalid time stamp " << timeStampToken
        );
        result = std::numeric_limits<double>::max();
      }
      #else
      double timeStamp = std::stod(timeStampToken);
      result           = std::max(result, timeStamp);
      #endif
    }
  }

  return result;
}

void tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::addNewDatasetToIndexFile(double timestamp) {
  std::ofstream indexFileOut;
  indexFileOut.open(_indexFile, std::ios::app);
  indexFileOut << std::endl << Token_BeginDataSet << std::endl;
  indexFileOut << std::setprecision(24) << std::scientific;
  indexFileOut << "  " << Token_TimeStamp << "  " << timestamp << std::endl;
  indexFileOut << std::endl << Token_EndDataSet << std::endl;
  indexFileOut.close();
}


void tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::addNewFileToCurrentDataSetInIndexFile(
  const std::string& filename
) {
  std::ifstream            ifs(_indexFile);
  std::vector<std::string> lines;
  for (std::string line; std::getline(ifs, line); /**/) {
    lines.push_back(line);
  }
  ifs.close();

  assertion2(lines.size() >= 2, filename, lines.size());

  if (lines[lines.size() - 1].find(Token_EndDataSet) == std::string::npos) {
    logWarning(
      "addNewFileToCurrentDataSetInIndexFile(...)",
      "last line in index file "
        << _indexFile << " has not been end of dataset as expected. Expected \"" << Token_EndDataSet << "\" but got \""
        << lines[lines.size() - 1] << "\""
    );
  }


  const std::string newEntry = "  include \"" + filename + "\"";
  lines[lines.size() - 1]    = newEntry;
  lines.push_back(Token_EndDataSet);
  std::ofstream indexFileOut;
  indexFileOut.open(_indexFile.c_str(), std::ios::out);
  for (auto& p : lines) {
    indexFileOut << p << std::endl;
  }
  indexFileOut.close();

  logDebug( "addNewFileToCurrentDataSetInIndexFile(...)", "added new entry " << newEntry << " to latest time stamp section within " << _indexFile << " (#lines:" << lines.size() << ")" );
}


void tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::createEmptyIndexFile() {
  logInfo("createEmptyIndexFile()", "create empty index file " << _indexFile);

  std::ofstream indexFileOut;
  indexFileOut.open(_indexFile.c_str(), std::ios::out);

  if ((!indexFileOut.fail()) && indexFileOut.is_open()) {
    indexFileOut << HEADER << "format ASCII" << std::endl;

    addNewDatasetToIndexFile(0.0);
  }

  if (!indexFileOut.is_open()) {
    logError("PeanoBinaryPatchFileWriter()", "have not been able to open file " << _indexFile);
  } else {
    indexFileOut.close();
  }
}

tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::PeanoBinaryPatchFileWriter(
  int                dimension,
  const std::string& fileName,
  const std::string& indexFileName,
  IndexFileMode      appendToIndexFile,
  double             timeStamp,
  int                precision
):
  _fileName(fileName),
  _indexFile(indexFileName),
  _writtenToFile(false),
  _dimensions(dimension) {

  assertion(dimension >= 2);
  assertion(dimension <= 3);

  if (fileName.rfind(".peano-patch-file") != std::string::npos) {
    logWarning(
      "writeToFile()",
      "filename should not end with .peano-patch-file as routine adds extension automatically. Chosen filename prefix="
        << fileName
    );
  }
  else {
    _fileName += ".peano-patch-file";
  }

  if (indexFileName.rfind(".peano-patch-file") != std::string::npos) {
    logWarning(
      "writeToFile()",
      "index filename should not end with .peano-patch-file as routine adds extension automatically. Chosen filename "
      "prefix="
        << indexFileName
    );
  }
  else {
    _indexFile += ".peano-patch-file";
  }

  clear();

  const double DefaultTimeStampPrecision = 1e-5;

  switch (appendToIndexFile) {
    case IndexFileMode::CreateNew:
      createEmptyIndexFile();
      addNewFileToCurrentDataSetInIndexFile(_fileName);
      break;
    case IndexFileMode::NoIndexFile:
      break;
    case IndexFileMode::AppendNewData:
      {
        // Only ever used to decide whether a *pre-existing* index file is
        // stale (see the "else if" below) - deliberately not reused for the
        // newDatasetBelongsToSnapshotBiggerThanLatestWrite decision further
        // down; see the comment there for why.
        const bool newDatasetBelongsToSnapshotSmallerThanLatestWrite =
          tarch::la::smaller(
            timeStamp,
            getLatestTimeStepInIndexFile(),
            tarch::la::relativeEpsNormalisedAgainstValueGreaterOne(
              timeStamp, getLatestTimeStepInIndexFile(), DefaultTimeStampPrecision
            )
          );

        if (not std::filesystem::exists(_indexFile)) {
          logInfo("PeanoBinaryPatchFileWriter(...)", "no index file " << _indexFile << " found. Create new one");
          createEmptyIndexFile();
        }
        else if (newDatasetBelongsToSnapshotSmallerThanLatestWrite) {
          logWarning(
            "PeanoBinaryPatchFileWriter(...)",
            "there is an index file "
              << _indexFile << " with data for time stamp " << getLatestTimeStepInIndexFile()
              << ". Will be overwritten as we dump data for time " << timeStamp
          );
          createEmptyIndexFile();
        }

        createBackupOfMetaFile();

        // Evaluated only now, after the index file has potentially just been
        // (re)created above: createEmptyIndexFile() itself already adds a
        // first dataset entry for timeStamp 0.0, so querying
        // getLatestTimeStepInIndexFile() here (rather than before the block
        // above ran) correctly picks that entry up. Evaluating this earlier,
        // alongside newDatasetBelongsToSnapshotSmallerThanLatestWrite above,
        // used to compare timeStamp against a stale "no data yet" sentinel
        // whenever the index file did not exist before this call, which made
        // this condition spuriously true on the very first writer of a run
        // and opened a second, empty dataset section as a result.
        const bool newDatasetBelongsToSnapshotBiggerThanLatestWrite =
          tarch::la::greater(
            timeStamp,
            getLatestTimeStepInIndexFile(),
            tarch::la::relativeEpsNormalisedAgainstValueGreaterOne(
              timeStamp, getLatestTimeStepInIndexFile(), DefaultTimeStampPrecision
            )
          );

        if (newDatasetBelongsToSnapshotBiggerThanLatestWrite) {
          logDebug(
            "PeanoBinaryPatchFileWriter(...)",
            "create new dataset for time stamp "
              << timeStamp << " linking to " << _fileName << " as last time stamp had been " << getLatestTimeStepInIndexFile()
          );
          addNewDatasetToIndexFile(timeStamp);
        }
        addNewFileToCurrentDataSetInIndexFile(_fileName);
      }
      break;
  }
  logDebug("PeanoBinaryPatchFileWriter(...)", "index file is ready");

  _snapshotFileOut << std::scientific << std::setprecision(precision);
}

tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::~PeanoBinaryPatchFileWriter() {}

void tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::writeMapping(
  int totalEntries, double* values
) {
  if (values != nullptr) {
    _snapshotFileOut << "  mapping ";
    for (int i = 0; i < totalEntries * _dimensions; i++) {
      _snapshotFileOut << " " << values[i];
    }
    _snapshotFileOut << std::endl;
  }
}

void tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::writeMetaData(const std::string& metaData) {
  if (!metaData.empty()) {
    _snapshotFileOut
      << "  begin meta-data" << std::endl
      << "    " << metaData << std::endl
      << "  end meta-data" << std::endl;
  }
}

tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::CellDataWriter* tarch::plotter::griddata::
  blockstructured::PeanoBinaryPatchFileWriter::createCellDataWriter(
    const std::string& identifier, int unknownsPerAxis, int recordsPerCell, const std::string& description
  ) {
  return new CellDataWriter(identifier, unknownsPerAxis, recordsPerCell, description, "", nullptr, *this);
}

tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::CellDataWriter* tarch::plotter::griddata::
  blockstructured::PeanoBinaryPatchFileWriter::createCellDataWriter(
    const std::string& identifier,
    int                unknownsPerAxis,
    int                recordsPerCell,
    const std::string& description,
    const std::string& metaData
  ) {
  return new CellDataWriter(identifier, unknownsPerAxis, recordsPerCell, description, metaData, nullptr, *this);
}

tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::CellDataWriter* tarch::plotter::griddata::
  blockstructured::PeanoBinaryPatchFileWriter::createCellDataWriter(
    const std::string& identifier,
    int                unknownsPerAxis,
    int                recordsPerCell,
    const std::string& description,
    const std::string& metaData,
    double*            mapping
  ) {
  return new CellDataWriter(identifier, unknownsPerAxis, recordsPerCell, description, metaData, mapping, *this);
}

tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::VertexDataWriter* tarch::plotter::griddata::
  blockstructured::PeanoBinaryPatchFileWriter::createVertexDataWriter(
    const std::string& identifier, int unknownsPerAxis, int recordsPerVertex, const std::string& description
  ) {
  return new VertexDataWriter(identifier, unknownsPerAxis, recordsPerVertex, description, "", nullptr, *this);
}

tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::VertexDataWriter* tarch::plotter::griddata::
  blockstructured::PeanoBinaryPatchFileWriter::createVertexDataWriter(
    const std::string& identifier,
    int                unknownsPerAxis,
    int                recordsPerVertex,
    const std::string& description,
    const std::string& metaData
  ) {
  return new VertexDataWriter(identifier, unknownsPerAxis, recordsPerVertex, description, metaData, nullptr, *this);
}

tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::VertexDataWriter* tarch::plotter::griddata::
  blockstructured::PeanoBinaryPatchFileWriter::createVertexDataWriter(
    const std::string& identifier,
    int                unknownsPerAxis,
    int                recordsPerVertex,
    const std::string& description,
    const std::string& metaData,
    double*            mapping
  ) {
  return new VertexDataWriter(identifier, unknownsPerAxis, recordsPerVertex, description, metaData, mapping, *this);
}

int tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::plotPatch(
  const tarch::la::Vector<2, double>& offset, const tarch::la::Vector<2, double>& size
) {
  if (_haveWrittenAtLeastOnePatch) {
    _snapshotFileOut << "end patch" << std::endl << std::endl;
  }

  _snapshotFileOut << "begin patch" << std::endl << "  offset";

  for (int d = 0; d < 2; d++) {
    _snapshotFileOut << " " << offset(d);
  }
  if (_dimensions == 3) {
    _snapshotFileOut << " 0";
  }
  _snapshotFileOut << std::endl;

  _snapshotFileOut << "  size";

  for (int d = 0; d < 2; d++) {
    _snapshotFileOut << " " << size(d);
  }
  if (_dimensions == 3) {
    _snapshotFileOut << " 0";
  }
  _snapshotFileOut << std::endl;

  _haveWrittenAtLeastOnePatch = true;

  _patchCounter++;
  return _patchCounter - 1;
}

int tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::plotPatch(
  const tarch::la::Vector<3, double>& offset, const tarch::la::Vector<3, double>& size
) {
  assertion(_dimensions == 3);

  if (_haveWrittenAtLeastOnePatch) {
    _snapshotFileOut << "end patch" << std::endl << std::endl;
  }

  _snapshotFileOut << "begin patch" << std::endl << "  offset";

  for (int d = 0; d < 3; d++) {
    _snapshotFileOut << " " << offset(d);
  }
  _snapshotFileOut << std::endl;

  _snapshotFileOut << "  size";

  for (int d = 0; d < 3; d++) {
    _snapshotFileOut << " " << size(d);
  }
  _snapshotFileOut << std::endl;

  _haveWrittenAtLeastOnePatch = true;

  _patchCounter++;
  return _patchCounter - 1;
}

bool tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::writeToFile() {
  assertion(!_writtenToFile);

  std::ofstream out;
  out.open(_fileName, std::ios::binary);
  if ((!out.fail()) && out.is_open()) {
    logDebug("writeToFile()", "opened data file " + _fileName);

    if (_haveWrittenAtLeastOnePatch) {
      _snapshotFileOut << "end patch" << std::endl << std::endl;
    }

    out << _snapshotFileOut.rdbuf();

    _writtenToFile = true;
    return true;
  } else {
    logError("close()", "unable to write output file " + _fileName);
    return false;
  }
}

bool tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::isOpen() { return !_writtenToFile; }

void tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::clear() {
  _writtenToFile = false;

  _snapshotFileOut.clear();

  _patchCounter = 0;

  _haveWrittenAtLeastOnePatch = false;

  _snapshotFileOut << HEADER << "format Binary" << std::endl << "dimensions " << _dimensions << std::endl;

  _snapshotFileOut << std::endl << std::endl;
}



