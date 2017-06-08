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
  bool                 append,
  int                  chunkSizeForExtendableDataset
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
	      dataSetCounterDataSpace, H5P_DEFAULT, H5P_DEFAULT
	    );

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
    // Create active dataset group
    //
    hid_t newGroup;
    newGroup = H5Gcreate(_file, getNameOfCurrentDataset().c_str(), H5P_DEFAULT,H5P_DEFAULT, H5P_DEFAULT);
    H5Gclose(newGroup);

    //
    // Create active dataset group
    //
    newGroup = H5Gcreate(_file, (getNameOfCurrentDataset()+"/metadata").c_str(), H5P_DEFAULT,H5P_DEFAULT, H5P_DEFAULT);
    H5Gclose(newGroup);

    newGroup = H5Gcreate(_file, (getNameOfCurrentDataset()+"/mapping").c_str(), H5P_DEFAULT,H5P_DEFAULT, H5P_DEFAULT);
    H5Gclose(newGroup);
  }
  #else
  logError( "PeanoHDF5PatchFileWriter()", "tried to use Peano's HDF5 writer though code has been compiled without -DHDF5" );
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

  _geometryData.push_back( offset(0) );
  _geometryData.push_back( offset(1) );
  _geometryData.push_back( size(0) );
  _geometryData.push_back( size(1) );

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

  _geometryData.push_back( offset(0) );
  _geometryData.push_back( offset(1) );
  _geometryData.push_back( offset(2) );
  _geometryData.push_back( size(0) );
  _geometryData.push_back( size(1) );
  _geometryData.push_back( size(2) );

  std::pair<int,int> result(_vertexCounter,_cellCounter);
  _vertexCounter += std::pow(_numberOfCellsPerAxis+1,_dimensions);
  _cellCounter   += std::pow(_numberOfCellsPerAxis+1,_dimensions);
  return result;
}


bool tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter::writeToFile( const std::string& filenamePrefix ) {
  assertion( _isOpen );

  #ifdef HDF5
  //
  // Create the data space with unlimited dimensions.
  hsize_t geometryTableDimensions[] = {
    2*static_cast<hsize_t>(_dimensions),
    _geometryData.size()/_dimensions/2
  };

  //
  // Set up handles/tables
  //
  hid_t geometryTable = H5Screate_simple(2, geometryTableDimensions, NULL);
  hid_t geometryDataset = H5Dcreate(
    _file,
    (getNameOfCurrentDataset()+"/geometry").c_str(),
    H5T_NATIVE_DOUBLE,
    geometryTable, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT
  );

  //
  // Write data
  //
  H5Dwrite(
    geometryDataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,
    _geometryData.data()
  );

  //
  // Close/release handles
  //
  hid_t result = H5Dclose(geometryDataset);
  H5Sclose(geometryTable);

  return result>=0;
  #else
  return true;
  #endif
}


bool tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter::isOpen() {
  return _isOpen;
}


void tarch::plotter::griddata::blockstructured::PeanoHDF5PatchFileWriter::clear() {
  _vertexCounter = 0;
  _cellCounter   = 0;
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
