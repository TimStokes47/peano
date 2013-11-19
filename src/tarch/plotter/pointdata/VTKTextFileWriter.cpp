#include "tarch/plotter/pointdata/VTKTextFileWriter.h"



tarch::plotter::pointdata::VTKTextFileWriter::VTKTextFileWriter():
  _vtkWriter(),
  _vertexWriter(_vtkWriter.createVertexWriter()),
  _cellWriter( _vtkWriter.createCellWriter()) {
}



tarch::plotter::pointdata::VTKTextFileWriter::~VTKTextFileWriter() {
  assertionMsg( _vertexWriter==0, "no close() called" );
  assertionMsg( _cellWriter==0, "no close() called" );
}


tarch::plotter::pointdata::Writer::PointDataWriter*    tarch::plotter::pointdata::VTKTextFileWriter::createPointDataWriter( const std::string& identifier, int recordsPerPoint ) {
  return new PointDataWriter(*this,identifier,recordsPerPoint);
}


void tarch::plotter::pointdata::VTKTextFileWriter::writeToFile( const std::string& filename ) {
  _vtkWriter.writeToFile(filename);
}


bool tarch::plotter::pointdata::VTKTextFileWriter::isOpen() {
  return _vtkWriter.isOpen();
}


void tarch::plotter::pointdata::VTKTextFileWriter::clear() {
  _vtkWriter.clear();
}


void tarch::plotter::pointdata::VTKTextFileWriter::close() {
  _vertexWriter->close();
  _cellWriter->close();

  delete _vertexWriter;
  delete _cellWriter;

  _vertexWriter            = 0;
  _cellWriter              = 0;
}


int tarch::plotter::pointdata::VTKTextFileWriter::plotPoint(const tarch::la::Vector<2,double>& position) {
  const int particleNumber = _vertexWriter->plotVertex(position);
  _cellWriter->plotPoint(particleNumber);
  return particleNumber;
}


int tarch::plotter::pointdata::VTKTextFileWriter::plotPoint(const tarch::la::Vector<3,double>& position) {
  const int particleNumber = _vertexWriter->plotVertex(position);
  _cellWriter->plotPoint(particleNumber);
  return particleNumber;
}


tarch::plotter::pointdata::VTKTextFileWriter::PointDataWriter::PointDataWriter(VTKTextFileWriter& myWriter, const std::string& identifier, int recordsPerPoint):
  _myWriter(myWriter),
  _particleDataWriter(_myWriter._vtkWriter.createVertexDataWriter(identifier,recordsPerPoint)) {
}


tarch::plotter::pointdata::VTKTextFileWriter::PointDataWriter::~PointDataWriter() {
  delete _particleDataWriter;
}


void tarch::plotter::pointdata::VTKTextFileWriter::PointDataWriter::plot( int index, double value ) {
  _particleDataWriter->plotVertex(index,value);
}


void tarch::plotter::pointdata::VTKTextFileWriter::PointDataWriter::plot( int index, const tarch::la::Vector<2,double>& value ) {
  _particleDataWriter->plotVertex(index,value);
}


void tarch::plotter::pointdata::VTKTextFileWriter::PointDataWriter::plot( int index, const tarch::la::Vector<3,double>& value ) {
  _particleDataWriter->plotVertex(index,value);
}


double tarch::plotter::pointdata::VTKTextFileWriter::PointDataWriter::getMinValue() const {
  return _particleDataWriter->getMinValue();
}


double tarch::plotter::pointdata::VTKTextFileWriter::PointDataWriter::getMaxValue() const {
  return _particleDataWriter->getMinValue();
}

void tarch::plotter::pointdata::VTKTextFileWriter::PointDataWriter::close() {
  _particleDataWriter->close();
}


void tarch::plotter::pointdata::VTKTextFileWriter::PointDataWriter::assignRemainingPointsDefaultValues() {
  _particleDataWriter->assignRemainingVerticesDefaultValues();
}
