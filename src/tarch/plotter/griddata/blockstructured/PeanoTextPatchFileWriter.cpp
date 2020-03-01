#include <fstream>

#include "PeanoTextPatchFileWriter.h"

#include "tarch/mpi/Rank.h"

tarch::logging::Log tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::_log( "tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter" );


const std::string tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::HEADER =
"# \n" \
"# Peano patch file \n" \
"# Version 0.2 \n" \
"# \n";



//void tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::addDataFileToIndexFile( const std::string& filename) {
/*
  std::ofstream     _metaFileOut;
  if (append) {
    _metaFileOut.open( (filename+".peano-patch-file").c_str(), std::ios::app );
  }

  if (tarch::mpi::Rank::getInstance().isGlobalMaster()) {
    _metaFileOut << std::endl << "begin dataset" << std::endl;

    #ifdef Parallel
    for (int i=0; i<tarch::mpi::Rank::getInstance().getNumberOfRanks(); i++) {
      std::ostringstream referencedFilename;
      if (filenamePrefix.find("/")!=std::string::npos) {
        referencedFilename << filenamePrefix.substr( filenamePrefix.rfind("/")+1 );
      }
      else {
        referencedFilename << filenamePrefix;
      }
      referencedFilename << "-rank-" << i
                         << ".peano-patch-file";
      _metaFileOut << "  include \"" << referencedFilename.str() << "\"" << std::endl;
    }
    #else
    std::ostringstream referencedFilename;
    if (filenamePrefix.find("/")!=std::string::npos) {
      referencedFilename << filenamePrefix.substr( filenamePrefix.rfind("/")+1 );
    }
    else {
      referencedFilename << filenamePrefix;
    }
    referencedFilename << ".peano-patch-file";
    _metaFileOut << "  include \"" << referencedFilename.str() << "\"" << std::endl;
    #endif

    _metaFileOut << "end dataset" << std::endl;
  }

*/
//}


//void tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::addDatasetToIndexFile( const std::vector< std::string >& filenames) {

//}


void tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::createEmptyIndexFile() {
  if (_indexFile.rfind(".peano-patch-file")!=std::string::npos) {
    logWarning( "PeanoTextPatchFileWriter()", "filename should not end with .peano-patch-file as routine adds extension automatically. Chosen filename=" << _indexFile );
  }

  std::ofstream     indexFileOut;

  indexFileOut.open( (_indexFile + ".peano-patch-file").c_str(), std::ios::out );

  if ( (!indexFileOut.fail()) && indexFileOut.is_open() ) {
    indexFileOut << HEADER
                 << "format ASCII" << std::endl;
  }

  if ( !indexFileOut.is_open() ) {
    logError("PeanoTextPatchFileWriter()", "have not been able to open file " << _indexFile << ".peano-patch-file");
  }
  indexFileOut.close();
}


tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::PeanoTextPatchFileWriter(
  int                 dimension,
  const std::string&  indexFile
):
  _writtenToFile(false),
  _dimensions(dimension),
  _indexFile(indexFile) {
  assertion( dimension>=2 );
  assertion( dimension<=3 );

  clear();

  createEmptyIndexFile();
}


tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::~PeanoTextPatchFileWriter() {
}


void tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::writeMapping(int totalEntries, double* values) {
  if (values!=nullptr) {
    _snapshotFileOut << "  begin mapping" << std::endl;
    for (int i=0; i<totalEntries * _dimensions; i++) {
      _snapshotFileOut << " " << values[i];
    }
    _snapshotFileOut << std::endl << "  end mapping" << std::endl;
  }
}


void tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::writeMetaData(const std::string& metaData) {
  if (!metaData.empty()) {
    _snapshotFileOut << "  begin meta-data" << std::endl
                     << "    " << metaData << std::endl
                     << "  end meta-data" << std::endl;
  }
}


tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::CellDataWriter*
tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::createCellDataWriter( const std::string& identifier, int unknownsPerAxis, int recordsPerCell ) {
  return new CellDataWriter(identifier, unknownsPerAxis, recordsPerCell, "", nullptr, *this);
}


tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::CellDataWriter*
tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::createCellDataWriter( const std::string& identifier, int unknownsPerAxis, int recordsPerCell, const std::string& metaData ) {
  return new CellDataWriter(identifier, unknownsPerAxis, recordsPerCell, metaData, nullptr, *this);
}


tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::CellDataWriter*
tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::createCellDataWriter( const std::string& identifier, int unknownsPerAxis, int recordsPerCell, const std::string& metaData, double* mapping ) {
  return new CellDataWriter(identifier, unknownsPerAxis, recordsPerCell, metaData, mapping, *this);
}


tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::VertexDataWriter*
tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::createVertexDataWriter( const std::string& identifier, int unknownsPerAxis, int recordsPerVertex ) {
  return new VertexDataWriter(identifier, unknownsPerAxis, recordsPerVertex , "", nullptr, *this);
}


tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::VertexDataWriter*
tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::createVertexDataWriter( const std::string& identifier, int unknownsPerAxis, int recordsPerVertex, const std::string& metaData ) {
  return new VertexDataWriter(identifier, unknownsPerAxis, recordsPerVertex , metaData, nullptr, *this);
}


tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::VertexDataWriter*
tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::createVertexDataWriter( const std::string& identifier, int unknownsPerAxis, int recordsPerVertex, const std::string& metaData, double* mapping ) {
  return new VertexDataWriter(identifier, unknownsPerAxis, recordsPerVertex , metaData, mapping, *this);
}


int tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::plotPatch(
  const tarch::la::Vector<2,double>& offset,
  const tarch::la::Vector<2,double>& size
) {
  if (_haveWrittenAtLeastOnePatch) {
    _snapshotFileOut << "end patch" << std::endl << std::endl;
  }

  _snapshotFileOut << "begin patch" << std::endl
       << "  offset";

  for (int d=0; d<2; d++) {
    _snapshotFileOut << " " << offset(d);
  }
  if (_dimensions==3) {
    _snapshotFileOut << " 0";
  }
  _snapshotFileOut << std::endl;

  _snapshotFileOut << "  size";

  for (int d=0; d<2; d++) {
    _snapshotFileOut << " " << size(d);
  }
  if (_dimensions==3) {
    _snapshotFileOut << " 0";
  }
  _snapshotFileOut << std::endl;

  _haveWrittenAtLeastOnePatch = true;

  _patchCounter++;
  return _patchCounter-1;
}


int tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::plotPatch(
  const tarch::la::Vector<3,double>& offset,
  const tarch::la::Vector<3,double>& size
) {
  assertion( _dimensions==3 );

  if (_haveWrittenAtLeastOnePatch) {
    _snapshotFileOut << "end patch" << std::endl << std::endl;
  }

  _snapshotFileOut << "begin patch" << std::endl
       << "  offset";

  for (int d=0; d<3; d++) {
    _snapshotFileOut << " " << offset(d);
  }
  _snapshotFileOut << std::endl;

  _snapshotFileOut << "  size";

  for (int d=0; d<3; d++) {
    _snapshotFileOut << " " << size(d);
  }
  _snapshotFileOut << std::endl;

  _haveWrittenAtLeastOnePatch = true;

  _patchCounter++;
  return _patchCounter-1;
}


bool tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::writeToFile( const std::string& filenamePrefix ) {
  assertion( !_writtenToFile );


  if (filenamePrefix.rfind(".peano-patch-file")!=std::string::npos) {
    logWarning( "writeToFile()", "filename should not end with .peano-patch-file as routine adds extension automatically. Chosen filename prefix=" << filenamePrefix );
  }
  std::ostringstream filenameStream;
  filenameStream << filenamePrefix
    #ifdef Parallel
                 << "-rank-" << tarch::mpi::Rank::getInstance().getRank()
    #endif
                 << ".peano-patch-file";
  const std::string filename = filenameStream.str();

  std::ofstream out;
  out.open( filename.c_str(), std::ios::binary );
  if ( (!out.fail()) && out.is_open() ) {
    _log.debug( "writeToFile()", "opened data file " + filename );

    if (_haveWrittenAtLeastOnePatch) {
      _snapshotFileOut << "end patch" << std::endl << std::endl;
    }

    out << _snapshotFileOut.rdbuf();

    _writtenToFile = true;
    return true;
  }
  else {
    _log.error( "close()", "unable to write output file " + filename );
    return false;
  }
}


bool tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::isOpen() {
  return !_writtenToFile;
}


void tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::clear() {
  _writtenToFile       = false;

  _snapshotFileOut.clear();

  _patchCounter = 0;

  _haveWrittenAtLeastOnePatch = false;

  _snapshotFileOut << HEADER
      << "format ASCII" << std::endl
      << "dimensions " << _dimensions  << std::endl;

  _snapshotFileOut << std::endl << std::endl;
}
