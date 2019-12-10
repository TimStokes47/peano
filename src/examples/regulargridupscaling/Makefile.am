bin_PROGRAMS = PeanoRegularGridUpscaling2d_debug PeanoRegularGridUpscaling2d PeanoRegularGridUpscaling3d_debug PeanoRegularGridUpscaling3d

PeanoRegularGridUpscalingSources = \
  main.cpp \
  MyObserver.cpp

PeanoRegularGridUpscaling2d_debug_CXXFLAGS  = -g3 -O0 -I$(top_builddir)/src $(PEANO_CXXFLAGS) -DPeanoDebug=4 -DDimensions=2
PeanoRegularGridUpscaling2d_debug_LDADD     = $(top_builddir)/src/libPeano4Core2d_debug.a $(top_builddir)/src/libTarch_debug.a
PeanoRegularGridUpscaling2d_debug_SOURCES   = $(PeanoRegularGridUpscalingSources)
PeanoRegularGridUpscaling2d_debug_LDFLAGS   = -L$(top_builddir)/src $(PEANO_LDFLAGS)

PeanoRegularGridUpscaling2d_CXXFLAGS  = -I$(top_builddir)/src $(PEANO_CXXFLAGS) -DPeanoDebug=0 -DDimensions=2
PeanoRegularGridUpscaling2d_LDADD     = $(top_builddir)/src/libPeano4Core2d.a $(top_builddir)/src/libTarch.a
PeanoRegularGridUpscaling2d_LDFLAGS   = -L$(top_builddir)/src $(PEANO_LDFLAGS)
PeanoRegularGridUpscaling2d_SOURCES   = $(PeanoRegularGridUpscalingSources)

PeanoRegularGridUpscaling3d_debug_CXXFLAGS  = -g3 -O0 -I$(top_builddir)/src $(PEANO_CXXFLAGS) -DPeanoDebug=4 -DDimensions=3
PeanoRegularGridUpscaling3d_debug_LDADD     = $(top_builddir)/src/libPeano4Core3d_debug.a $(top_builddir)/src/libTarch_debug.a
PeanoRegularGridUpscaling3d_debug_LDFLAGS   = -L$(top_builddir)/src $(PEANO_LDFLAGS)
PeanoRegularGridUpscaling3d_debug_SOURCES   = $(PeanoRegularGridUpscalingSources)

PeanoRegularGridUpscaling3d_CXXFLAGS  = -I$(top_builddir)/src $(PEANO_CXXFLAGS)  -DPeanoDebug=0 -DDimensions=3
PeanoRegularGridUpscaling3d_LDADD     = $(top_builddir)/src/libPeano4Core3d.a $(top_builddir)/src/libTarch.a
PeanoRegularGridUpscaling3d_LDFLAGS   = -L$(top_builddir)/src $(PEANO_LDFLAGS)
PeanoRegularGridUpscaling3d_SOURCES   = $(PeanoRegularGridUpscalingSources)
