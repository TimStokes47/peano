#include "tarch/plotter/griddata/blockstructured/PeanoHDF5PatchFileWriter.h"



tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter::CellDataWriter::CellDataWriter(
  const std::string& identifier,
  int                numberOfUnknowns,
  const std::string& metaData,
  double*            mapping,
  tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter& writer
):
  _writer(writer),
  _identifier(identifier),
  _numberOfUnknowns(numberOfUnknowns),
  _patchCounter(0) {
/*
  _writer._out << "begin cell-values \"" << identifier << "\"" << std::endl
               << "  number-of-unknowns " << _numberOfUnknowns << std::endl;

  if (!metaData.empty()) {
    _writer._out << "  begin meta-data" << std::endl
                 << "    " << metaData << std::endl
                 << "  end meta-data" << std::endl;
  }

  if (mapping!=nullptr) {
    _writer._out << "  begin mapping" << std::endl;
    for (int i=0; i<_writer.getVerticesPerPatch(); i++) {
      _writer._out << " " << mapping[i];
    }
    _writer._out << "  end mapping" << std::endl;
  }

  _writer._out << "end cell-values" << std::endl << std::endl;
*/
}


tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter::CellDataWriter::~CellDataWriter() {
}


void tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter::CellDataWriter::plotCell( int index, double value ) {
  _out << " " << value;
  for (int i=1; i<_numberOfUnknowns; i++) {
    _out << " 0";
  }
  _patchCounter++;
  flushIfPatchIsComplete();
}


void tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter::CellDataWriter::plotCell( int index, const tarch::la::Vector<2,double>& value ) {
  _out << " " << value(0) << " " << value(1);
  for (int i=2; i<_numberOfUnknowns; i++) {
    _out << " 0";
  }
  _patchCounter++;
  flushIfPatchIsComplete();
}


void tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter::CellDataWriter::plotCell( int index, const tarch::la::Vector<3,double>& value ) {
  _out << " " << value(0) << " " << value(1) << " " << value(2);
  for (int i=3; i<_numberOfUnknowns; i++) {
    _out << " 0";
  }
  _patchCounter++;
  flushIfPatchIsComplete();
}


void tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter::CellDataWriter::plotCell( int index, double* values, int numberOfValues ) {
  for (int i=0; i<numberOfValues; i++) {
    _out << " " << values[i];
  }
  for (int i=numberOfValues; i<_numberOfUnknowns; i++) {
    _out << " 0";
  }
  _patchCounter++;
  flushIfPatchIsComplete();
}


void tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter::CellDataWriter::close() {
}


void tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter::CellDataWriter::flushIfPatchIsComplete() {
/*
  if (_patchCounter>=_writer.getCellsPerPatch()) {
    _out << std::flush;
    _writer._out << "  begin cell-values \"" << _identifier << "\"" << std::endl
                 << "    " << _out.rdbuf() << std::endl
                 << "  end cell-values" << std::endl;
    _out.clear();
    _patchCounter = 0;
  }
*/
}


void tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter::CellDataWriter::assignRemainingCellsDefaultValues() {
  while (_patchCounter<_writer.getCellsPerPatch()) {
    plotCell(-1,0.0);
  }
}

