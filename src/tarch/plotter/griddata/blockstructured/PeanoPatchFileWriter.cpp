#include "tarch/plotter/griddata/blockstructured/PeanoPatchFileWriter.h"
#include <fstream>

#include "tarch/parallel/Node.h"


tarch::logging::Log tarch::plotter::griddata::blockstructured::PeanoPatchFileWriter::_log( "tarch::plotter::griddata::blockstructured::PeanoPatchFileWriter" );


const std::string tarch::plotter::griddata::blockstructured::PeanoPatchFileWriter::HEADER =
"# \n" \
"# Peano patch file \n" \
"# Version 0.1 \n" \
"# \n";



tarch::plotter::griddata::blockstructured::PeanoPatchFileWriter::PeanoPatchFileWriter(int dimension, int numberOfCellsPerAxis):
  _writtenToFile(false),
  _dimensions(dimension),
  _numberOfCellsPerAxis(numberOfCellsPerAxis) {
  assertion( dimension>=2 );
  assertion( dimension<=3 );
  assertion( numberOfCellsPerAxis>1 );

  clear();
}


tarch::plotter::griddata::blockstructured::PeanoPatchFileWriter::CellDataWriter*
tarch::plotter::griddata::blockstructured::PeanoPatchFileWriter::createCellDataWriter( const std::string& identifier, int recordsPerCell ) {
  _totalNumberOfUnknowns += recordsPerCell;
  return new CellDataWriter(identifier, recordsPerCell, _totalNumberOfUnknowns-recordsPerCell, "", nullptr, *this);
}


tarch::plotter::griddata::blockstructured::PeanoPatchFileWriter::CellDataWriter*
tarch::plotter::griddata::blockstructured::PeanoPatchFileWriter::createCellDataWriter( const std::string& identifier, int recordsPerCell, const std::string& metaData ) {
  _totalNumberOfUnknowns += recordsPerCell;
  return new CellDataWriter(identifier, recordsPerCell, _totalNumberOfUnknowns-recordsPerCell, metaData, nullptr, *this);
}


tarch::plotter::griddata::blockstructured::PeanoPatchFileWriter::CellDataWriter*
tarch::plotter::griddata::blockstructured::PeanoPatchFileWriter::createCellDataWriter( const std::string& identifier, int recordsPerCell, const std::string& metaData, double* mapping ) {
  _totalNumberOfUnknowns += recordsPerCell;
  return new CellDataWriter(identifier, recordsPerCell, _totalNumberOfUnknowns-recordsPerCell, metaData, mapping, *this);
}


tarch::plotter::griddata::blockstructured::PeanoPatchFileWriter::VertexDataWriter*
tarch::plotter::griddata::blockstructured::PeanoPatchFileWriter::createVertexDataWriter( const std::string& identifier, int recordsPerVertex ) {
  _totalNumberOfUnknowns += recordsPerVertex;
  return new VertexDataWriter(identifier, recordsPerVertex , _totalNumberOfUnknowns-recordsPerVertex, "", nullptr, *this);
}


tarch::plotter::griddata::blockstructured::PeanoPatchFileWriter::VertexDataWriter*
tarch::plotter::griddata::blockstructured::PeanoPatchFileWriter::createVertexDataWriter( const std::string& identifier, int recordsPerVertex, const std::string& metaData ) {
  _totalNumberOfUnknowns += recordsPerVertex;
  return new VertexDataWriter(identifier, recordsPerVertex , _totalNumberOfUnknowns-recordsPerVertex, metaData, nullptr, *this);
}


tarch::plotter::griddata::blockstructured::PeanoPatchFileWriter::VertexDataWriter*
tarch::plotter::griddata::blockstructured::PeanoPatchFileWriter::createVertexDataWriter( const std::string& identifier, int recordsPerVertex, const std::string& metaData, double* mapping ) {
  _totalNumberOfUnknowns += recordsPerVertex;
  return new VertexDataWriter(identifier, recordsPerVertex , _totalNumberOfUnknowns-recordsPerVertex, metaData, mapping, *this);
}


std::pair<int,int> tarch::plotter::griddata::blockstructured::PeanoPatchFileWriter::plotPatch(
  const tarch::la::Vector<2,double>& offset,
  const tarch::la::Vector<2,double>& size
) {
  if (_haveWrittenAtLeastOnePatch) {
    _out << "end patch" << std::endl << std::endl;
  }

  _out << "begin patch" << std::endl
       << "  offset";

  for (int d=0; d<2; d++) {
    _out << " " << offset(d);
  }
  if (_dimensions==3) {
    _out << " 0";
  }
  _out << std::endl;

  _out << "  size";

  for (int d=0; d<2; d++) {
    _out << " " << size(d);
  }
  if (_dimensions==3) {
    _out << " 0";
  }
  _out << std::endl;

  _haveWrittenAtLeastOnePatch = true;

  std::pair<int,int> result(_vertexCounter,_cellCounter);
  _vertexCounter += std::pow(_numberOfCellsPerAxis+1,_dimensions);
  _cellCounter   += std::pow(_numberOfCellsPerAxis+1,_dimensions);
  return result;
}


std::pair<int,int> tarch::plotter::griddata::blockstructured::PeanoPatchFileWriter::plotPatch(
  const tarch::la::Vector<3,double>& offset,
  const tarch::la::Vector<3,double>& size
) {
  assertion( _dimensions==3 );

  if (_haveWrittenAtLeastOnePatch) {
    _out << "end patch" << std::endl << std::endl;
  }

  _out << "begin patch" << std::endl
       << "  offset";

  for (int d=0; d<3; d++) {
    _out << " " << offset(d);
  }
  _out << std::endl;

  _out << "  size";

  for (int d=0; d<3; d++) {
    _out << " " << size(d);
  }
  _out << std::endl;

  _haveWrittenAtLeastOnePatch = true;

  std::pair<int,int> result(_vertexCounter,_cellCounter);
  _vertexCounter += std::pow(_numberOfCellsPerAxis+1,_dimensions);
  _cellCounter   += std::pow(_numberOfCellsPerAxis+1,_dimensions);
  return result;
}


bool tarch::plotter::griddata::blockstructured::PeanoPatchFileWriter::writeToFile( const std::string& filenamePrefix ) {
  assertion( !_writtenToFile );

  if (filenamePrefix.rfind(".ppf")!=std::string::npos) {
    logWarning( "writeToFile()", "filename should not end with .ppf as routine adds extension automatically. Chosen filename prefix=" << filenamePrefix );
  }
  std::ostringstream filenameStream;
  filenameStream << filenamePrefix
    #ifdef Parallel
                 << "-rank-" << tarch::parallel::Node::getInstance().getRank()
    #endif
                 << ".ppf";
  const std::string filename = filenameStream.str();

  std::ofstream out;
  out.open( filename.c_str(), std::ios::binary );
  if ( (!out.fail()) && out.is_open() ) {
    _log.debug( "close()", "opened data file " + filename );

    if (_haveWrittenAtLeastOnePatch) {
      _out << "end patch" << std::endl << std::endl;
    }

    out << _out.rdbuf();

    _writtenToFile = true;
    return true;
  }
  else {
    _log.error( "close()", "unable to write output file " + filename );
    return false;
  }
}


bool tarch::plotter::griddata::blockstructured::PeanoPatchFileWriter::isOpen() {
  return !_writtenToFile;
}


void tarch::plotter::griddata::blockstructured::PeanoPatchFileWriter::clear() {
  _writtenToFile       = false;

  _out.clear();

  _vertexCounter = 0;
  _cellCounter   = 0;
  _haveWrittenAtLeastOnePatch = false;


  _out << HEADER
      << "format ASCII" << std::endl
      << "dimensions " << _dimensions  << std::endl
      << "patch-size" ;
  for (int d=0; d<_dimensions; d++) {
    _out << " " << _numberOfCellsPerAxis;
  }
  _out << std::endl << std::endl;
}


void tarch::plotter::griddata::blockstructured::PeanoPatchFileWriter::addMetaData(const std::string& metaData) {
  _out << "begin meta-data" << std::endl
       << metaData << std::endl
       << "end meta-data" << std::endl << std::endl;
}


int tarch::plotter::griddata::blockstructured::PeanoPatchFileWriter::getCellsPerPatch() const {
  return std::pow( _numberOfCellsPerAxis, _dimensions );
}


int tarch::plotter::griddata::blockstructured::PeanoPatchFileWriter::getVerticesPerPatch() const {
  return std::pow( _numberOfCellsPerAxis+1, _dimensions );
}


