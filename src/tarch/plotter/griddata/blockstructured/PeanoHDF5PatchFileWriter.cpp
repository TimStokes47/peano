#include "tarch/plotter/griddata/blockstructured/PeanoHDF5PatchFileWriter.h"
#include <fstream>

#include "tarch/parallel/Node.h"


tarch::logging::Log tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter::_log( "tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter" );


const std::string tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter::HEADER =
"# \n" \
"# Peano HDF5 patch file \n" \
"# Version 0.1 \n" \
"# \n";



tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter::PeanoHDF5PatchFileWriter(
  int                  dimension,
  int                  numberOfCellsPerAxis,
  const std::string&   filename,
  bool                 append
):
  _dimensions(dimension),
  _numberOfCellsPerAxis(numberOfCellsPerAxis) {
  assertion( dimension>=2 );
  assertion( dimension<=3 );
  assertion( numberOfCellsPerAxis>1 );

  clear();

  if (filename.rfind(".hdf5")!=std::string::npos) {
    logWarning( "PeanoHDF5PatchFileWriter()", "filename should not end with .h5 as routine adds extension automatically. Chosen filename=" << filename );
  }

  #ifdef HDF5
  if (append) {
    _file = H5Fopen((filename+".hdf5").c_str(), H5F_ACC_RDWR, H5P_DEFAULT);
  }
  else {
    _file = H5Fcreate((filename+".hdf5").c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  }

  if (_file<0) {
    logError( "PeanoHDF5PatchFileWriter()", "failed to obtain file handle for HDF5 file " << filename << ". Wanted to append data=" << append );
    _isOpen = false;
  }
  else {
    _isOpen = true;

    //
    // Create the dataset counter attribute on the root level
    //
    if (!append) {
      /*
       * Create scalar attribute.
       */
      hid_t dataSetCounterDataSpace = H5Screate(H5S_SCALAR);
      hid_t attribute = H5Acreate(
	_file, "Number of datasets", H5T_NATIVE_INT,
	dataSetCounterDataSpace, H5P_DEFAULT, H5P_DEFAULT);

      /*
      * Write scalar attribute.
      */
      int i = 0;
      H5Awrite(attribute, H5T_NATIVE_INT, &i);

      H5Aclose(attribute);
      H5Sclose(dataSetCounterDataSpace);
    }

    //
    // Increase the dataset counter and thus init _numberOfActiveDataset
    //
    hid_t attribute = H5Aopen_by_name(_file, "/", "Number of datasets", H5P_DEFAULT, H5P_DEFAULT);
    H5Aread(attribute, H5T_NATIVE_INT, &_numberOfActiveDataset);
    _numberOfActiveDataset++;
    H5Awrite(attribute, H5T_NATIVE_INT, &_numberOfActiveDataset);
    H5Aclose(attribute);
    _numberOfActiveDataset--;

    //
    // Create active dataset
    //
    hid_t newGroup = H5Gcreate(_file, getNameOfCurrentDataset().c_str(), H5P_DEFAULT,H5P_DEFAULT, H5P_DEFAULT);
    H5Gclose(newGroup);
  }
  #else
  _isOpen = false;
  #endif
}


std::string  tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter::getNameOfCurrentDataset() const {
  return "/dataset-" + std::to_string(_numberOfActiveDataset);
}


tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter::~PeanoHDF5PatchFileWriter() {
  #ifdef HDF5
  if (_file>=0) {
    herr_t   status = H5Fclose(_file);

    if (status<0) {
      logError( "PeanoHDF5PatchFileWriter()", "failed to close HDF5 file" );
    }
  }
  #endif

  _isOpen = false;
}


tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter::CellDataWriter*
tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter::createCellDataWriter( const std::string& identifier, int recordsPerCell ) {
  assertion( _isOpen );
  return new CellDataWriter(identifier, recordsPerCell, "", nullptr, *this);
}


tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter::CellDataWriter*
tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter::createCellDataWriter( const std::string& identifier, int recordsPerCell, const std::string& metaData ) {
  assertion( _isOpen );
  return new CellDataWriter(identifier, recordsPerCell, metaData, nullptr, *this);
}


tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter::CellDataWriter*
tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter::createCellDataWriter( const std::string& identifier, int recordsPerCell, const std::string& metaData, double* mapping ) {
  assertion( _isOpen );
  return new CellDataWriter(identifier, recordsPerCell, metaData, mapping, *this);
}


tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter::VertexDataWriter*
tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter::createVertexDataWriter( const std::string& identifier, int recordsPerVertex ) {
  assertion( _isOpen );
  return new VertexDataWriter(identifier, recordsPerVertex , "", nullptr, *this);
}


tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter::VertexDataWriter*
tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter::createVertexDataWriter( const std::string& identifier, int recordsPerVertex, const std::string& metaData ) {
  assertion( _isOpen );
  return new VertexDataWriter(identifier, recordsPerVertex , metaData, nullptr, *this);
}


tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter::VertexDataWriter*
tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter::createVertexDataWriter( const std::string& identifier, int recordsPerVertex, const std::string& metaData, double* mapping ) {
  assertion( _isOpen );
  return new VertexDataWriter(identifier, recordsPerVertex , metaData, mapping, *this);
}


std::pair<int,int> tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter::plotPatch(
  const tarch::la::Vector<2,double>& offset,
  const tarch::la::Vector<2,double>& size
) {
  assertion( _isOpen );
  assertionEquals( _dimensions, 2 );
/*
  if (_haveWrittenAtLeastOnePatch) {
//    _out << "end patch" << std::endl << std::endl;
  }
*/

//  _out << "begin patch" << std::endl
//       << "  offset";

  for (int d=0; d<2; d++) {
//    _out << " " << offset(d);
  }
  if (_dimensions==3) {
//    _out << " 0";
  }
//  _out << std::endl;

/*
  _out << "  size";

  for (int d=0; d<2; d++) {
    _out << " " << size(d);
  }
  if (_dimensions==3) {
    _out << " 0";
  }
  _out << std::endl;
*/

//  _haveWrittenAtLeastOnePatch = true;

  std::pair<int,int> result(_vertexCounter,_cellCounter);
  _vertexCounter += std::pow(_numberOfCellsPerAxis+1,_dimensions);
  _cellCounter   += std::pow(_numberOfCellsPerAxis+1,_dimensions);
  return result;
}


std::pair<int,int> tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter::plotPatch(
  const tarch::la::Vector<3,double>& offset,
  const tarch::la::Vector<3,double>& size
) {
  assertion( _isOpen );
  assertionEquals( _dimensions, 2 );
/*
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
*/

  std::pair<int,int> result(_vertexCounter,_cellCounter);
  _vertexCounter += std::pow(_numberOfCellsPerAxis+1,_dimensions);
  _cellCounter   += std::pow(_numberOfCellsPerAxis+1,_dimensions);
  return result;
}


bool tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter::writeToFile( const std::string& filenamePrefix ) {
  assertion( _isOpen );

/*
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
*/

/*
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
*/
  return true;
}


bool tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter::isOpen() {
  return _isOpen;
}


void tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter::clear() {
//  _out.clear();

  _vertexCounter = 0;
  _cellCounter   = 0;


/*
  _out << HEADER
      << "format ASCII" << std::endl
      << "dimensions " << _dimensions  << std::endl
      << "patch-size" ;
  for (int d=0; d<_dimensions; d++) {
    _out << " " << _numberOfCellsPerAxis;
  }
  _out << std::endl << std::endl;
*/
}


void tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter::addMetaData(const std::string& metaData) {
/*
  _out << "begin meta-data" << std::endl
       << metaData << std::endl
       << "end meta-data" << std::endl << std::endl;
*/
}


int tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter::getCellsPerPatch() const {
  return std::pow( _numberOfCellsPerAxis, _dimensions );
}


int tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter::getVerticesPerPatch() const {
  return std::pow( _numberOfCellsPerAxis+1, _dimensions );
}
