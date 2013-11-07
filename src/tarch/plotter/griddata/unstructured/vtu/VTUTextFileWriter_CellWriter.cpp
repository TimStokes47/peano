//#include "tarch/plotter/griddata/unstructured/vtu/VTUTextFileWriter.h"
//
//
//tarch::plotter::griddata::unstructured::vtu::VTUTextFileWriter::CellWriter::CellWriter(VTUTextFileWriter& writer):
//  _currentCellNumber(0),
//  _myWriter(writer),
//  _cellListEntries(0),
//  _cellTypeOut(),
//  _cellConnectivityOut(),
//  _cellOffsetsOut(),
//  _offset(0){
//  assertion( _myWriter._numberOfCells==0 );
//  assertion( _myWriter._numberOfCellEntries==0 );
//}
//
//
//tarch::plotter::griddata::unstructured::vtu::VTUTextFileWriter::CellWriter::~CellWriter() {
//  if (_currentCellNumber>=0) {
//    close();
//  }
//}
//
//
//int tarch::plotter::griddata::unstructured::vtu::VTUTextFileWriter::CellWriter::plotPoint(int vertexIndex) {
//  assertion( _currentCellNumber>=0 );
//  assertion( _cellListEntries>=0 );
//
//  _currentCellNumber++;
//  _cellListEntries += 2;
//
//
//  _offset += 1;
//  _cellOffsetsOut << _offset << " ";
//  _cellConnectivityOut << vertexIndex << " ";
//  _cellTypeOut << "1" << std::endl;
//
//  return _currentCellNumber-1;
//}
//
//
//int tarch::plotter::griddata::unstructured::vtu::VTUTextFileWriter::CellWriter::plotHexahedron(int vertexIndex[8]) {
//  assertion( _currentCellNumber>=0 );
//  assertion( _cellListEntries>=0 );
//
//  _currentCellNumber++;
//  _cellListEntries += 9;
//
//
//  _offset += 8;
//  _cellOffsetsOut << _offset << " ";
//  _cellConnectivityOut << vertexIndex[0] << " "
//          << vertexIndex[1] << " "
//          << vertexIndex[2] << " "
//          << vertexIndex[3] << " "
//          << vertexIndex[4] << " "
//          << vertexIndex[5] << " "
//          << vertexIndex[6] << " "
//          << vertexIndex[7] << " ";
//  _cellTypeOut << "11" << std::endl;
//
//  return _currentCellNumber-1;
//}
//
//
//int tarch::plotter::griddata::unstructured::vtu::VTUTextFileWriter::CellWriter::plotQuadrangle(int vertexIndex[4]) {
//  assertion( _currentCellNumber>=0 );
//  assertion( _cellListEntries>=0 );
//
//  _currentCellNumber++;
//  _cellListEntries += 5;
//
//
//  _offset += 4;
//  _cellOffsetsOut << _offset << " ";
//  _cellConnectivityOut << vertexIndex[0] << " "
//          << vertexIndex[1] << " "
//          << vertexIndex[2] << " "
//          << vertexIndex[3] << " ";
//  _cellTypeOut << "8" << std::endl;
//
//  return _currentCellNumber-1;
//}
//
//
//int tarch::plotter::griddata::unstructured::vtu::VTUTextFileWriter::CellWriter::plotLine(int vertexIndex[2]) {
//  assertion( _currentCellNumber>=0 );
//  assertion( _cellListEntries>=0 );
//
//  _currentCellNumber++;
//  _cellListEntries += 3;
//
//  _offset += 2;
//  _cellOffsetsOut << _offset << " ";
//  _cellConnectivityOut << vertexIndex[0] << " "
//          << vertexIndex[1] << " ";
//  _cellTypeOut << "3" << std::endl;
//
//  return _currentCellNumber-1;
//}
//
//
//int tarch::plotter::griddata::unstructured::vtu::VTUTextFileWriter::CellWriter::plotTriangle(int vertexIndex[3]) {
//  assertion( _currentCellNumber>=0 );
//  assertion( _cellListEntries>=0 );
//
//  _currentCellNumber++;
//  _cellListEntries += 4;
//
//  _offset += 3;
//  _cellOffsetsOut << _offset << " ";
//  _cellConnectivityOut << vertexIndex[0] << " "
//          << vertexIndex[1] << " "
//          << vertexIndex[2] << " ";
//  _cellTypeOut << "5" << std::endl;
//
//  return _currentCellNumber-1;
//}
//
//
//void tarch::plotter::griddata::unstructured::vtu::VTUTextFileWriter::CellWriter::close() {
//  assertion( _myWriter._numberOfCells==0 );
//  assertion( _myWriter._numberOfCellEntries==0 );
//  assertionMsg( _myWriter.isOpen(), "Maybe you forgot to call close() on a data writer before you destroy your writer?" );
//
//  _myWriter._numberOfCells       = _currentCellNumber;
//  _myWriter._numberOfCellEntries = _cellListEntries;
//
//  _myWriter._cellTypeDescription  = _cellTypeOut.str();
//  _myWriter._offsets = _cellOffsetsOut.str();
//  _myWriter._connectivity = _cellConnectivityOut.str();
//
//  _currentCellNumber = -1;
//  _cellListEntries   = -1;
//
//
//}
