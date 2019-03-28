AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = convert

convertSources = \
  convert.cpp \
  PeanoDataSet.cpp \
  PeanoMetaFile.cpp \
  PeanoPatch.cpp \
  PeanoPatchData.cpp \
  input/Parser.cpp \
  input/PeanoTextPatchFileReader.cpp \
  input/PeanoTextMetaFileReader.cpp \
  output/VTUWriter.cpp \
  PeanoVariable.cpp

convert_CXXFLAGS  = $(PEANO_CXXFLAGS) $(OPENMP_CXXFLAGS)
convert_LDFLAGS   = -L$(top_builddir)/src $(PEANO_LDFLAGS) -lTarch_debug -lstdc++fs -lvtkIOXML -lvtkCommonExecutionModel -lvtkCommonCore -lvtkCommonDataModel -lvtkFiltersCore -lvtkFiltersGeneral
convert_SOURCES   = $(convertSources)


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
