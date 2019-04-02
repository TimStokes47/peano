#include "PeanoTextPatchFileWriter.h"
#include "tarch/la/ScalarOperations.h"


tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::VertexDataWriter::VertexDataWriter(
  const std::string& identifier,
  int                unknownsPerAxis,
  int                numberOfUnknowns,
  const std::string& metaData,
  double*            mapping,
  tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter& writer
):
  _writer(writer),
  _identifier(identifier),
  _numberOfVerticesPerAxis(unknownsPerAxis),
  _numberOfUnknowns(numberOfUnknowns),
  _patchCounter(0) {
  _writer._snapshotFileOut << "begin vertex-values \"" << identifier << "\"" << std::endl
                           << "  number-of-unknowns " << _numberOfUnknowns << std::endl
                           << "  number-of-vertices-per-axis " << _numberOfVerticesPerAxis << std::endl;

  _writer.writeMetaData(metaData);
  _writer.writeMapping(getVerticesPerPatch(),mapping);

  _writer._snapshotFileOut << "end vertex-values" << std::endl << std::endl;
}


tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::VertexDataWriter::~VertexDataWriter() {
}


int tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::VertexDataWriter::getVerticesPerPatch() const {
  return tarch::la::aPowI(_writer._dimensions,_numberOfVerticesPerAxis);
}


void tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::VertexDataWriter::plotVertex( int index, double value ) {
  assertion( !std::isnan(value) );
  _out << " " << value;
  for (int i=1; i<_numberOfUnknowns; i++) {
    _out << " 0";
  }
  _patchCounter++;
  flushIfPatchIsComplete();
}


void tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::VertexDataWriter::plotVertex( int index, double* values ) {
	assertion(false);
/*
  for (int i=0; i<numberOfValues; i++) {
    _out << " " << values[i];
    assertion( !std::isnan(values[i]) );
  }
  for (int i=numberOfValues; i<_numberOfUnknowns; i++) {
    _out << " 0";
  }
  _patchCounter++;
  flushIfPatchIsComplete();
*/
}


void tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::VertexDataWriter::close() {
}


void tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::VertexDataWriter::assignRemainingVerticesDefaultValues() {
  if (_patchCounter>=getVerticesPerPatch()) {
    _out << std::flush;
    _writer._snapshotFileOut << "  begin vertex-values \"" << _identifier << "\"" << std::endl
                 << "    " << _out.rdbuf() << std::endl
                 << "  end vertex-values" << std::endl;
    _out.clear();
    _patchCounter = 0;
  }
}


void tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::VertexDataWriter::flushIfPatchIsComplete() {
  if (_patchCounter>=getVerticesPerPatch()) {
    _out << std::flush;
    _writer._snapshotFileOut << "  begin vertex-values \"" << _identifier << "\"" << std::endl
                 << "    " << _out.rdbuf() << std::endl
                 << "  end vertex-values" << std::endl;
    _out.clear();
    _patchCounter = 0;
  }
}
