bin_PROGRAMS = PeanoRegularGridUpscaling2d_debug PeanoRegularGridUpscaling2d PeanoRegularGridUpscaling3d_debug PeanoRegularGridUpscaling3d

PeanoRegularGridUpscalingSources = \
  main.cpp \
  MyObserver.cpp

PeanoRegularGridUpscaling2d_debug_CXXFLAGS  = -I$(top_builddir)/src $(PEANO_CXXFLAGS) -DPeanoDebug=4 -DDimensions=2
PeanoRegularGridUpscaling2d_debug_LDFLAGS   = -L$(top_builddir)/src $(PEANO_LDFLAGS) -lTarch_debug -lPeano4Core2d_debug
PeanoRegularGridUpscaling2d_debug_SOURCES   = $(PeanoRegularGridUpscalingSources)

PeanoRegularGridUpscaling2d_CXXFLAGS  = -I$(top_builddir)/src $(PEANO_CXXFLAGS)  -DPeanoDebug=1 -DDimensions=2
PeanoRegularGridUpscaling2d_LDFLAGS   = -L$(top_builddir)/src $(PEANO_LDFLAGS) -lTarch -lPeano4Core2d
PeanoRegularGridUpscaling2d_SOURCES   = $(PeanoRegularGridUpscalingSources)

PeanoRegularGridUpscaling3d_debug_CXXFLAGS  = -I$(top_builddir)/src $(PEANO_CXXFLAGS) -DPeanoDebug=4 -DDimensions=3
PeanoRegularGridUpscaling3d_debug_LDFLAGS   = -L$(top_builddir)/src $(PEANO_LDFLAGS) -lTarch_debug -lPeano4Core3d_debug
PeanoRegularGridUpscaling3d_debug_SOURCES   = $(PeanoRegularGridUpscalingSources)

PeanoRegularGridUpscaling3d_CXXFLAGS  = -I$(top_builddir)/src $(PEANO_CXXFLAGS)  -DPeanoDebug=1 -DDimensions=3
PeanoRegularGridUpscaling3d_LDFLAGS   = -L$(top_builddir)/src $(PEANO_LDFLAGS) -lTarch -lPeano4Core3d
PeanoRegularGridUpscaling3d_SOURCES   = $(PeanoRegularGridUpscalingSources)
