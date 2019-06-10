if UseDelta
  bin_PROGRAMS = Delta_debug Delta
endif

#
# I recommend 
#

DeltaSources = \
  main.cpp \
  InitData.cpp \
  MyObserver.cpp \
  CompositeMapping.cpp \
  PeanoFormatCellDataPlotter.cpp


Delta_CXXFLAGS  = -I$(top_builddir)/src -DPeanoDebug=0 -DDimensions=3
Delta_LDFLAGS   = -L$(top_builddir)/src $(PEANO_LDFLAGS) -lTarch -lPeano4Core3d
Delta_SOURCES   = $(DeltaSources)


# -lDelta  -lDelta_debug

Delta_debug_CXXFLAGS  = -I$(top_builddir)/src -g3 -O0 -DPeanoDebug=1 -DDimensions=3
Delta_debug_LDFLAGS   = -L$(top_builddir)/src $(PEANO_LDFLAGS) -lTarch_debug -lPeano4Core3d_debug
Delta_debug_SOURCES   = $(DeltaSources)
