//#include "tarch/plotter/griddata/unstructured/vtu/VTUTextFileWriter.h"
//
//#include <stdio.h>
//#include <fstream>
//#include <iomanip>
//
//tarch::logging::Log tarch::plotter::griddata::unstructured::vtu::VTUTextFileWriter::_log( "tarch::plotter::griddata::unstructured::vtu::VTUTextFileWriter" );
//
//
//tarch::plotter::griddata::unstructured::vtu::VTUTextFileWriter::VTUTextFileWriter(const int precision):
//  _writtenToFile(false),
//  _precision(precision),
//  _doubleOrFloat(setDoubleOrFloatString(precision)),
//  _numberOfVertices(0),
//  _numberOfCells(0),
//  _numberOfCellEntries(0) {
//}
//
//
//tarch::plotter::griddata::unstructured::vtu::VTUTextFileWriter::~VTUTextFileWriter() {
//  if (!_writtenToFile) {
//    assertionEqualsMsg( _numberOfVertices,    0, "Still vertices in vtu writer pipeline. Maybe you forgot to call writeToFile() on a data vtu writer?" );
//    assertionEqualsMsg( _numberOfCells,       0, "Still cells in vtu writer pipeline. Maybe you forgot to call writeToFile() on a data vtu writer?" );
//    assertionEqualsMsg( _numberOfCellEntries, 0, "Still cell entries in vtu writer pipeline. Maybe you forgot to call writeToFile() on a data vtu writer?" );
//  }
//}
//
//
//std::string tarch::plotter::griddata::unstructured::vtu::VTUTextFileWriter::setDoubleOrFloatString(const int precision) {
//  if (precision < 7){
//    return "float";
//  } else {
//    return "double";
//  }
//}
//
//
//void tarch::plotter::griddata::unstructured::vtu::VTUTextFileWriter::clear() {
//  _writtenToFile       = false;
//  _numberOfVertices    = 0;
//  _numberOfCells       = 0;
//  _numberOfCellEntries = 0;
//  _vertexDescription      = "";
//  _cellTypeDescription    = "";
//  _vertexDataDescription  = "";
//  _cellDataDescription    = "";
//  _vertexDataDescriptionParallel  = "";
//  _cellDataDescriptionParallel    = "";
//}
//
//
//#ifdef Parallel
//void tarch::plotter::griddata::unstructured::vtu::VTUTextFileWriter::writePVTU(const std::string& filenameComplete, const std::string& filenameShort, int timestepnumber){
////  @todo
//  std::ofstream out;
//  out.open( filenameComplete.c_str() );
//  if ( (!out.fail()) && out.is_open() ) {
//    _log.debug( "close()", "opened data file " + filenameComplete );
//
//    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>" << std::endl;
//    out << "<VTKFile byte_order=\"LittleEndian\" type=\"PUnstructuredGrid\" version=\"0.1\">" << std::endl;
//    out << "<PUnstructuredGrid GhostLevel=\"0\">" << std::endl;
//    if (_numberOfVertices>0 && !_vertexDataDescription.empty()) {
//      out << "<PPointData>" << std::endl;
//      out << _vertexDataDescriptionParallel;
//      out << "</PPointData>" << std::endl;
//    }
//
//    if (_numberOfCells>0 && !_cellDataDescription.empty() ) {
//      out << "<PCellData>" << std::endl;
//      out << _cellDataDescriptionParallel;
//      out << "</PCellData>" << std::endl;
//    }
//
//
//    out << "<PPoints>" << std::endl;
//    out << "<PDataArray Name=\"points\" NumberOfComponents=\"3\" format=\"ascii\" type=\"Float32\"/>" << std::endl;
//    out << "</PPoints>" << std::endl;
//
//
//    out << "<PCells>" << std::endl;
//    out << "<PDataArray Name=\"connectivity\" NumberOfComponents=\"1\" format=\"ascii\" type=\"Int32\"/>" << std::endl;
//    out << "<PDataArray Name=\"offsets\" NumberOfComponents=\"1\" format=\"ascii\" type=\"Int32\"/>" << std::endl;
//    out << "<PDataArray Name=\"types\" NumberOfComponents=\"1\" format=\"ascii\" type=\"UInt8\"/>" << std::endl;
//    out << "</PCells>" << std::endl;
//
//    for(int i=0; i<tarch::parallel::Node::getInstance().getNumberOfNodes(); i++){
//      out << "<Piece Source=\"" << filenameShort  << ".cpu-"
//          << std::setw(
//              (unsigned int) (log(
//                  (double) tarch::parallel::Node::getInstance().getNumberOfNodes())
//                  / log(10.0)) + 1) << std::setfill('0') << i << ".";
//      out << timestepnumber << std::string(".vtu");
//      out << "\"/>" << std::endl;
//    }
//    out << "</PUnstructuredGrid>" << std::endl;
//    out << "</VTKFile>" << std::endl;
//    _log.debug( "close()", "data written to " + filenameComplete );
//  }
//  else {
//    _log.error( "close()", "unable to write output file " + filenameComplete );
//  }
//
//  _writtenToFile = true;
//
//}
//#endif
//
//
//void tarch::plotter::griddata::unstructured::vtu::VTUTextFileWriter::writeToFile( const std::string& filename ) {
//  assertion( !_writtenToFile );
//
//  std::ofstream out;
//  out.open( filename.c_str() );
//  if ( (!out.fail()) && out.is_open() ) {
//    _log.debug( "close()", "opened data file " + filename );
//    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>" << std::endl;
//    out << "<VTKFile type=\"UnstructuredGrid\" version=\"0.1\" byte_order=\"LittleEndian\">"<< std::endl;
//    out << "<UnstructuredGrid>" << std::endl;
//    out << "<Piece NumberOfPoints=\""<< _numberOfVertices << "\" NumberOfCells=\"" << _numberOfCells <<"\">" << std::endl;
//
//    //TODO: is number of Components dynamical?
//    out << "<Points>" << std::endl;
//    out << "<DataArray Name=\"points\" type=\"Float32\" NumberOfComponents=\""<< "3"<<"\" format=\"ascii\">" << std::endl;  //TODO prüfen ob numberOfComponents als dynamische Variable vorliegt
//    out << _vertexDescription ;
//    out << "</DataArray>" << std::endl;
//    out << "</Points>" << std::endl;
//
//    out << "<Cells>" << std::endl;
//    out << "<DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">" << std::endl;
//    out << _connectivity << std::endl;
//    out << "</DataArray>" << std::endl;
//    out << "<DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\">" << std::endl;
//    out << _offsets << std::endl;
//    out << "</DataArray>" << std::endl;
//    out << "<DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\">" << std::endl;
//    out << _cellTypeDescription << std::endl;
//    out << "</DataArray>" << std::endl;
//    out << "</Cells>" << std::endl;
//
//    //TODO: atm every PointData is of format float32. Could be adjusted from case to case
//    //Data and xml-Syntax is written in _VertexDataWriter
//    if (_numberOfVertices>0 && !_vertexDataDescription.empty()) {
//      out << "<PointData>";
//      out << _vertexDataDescription ;
//      out << "</PointData>" << std::endl;
//    }
//
//    //TODO: atm every cellData is of format float32. Could be adjusted from case to case
//    //Data and xml-Syntax is written in _CellDataWriter
//    if (_numberOfCells>0 && !_cellDataDescription.empty() ) {
//      out << "<CellData>" ;
//      out << _cellDataDescription << std::endl;
//      out << "</CellData>" << std::endl;
//    }
//
//    out << "</Piece>" << std::endl;
//    out << "</UnstructuredGrid>" << std::endl;
//    out << "</VTKFile>" << std::endl;
//
//    _log.debug( "close()", "data written to " + filename );
//  }
//  else {
//    _log.error( "close()", "unable to write output file " + filename );
//  }
//
//  _writtenToFile = true;
//}
//
//
//bool tarch::plotter::griddata::unstructured::vtu::VTUTextFileWriter::isOpen() {
//  return !_writtenToFile;
//}
//
//
//tarch::plotter::griddata::unstructured::UnstructuredGridWriter::VertexWriter*
//tarch::plotter::griddata::unstructured::vtu::VTUTextFileWriter::createVertexWriter() {
//  return new tarch::plotter::griddata::unstructured::vtu::VTUTextFileWriter::VertexWriter(*this);
//}
//
//
//tarch::plotter::griddata::unstructured::UnstructuredGridWriter::CellWriter*
//tarch::plotter::griddata::unstructured::vtu::VTUTextFileWriter::createCellWriter() {
//  return new tarch::plotter::griddata::unstructured::vtu::VTUTextFileWriter::CellWriter(*this);
//}
//
//
//void tarch::plotter::griddata::unstructured::vtu::VTUTextFileWriter::validateDataWriterIdentifier( const std::string& identifier ) const {
//  if (identifier.empty()) {
//    logWarning(
//        "validateDataWriterIdentifier(string)",
//        "identifier for vtu file is empty. Spaces are not allowed for vtu data field identifiers and some vtu visualisers might crash."
//    );
//  }
//  if (identifier.find(' ')!=std::string::npos) {
//    logWarning(
//        "validateDataWriterIdentifier(string)",
//        "identifier \"" << identifier << "\" contains spaces. Spaces are not allowed for vtu data field identifiers and some vtu visualisers might crash."
//    );
//  }
//}
//
//
//tarch::plotter::griddata::Writer::CellDataWriter*    tarch::plotter::griddata::unstructured::vtu::VTUTextFileWriter::createCellDataWriter( const std::string& identifier, int recordsPerCell ) {
//  validateDataWriterIdentifier(identifier);
//  return new tarch::plotter::griddata::unstructured::vtu::VTUTextFileWriter::CellDataWriter(identifier,*this, recordsPerCell);
//}
//
//
//tarch::plotter::griddata::Writer::VertexDataWriter*  tarch::plotter::griddata::unstructured::vtu::VTUTextFileWriter::createVertexDataWriter( const std::string& identifier, int recordsPerVertex ) {
//  validateDataWriterIdentifier(identifier);
//  return new tarch::plotter::griddata::unstructured::vtu::VTUTextFileWriter::VertexDataWriter(identifier,*this, recordsPerVertex);
//}
