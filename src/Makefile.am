AUTOMAKE_OPTIONS = subdir-objects
#
# By default, automake does dfs. We need the libs to be built first however.
# So we add the . explicitly.
#
SUBDIRS = . examples/grid examples/integerdiffusionthroughfaces examples/delta examples/regulargridupscaling visualisation


#
# I want Peano4 to be as fast as possible and thus build it as static library
# such that a linker/compiler can ipo. One of the other reasons for this is 
# that I have a couple of singletons and I did struggle with singletons and 
# shared libraries. 
#
lib_LIBRARIES = libTarch.a libTarch_debug.a libTarch_trace.a libPeano4Core2d.a libPeano4Core2d_debug.a libPeano4Core2d_trace.a libPeano4Core3d.a libPeano4Core3d_debug.a libPeano4Core3d_trace.a


libTarch_a_CXXFLAGS = -DPeanoDebug=0 $(PEANO_CXXFLAGS)
libTarch_trace_a_CXXFLAGS = -DPeanoDebug=1 -ggdb3 -O0 $(PEANO_CXXFLAGS)
libTarch_debug_a_CXXFLAGS = -DPeanoDebug=4 -ggdb3 -O0 $(PEANO_CXXFLAGS)


libPeano4Core2d_a_CXXFLAGS        = -DDimensions=2 -DPeanoDebug=0 $(PEANO_CXXFLAGS)
libPeano4Core2d_trace_a_CXXFLAGS  = -DDimensions=2 -DPeanoDebug=1 -ggdb3 -O0 $(PEANO_CXXFLAGS)
libPeano4Core2d_debug_a_CXXFLAGS  = -DDimensions=2 -DPeanoDebug=4 -ggdb3 -O0 $(PEANO_CXXFLAGS)
libPeano4Core3d_a_CXXFLAGS        = -DDimensions=3 -DPeanoDebug=0 $(PEANO_CXXFLAGS)
libPeano4Core3d_trace_a_CXXFLAGS  = -DDimensions=3 -DPeanoDebug=1 -ggdb3 -O0 $(PEANO_CXXFLAGS)
libPeano4Core3d_debug_a_CXXFLAGS  = -DDimensions=3 -DPeanoDebug=4 -ggdb3 -O0 $(PEANO_CXXFLAGS)


#
# All sources below
#
Peano4Core_SOURCES = peano4/peano.cpp \
  peano4/grid/AutomatonState.cpp \
  peano4/grid/EmptyTraversalObserver.cpp \
  peano4/grid/GridControlEvent.cpp \
  peano4/grid/GridVertex.cpp \
  peano4/grid/GridStatistics.cpp \
  peano4/grid/GridTraversalEvent.cpp \
  peano4/grid/PeanoCurve.cpp \
  peano4/grid/Spacetree.cpp \
  peano4/grid/TraversalObserver.cpp \
  peano4/grid/TraversalVTKPlotter.cpp \
  peano4/grid/tests/SpacetreeTest.cpp \
  peano4/parallel/Node.cpp \
  peano4/parallel/SpacetreeSet.cpp \
  peano4/parallel/StartTraversalMessage.cpp \
  peano4/parallel/Tasks.cpp \
  peano4/parallel/TreeEntry.cpp \
  peano4/parallel/TreeManagementMessage.cpp \
  peano4/parallel/tests/PingPongTest.cpp \
  peano4/parallel/tests/NodeTest.cpp \
  peano4/stacks/STDVectorStack.cpp \
  peano4/utils/Loop.cpp



Tarch_SOURCES= tarch/tarch.cpp \
 tarch/la/tests/MatrixVectorTest.cpp \
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
 tarch/multicore/tbb/TBB.cpp \
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
 tarch/plotter/griddata/VTUTimeSeriesWriter.cpp \
 tarch/plotter/griddata/blockstructured/PeanoTextPatchFileWriter.cpp \
 tarch/plotter/griddata/blockstructured/PeanoTextPatchFileWriter_VertexDataWriter.cpp \
 tarch/plotter/griddata/blockstructured/PeanoHDF5PatchFileWriter.cpp \
 tarch/plotter/griddata/blockstructured/PeanoHDF5PatchFileWriter_CellDataWriter.cpp \
 tarch/plotter/griddata/blockstructured/PeanoHDF5PatchFileWriter_VertexDataWriter.cpp \
 tarch/plotter/griddata/blockstructured/PeanoTextPatchFileWriter_CellDataWriter.cpp \
 tarch/plotter/pointdata/vtk/VTKTextFileWriter.cpp \
 tarch/plotter/pointdata/vtk/VTKBinaryFileWriter.cpp \
 tarch/plotter/pointdata/vtk/VTKBinaryFileWriter_PointDataWriter.cpp \
 tarch/mpi/Rank.cpp \
 tarch/mpi/mpi.cpp \
 tarch/mpi/IntegerMessage.cpp \
 tarch/mpi/StringMessage.cpp \
 tarch/mpi/StringTools.cpp \
 tarch/logging/CommandLineLogger.cpp \
 tarch/logging/ChromeTraceFileLogger.cpp \
 tarch/logging/Log.cpp \
 tarch/logging/LogFilterFileReader.cpp \
 tarch/logging/LogFilter.cpp \
 tarch/timing/Watch.cpp \
 tarch/timing/Measurement.cpp \
 tarch/timing/GlidingAverageMeasurement.cpp \
 tarch/services/Service.cpp \
 tarch/services/ServiceRepository.cpp 


#
# Map defined source sequences onto libraries
#
libTarch_a_SOURCES       = $(Tarch_SOURCES)
libTarch_trace_a_SOURCES = $(Tarch_SOURCES)
libTarch_debug_a_SOURCES = $(Tarch_SOURCES)

libPeano4Core2d_a_SOURCES       = $(Peano4Core_SOURCES)
libPeano4Core2d_trace_a_SOURCES = $(Peano4Core_SOURCES)
libPeano4Core2d_debug_a_SOURCES = $(Peano4Core_SOURCES)
libPeano4Core3d_a_SOURCES       = $(Peano4Core_SOURCES)
libPeano4Core3d_trace_a_SOURCES = $(Peano4Core_SOURCES)
libPeano4Core3d_debug_a_SOURCES = $(Peano4Core_SOURCES)

