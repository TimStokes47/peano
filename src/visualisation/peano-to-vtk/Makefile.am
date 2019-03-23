bin_PROGRAMS = PeanoToVTK

PeanoToVTKSources = \
  Convert.cpp \
  PeanoConverter.cpp \
  PeanoDataSet.cpp \
  PeanoMetaFile.cpp \
  PeanoPatch.cpp \
  PeanoPatchData.cpp \
  PeanoReader.cpp \
  PeanoVariable.cpp

PeanoToVTK_CXXFLAGS  = -I$(top_builddir)/src -DPeanoDebug=1 -DDimensions=2
PeanoToVTK_LDFLAGS   = -L$(top_builddir)/src  -lTarch_debug -lPeano4Core2d_debug
PeanoToVTK_SOURCES   = $(PeanoToVTKSources)


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
