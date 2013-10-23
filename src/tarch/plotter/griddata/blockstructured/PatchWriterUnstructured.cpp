#include "tarch/plotter/griddata/blockstructured/PatchWriterUnstructured.h"


tarch::plotter::griddata::blockstructured::PatchWriterUnstructured::PatchWriterUnstructured(tarch::plotter::griddata::unstructured::UnstructuredGridWriter*  writer):
  _writer(writer),
  _vertexWriter(0),
  _cellWriter(0) {
  _vertexWriter = _writer->createVertexWriter();
  _cellWriter   = _writer->createCellWriter();
}


tarch::plotter::griddata::blockstructured::PatchWriterUnstructured::~PatchWriterUnstructured() {
  if (_vertexWriter!=0) {
    _vertexWriter->close();
    delete _vertexWriter;
    _vertexWriter = 0;
  }

  if (_cellWriter!=0) {
    _cellWriter->close();
    delete _cellWriter;
    _cellWriter   = 0;
  }

  delete _writer;
}


void tarch::plotter::griddata::blockstructured::PatchWriterUnstructured::writeToFile( const std::string& filename ) {
  delete _vertexWriter;
  delete _cellWriter;

  _vertexWriter = 0;
  _cellWriter   = 0;

  _writer->writeToFile( filename );
}


bool tarch::plotter::griddata::blockstructured::PatchWriterUnstructured::isOpen() {
  return _writer->isOpen();
}


void tarch::plotter::griddata::blockstructured::PatchWriterUnstructured::clear() {
  _writer->clear();
}


tarch::plotter::griddata::blockstructured::PatchWriter::SinglePatchWriter*   tarch::plotter::griddata::blockstructured::PatchWriterUnstructured::createSinglePatchWriter() {
  return new tarch::plotter::griddata::blockstructured::PatchWriterUnstructured::SinglePatchWriter(
    *_vertexWriter,
    *_cellWriter
  );
}


tarch::plotter::griddata::Writer::CellDataWriter*    tarch::plotter::griddata::blockstructured::PatchWriterUnstructured::createCellDataWriter( const std::string& identifier, int recordsPerCell ) {
  return _writer->createCellDataWriter(identifier,recordsPerCell);
}


tarch::plotter::griddata::Writer::VertexDataWriter*  tarch::plotter::griddata::blockstructured::PatchWriterUnstructured::createVertexDataWriter( const std::string& identifier, int recordsPerVertex ) {
  return _writer->createVertexDataWriter(identifier,recordsPerVertex);
}


tarch::plotter::griddata::blockstructured::PatchWriterUnstructured::SinglePatchWriter::SinglePatchWriter(
  tarch::plotter::griddata::unstructured::UnstructuredGridWriter::VertexWriter&  vertexWriter,
  tarch::plotter::griddata::unstructured::UnstructuredGridWriter::CellWriter&    cellWriter
):
  _vertexWriter(vertexWriter),
  _cellWriter(cellWriter) {
}


tarch::plotter::griddata::blockstructured::PatchWriterUnstructured::SinglePatchWriter::~SinglePatchWriter() {
}


std::pair<int,int> tarch::plotter::griddata::blockstructured::PatchWriterUnstructured::SinglePatchWriter::plotPatch(
  const tarch::la::Vector<2,double>& offset,
  const tarch::la::Vector<2,double>& size,
  const tarch::la::Vector<2,int>&    cells
) {
  int firstVertex = -1;

  for (int y=0; y<cells(1)+1; y++)
  for (int x=0; x<cells(0)+1; x++) {
    tarch::la::Vector<2, double> p;

    p(0) = offset(0) + x * size(0) / cells(0);
    p(1) = offset(1) + y * size(1) / cells(1);

    const int newVertexNumber = _vertexWriter.plotVertex(p);
    firstVertex = firstVertex==-1 ? newVertexNumber : firstVertex;
  }

  int firstCell = -1;

  for (int y=0; y<cells(1); y++)
  for (int x=0; x<cells(0); x++) {
    int cellsVertexIndices[4];
    cellsVertexIndices[0] = firstVertex + (x+0) + (y+0) * (cells(0)+1);
    cellsVertexIndices[1] = firstVertex + (x+1) + (y+0) * (cells(0)+1);
    cellsVertexIndices[2] = firstVertex + (x+0) + (y+1) * (cells(0)+1);
    cellsVertexIndices[3] = firstVertex + (x+1) + (y+1) * (cells(0)+1);
    const int newCellNumber = _cellWriter.plotQuadrangle(cellsVertexIndices);
    firstCell = firstCell==-1 ? newCellNumber : firstCell;
  }

  return std::pair<int,int>(firstVertex,firstCell);
}


std::pair<int,int> tarch::plotter::griddata::blockstructured::PatchWriterUnstructured::SinglePatchWriter::plotPatch(
  const tarch::la::Vector<3,double>& offset,
  const tarch::la::Vector<3,double>& size,
  const tarch::la::Vector<3,int>&    cells
) {
  int firstVertex = -1;

  for (int z=0; z<cells(2)+1; z++)
  for (int y=0; y<cells(1)+1; y++)
  for (int x=0; x<cells(0)+1; x++) {
    tarch::la::Vector<3, double> p;

    p(0) = offset(0) + x * size(0) / cells(0);
    p(1) = offset(1) + y * size(1) / cells(1);
    p(2) = offset(2) + z * size(2) / cells(2);

    const int newVertexNumber = _vertexWriter.plotVertex(p);
    firstVertex = firstVertex==-1 ? newVertexNumber : firstVertex;
  }

  int firstCell = -1;

  for (int z=0; z<cells(2); z++)
  for (int y=0; y<cells(1); y++)
  for (int x=0; x<cells(0); x++) {
    int cellsVertexIndices[8];
    cellsVertexIndices[0] = firstVertex + (x+0) + (y+0) * (cells(0)+1) + (z+0) * (cells(0)+1) * (cells(1)+1);
    cellsVertexIndices[1] = firstVertex + (x+1) + (y+0) * (cells(0)+1) + (z+0) * (cells(0)+1) * (cells(1)+1);
    cellsVertexIndices[2] = firstVertex + (x+0) + (y+1) * (cells(0)+1) + (z+0) * (cells(0)+1) * (cells(1)+1);
    cellsVertexIndices[3] = firstVertex + (x+1) + (y+1) * (cells(0)+1) + (z+0) * (cells(0)+1) * (cells(1)+1);
    cellsVertexIndices[4] = firstVertex + (x+0) + (y+0) * (cells(0)+1) + (z+1) * (cells(0)+1) * (cells(1)+1);
    cellsVertexIndices[5] = firstVertex + (x+1) + (y+0) * (cells(0)+1) + (z+1) * (cells(0)+1) * (cells(1)+1);
    cellsVertexIndices[6] = firstVertex + (x+0) + (y+1) * (cells(0)+1) + (z+1) * (cells(0)+1) * (cells(1)+1);
    cellsVertexIndices[7] = firstVertex + (x+1) + (y+1) * (cells(0)+1) + (z+1) * (cells(0)+1) * (cells(1)+1);
    const int newCellNumber = _cellWriter.plotHexahedron(cellsVertexIndices);
    firstCell = firstCell==-1 ? newCellNumber : firstCell;
  }

  return std::pair<int,int>(firstVertex,firstCell);
}


void tarch::plotter::griddata::blockstructured::PatchWriterUnstructured::SinglePatchWriter::close() {
  _vertexWriter.close();
  _cellWriter.close();
};



//tarch::logging::Log peanoclaw::PatchPlotter::_log( "peanoclaw::PatchPlotter" );
//
//peanoclaw::PatchPlotter::PatchPlotter(
//  tarch::plotter::plotter::griddata::unstructured::vtk::VTKTextFileWriter& vtkWriter,
//  int unknownsPerSubcell,
//  int auxFieldsPerSubcell
//) : _vtkWriter(vtkWriter), _gap(0.01) {
//
//  _vertex2IndexMap.clear();
//  _vertexWriter     = _vtkWriter.createVertexWriter();
//  _cellWriter       = _vtkWriter.createCellWriter();
//
//  _cellSubdivisionFactorWriter = _vtkWriter.createCellDataWriter("SubdivisionFactor",1);
//  _cellGhostLayerWidthWriter   = _vtkWriter.createCellDataWriter("GhostlayerWidth",1);
//  for(int i = 0; i < unknownsPerSubcell; i++) {
//    std::stringstream stringstream;
//    stringstream << "q" << i;
//    _cellQWriter.push_back( _vtkWriter.createCellDataWriter(stringstream.str(), 1) );
//  }
//  for(int i = 0; i < auxFieldsPerSubcell; i++) {
//    std::stringstream stringstream;
//    stringstream << "aux" << i;
//    _cellAuxWriter.push_back( _vtkWriter.createCellDataWriter(stringstream.str(), 1) );
//  }
//  _cellTimeOldWriter           = _vtkWriter.createCellDataWriter("timeOld", 1);
//  _cellTimeNewWriter           = _vtkWriter.createCellDataWriter("timeNew", 1);
//  _cellDemandedMeshWidthWriter = _vtkWriter.createCellDataWriter("demandedMeshWidth", 1);
//  _cellAgeWriter               = _vtkWriter.createCellDataWriter("age", 1);
//}
