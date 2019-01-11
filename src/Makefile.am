AUTOMAKE_OPTIONS = subdir-objects

lib_LTLIBRARIES =  libTarch.la libTarch_debug.la libPeano4Core.la libPeano4Core_debug.la

#libPeano_la_LDFLAGS = -static
#libPeano_la_CXXFLAGS = -static

libTarch_la_LDFLAGS = 
libTarch_la_CXXFLAGS = 

libTarch_debug_la_LDFLAGS = 
libTarch_debug_la_CXXFLAGS = -DPeanoDebug=1 

libPeano4Core_la_LDFLAGS = 
libPeano4Core_la_CXXFLAGS = 

libPeano4Core_debug_la_LDFLAGS = 
libPeano4Core_debug_la_CXXFLAGS = -DPeanoDebug=1 




libPeano4Core_la_SOURCES= OBJS += peano.cpp \
  grid/GridVertex.cpp \
  grid/AutomatonState.cpp \
  grid/Spacetree.cpp \
  grid/PeanoCurve.cpp \
  grid/EmptyTraversalObserver.cpp \
  grid/GridControlEvent.cpp \
  grid/TraversalVTKPlotter.cpp \
  grid/GridStatistics.cpp \
  grid/tests/SpacetreeTest.cpp \
  parallel/Node.cpp \
  parallel/Tasks.cpp \
  parallel/TreeEntry.cpp \
  parallel/SpacetreeSet.cpp \
  utils/Loop.cpp



libPeano4Core_debug_la_SOURCES= $(libPeano4Core_la_SOURCES)


libTarch_la_SOURCES= tarch/la/tests/MatrixVectorTest.cpp \
 tarch/la/tests/MatrixTest.cpp \
 tarch/la/tests/ScalarTest.cpp \
 tarch/la/tests/GramSchmidtTest.cpp \
 tarch/la/tests/LUDecompositionTest.cpp \
 tarch/la/tests/VectorTest.cpp \
 tarch/la/ScalarOperations.cpp \
 tarch/la/Scalar.cpp \
 tarch/multicore/RecursiveLock.cpp \
 tarch/multicore/Core.cpp \
 tarch/multicore/BooleanSemaphore.cpp \
 tarch/multicore/RecursiveSemaphore.cpp \
 tarch/multicore/Lock.cpp \
 tarch/multicore/Tasks.cpp \
 tarch/multicore/cpp/Core.cpp \
 tarch/multicore/cpp/Tasks.cpp \
 tarch/multicore/cpp/BooleanSemaphore.cpp \
 tarch/multicore/cpp/RecursiveSemaphore.cpp \
 tarch/multicore/tbb/Core.cpp \
 tarch/multicore/tbb/Tasks.cpp \
 tarch/multicore/tbb/BooleanSemaphore.cpp \
 tarch/multicore/tbb/RecursiveSemaphore.cpp \
 tarch/tests/TestCaseCollection.cpp \
 tarch/tests/TreeTestCaseCollection.cpp \
 tarch/tests/TestCase.cpp \
 tarch/tests/TestCaseRegistry.cpp \
 tarch/plotter/griddata/unstructured/vtk/VTUTextFileWriter_CellWriter.cpp \
 tarch/plotter/griddata/unstructured/vtk/VTKTextFileWriter_VertexDataWriter.cpp \
 tarch/plotter/griddata/unstructured/vtk/VTKTextFileWriter_VertexWriter.cpp \
 tarch/plotter/griddata/unstructured/vtk/tests/VTKBinaryFileTestCase.cpp \
 tarch/plotter/griddata/unstructured/vtk/VTKBinaryFileWriter_CellWriter.cpp \
 tarch/plotter/griddata/unstructured/vtk/VTUTextFileWriter_CellDataWriter.cpp \
 tarch/plotter/griddata/unstructured/vtk/VTKTextFileWriter_CellWriter.cpp \
 tarch/plotter/griddata/unstructured/vtk/VTUTextFileWriter_VertexDataWriter.cpp \
 tarch/plotter/griddata/unstructured/vtk/VTKBinaryFileWriter.cpp \
 tarch/plotter/griddata/unstructured/vtk/VTUTextFileWriter.cpp \
 tarch/plotter/griddata/unstructured/vtk/VTKBinaryFileWriter_VertexWriter.cpp \
 tarch/plotter/griddata/unstructured/vtk/VTKBinaryFileWriter_CellDataWriter.cpp \
 tarch/plotter/griddata/unstructured/vtk/VTKTextFileWriter_CellDataWriter.cpp \
 tarch/plotter/griddata/unstructured/vtk/VTKBinaryFileWriter_VertexDataWriter.cpp \
 tarch/plotter/griddata/unstructured/vtk/VTUTextFileWriter_VertexWriter.cpp \
 tarch/plotter/griddata/unstructured/vtk/VTKTextFileWriter.cpp \
 tarch/plotter/griddata/regular/CartesianGridArrayWriter.cpp \
 tarch/plotter/griddata/regular/vtk/VTKTextFileWriter.cpp \
 tarch/plotter/griddata/regular/CartesianGridArrayWriter_CellDataWriter.cpp \
 tarch/plotter/griddata/regular/CartesianGridArrayWriter_VertexDataWriter.cpp \
 tarch/plotter/griddata/VTUTimeSeriesWriter.cpp \
 tarch/plotter/griddata/blockstructured/PeanoTextPatchFileWriter.cpp \
 tarch/plotter/griddata/blockstructured/PatchWriter.cpp \
 tarch/plotter/griddata/blockstructured/PeanoTextPatchFileWriter_VertexDataWriter.cpp \
 tarch/plotter/griddata/blockstructured/PeanoHDF5PatchFileWriter.cpp \
 tarch/plotter/griddata/blockstructured/PeanoHDF5PatchFileWriter_CellDataWriter.cpp \
 tarch/plotter/griddata/blockstructured/PeanoPatchFileWriter.cpp \
 tarch/plotter/griddata/blockstructured/PeanoHDF5PatchFileWriter_VertexDataWriter.cpp \
 tarch/plotter/griddata/blockstructured/PeanoTextPatchFileWriter_CellDataWriter.cpp \
 tarch/plotter/griddata/blockstructured/PatchWriterUnstructured.cpp \
 tarch/plotter/pointdata/vtk/VTKTextFileWriter.cpp \
 tarch/plotter/pointdata/vtk/VTKBinaryFileWriter.cpp \
 tarch/plotter/pointdata/vtk/VTKBinaryFileWriter_PointDataWriter.cpp \
 tarch/parallel/messages/ActivationMessage.cpp \
 tarch/parallel/messages/JobRequestMessage.cpp \
 tarch/parallel/messages/RegisterAtNodePoolMessage.cpp \
 tarch/parallel/messages/WorkerRequestMessage.cpp \
 tarch/parallel/messages/NodePoolAnswerMessage.cpp \
 tarch/parallel/Node.cpp \
 tarch/parallel/FCFSNodePoolStrategy.cpp \
 tarch/parallel/MPIConstants.cpp \
 tarch/parallel/NodePool.cpp \
 tarch/logging/CommandLineLogger.cpp \
 tarch/logging/Log.cpp \
 tarch/logging/LogFilterFileReader.cpp \
 tarch/timing/Watch.cpp \
 tarch/timing/Measurement.cpp \
 tarch/timing/GlidingAverageMeasurement.cpp \
 tarch/services/Service.cpp \
 tarch/services/ServiceRepository.cpp 


libTarch_debug_la_SOURCES= $(libTarch_la_SOURCES)


