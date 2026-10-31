#include "PeanoBinaryPatchFileWriter.h"
#include "tarch/la/ScalarOperations.h"

#include <iomanip>


tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::CellDataWriter::CellDataWriter(
  const std::string& identifier,
  int                unknownsPerAxis,
  int                numberOfUnknowns, const std::string& description,
  const std::string& metaData,
  double*            mapping,
  tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter& writer
):
  _writer(writer),
  _identifier(identifier),
  _numberOfCellsPerAxis(unknownsPerAxis),
  _numberOfUnknowns(numberOfUnknowns),
  _entryCounter(0) {
  _writer._snapshotFileOut << "begin cell-metadata \"" << identifier << "\"" << std::endl
               << "  number-of-unknowns " << _numberOfUnknowns << std::endl
               << "  number-of-dofs-per-axis " << _numberOfCellsPerAxis << std::endl
               << "  description " << description << std::endl;

  _writer.writeMetaData(metaData);
  _writer.writeMapping(getCellsPerPatch(),mapping);

  _writer._snapshotFileOut << "end cell-metadata" << std::endl << std::endl;
}


tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::CellDataWriter::~CellDataWriter() {}


int tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::CellDataWriter::getCellsPerPatch() const {
  return tarch::la::aPowI(_writer._dimensions,_numberOfCellsPerAxis);
}


void tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::CellDataWriter::setPrecision( [[maybe_unused]] int precision ) {}


void tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::CellDataWriter::plotCell( [[maybe_unused]] int index, double value ) {
  for (int i=0; i<_numberOfUnknowns; i++) {
    _out.push_back(value);
    _entryCounter++;
  }
  flushIfPatchIsComplete();
}


void tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::CellDataWriter::plotCell( [[maybe_unused]] int index, double* values ) {
  for (int i=0; i<_numberOfUnknowns; i++) {
    _out.push_back(values[i]);
    _entryCounter++;
  }
  flushIfPatchIsComplete();
}


void tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::CellDataWriter::close() {}


void tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::CellDataWriter::flushIfPatchIsComplete() {
  if (_entryCounter>=getCellsPerPatch()*_numberOfUnknowns) {
    _writer._snapshotFileOut << "  begin cell-values \"" << _identifier << "\"" << std::endl;
    _writer._snapshotFileOut.write( reinterpret_cast<const char*>(_out.data()), _out.size()*sizeof(double) );
    _writer._snapshotFileOut << std::endl
                 << "  end cell-values" << std::endl;
    _out.clear();
    _entryCounter = 0;
  }
}


void tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::CellDataWriter::assignRemainingCellsDefaultValues() {
  assertionMsg( false, "not implemented" );
}


int tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::CellDataWriter::getFirstCellWithinPatch(int index) const {
  return index * getCellsPerPatch();
}
