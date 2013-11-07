//#include "tarch/plotter/griddata/unstructured/vtu/tests/VTUWriterTest.h"
//
//#include "tarch/plotter/griddata/unstructured/vtu/VTUTextFileWriter.h"
//
//#include "peano/utils/Globals.h"
//
//#include "tarch/Assertions.h"
//
//
//
//#include "tarch/tests/TestCaseFactory.h"
//registerIntegrationTest(tarch::plotter::griddata::unstructured::vtu::tests::VTUWriterTest)
//
//
//tarch::plotter::griddata::unstructured::vtu::tests::VTUWriterTest::VTUWriterTest():
//  TestCase( "tarch::plotter::griddata::unstructured::vtu::tests::VTUWriterTest" ) {
//}
//
//
//tarch::plotter::griddata::unstructured::vtu::tests::VTUWriterTest::~VTUWriterTest() {
//}
//
//
//void tarch::plotter::griddata::unstructured::vtu::tests::VTUWriterTest::run() {
//  testMethod( testProblemEmptyFile );
//  testMethod( test2DProblemVertices );
//  testMethod( test2DProblemCells );
//  testMethod( test2DProblemOneScalarDataRecord );
//  testMethod( test2DProblemTwoScalarDataRecords );
//  testMethod( test3DProblemVertices );
//  testMethod( test3DProblemCells );
//}
//
//void tarch::plotter::griddata::unstructured::vtu::tests::VTUWriterTest::setUp() {
//
//}
//
//
//void tarch::plotter::griddata::unstructured::vtu::tests::VTUWriterTest::create2DVertices(tarch::plotter::griddata::unstructured::UnstructuredGridWriter& writer) {
//  tarch::plotter::griddata::unstructured::UnstructuredGridWriter::VertexWriter* vertexWriter = writer.createVertexWriter();
//
//  tarch::la::Vector<2,double> position;
//  int number;
//
//  assignList(position) = 0.1, 0.2;
//  number = vertexWriter->plotVertex(position);
//  validateEquals( number, 0 );
//  assignList(position) = 1.3, 0.2;
//  number = vertexWriter->plotVertex(position);
//  validateEquals( number, 1 );
//  assignList(position) = 0.1, 1.7;
//  number = vertexWriter->plotVertex(position);
//  validateEquals( number, 2 );
//  assignList(position) = 1.3, 1.7;
//  number = vertexWriter->plotVertex(position);
//  validateEquals( number, 3 );
//
//  vertexWriter->close();
//  delete vertexWriter;
//}
//
//
//void tarch::plotter::griddata::unstructured::vtu::tests::VTUWriterTest::create2DCells(tarch::plotter::griddata::unstructured::UnstructuredGridWriter& writer) {
//  tarch::plotter::griddata::unstructured::UnstructuredGridWriter::CellWriter* elementWriter = writer.createCellWriter();
//
//  int vertexIndex[4];
//
//  int number;
//
//  vertexIndex[0] = 0;
//  vertexIndex[1] = 1;
//  vertexIndex[2] = 2;
//  vertexIndex[3] = 3;
//
//  number = elementWriter->plotQuadrangle(vertexIndex);
//  validateEquals( number, 0 );
//
//  elementWriter->close();
//  delete elementWriter;
//}
//
//
//void tarch::plotter::griddata::unstructured::vtu::tests::VTUWriterTest::create2DScalarDataRecord(tarch::plotter::griddata::unstructured::UnstructuredGridWriter& writer, const std::string& identifier) {
//  tarch::plotter::griddata::unstructured::UnstructuredGridWriter::VertexDataWriter* dataWriter = writer.createVertexDataWriter( identifier, 1 );
//
//  dataWriter->plotVertex(0,-0.001);
//  dataWriter->plotVertex(1,-0.002);
//  dataWriter->plotVertex(2,-0.003);
//  dataWriter->plotVertex(3,-0.004);
//
//  dataWriter->close();
//  delete dataWriter;
//}
//
//
//
//void tarch::plotter::griddata::unstructured::vtu::tests::VTUWriterTest::testProblemEmptyFile() {
//  tarch::plotter::griddata::unstructured::vtu::VTUTextFileWriter  myWriter;
//  myWriter.writeToFile(_outputDirectory +  "TestEmptyProblem-text.vtu");
//}
//
//
//void tarch::plotter::griddata::unstructured::vtu::tests::VTUWriterTest::test2DProblemVertices() {
//  tarch::plotter::griddata::unstructured::vtu::VTUTextFileWriter  writer;
//
//  create2DVertices(writer);
//
//  writer.writeToFile( _outputDirectory + "Test2DProblemVertices-text.vtu");
//}
//
//
//void tarch::plotter::griddata::unstructured::vtu::tests::VTUWriterTest::test2DProblemCells() {
//  tarch::plotter::griddata::unstructured::vtu::VTUTextFileWriter  writer;
//
//  create2DVertices(writer);
//  create2DCells(writer);
//
//  writer.writeToFile(_outputDirectory + "Test2DProblemCells-text.vtu");
//}
//
//
//void tarch::plotter::griddata::unstructured::vtu::tests::VTUWriterTest::test2DProblemOneScalarDataRecord() {
//  tarch::plotter::griddata::unstructured::vtu::VTUTextFileWriter  writer;
//
//  create2DVertices(writer);
//  create2DCells(writer);
//  create2DScalarDataRecord(writer, "firstscalardata");
//
//  writer.writeToFile(_outputDirectory + "Test2DProblemOneScalarDataRecord-text.vtu");
//}
//
//
//void tarch::plotter::griddata::unstructured::vtu::tests::VTUWriterTest::test2DProblemTwoScalarDataRecords() {
//  tarch::plotter::griddata::unstructured::vtu::VTUTextFileWriter  writer;
//
//  create2DVertices(writer);
//  create2DCells(writer);
//  create2DScalarDataRecord(writer, "firstscalardata");
//  create2DScalarDataRecord(writer, "secondscalardata");
//
//  writer.writeToFile(_outputDirectory + "Test2DProblemTwoScalarDataRecords-text.vtu");
//}
//
//
//void tarch::plotter::griddata::unstructured::vtu::tests::VTUWriterTest::create3DVertices(tarch::plotter::griddata::unstructured::UnstructuredGridWriter& writer) {
//  tarch::plotter::griddata::unstructured::UnstructuredGridWriter::VertexWriter* vertexWriter = writer.createVertexWriter();
//
//  tarch::la::Vector<3, double> p0,p1,p2,p3,p4,p5,p6,p7;
//
//  assignList(p0) = 0.1, 0.2, 0.3;
//  assignList(p1) = 1.1, 0.2, 0.3;
//  assignList(p2) = 0.1, 2.2, 0.3;
//  assignList(p3) = 1.1, 2.2, 0.3;
//  assignList(p4) = 0.1, 0.2, 3.3;
//  assignList(p5) = 1.1, 0.2, 3.3;
//  assignList(p6) = 0.1, 2.2, 3.3;
//  assignList(p7) = 1.1, 2.2, 3.3;
//
//  int number;
//
//  number = vertexWriter->plotVertex(p0);
//  validateEquals( number, 0 );
//  number = vertexWriter->plotVertex(p1);
//  validateEquals( number, 1 );
//  number = vertexWriter->plotVertex(p2);
//  validateEquals( number, 2 );
//  number = vertexWriter->plotVertex(p3);
//  validateEquals( number, 3 );
//  number = vertexWriter->plotVertex(p4);
//  validateEquals( number, 4 );
//  number = vertexWriter->plotVertex(p5);
//  validateEquals( number, 5 );
//  number = vertexWriter->plotVertex(p6);
//  validateEquals( number, 6 );
//  number = vertexWriter->plotVertex(p7);
//  validateEquals( number, 7 );
//
//  vertexWriter->close();
//  delete vertexWriter;
//}
//
//
//void tarch::plotter::griddata::unstructured::vtu::tests::VTUWriterTest::test3DProblemVertices() {
//  tarch::plotter::griddata::unstructured::vtu::VTUTextFileWriter  writer;
//  create3DVertices(writer);
//
//  writer.writeToFile(_outputDirectory + "Test3DProblemVertices-text.vtu" );
//}
//
//
//
//void tarch::plotter::griddata::unstructured::vtu::tests::VTUWriterTest::test3DProblemCells() {
//  tarch::plotter::griddata::unstructured::vtu::VTUTextFileWriter  writer;
//  create3DVertices(writer);
//
//  tarch::plotter::griddata::unstructured::UnstructuredGridWriter::CellWriter* elementWriter = writer.createCellWriter();
//
//  int vertexIndex[8] = {4,5,0,1,6,7,2,3};
//
//  elementWriter->plotHexahedron(vertexIndex);
//
//  elementWriter->close();
//  delete elementWriter;
//
//  writer.writeToFile(_outputDirectory + "Test3DProblemCells-text.vtu");
//}
//
