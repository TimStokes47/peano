bin_PROGRAMS = PeanoGrid2d_debug PeanoGrid2d PeanoGrid3d_debug PeanoGrid3d

PeanoGridSources = \
  main.cpp \
  MyObserver.cpp

PeanoGrid2d_debug_CXXFLAGS  = -I$(top_builddir)/src $(PEANO_CXXFLAGS) -DPeanoDebug=4 -DDimensions=2
PeanoGrid2d_debug_LDADD     = $(top_builddir)/src/libPeano4Core2d_debug.a $(top_builddir)/src/libTarch_debug.a
PeanoGrid2d_debug_LDFLAGS   = -L$(top_builddir)/src $(PEANO_LDFLAGS)
PeanoGrid2d_debug_SOURCES   = $(PeanoGridSources)

PeanoGrid2d_CXXFLAGS  = -I$(top_builddir)/src $(PEANO_CXXFLAGS)  -DPeanoDebug=1 -DDimensions=2
PeanoGrid2d_LDADD     = $(top_builddir)/src/libPeano4Core2d.a $(top_builddir)/src/libTarch.a
PeanoGrid2d_LDFLAGS   = -L$(top_builddir)/src $(PEANO_LDFLAGS)
PeanoGrid2d_SOURCES   = $(PeanoGridSources)

PeanoGrid3d_debug_CXXFLAGS  = -I$(top_builddir)/src $(PEANO_CXXFLAGS) -DPeanoDebug=4 -DDimensions=3
PeanoGrid3d_debug_LDADD     = $(top_builddir)/src/libPeano4Core3d_debug.a $(top_builddir)/src/libTarch_debug.a
PeanoGrid3d_debug_LDFLAGS   = -L$(top_builddir)/src $(PEANO_LDFLAGS)
PeanoGrid3d_debug_SOURCES   = $(PeanoGridSources)

PeanoGrid3d_CXXFLAGS  = -I$(top_builddir)/src $(PEANO_CXXFLAGS)  -DPeanoDebug=1 -DDimensions=3
PeanoGrid3d_LDADD     = $(top_builddir)/src/libPeano4Core3d.a $(top_builddir)/src/libTarch.a
PeanoGrid3d_LDFLAGS   = -L$(top_builddir)/src $(PEANO_LDFLAGS)
PeanoGrid3d_SOURCES   = $(PeanoGridSources)
