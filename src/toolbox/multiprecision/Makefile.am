AUTOMAKE_OPTIONS = subdir-objects


noinst_LTLIBRARIES = libToolboxMultiprecision.la libToolboxMultiprecision_debug.la libToolboxMultiprecision_trace.la


ToolboxMultiprecision_SOURCES = \
  CompressedFloatingPointNumbers.cpp \
  tests/CompressedFloatingPointNumbersTest.cpp
  

libToolboxMultiprecision_la_CXXFLAGS       = -DDimensions=2 -DPeanoDebug=0 $(PEANO_CXXFLAGS)
libToolboxMultiprecision_trace_la_CXXFLAGS = -DDimensions=2 -DPeanoDebug=1 -g3 -O0 $(PEANO_CXXFLAGS)
libToolboxMultiprecision_debug_la_CXXFLAGS = -DDimensions=2 -DPeanoDebug=4 -g3 -O0 $(PEANO_CXXFLAGS)

libToolboxMultiprecision_la_LDFLAGS       =  $(PEANO_LDFLAGS)  -Wl,--whole-archive
libToolboxMultiprecision_trace_la_LDFLAGS =  $(PEANO_LDFLAGS)  -Wl,--whole-archive
libToolboxMultiprecision_debug_la_LDFLAGS =  $(PEANO_LDFLAGS)  -Wl,--whole-archive

libToolboxMultiprecision_la_SOURCES       = $(ToolboxMultiprecision_SOURCES)
libToolboxMultiprecision_trace_la_SOURCES = $(ToolboxMultiprecision_SOURCES)
libToolboxMultiprecision_debug_la_SOURCES = $(ToolboxMultiprecision_SOURCES)

