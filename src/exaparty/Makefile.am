AUTOMAKE_OPTIONS = subdir-objects

if UseExaParty
  lib_LIBRARIES = libExaPartyCore2d.a  libExaPartyCore2d_debug.a  libExaPartyCore2d_stats.a  libExaPartyCore2d_asserts.a  libExaPartyCore2d_trace.a \
                  libExaPartyCore3d.a  libExaPartyCore3d_debug.a  libExaPartyCore3d_stats.a  libExaPartyCore3d_asserts.a  libExaPartyCore3d_trace.a
endif



ExaPartySources = \
  CellMetaData.cpp \
  GridControlEvents.cpp \
  ParticleSpecies.cpp \
  exaparty.cpp \
  TaskEnumerator.cpp \
  TaskNumber.cpp \
  UnitTests.cpp \
  UserInterface.cpp \
  boundaryconditions/FixedBoundary.cpp \
  boundaryconditions/Utils.cpp \
  kernels/ParticleSetIterators.cpp \
  kernels/legacy/Density.cpp \
  kernels/legacy/HydroForce.cpp \
  kernels/legacy/SmoothingLength.cpp \
  kernels/legacy/Swift.cpp \
  timestepping/TimeStepping.cpp \
  dependencychecks/DependencyChecks.cpp

libExaPartyCore2d_a_CXXFLAGS          = $(CXXFLAGS_PEANO_2D_RELEASE)
libExaPartyCore2d_trace_a_CXXFLAGS    = $(CXXFLAGS_PEANO_2D_TRACE)  
libExaPartyCore2d_asserts_a_CXXFLAGS  = $(CXXFLAGS_PEANO_2D_ASSERTS)  
libExaPartyCore2d_debug_a_CXXFLAGS    = $(CXXFLAGS_PEANO_2D_DEBUG)  
libExaPartyCore2d_stats_a_CXXFLAGS    = $(CXXFLAGS_PEANO_2D_STATS)

libExaPartyCore3d_a_CXXFLAGS          = $(CXXFLAGS_PEANO_3D_RELEASE)
libExaPartyCore3d_trace_a_CXXFLAGS    = $(CXXFLAGS_PEANO_3D_TRACE)  
libExaPartyCore3d_asserts_a_CXXFLAGS  = $(CXXFLAGS_PEANO_3D_ASSERTS)  
libExaPartyCore3d_debug_a_CXXFLAGS    = $(CXXFLAGS_PEANO_3D_DEBUG)  
libExaPartyCore3d_stats_a_CXXFLAGS    = $(CXXFLAGS_PEANO_3D_STATS)


libExaPartyCore2d_a_SOURCES         = $(ExaPartySources)
libExaPartyCore2d_trace_a_SOURCES   = $(ExaPartySources)
libExaPartyCore2d_asserts_a_SOURCES = $(ExaPartySources)
libExaPartyCore2d_debug_a_SOURCES   = $(ExaPartySources)
libExaPartyCore2d_stats_a_SOURCES   = $(ExaPartySources)

libExaPartyCore3d_a_SOURCES         = $(ExaPartySources)
libExaPartyCore3d_trace_a_SOURCES   = $(ExaPartySources)
libExaPartyCore3d_asserts_a_SOURCES = $(ExaPartySources)
libExaPartyCore3d_debug_a_SOURCES   = $(ExaPartySources)
libExaPartyCore3d_stats_a_SOURCES   = $(ExaPartySources)


