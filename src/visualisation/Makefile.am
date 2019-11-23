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

convert_CXXFLAGS  = $(PEANO_CXXFLAGS) $(OPENMP_CXXFLAGS)
convert_LDADD     = $(top_builddir)/src/libTarch.a
convert_LDFLAGS   = $(PEANO_LDFLAGS) -lstdc++fs -lvtkIOXML -lvtkCommonExecutionModel -lvtkCommonCore -lvtkCommonDataModel -lvtkFiltersCore -lvtkFiltersGeneral
convert_SOURCES   = $(convertSources)

convert_debug_CXXFLAGS  = $(PEANO_CXXFLAGS) $(OPENMP_CXXFLAGS) -O0 -g3 -DPeanoDebug=1
convert_debug_LDADD     = $(top_builddir)/src/libTarch_debug.a
convert_debug_LDFLAGS   = $(PEANO_LDFLAGS) -lstdc++fs -lvtkIOXML -lvtkCommonExecutionModel -lvtkCommonCore -lvtkCommonDataModel -lvtkFiltersCore -lvtkFiltersGeneral
convert_debug_SOURCES   = $(convertSources)

#ifeq ($(VTK_INCLUDE),)
#  $(error "ERROR: Environment variable VTK_INCLUDE not set (usually something alike -L/usr/include/vtk-8.1)")
#endif

#ifeq ($(VTK_LIB),)
#  $(error "ERROR: Environment variable VTK_LIB not set (usually -I/usr/lib64)")
#endif

#INCLUDE_OMP=-fopenmp
#LINK_OMP=-fopenmp


#build-gcc: CC=g++ -std=c++0x
#build-gcc: CFLAGS=
#build-gcc: LFLAGS=-lstdc++fs
#build-gcc: header build



#build: $(OBJECTS)#
##	$(LL) $(OBJECT#S) $(VTK_LIB) -o $(EXECUTABLE) $(LFLAGS) -lvtkIOXML -lvtkCommonExecutionModel -lvtkCommonCore -lvtkCommonDataModel -lvtkFiltersCore -lvtkFiltersGeneral
#	@echo#
#	@echo build of Peano file converter successful
#	@echo visit Peano\'s homepage at http://www.peano-framework.org
