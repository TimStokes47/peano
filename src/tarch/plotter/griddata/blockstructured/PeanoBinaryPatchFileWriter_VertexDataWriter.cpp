#include "PeanoBinaryPatchFileWriter.h"
#include "tarch/la/ScalarOperations.h"

#include <iomanip>

tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::VertexDataWriter::VertexDataWriter(
  const std::string& identifier,
  int                unknownsPerAxis,
  int                numberOfUnknowns, const std::string& description,
  const std::string& metaData,
  double*            mapping,
  tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter& writer
):
  _writer(writer),
  _identifier(identifier),
  _numberOfVerticesPerAxis(unknownsPerAxis),
  _numberOfUnknowns(numberOfUnknowns),
  _entryCounter(0) {
  _writer._snapshotFileOut << "begin vertex-metadata \"" << identifier << "\"" << std::endl
                           << "  number-of-unknowns " << _numberOfUnknowns << std::endl
                           << "  number-of-dofs-per-axis " << _numberOfVerticesPerAxis << std::endl
                           << "  description " << description << std::endl;

  _writer.writeMetaData(metaData);
  _writer.writeMapping(getVerticesPerPatch(),mapping);

  _writer._snapshotFileOut << "end vertex-metadata" << std::endl << std::endl;
}

tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::VertexDataWriter::~VertexDataWriter() {}

void tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::VertexDataWriter::setPrecision( [[maybe_unused]] int precision ) {}

int tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::VertexDataWriter::getVerticesPerPatch() const {
  return tarch::la::aPowI(_writer._dimensions,_numberOfVerticesPerAxis);
}

void tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::VertexDataWriter::plotVertex( [[maybe_unused]] int index, double value ) {
  for (int i=0; i<_numberOfUnknowns; i++) {
    _out.push_back(value);
    _entryCounter++;
  }
  flushIfPatchIsComplete();
}

void tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::VertexDataWriter::plotVertex( [[maybe_unused]] int index, double* values ) {
  for (int i=0; i<_numberOfUnknowns; i++) {
    _out.push_back(values[i]);
    _entryCounter++;
  }
  flushIfPatchIsComplete();
}

void tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::VertexDataWriter::close() {}

void tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::VertexDataWriter::assignRemainingVerticesDefaultValues() {
/*
  while (_entryCounter<getVerticesPerPatch()) {
    _out << std::flush;
    _writer._snapshotFileOut << "  begin vertex-values \"" << _identifier << "\"" << std::endl
                 << "    " << _out.rdbuf() << std::endl
                 << "  end vertex-values" << std::endl;
    _out.clear();
    _entryCounter = 0;
  }
*/
  assertionMsg(false, "not yet implemented" );
}

void tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::VertexDataWriter::flushIfPatchIsComplete() {
  if (_entryCounter>=getVerticesPerPatch()*_numberOfUnknowns) {
    _writer._snapshotFileOut << "  begin vertex-values \"" << _identifier << "\"" << std::endl;
    _writer._snapshotFileOut.write( reinterpret_cast<const char*>(_out.data()), _out.size()*sizeof(double) );
    _writer._snapshotFileOut << std::endl
                 << "  end vertex-values" << std::endl;
    _out.clear();
    _entryCounter = 0;
  }
}

int tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter::VertexDataWriter::getFirstVertexWithinPatch(int index) const {
  return index * getVerticesPerPatch();
}
