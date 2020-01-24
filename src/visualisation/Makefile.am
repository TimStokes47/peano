AUTOMAKE_OPTIONS = subdir-objects

if UseVTK
  bin_PROGRAMS = convert convert_debug
endif



convertSources = \
  convert.cpp \
  data/DataSet.cpp \
  data/PatchData.cpp \
  data/Variable.cpp \
  filter/Copy.cpp \
  filter/Intersection.cpp \
  filter/SelectValue.cpp \
  filter/SeparateResolutions.cpp \
  input/Parser.cpp \
  input/PeanoTextPatchFileReader.cpp \
  output/PeanoWriter.cpp \
  output/VTUWriter.cpp
  
#VTK_LIBRARY_EXTENSION = -8.90
VTK_LIBRARY_EXTENSION = 
VTK_REQUIRED_LIBS     =  -lvtkIOXML$(VTK_LIBRARY_EXTENSION) \
                         -lvtkCommonExecutionModel$(VTK_LIBRARY_EXTENSION) \
                         -lvtkCommonCore$(VTK_LIBRARY_EXTENSION) \
                         -lvtkCommonDataModel$(VTK_LIBRARY_EXTENSION) \
                         -lvtkFiltersCore$(VTK_LIBRARY_EXTENSION) \
                         -lvtkFiltersGeneral$(VTK_LIBRARY_EXTENSION)

convert_CXXFLAGS  = $(PEANO_CXXFLAGS) $(OPENMP_CXXFLAGS)
convert_LDADD     = $(top_builddir)/src/libTarch.a
convert_LDFLAGS   = $(PEANO_LDFLAGS) $(LDFLAGS) -lstdc++fs $(VTK_REQUIRED_LIBS)
convert_SOURCES   = $(convertSources)

convert_debug_CXXFLAGS  = $(PEANO_CXXFLAGS) -O0 -g3 -DPeanoDebug=4
convert_debug_LDADD     = $(top_builddir)/src/libTarch_debug.a
convert_debug_LDFLAGS   = $(PEANO_LDFLAGS) $(LDFLAGS) -lstdc++fs $(VTK_REQUIRED_LIBS)
convert_debug_SOURCES   = $(convertSources)

