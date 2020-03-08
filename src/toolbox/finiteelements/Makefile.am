AUTOMAKE_OPTIONS = subdir-objects


noinst_LTLIBRARIES = libToolboxFiniteElements2d.la libToolboxFiniteElements2d_debug.la libToolboxFiniteElements2d_trace.la \
                     libToolboxFiniteElements3d.la libToolboxFiniteElements3d_debug.la libToolboxFiniteElements3d_trace.la


ToolboxFiniteElements_SOURCES = \
  BSplinesStencilFactory.cpp \
  ElementMatrix.cpp \
  Stencil.cpp \
  StencilFactory.cpp \
  tests/StencilFactoryTest.cpp
  

libToolboxFiniteElements2d_la_CXXFLAGS       = -DDimensions=2 -DPeanoDebug=0 $(PEANO_CXXFLAGS)
libToolboxFiniteElements2d_trace_la_CXXFLAGS = -DDimensions=2 -DPeanoDebug=1 -g3 -O0 $(PEANO_CXXFLAGS)
libToolboxFiniteElements2d_debug_la_CXXFLAGS = -DDimensions=2 -DPeanoDebug=4 -g3 -O0 $(PEANO_CXXFLAGS)

libToolboxFiniteElements3d_la_CXXFLAGS       = -DDimensions=3 -DPeanoDebug=0 $(PEANO_CXXFLAGS)
libToolboxFiniteElements3d_trace_la_CXXFLAGS = -DDimensions=3 -DPeanoDebug=1 -g3 -O0 $(PEANO_CXXFLAGS)
libToolboxFiniteElements3d_debug_la_CXXFLAGS = -DDimensions=3 -DPeanoDebug=4 -g3 -O0 $(PEANO_CXXFLAGS)
  

libToolboxFiniteElements2d_la_LDFLAGS       =  $(PEANO_LDFLAGS)  -Wl,--whole-archive
libToolboxFiniteElements2d_trace_la_LDFLAGS =  $(PEANO_LDFLAGS)  -Wl,--whole-archive
libToolboxFiniteElements2d_debug_la_LDFLAGS =  $(PEANO_LDFLAGS)  -Wl,--whole-archive

libToolboxFiniteElements3d_la_LDFLAGS       =  $(PEANO_LDFLAGS)  -Wl,--whole-archive
libToolboxFiniteElements3d_trace_la_LDFLAGS =  $(PEANO_LDFLAGS)  -Wl,--whole-archive
libToolboxFiniteElements3d_debug_la_LDFLAGS =  $(PEANO_LDFLAGS)  -Wl,--whole-archive


libToolboxFiniteElements2d_la_SOURCES       = $(ToolboxFiniteElements_SOURCES)
libToolboxFiniteElements2d_trace_la_SOURCES = $(ToolboxFiniteElements_SOURCES)
libToolboxFiniteElements2d_debug_la_SOURCES = $(ToolboxFiniteElements_SOURCES)

libToolboxFiniteElements3d_la_SOURCES       = $(ToolboxFiniteElements_SOURCES)
libToolboxFiniteElements3d_trace_la_SOURCES = $(ToolboxFiniteElements_SOURCES)
libToolboxFiniteElements3d_debug_la_SOURCES = $(ToolboxFiniteElements_SOURCES)

