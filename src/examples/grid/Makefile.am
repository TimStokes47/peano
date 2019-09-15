bin_PROGRAMS = PeanoGrid2d_debug PeanoGrid2d PeanoGrid3d_debug PeanoGrid3d

PeanoGridSources = \
  main.cpp \
  MyObserver.cpp

PeanoGrid2d_debug_CXXFLAGS  = -I$(top_builddir)/src $(PEANO_CXXFLAGS) -DPeanoDebug=1 -DDimensions=2
PeanoGrid2d_debug_LDFLAGS   = -L$(top_builddir)/src $(PEANO_LDFLAGS) -lTarch_debug -lPeano4Core2d_debug
PeanoGrid2d_debug_SOURCES   = $(PeanoGridSources)

PeanoGrid2d_CXXFLAGS  = -I$(top_builddir)/src $(PEANO_CXXFLAGS) -DDimensions=2
PeanoGrid2d_LDFLAGS   = -L$(top_builddir)/src $(PEANO_LDFLAGS) -lTarch -lPeano4Core2d
PeanoGrid2d_SOURCES   = $(PeanoGridSources)

PeanoGrid3d_debug_CXXFLAGS  = -I$(top_builddir)/src $(PEANO_CXXFLAGS) -DPeanoDebug=1 -DDimensions=3
PeanoGrid3d_debug_LDFLAGS   = -L$(top_builddir)/src $(PEANO_LDFLAGS) -lTarch_debug -lPeano4Core3d_debug
PeanoGrid3d_debug_SOURCES   = $(PeanoGridSources)

PeanoGrid3d_CXXFLAGS  = -I$(top_builddir)/src $(PEANO_CXXFLAGS) -DDimensions=3
PeanoGrid3d_LDFLAGS   = -L$(top_builddir)/src $(PEANO_LDFLAGS) -lTarch -lPeano4Core3d
PeanoGrid3d_SOURCES   = $(PeanoGridSources)
