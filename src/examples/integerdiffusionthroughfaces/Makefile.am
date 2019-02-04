bin_PROGRAMS = PeanoDiffusionThroughFaces2d_debug PeanoDiffusionThroughFaces2d PeanoDiffusionThroughFaces3d_debug PeanoDiffusionThroughFaces3d

PeanoDiffusionThroughFacesSources = \
  main.cpp \
  CellDataPlotter.cpp \
  CompositeMapping.cpp \
  MyMapping.cpp \
  MyObserver.cpp

PeanoDiffusionThroughFaces2d_debug_CXXFLAGS  = -I$(top_builddir)/src -DPeanoDebug=1 -DDimensions=2
PeanoDiffusionThroughFaces2d_debug_LDFLAGS   = -L$(top_builddir)/src  -lTarch_debug -lPeano4Core2d_debug
PeanoDiffusionThroughFaces2d_debug_SOURCES   = $(PeanoDiffusionThroughFacesSources)

PeanoDiffusionThroughFaces2d_CXXFLAGS  = -I$(top_builddir)/src -DDimensions=2
PeanoDiffusionThroughFaces2d_LDFLAGS   = -L$(top_builddir)/src  -lTarch -lPeano4Core2d
PeanoDiffusionThroughFaces2d_SOURCES   = $(PeanoDiffusionThroughFacesSources)

PeanoDiffusionThroughFaces3d_debug_CXXFLAGS  = -I$(top_builddir)/src -DPeanoDebug=1 -DDimensions=3
PeanoDiffusionThroughFaces3d_debug_LDFLAGS   = -L$(top_builddir)/src -lTarch_debug -lPeano4Core3d_debug
PeanoDiffusionThroughFaces3d_debug_SOURCES   = $(PeanoDiffusionThroughFacesSources)

PeanoDiffusionThroughFaces3d_CXXFLAGS  = -I$(top_builddir)/src -DDimensions=3
PeanoDiffusionThroughFaces3d_LDFLAGS   = -L$(top_builddir)/src -lTarch -lPeano4Core3d
PeanoDiffusionThroughFaces3d_SOURCES   = $(PeanoDiffusionThroughFacesSources)
