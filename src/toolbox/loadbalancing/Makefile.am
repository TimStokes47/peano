AUTOMAKE_OPTIONS = subdir-objects


lib_LIBRARIES = libToolboxLoadBalancing.a libToolboxLoadBalancing_debug.a libToolboxLoadBalancing_trace.a  libToolboxLoadBalancing_asserts.a


ToolboxLoadBalancing_SOURCES = \
  RecursiveGuided.cpp
  

libToolboxLoadBalancing_a_CXXFLAGS         = -DPeanoDebug=0 $(PEANO_CXXFLAGS)
libToolboxLoadBalancing_trace_a_CXXFLAGS   = -DPeanoDebug=1 -g3 -O0 $(PEANO_CXXFLAGS)
libToolboxLoadBalancing_asserts_a_CXXFLAGS = -DPeanoDebug=2 -g3 -O0 $(PEANO_CXXFLAGS)
libToolboxLoadBalancing_debug_a_CXXFLAGS   = -DPeanoDebug=4 -g3 -O0 $(PEANO_CXXFLAGS)

libToolboxLoadBalancing_a_SOURCES          = $(ToolboxLoadBalancing_SOURCES)
libToolboxLoadBalancing_trace_a_SOURCES    = $(ToolboxLoadBalancing_SOURCES)
libToolboxLoadBalancing_asserts_a_SOURCES  = $(ToolboxLoadBalancing_SOURCES)
libToolboxLoadBalancing_debug_a_SOURCES    = $(ToolboxLoadBalancing_SOURCES)

