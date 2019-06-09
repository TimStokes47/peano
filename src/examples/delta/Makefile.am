if UseDelta
  bin_PROGRAMS = Delta_debug Delta
endif


DeltaSources = \
  main.cpp \
  MyObserver.cpp \
  PeanoFormatCellDataPlotter.cpp

Delta_debug_CXXFLAGS  = -I$(top_builddir)/src -DPeanoDebug=1 -DDimensions=3
Delta_debug_LDFLAGS   = -L$(top_builddir)/src $(PEANO_LDFLAGS) -lTarch_debug -lPeano4Core2d_debug -lDelta_debug
Delta_debug_SOURCES   = $(DeltaSources)

Delta_CXXFLAGS  = -I$(top_builddir)/src -DPeanoDebug=0 -DDimensions=3
Delta_LDFLAGS   = -L$(top_builddir)/src $(PEANO_LDFLAGS) -lTarch -lPeano4Core2d -lDelta
Delta_SOURCES   = $(DeltaSources)
