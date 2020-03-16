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
  
  
#
# find . -name 'libvtk*.so*' -exec nm --print-file-name --defined-only --dynamic {} \; | grep "vtkFilter"
#  


#
# Die Files hier sind vtk 8.2 evtl. Muessen wir im configure die Versionen unterscheiden und die libs dort setzen
#
VTK_REQUIRED_LIBS     =  -lvtkIOXML$(VTK_LIBRARY_EXTENSION) \
                         -lvtkCommonExecutionModel$(VTK_LIBRARY_EXTENSION) \
                         -lvtkCommonCore$(VTK_LIBRARY_EXTENSION) \
                         -lvtkCommonDataModel$(VTK_LIBRARY_EXTENSION) \
                         -lvtkFiltersCore$(VTK_LIBRARY_EXTENSION) \
                         -lvtkFiltersGeneral$(VTK_LIBRARY_EXTENSION) \
                         -lvtkIOXMLParser$(VTK_LIBRARY_EXTENSION) \
                         -lvtkIOCore$(VTK_LIBRARY_EXTENSION) \
                         -lvtkCommonSystem$(VTK_LIBRARY_EXTENSION) \
                         -lvtkCommonTransforms$(VTK_LIBRARY_EXTENSION) \
                         -lvtkCommonMisc$(VTK_LIBRARY_EXTENSION) \
                         -lvtkCommonMath$(VTK_LIBRARY_EXTENSION) \
                         -lvtksys$(VTK_LIBRARY_EXTENSION) 


convert_CXXFLAGS  = $(PEANO_CXXFLAGS) $(OPENMP_CXXFLAGS) 
convert_LDADD     = -lTarch
convert_LDFLAGS   = -L$(top_builddir)/src $(PEANO_LDFLAGS) $(OPENMP_CXXFLAGS) $(LDFLAGS) -lstdc++fs $(VTK_REQUIRED_LIBS)
convert_SOURCES   = $(convertSources)

convert_debug_CXXFLAGS  = $(PEANO_CXXFLAGS) $(OPENMP_CXXFLAGS) -O0 -g3 -DPeanoDebug=4 
convert_debug_LDADD     = -lTarch_debug
convert_debug_LDFLAGS   = -L$(top_builddir)/src $(PEANO_LDFLAGS) $(OPENMP_CXXFLAGS) $(LDFLAGS) -lstdc++fs $(VTK_REQUIRED_LIBS)
convert_debug_SOURCES   = $(convertSources)

