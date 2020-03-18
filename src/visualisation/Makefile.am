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
if VTK_LIBRARY_VERSION_8
VTK_REQUIRED_LIBS     =  -lvtkIOXML$(VTK_LIBRARY_SUFFIX) \
                         -lvtkCommonExecutionModel$(VTK_LIBRARY_SUFFIX) \
                         -lvtkCommonCore$(VTK_LIBRARY_SUFFIX) \
                         -lvtkCommonDataModel$(VTK_LIBRARY_SUFFIX) \
                         -lvtkFiltersCore$(VTK_LIBRARY_SUFFIX) \
                         -lvtkFiltersGeneral$(VTK_LIBRARY_SUFFIX) \
                         -lvtkIOXMLParser$(VTK_LIBRARY_SUFFIX) \
                         -lvtkIOCore$(VTK_LIBRARY_SUFFIX) \
                         -lvtkCommonSystem$(VTK_LIBRARY_SUFFIX) \
                         -lvtkCommonTransforms$(VTK_LIBRARY_SUFFIX) \
                         -lvtkCommonMisc$(VTK_LIBRARY_SUFFIX) \
                         -lvtkCommonMath$(VTK_LIBRARY_SUFFIX) \
                         -lvtksys$(VTK_LIBRARY_SUFFIX) 
endif


if VTK_LIBRARY_VERSION_9
#VTK_REQUIRED_LIBS     =  
#  -lvtkCommonCore$(VTK_LIBRARY_SUFFIX) \
#  -lvtkCommonColor$(VTK_LIBRARY_SUFFIX) \
#  -lvtkCommonComputationalGeometry$(VTK_LIBRARY_SUFFIX) \
#  -lvtkCommonDataModel$(VTK_LIBRARY_SUFFIX) \
#  -lvtkCommonExecutionModel$(VTK_LIBRARY_SUFFIX) \
#  -lvtkCommonMath$(VTK_LIBRARY_SUFFIX) \
#  -lvtkCommonMisc$(VTK_LIBRARY_SUFFIX) \
#  -lvtkCommonSystem$(VTK_LIBRARY_SUFFIX) \
#  -lvtkCommonTransforms$(VTK_LIBRARY_SUFFIX) \
#  -lvtksys$(VTK_LIBRARY_SUFFIX) \
#  -lvtkfreetype$(VTK_LIBRARY_SUFFIX) \
#  -lvtkloguru$(VTK_LIBRARY_SUFFIX) \
#  -lvtkFiltersParallel$(VTK_LIBRARY_SUFFIX) \
#  -lvtkFiltersCore$(VTK_LIBRARY_SUFFIX) \
#  -lvtkFiltersSources$(VTK_LIBRARY_SUFFIX) \
#  -lvtkIOXML$(VTK_LIBRARY_SUFFIX) \
#  -lvtkIOExport$(VTK_LIBRARY_SUFFIX) \
#  -lvtkIOLegacy$(VTK_LIBRARY_SUFFIX) \
#  -lvtkIOParallel$(VTK_LIBRARY_SUFFIX) \
#  -lvtkParallelCore$(VTK_LIBRARY_SUFFIX) \
#  -lvtkRenderingCore$(VTK_LIBRARY_SUFFIX) \
#  -lvtkRenderingFreeType$(VTK_LIBRARY_SUFFIX) \
#  -lvtkjsoncpp$(VTK_LIBRARY_SUFFIX)
  
VTK_REQUIRED_LIBS     =  \
-lvtkChartsCore$(VTK_LIBRARY_SUFFIX)                   -lvtkexpat$(VTK_LIBRARY_SUFFIX)                   -lvtkFiltersPoints$(VTK_LIBRARY_SUFFIX)        -lvtkhdf5$(VTK_LIBRARY_SUFFIX)                  -lvtkInteractionImage$(VTK_LIBRARY_SUFFIX)    -lvtkIOImport$(VTK_LIBRARY_SUFFIX)       -lvtkIOTecplotTable$(VTK_LIBRARY_SUFFIX)  -lvtknetcdf$(VTK_LIBRARY_SUFFIX)                 -lvtkRenderingLabel$(VTK_LIBRARY_SUFFIX)          -lvtkViewsContext2D$(VTK_LIBRARY_SUFFIX) \
-lvtkCommonColor$(VTK_LIBRARY_SUFFIX)                  -lvtkFiltersAMR$(VTK_LIBRARY_SUFFIX)              -lvtkFiltersProgrammable$(VTK_LIBRARY_SUFFIX)  -lvtkhdf5_hl$(VTK_LIBRARY_SUFFIX)               -lvtkInteractionStyle$(VTK_LIBRARY_SUFFIX)    -lvtkIOInfovis$(VTK_LIBRARY_SUFFIX)      -lvtkIOVeraOut$(VTK_LIBRARY_SUFFIX)       -lvtkogg$(VTK_LIBRARY_SUFFIX)                    -lvtkRenderingLOD$(VTK_LIBRARY_SUFFIX)            -lvtkViewsCore$(VTK_LIBRARY_SUFFIX) \
-lvtkCommonComputationalGeometry$(VTK_LIBRARY_SUFFIX)  -lvtkFiltersCore$(VTK_LIBRARY_SUFFIX)             -lvtkFiltersPython$(VTK_LIBRARY_SUFFIX)        -lvtkImagingColor$(VTK_LIBRARY_SUFFIX)          -lvtkInteractionWidgets$(VTK_LIBRARY_SUFFIX)  -lvtkIOLegacy$(VTK_LIBRARY_SUFFIX)       -lvtkIOVideo$(VTK_LIBRARY_SUFFIX)         -lvtkParallelCore$(VTK_LIBRARY_SUFFIX)           -lvtkRenderingOpenGL2$(VTK_LIBRARY_SUFFIX)        -lvtkViewsInfovis$(VTK_LIBRARY_SUFFIX) \
-lvtkCommonCore$(VTK_LIBRARY_SUFFIX)                   -lvtkFiltersExtraction$(VTK_LIBRARY_SUFFIX)       -lvtkFiltersSelection$(VTK_LIBRARY_SUFFIX)     -lvtkImagingCore$(VTK_LIBRARY_SUFFIX)           -lvtkIOAMR$(VTK_LIBRARY_SUFFIX)               -lvtkIOLSDyna$(VTK_LIBRARY_SUFFIX)       -lvtkIOXML$(VTK_LIBRARY_SUFFIX)           -lvtkParallelDIY$(VTK_LIBRARY_SUFFIX)            -lvtkRenderingSceneGraph$(VTK_LIBRARY_SUFFIX)     -lvtkWrappingPythonCore$(VTK_LIBRARY_SUFFIX) \
-lvtkCommonDataModel$(VTK_LIBRARY_SUFFIX)              -lvtkFiltersFlowPaths$(VTK_LIBRARY_SUFFIX)        -lvtkFiltersSMP$(VTK_LIBRARY_SUFFIX)           -lvtkImagingFourier$(VTK_LIBRARY_SUFFIX)        -lvtkIOAsynchronous$(VTK_LIBRARY_SUFFIX)      -lvtkIOMINC$(VTK_LIBRARY_SUFFIX)         -lvtkIOXMLParser$(VTK_LIBRARY_SUFFIX)     -lvtkpng$(VTK_LIBRARY_SUFFIX)                    -lvtkRenderingUI$(VTK_LIBRARY_SUFFIX)             -lvtkWrappingTools$(VTK_LIBRARY_SUFFIX) \
-lvtkCommonExecutionModel$(VTK_LIBRARY_SUFFIX)         -lvtkFiltersGeneral$(VTK_LIBRARY_SUFFIX)          -lvtkFiltersSources$(VTK_LIBRARY_SUFFIX)       -lvtkImagingGeneral$(VTK_LIBRARY_SUFFIX)        -lvtkIOCityGML$(VTK_LIBRARY_SUFFIX)           -lvtkIOMotionFX$(VTK_LIBRARY_SUFFIX)     -lvtkjpeg$(VTK_LIBRARY_SUFFIX)            -lvtkpugixml$(VTK_LIBRARY_SUFFIX)                -lvtkRenderingVolume$(VTK_LIBRARY_SUFFIX)         -lvtkzlib$(VTK_LIBRARY_SUFFIX) \
-lvtkCommonMath$(VTK_LIBRARY_SUFFIX)                   -lvtkFiltersGeneric$(VTK_LIBRARY_SUFFIX)          -lvtkFiltersStatistics$(VTK_LIBRARY_SUFFIX)    -lvtkImagingHybrid$(VTK_LIBRARY_SUFFIX)         -lvtkIOCore$(VTK_LIBRARY_SUFFIX)              -lvtkIOMovie$(VTK_LIBRARY_SUFFIX)        -lvtkjsoncpp$(VTK_LIBRARY_SUFFIX)         -lvtkPythonContext2D$(VTK_LIBRARY_SUFFIX)        -lvtkRenderingVolumeOpenGL2$(VTK_LIBRARY_SUFFIX) \
-lvtkCommonMisc$(VTK_LIBRARY_SUFFIX)                   -lvtkFiltersGeometry$(VTK_LIBRARY_SUFFIX)         -lvtkFiltersTexture$(VTK_LIBRARY_SUFFIX)       -lvtkImagingMath$(VTK_LIBRARY_SUFFIX)           -lvtkIOEnSight$(VTK_LIBRARY_SUFFIX)           -lvtkIONetCDF$(VTK_LIBRARY_SUFFIX)       -lvtklibharu$(VTK_LIBRARY_SUFFIX)         -lvtkPythonInterpreter$(VTK_LIBRARY_SUFFIX)      -lvtkRenderingVtkJS$(VTK_LIBRARY_SUFFIX) \
-lvtkCommonSystem$(VTK_LIBRARY_SUFFIX)                 -lvtkFiltersHybrid$(VTK_LIBRARY_SUFFIX)           -lvtkFiltersTopology$(VTK_LIBRARY_SUFFIX)      -lvtkImagingMorphological$(VTK_LIBRARY_SUFFIX)  -lvtkIOExodus$(VTK_LIBRARY_SUFFIX)            -lvtkIOOggTheora$(VTK_LIBRARY_SUFFIX)    -lvtklibproj$(VTK_LIBRARY_SUFFIX)         -lvtkRenderingAnnotation$(VTK_LIBRARY_SUFFIX)    -lvtksqlite$(VTK_LIBRARY_SUFFIX) \
-lvtkCommonTransforms$(VTK_LIBRARY_SUFFIX)             -lvtkFiltersHyperTree$(VTK_LIBRARY_SUFFIX)        -lvtkFiltersVerdict$(VTK_LIBRARY_SUFFIX)       -lvtkImagingSources$(VTK_LIBRARY_SUFFIX)        -lvtkIOExport$(VTK_LIBRARY_SUFFIX)            -lvtkIOParallel$(VTK_LIBRARY_SUFFIX)     -lvtklibxml2$(VTK_LIBRARY_SUFFIX)         -lvtkRenderingContext2D$(VTK_LIBRARY_SUFFIX)     -lvtksys$(VTK_LIBRARY_SUFFIX) \
-lvtkDICOMParser$(VTK_LIBRARY_SUFFIX)                  -lvtkFiltersImaging$(VTK_LIBRARY_SUFFIX)          -lvtkfreetype$(VTK_LIBRARY_SUFFIX)             -lvtkImagingStatistics$(VTK_LIBRARY_SUFFIX)     -lvtkIOExportGL2PS$(VTK_LIBRARY_SUFFIX)       -lvtkIOParallelXML$(VTK_LIBRARY_SUFFIX)  -lvtkloguru$(VTK_LIBRARY_SUFFIX)          -lvtkRenderingCore$(VTK_LIBRARY_SUFFIX)          -lvtkTestingRendering$(VTK_LIBRARY_SUFFIX) \
-lvtkDomainsChemistry$(VTK_LIBRARY_SUFFIX)             -lvtkFiltersModeling$(VTK_LIBRARY_SUFFIX)         -lvtkGeovisCore$(VTK_LIBRARY_SUFFIX)           -lvtkImagingStencil$(VTK_LIBRARY_SUFFIX)        -lvtkIOExportPDF$(VTK_LIBRARY_SUFFIX)         -lvtkIOPLY$(VTK_LIBRARY_SUFFIX)          -lvtklz4$(VTK_LIBRARY_SUFFIX)             -lvtkRenderingFreeType$(VTK_LIBRARY_SUFFIX)      -lvtktheora$(VTK_LIBRARY_SUFFIX) \
-lvtkdoubleconversion$(VTK_LIBRARY_SUFFIX)             -lvtkFiltersParallel$(VTK_LIBRARY_SUFFIX)         -lvtkgl2ps$(VTK_LIBRARY_SUFFIX)                -lvtkInfovisCore$(VTK_LIBRARY_SUFFIX)           -lvtkIOGeometry$(VTK_LIBRARY_SUFFIX)          -lvtkIOSegY$(VTK_LIBRARY_SUFFIX)         -lvtklzma$(VTK_LIBRARY_SUFFIX)            -lvtkRenderingGL2PSOpenGL2$(VTK_LIBRARY_SUFFIX)  -lvtktiff$(VTK_LIBRARY_SUFFIX) \
-lvtkexodusII$(VTK_LIBRARY_SUFFIX)                     -lvtkFiltersParallelImaging$(VTK_LIBRARY_SUFFIX)  -lvtkglew$(VTK_LIBRARY_SUFFIX)                 -lvtkInfovisLayout$(VTK_LIBRARY_SUFFIX)         -lvtkIOImage$(VTK_LIBRARY_SUFFIX)             -lvtkIOSQL$(VTK_LIBRARY_SUFFIX)          -lvtkmetaio$(VTK_LIBRARY_SUFFIX)          -lvtkRenderingImage$(VTK_LIBRARY_SUFFIX)         -lvtkverdict$(VTK_LIBRARY_SUFFIX) \
-lpython3
endif


convert_CXXFLAGS  = $(PEANO_CXXFLAGS) $(OPENMP_CXXFLAGS) 
convert_LDADD     = -lTarch
convert_LDFLAGS   = -L$(top_builddir)/src $(PEANO_LDFLAGS) $(OPENMP_CXXFLAGS) $(LDFLAGS) -lstdc++fs $(VTK_REQUIRED_LIBS)
convert_SOURCES   = $(convertSources)

convert_debug_CXXFLAGS  = $(PEANO_CXXFLAGS) $(OPENMP_CXXFLAGS) -O0 -g3 -DPeanoDebug=4 
convert_debug_LDADD     = -lTarch_debug
convert_debug_LDFLAGS   = -L$(top_builddir)/src $(PEANO_LDFLAGS) $(OPENMP_CXXFLAGS) $(LDFLAGS) -lstdc++fs $(VTK_REQUIRED_LIBS)
convert_debug_SOURCES   = $(convertSources)

