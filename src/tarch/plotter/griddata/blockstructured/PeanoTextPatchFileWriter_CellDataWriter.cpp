#include "PeanoTextPatchFileWriter.h"
#include "tarch/la/ScalarOperations.h"


tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::CellDataWriter::CellDataWriter(
  const std::string& identifier,
  int                unknownsPerAxis,
  int                numberOfUnknowns,
  const std::string& metaData,
  double*            mapping,
  tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter& writer
):
  _writer(writer),
  _identifier(identifier),
  _numberOfCellsPerAxis(unknownsPerAxis),
  _numberOfUnknowns(numberOfUnknowns),
  _patchCounter(0) {
  _writer._snapshotFileOut << "begin cell-values \"" << identifier << "\"" << std::endl
               << "  number-of-unknowns " << _numberOfUnknowns << std::endl
               << "  number-of-cells-per-axis " << _numberOfCellsPerAxis << std::endl;

  _writer.writeMetaData(metaData);
  _writer.writeMapping(getCellsPerPatch(),mapping);

  _writer._snapshotFileOut << "end cell-values" << std::endl << std::endl;
}


tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::CellDataWriter::~CellDataWriter() {
}


int tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::CellDataWriter::getCellsPerPatch() const {
  return tarch::la::aPowI(_writer._dimensions,_numberOfCellsPerAxis);
}


void tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::CellDataWriter::plotCell( int index, double value ) {
  assertion( !std::isnan(value) );
  _out << " " << value;
  for (int i=1; i<_numberOfUnknowns; i++) {
    _out << " 0";
  }
  _patchCounter++;
  flushIfPatchIsComplete();
}


void tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::CellDataWriter::plotCell( int index, double* values ) {
	assertion(false);
/*
  for (int i=0; i<numberOfValues; i++) {
    assertion( !std::isnan(values[i]) );
    _out << " " << values[i];
  }
  for (int i=numberOfValues; i<_numberOfUnknowns; i++) {
    _out << " 0";
  }
  _patchCounter++;
  flushIfPatchIsComplete();
*/
}


void tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::CellDataWriter::close() {
}


void tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::CellDataWriter::flushIfPatchIsComplete() {
  if (_patchCounter>=getCellsPerPatch()) {
    _out << std::flush;
    _writer._snapshotFileOut << "  begin cell-values \"" << _identifier << "\"" << std::endl
                 << "    " << _out.rdbuf() << std::endl
                 << "  end cell-values" << std::endl;
    _out.clear();
    _patchCounter = 0;
  }
}


void tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::CellDataWriter::assignRemainingCellsDefaultValues() {
  while (_patchCounter<getCellsPerPatch()) {
    plotCell(-1,0.0);
  }
}
