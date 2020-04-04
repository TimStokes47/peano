AUTOMAKE_OPTIONS = subdir-objects

if UseExaHyPE
  lib_LIBRARIES = libExaHyPE2Core2d.a  libExaHyPE2Core2d_debug.a  libExaHyPE2Core2d_asserts.a  libExaHyPE2Core2d_trace.a \
                  libExaHyPE2Core3d.a  libExaHyPE2Core3d_debug.a  libExaHyPE2Core3d_asserts.a  libExaHyPE2Core3d_trace.a
endif



ExaHyPE2Sources = \
  RefinementControl.cpp


 
libExaHyPE2Core2d_a_CXXFLAGS          = -DDimensions=2 -DPeanoDebug=0 $(PEANO_CXXFLAGS) $(OPENMP_CXXFLAGS) 
libExaHyPE2Core2d_trace_a_CXXFLAGS    = -DDimensions=2 -DPeanoDebug=1 $(PEANO_CXXFLAGS) $(OPENMP_CXXFLAGS) 
libExaHyPE2Core2d_asserts_a_CXXFLAGS  = -DDimensions=2 -DPeanoDebug=2 $(PEANO_CXXFLAGS) $(OPENMP_CXXFLAGS) 
libExaHyPE2Core2d_debug_a_CXXFLAGS    = -DDimensions=2 -DPeanoDebug=4 $(PEANO_CXXFLAGS) $(OPENMP_CXXFLAGS) 

 
libExaHyPE2Core3d_a_CXXFLAGS          = -DDimensions=3 -DPeanoDebug=0 $(PEANO_CXXFLAGS) $(OPENMP_CXXFLAGS) 
libExaHyPE2Core3d_trace_a_CXXFLAGS    = -DDimensions=3 -DPeanoDebug=1 $(PEANO_CXXFLAGS) $(OPENMP_CXXFLAGS) 
libExaHyPE2Core3d_asserts_a_CXXFLAGS  = -DDimensions=3 -DPeanoDebug=2 $(PEANO_CXXFLAGS) $(OPENMP_CXXFLAGS) 
libExaHyPE2Core3d_debug_a_CXXFLAGS    = -DDimensions=3 -DPeanoDebug=4 $(PEANO_CXXFLAGS) $(OPENMP_CXXFLAGS) 



libExaHyPE2Core2d_a_SOURCES         = $(convertSources)
libExaHyPE2Core2d_trace_a_SOURCES   = $(convertSources)
libExaHyPE2Core2d_asserts_a_SOURCES = $(convertSources)
libExaHyPE2Core2d_debug_a_SOURCES   = $(convertSources)


libExaHyPE2Core3d_a_SOURCES         = $(convertSources)
libExaHyPE2Core3d_trace_a_SOURCES   = $(convertSources)
libExaHyPE2Core3d_asserts_a_SOURCES = $(convertSources)
libExaHyPE2Core3d_debug_a_SOURCES   = $(convertSources)

