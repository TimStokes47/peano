AUTOMAKE_OPTIONS = subdir-objects
#
# By default, automake does dfs. We need the libs to be built first however.
# So we add the . explicitly.
#
SUBDIRS = . examples/grid


lib_LIBRARIES =  libTarch.a libTarch_debug.a libPeano4Core2d.a libPeano4Core2d_debug.a libPeano4Core3d.a libPeano4Core3d_debug.a
#lib_LTLIBRARIES =  libTarch.a libTarch_debug.a libPeano4Core2d.a libPeano4Core2d_debug.a libPeano4Core3d.a libPeano4Core3d_debug.a


libTarch_a_CXXFLAGS = 
#libTarch_a_LDFLAGS =  

libTarch_debug_a_CXXFLAGS = -DPeanoDebug=1 -g3 
#libTarch_debug_a_LDFLAGS =  -g3

libPeano4Core2d_a_CXXFLAGS = -DDimensions=2
#libPeano4Core2d_a_LDFLAGS =  

libPeano4Core2d_debug_a_CXXFLAGS = -DDimensions=2 -DPeanoDebug=1 -g3
#libPeano4Core2d_debug_a_LDFLAGS =   -g3

libPeano4Core3d_a_CXXFLAGS = -DDimensions=3
#libPeano4Core3d_a_LDFLAGS = 

libPeano4Core3d_debug_a_CXXFLAGS = -DDimensions=3 -DPeanoDebug=1 -g3
#libPeano4Core3d_debug_a_LDFLAGS =  -g3




Peano4Core_SOURCES = peano4/peano.cpp \
  peano4/grid/GridVertex.cpp \
  peano4/grid/AutomatonState.cpp \
  peano4/grid/Spacetree.cpp \
  peano4/grid/PeanoCurve.cpp \
  peano4/grid/EmptyTraversalObserver.cpp \
  peano4/grid/GridControlEvent.cpp \
  peano4/grid/TraversalVTKPlotter.cpp \
  peano4/grid/GridStatistics.cpp \
  peano4/grid/tests/SpacetreeTest.cpp \
  peano4/parallel/Node.cpp \
  peano4/parallel/Tasks.cpp \
  peano4/parallel/TreeEntry.cpp \
  peano4/parallel/SpacetreeSet.cpp \
  peano4/utils/Loop.cpp



libPeano4Core2d_a_SOURCES = $(Peano4Core_SOURCES)
libPeano4Core2d_debug_a_SOURCES = $(Peano4Core_SOURCES)

libPeano4Core3d_a_SOURCES = $(Peano4Core_SOURCES)
libPeano4Core3d_debug_a_SOURCES = $(Peano4Core_SOURCES)


libTarch_a_SOURCES= tarch/la/tests/MatrixVectorTest.cpp \
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


libTarch_debug_a_SOURCES= $(libTarch_la_SOURCES)


