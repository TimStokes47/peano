bin_PROGRAMS = UnitTests2d UnitTests3d

UnitTestsSources = main.cpp

UnitTests2d_CXXFLAGS  = -I$(top_builddir)/src $(PEANO_CXXFLAGS) -DPeanoDebug=4 -DDimensions=2
UnitTests2d_LDADD     = $(top_builddir)/src/libPeano4Core2d_debug.a $(top_builddir)/src/libTarch_debug.a
UnitTests2d_LDFLAGS   = -L$(top_builddir)/src $(PEANO_LDFLAGS)
UnitTests2d_SOURCES   = $(UnitTests)

UnitTests3d_CXXFLAGS  = -I$(top_builddir)/src $(PEANO_CXXFLAGS) -DPeanoDebug=4 -DDimensions=3
UnitTests3d_LDADD     = $(top_builddir)/src/libPeano4Core3d_debug.a $(top_builddir)/src/libTarch_debug.a
UnitTests3d_LDFLAGS   = -L$(top_builddir)/src $(PEANO_LDFLAGS)
UnitTests3d_SOURCES   = $(UnitTests)

