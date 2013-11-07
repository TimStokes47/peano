//#ifndef _PLOTTER_VTUWRITERTEST_H_
//#define _PLOTTER_VTUWRITERTEST_H_
//
//
//#include "tarch/tests/TestCase.h"
//
//#include "tarch/plotter/griddata/unstructured/UnstructuredGridWriter.h"
//
//#include <string>
//
//
//namespace tarch {
//  namespace plotter {
//    namespace griddata {
//      namespace unstructured {
//        namespace vtu {
//          namespace tests {
//            class VTUWriterTest;
//          }
//        }
//      }
//    }
//  }
//}
//
//
//class tarch::plotter::griddata::unstructured::vtu::tests::VTUWriterTest: public tarch::tests::TestCase {
//  private:
//    void create2DVertices(tarch::plotter::griddata::unstructured::UnstructuredGridWriter& writer);
//    void create2DCells(tarch::plotter::griddata::unstructured::UnstructuredGridWriter& writer);
//    void create2DScalarDataRecord(tarch::plotter::griddata::unstructured::UnstructuredGridWriter& writer, const std::string& identifier);
//
//    void create3DVertices(tarch::plotter::griddata::unstructured::UnstructuredGridWriter& writer);
//
//    void testProblemEmptyFile();
//    void test2DProblemVertices();
//    void test2DProblemCells();
//    void test2DProblemOneScalarDataRecord();
//    void test2DProblemTwoScalarDataRecords();
//    void test3DProblemVertices();
//    void test3DProblemCells();
//  public:
//	VTUWriterTest();
//	virtual ~VTUWriterTest();
//	virtual void setUp();
//	virtual void run();
//};
//
//#endif
