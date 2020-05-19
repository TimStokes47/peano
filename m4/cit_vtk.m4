##############################################################################
# -*- Autoconf -*-
#
# Implements the AM_OPTIONS_VTK and AM_PATH_VTK macros.
# The AM_OPTIONS_VTK macro adds the --with-vtk=path option,
# and the AM_PATH_VTK macro is used to detect VTK presence,
# location and version.
#
# Modified from https://github.com/geodynamics/autoconf_cig/blob/1b6f2ed72d2365e0e9c3919321acabe57020e74c/cit_vtk.m4
#

##############################################################################
#
# CIT_OPTIONS_VTK
#
# Adds the --with-vtk=PATH option to the configure options
# Adds the --with-vtk-version=MAJOR.MINOR option to the configure options
#
AC_DEFUN([CIT_OPTIONS_VTK],[

    # vtk install prefix
    AC_ARG_WITH([vtk],
                [AC_HELP_STRING(
                    [--with-vtk],
                    [The prefix where VTK is installed @<:@default=/usr@:>@])],
                [with_vtk="$withval"],
                [with_vtk="/usr"])

    # alternatively, derive suffix from version number
    AC_ARG_WITH([vtk-version],
                [AC_HELP_STRING(
                    [--with-vtk-version],
                    [Version number of VTK library, e.g., for vtk-7.1/ the version number is "-7.1"])],
                [vtk_version="$withval"],
                [vtk_version=""])


    # detect largest version number in given path
    if test x$vtk_version == "x"; then
        vtk_version=$(ls $with_vtk/include/ | grep vtk | sort -r | head -n1 | tr '-' '\n' | tail -n1 )
        if test x$vtk_version != "x"; then
           AC_MSG_NOTICE([Detected VTK version $vtk_version at VTK prefix '$with_vtk'])
        else
           AC_MSG_ERROR([No VTK manually specified and no VTK version could be detected at VTK prefix '$with_vtk'!])
        fi
    fi
    vtk_suffix="-$vtk_version"
    if test x$vtk_version == "x"; then # might not be a suffix present
       vtk_suffix=""
    fi
])

##############################################################################
#
# CIT_PATH_VTK([minimum-version], [action-if-found], [action-if-not-found])
#
# NOTE: [minimum-version] must be in the form [X.Y.Z]
#
AC_DEFUN([CIT_PATH_VTK],[

    dnl do we want to check for VTK?
    if test x$with_vtk = "xyes"; then
        dnl in case user wrote --with-vtk=yes
        with_vtk="/usr"
    fi

    if test x$with_vtk != "xno"; then
        VTK_PREFIX="$with_vtk"

        # note: for VTK version 7.0 the file vtkCommonInstantiator.h no longer exists,
        #       but has been renamed to vtkInstantiator.h instead
        #AC_CHECK_FILE([$VTK_PREFIX/include/vtk$vtk_suffix/vtkCommonInstantiator.h],
        #              [vtkFound="OK"])

        # checks for common file: vtkVersion.h
        AC_CHECK_FILE([$VTK_PREFIX/include/vtk$vtk_suffix/vtkVersion.h],
                                    [vtkFound="OK"])

        AC_MSG_CHECKING([if VTK is installed in $VTK_PREFIX])

        if test -z "$vtkFound"; then

            dnl VTK not found!
            AC_MSG_RESULT([no])
            $3

        else

            dnl VTK found!
            AC_MSG_RESULT([yes])

            dnl remember the old flags
            AC_LANG_PUSH([C])
            OLD_CFLAGS=$CFLAGS
            OLD_CXXFLAGS=$CXXFLAGS
            OLD_LDFLAGS=$LDFLAGS
            OLD_LIBS=$LIBS

            dnl these are the VTK libraries of a default build.
            dnl figure out vtkCommon, vtkIO, vtkFiltering, plus dependencies (in case VTK libs are static)
            dnl order of libs is significant

            dnl set VTK flags
            VTK_INCLUDES="-I$VTK_PREFIX/include/vtk$vtk_suffix"

            # note: versions 6+ change library names
            maj=`echo $vtk_version | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
            VTK_MAJOR=$maj
            AC_MSG_NOTICE([VTK version $vtk_version - major version number is $VTK_MAJOR])
            VTK_LDFLAGS=""
            if test -d "$VTK_PREFIX/lib/vtk$vtk_suffix" ; then VTK_LDFLAGS+="-L$VTK_PREFIX/lib/vtk$vtk_suffix "; fi
            if test -d "$VTK_PREFIX/lib64/vtk$vtk_suffix" ; then VTK_LDFLAGS+="-L$VTK_PREFIX/lib64/vtk$vtk_suffix "; fi
            if test "${VTK_MAJOR}" -gt "5" ; then
              # for vtk versions 6+
              if test -d "$VTK_PREFIX/lib" ; then VTK_LDFLAGS+="-L$VTK_PREFIX/lib "; fi
            fi

            dnl in order to be able to compile the following test programs,
            dnl we need to add to our VTK settings to the current flags
            CFLAGS="$VTK_CFLAGS $CFLAGS"
            CXXFLAGS="$VTK_CXXFLAGS $CXXFLAGS"
            LDFLAGS="$VTK_LDFLAGS $LDFLAGS"
            AC_MSG_NOTICE([checking with linker flags $LDFLAGS])

            # tests common vtkIO library
            if test "${VTK_MAJOR}" -gt "5" ; then
              # for vtk versions 6+
              lib_vtkIO=vtkIOCore$vtk_suffix
            else
              # for vtk versions <= 5
              lib_vtkIO=vtkIO
            fi
            AC_MSG_NOTICE([checking VTK library file: -l$lib_vtkIO])
            found_vtk_lib=no
            AC_CHECK_LIB($lib_vtkIO, main, [found_vtk_lib=yes],
              [if test "x$VTK_LIBS" != "x" ; then
                LIBS="$LIBS -L$VTK_LIBS"
                AC_MSG_NOTICE([  ... with LIBS: $LIBS])
                AC_CHECK_LIB($lib_vtkIO, main, [found_vtk_lib=yes],
                [ if test "${VTK_MAJOR}" -gt "5" ; then
                    # checks name without suffix
                    vtk_suffix=""
                    lib_vtkIO=vtkIOCore$vtk_suffix
                    AC_CHECK_LIB($lib_vtkIO, main, [found_vtk_lib=yes],[])
                  fi
                ])
               fi
            ])
            if test "$found_vtk_lib" != "yes"; then
              # abort in case not found
              AC_MSG_ERROR([could not find VTK libraries])
            fi

            # additional libraries
            if test "${VTK_MAJOR}" -gt "5" ; then
              # for vtk versions 6+
              VTK_SUPPORT_LIBS="-lvtktiff$vtk_suffix -lvtkpng$vtk_suffix -lvtkjpeg$vtk_suffix -lvtkzlib$vtk_suffix -lvtkexpat$vtk_suffix -lvfw32 -lgdi32"
            else
              # for vtk versions <= 5
              VTK_SUPPORT_LIBS="-lvtktiff -lvtkpng -lvtkjpeg -lvtkzlib -lvtkexpat -lvfw32 -lgdi32"
            fi
            if test "${VTK_MAJOR}" -gt "5" ; then
              # version 6+
              AC_CHECK_LIB(vtkIOXML$vtk_suffix, main)
              AC_CHECK_LIB(vtkIOImage$vtk_suffix, main)
              AC_CHECK_LIB(vtkDICOMParser$vtk_suffix, main)
              AC_CHECK_LIB(vtkRenderingCore$vtk_suffix, main)
              AC_CHECK_LIB(vtkRenderingLabel$vtk_suffix, main)
              AC_CHECK_LIB(vtkRenderingAnnotation$vtk_suffix, main)
              AC_CHECK_LIB(vtkFiltersCore$vtk_suffix, main)
              AC_CHECK_LIB(vtkFiltersGeneric$vtk_suffix, main)
              AC_CHECK_LIB(vtkCommonCore$vtk_suffix, main)
              AC_CHECK_LIB(vtkInteractionStyle$vtk_suffix, main)
              AC_CHECK_LIB(vtkzlib$vtk_suffix, main)
              AC_CHECK_LIB(vtkexpat$vtk_suffix, main)
              AC_CHECK_LIB(vtksys$vtk_suffix, main)
            else
              # version <= 5
              AC_CHECK_LIB(vtkDICOMParser, main)
              AC_CHECK_LIB(vtkRendering, main)
              AC_CHECK_LIB(vtkGraphics, main)
              AC_CHECK_LIB(vtkFiltering, main)
              AC_CHECK_LIB(vtkGenericFiltering, main)
              AC_CHECK_LIB(vtkCommon, main)
              AC_CHECK_LIB(vtkzlib, main)
              AC_CHECK_LIB(vtkexpat, main)
              AC_CHECK_LIB(vtksys, main)
            fi
            AC_MSG_NOTICE([checking VTK library features in $lib_vtkIO])
            AC_CHECK_LIB($lib_vtkIO, strcmp, [], [
                # version check
                if test "${VTK_MAJOR}" -gt "5" ; then
                  # version 6+
                  VTK_SUPPORT_LIBS="-lvtktiff$vtk_suffix -lvtkpng$vtk_suffix -lvtkjpeg$vtk_suffix -lvtkzlib$vtk_suffix -lvtkexpat$vtk_suffix"
                else
                  # version <= 5
                  VTK_SUPPORT_LIBS="-lvtktiff -lvtkpng -lvtkjpeg -lvtkzlib -lvtkexpat"
                fi
                AC_CHECK_LIB($lib_vtkIO, abort, [], [
                    VTK_SUPPORT_LIBS="-ltiff -lpng -ljpeg -lz -lexpat"
                    AC_CHECK_LIB($lib_vtkIO, exit, [], [
                        VTK_SUPPORT_LIBS=""
                        AC_CHECK_LIB($lib_vtkIO, strstr, [], [
                            AC_MSG_ERROR([cannot link against VTK libraries])
                        ], [$VTK_SUPPORT_LIBS])
                    ], [$VTK_SUPPORT_LIBS])
                ], [$VTK_SUPPORT_LIBS])
            ], [$VTK_SUPPORT_LIBS])

            if test "${VTK_MAJOR}" -gt "5" ; then
              VTK_LIBS="-l$lib_vtkIO -lvtkIOXML$vtk_suffix -lvtkIOImage$vtk_suffix -lvtkDICOMParser$vtk_suffix -lvtkRenderingCore$vtk_suffix -lvtkRenderingLabel$vtk_suffix -lvtkRenderingAnnotation$vtk_suffix -lvtkFiltersCore$vtk_suffix -lvtkFiltersGeneric$vtk_suffix -lvtkCommonCore$vtk_suffix -lvtkInteractionStyle$vtk_suffix $VTK_SUPPORT_LIBS -lvtksys$vtk_suffix"
            else
              VTK_LIBS="-l$lib_vtkIO -lvtkDICOMParser -lvtkRendering -lvtkGraphics -lvtkFiltering -lvtkGenericFiltering -lvtkCommon $VTK_SUPPORT_LIBS -lvtksys"
            fi
            LIBS="$VTK_LIBS $LIBS"

            dnl now, check that we don't exceed the specified version
            if test -n "$1"; then
                dnl the version of VTK we need

                maj=`echo $1 | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
                min=`echo $1 | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
                rel=`echo $1 | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
                AC_MSG_CHECKING([if VTK version is at least $maj.$min.$rel])

                dnl check if the installed VTK is greater or not
                AC_COMPILE_IFELSE([
                    AC_LANG_PROGRAM([[
                        #include <vtk/vtkConfigure.h>
                        #include <stdio.h>
                        ]],[[
                        printf("VTK version is: %d.%d.%d",
                               VTK_MAJOR_VERSION,
                               VTK_MINOR_VERSION,
                               VTK_BUILD_VERSION);

                        #if VTK_MAJOR_VERSION < $maj
                        #error Installed VTK is too old!
                        #endif

                        #if VTK_MINOR_VERSION < $min
                        #error Installed VTK is too old!
                        #endif

                        #if VTK_BUILD_VERSION < $rel
                        #error Installed VTK is too old!
                        #endif
                    ]])
                ], [vtkVersion="OK"])

                dnl restore all flags without VTK values
                CFLAGS=$OLD_CFLAGS
                CXXFLAGS=$OLD_CXXFLAGS
                LDFLAGS=$OLD_LDFLAGS
                LIBS=$OLD_LIBS

                if test "$vtkVersion" = "OK"; then
                    AC_MSG_RESULT([yes])
                    $2
                else
                    AC_MSG_RESULT([no])
                    $3
                fi

            else

                dnl restore all flags without VTK values
                CFLAGS=$OLD_CFLAGS
                CXXFLAGS=$OLD_CXXFLAGS
                LDFLAGS=$OLD_LDFLAGS
                LIBS=$OLD_LIBS

                dnl if we don't have to check for minimum version
                dnl (because the user did not set that option),
                dnl then we can execute here the block action-if-found
                $2

            fi          #if [[ -n "$1" ]];

            dnl Don't move this up to where we restore flags, because it doesn't work.
            AC_LANG_POP([C])

            dnl Finally, define the C preprocessor variable HAVE_VTK
            AC_DEFINE([HAVE_VTK],,[define if the VTK library is available])

        fi          # if [[ -z "$vtkFound" ]];
    fi          # $with_vtk != "no"

    dnl Substitute the values of our VTK flags
    AC_SUBST(VTK_MAJOR)
    AC_SUBST(VTK_INCLUDES)
    AC_SUBST(VTK_LDFLAGS)
    AC_SUBST(VTK_LIBS)
])



# vim: syntax=config

# TODO(Dominic): Keep the following in case we need specific libraries
##
## find . -name 'libvtk*.so*' -exec nm --print-file-name --defined-only --dynamic {} \; | grep "vtkFilter"
##  
#if VTK_LIBRARY_VERSION_7
#VTK_REQUIRED_LIBS     = -lvtkalglib$(VTK_LIBRARY_SUFFIX) \
#						-lvtkChartsCore$(VTK_LIBRARY_SUFFIX) \
#						-lvtkChartsCorePython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkChartsCoreTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkCommonColor$(VTK_LIBRARY_SUFFIX) \
#						-lvtkCommonColorPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkCommonColorTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkCommonComputationalGeometry$(VTK_LIBRARY_SUFFIX) \
#						-lvtkCommonComputationalGeometryPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkCommonComputationalGeometryTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkCommonCore$(VTK_LIBRARY_SUFFIX) \
#						-lvtkCommonCorePython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkCommonCoreTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkCommonDataModel$(VTK_LIBRARY_SUFFIX) \
#						-lvtkCommonDataModelPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkCommonDataModelTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkCommonExecutionModel$(VTK_LIBRARY_SUFFIX) \
#						-lvtkCommonExecutionModelPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkCommonExecutionModelTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkCommonMath$(VTK_LIBRARY_SUFFIX) \
#						-lvtkCommonMathPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkCommonMathTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkCommonMisc$(VTK_LIBRARY_SUFFIX) \
#						-lvtkCommonMiscPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkCommonMiscTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkCommonSystem$(VTK_LIBRARY_SUFFIX) \
#						-lvtkCommonSystemPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkCommonSystemTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkCommonTransforms$(VTK_LIBRARY_SUFFIX) \
#						-lvtkCommonTransformsPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkCommonTransformsTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkDICOMParser$(VTK_LIBRARY_SUFFIX) \
#						-lvtkDomainsChemistry$(VTK_LIBRARY_SUFFIX) \
#						-lvtkDomainsChemistryOpenGL2$(VTK_LIBRARY_SUFFIX) \
#						-lvtkDomainsChemistryOpenGL2Python36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkDomainsChemistryPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkDomainsChemistryTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkexoIIc$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersAMR$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersAMRPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersAMRTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersCore$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersCorePython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersCoreTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersExtraction$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersExtractionPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersExtractionTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersFlowPaths$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersFlowPathsPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersFlowPathsTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersGeneral$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersGeneralPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersGeneralTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersGeneric$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersGenericPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersGenericTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersGeometry$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersGeometryPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersGeometryTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersHybrid$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersHybridPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersHybridTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersHyperTree$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersHyperTreePython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersHyperTreeTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersImaging$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersImagingPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersImagingTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersModeling$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersModelingPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersModelingTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersParallel$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersParallelDIY2$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersParallelDIY2Python36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersParallelDIYIITCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersParallelFlowPaths$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersParallelFlowPathsPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersParallelFlowPathsTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersParallelGeometry$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersParallelGeometryPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersParallelGeometryTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersParallelImaging$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersParallelImagingPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersParallelImagingTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersParallelMPI$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersParallelMPIPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersParallelMPITCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersParallelPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersParallelStatistics$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersParallelStatisticsPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersParallelStatisticsTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersParallelTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersPoints$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersPointsPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersPointsTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersProgrammable$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersProgrammablePython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersProgrammableTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersPython$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersPythonPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersReebGraph$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersReebGraphPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersReebGraphTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersSelection$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersSelectionPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersSelectionTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersSMP$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersSMPPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersSMPTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersSources$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersSourcesPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersSourcesTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersStatistics$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersStatisticsPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersStatisticsTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersTexture$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersTexturePython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersTextureTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersVerdict$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersVerdictPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkFiltersVerdictTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkGeovisCore$(VTK_LIBRARY_SUFFIX) \
#						-lvtkGeovisCorePython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkGeovisCoreTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkImagingColor$(VTK_LIBRARY_SUFFIX) \
#						-lvtkImagingColorPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkImagingColorTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkImagingCore$(VTK_LIBRARY_SUFFIX) \
#						-lvtkImagingCorePython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkImagingCoreTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkImagingFourier$(VTK_LIBRARY_SUFFIX) \
#						-lvtkImagingFourierPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkImagingFourierTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkImagingGeneral$(VTK_LIBRARY_SUFFIX) \
#						-lvtkImagingGeneralPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkImagingGeneralTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkImagingHybrid$(VTK_LIBRARY_SUFFIX) \
#						-lvtkImagingHybridPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkImagingHybridTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkImagingMath$(VTK_LIBRARY_SUFFIX) \
#						-lvtkImagingMathPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkImagingMathTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkImagingMorphological$(VTK_LIBRARY_SUFFIX) \
#						-lvtkImagingMorphologicalPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkImagingMorphologicalTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkImagingSources$(VTK_LIBRARY_SUFFIX) \
#						-lvtkImagingSourcesPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkImagingSourcesTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkImagingStatistics$(VTK_LIBRARY_SUFFIX) \
#						-lvtkImagingStatisticsPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkImagingStatisticsTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkImagingStencil$(VTK_LIBRARY_SUFFIX) \
#						-lvtkImagingStencilPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkImagingStencilTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkInfovisBoostGraphAlgorithms$(VTK_LIBRARY_SUFFIX) \
#						-lvtkInfovisBoostGraphAlgorithmsPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkInfovisBoostGraphAlgorithmsTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkInfovisCore$(VTK_LIBRARY_SUFFIX) \
#						-lvtkInfovisCorePython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkInfovisCoreTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkInfovisLayout$(VTK_LIBRARY_SUFFIX) \
#						-lvtkInfovisLayoutPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkInfovisLayoutTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkInteractionImage$(VTK_LIBRARY_SUFFIX) \
#						-lvtkInteractionImagePython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkInteractionImageTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkInteractionStyle$(VTK_LIBRARY_SUFFIX) \
#						-lvtkInteractionStylePython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkInteractionStyleTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkInteractionWidgets$(VTK_LIBRARY_SUFFIX) \
#						-lvtkInteractionWidgetsPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkInteractionWidgetsTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOAMR$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOAMRPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOAMRTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOCore$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOCorePython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOCoreTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOEnSight$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOEnSightPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOEnSightTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOExodus$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOExodusPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOExodusTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOExport$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOExportPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOExportTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOFFMPEG$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOFFMPEGPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOFFMPEGTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOGDAL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOGDALPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOGDALTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOGeoJSON$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOGeoJSONPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOGeoJSONTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOGeometry$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOGeometryPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOGeometryTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOImage$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOImagePython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOImageTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOImport$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOImportPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOImportTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOInfovis$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOInfovisPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOInfovisTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOLegacy$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOLegacyPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOLegacyTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOLSDyna$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOLSDynaPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOLSDynaTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOMINC$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOMINCPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOMINCTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOMovie$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOMoviePython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOMovieTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOMPIImage$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOMPIImagePython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOMPIImageTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOMPIParallel$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOMPIParallelPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOMPIParallelTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOMySQL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOMySQLPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOMySQLTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIONetCDF$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIONetCDFPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIONetCDFTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOODBC$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOODBCPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOODBCTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOParallel$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOParallelExodus$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOParallelExodusPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOParallelExodusTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOParallelLSDyna$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOParallelLSDynaPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOParallelLSDynaTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOParallelNetCDF$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOParallelNetCDFPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOParallelNetCDFTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOParallelPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOParallelTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOParallelXML$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOParallelXMLPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOParallelXMLTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOPLY$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOPLYPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOPLYTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOPostgreSQL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOPostgreSQLPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOPostgreSQLTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOSQL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOSQLPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOSQLTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOTecplotTable$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOTecplotTablePython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOTecplotTableTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOVideo$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOVideoPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOVideoTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOVPIC$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOVPICPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOVPICTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOXdmf2$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOXdmf2Python36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOXdmfIITCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOXML$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOXMLParser$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOXMLParserPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOXMLParserTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOXMLPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkIOXMLTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkLocalExample$(VTK_LIBRARY_SUFFIX) \
#						-lvtkLocalExampleTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkmetaio$(VTK_LIBRARY_SUFFIX) \
#						-lvtkParallelCore$(VTK_LIBRARY_SUFFIX) \
#						-lvtkParallelCorePython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkParallelCoreTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkParallelMPI4Py$(VTK_LIBRARY_SUFFIX) \
#						-lvtkParallelMPI4PyPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkParallelMPI$(VTK_LIBRARY_SUFFIX) \
#						-lvtkParallelMPIPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkParallelMPITCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkPythonInterpreter$(VTK_LIBRARY_SUFFIX) \
#						-lvtkPythonInterpreterPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkPythonInterpreterTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingAnnotation$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingAnnotationPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingAnnotationTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingChemistryOpenGLIITCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingContext2D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingContext2DPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingContextIIDTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingContextOpenGL2$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingContextOpenGL2Python36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingContextOpenGLIITCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingCore$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingCorePython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingCoreTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingExternal$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingExternalPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingExternalTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingFreeType$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingFreeTypeFontConfig$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingFreeTypePython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingFreeTypeTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingGL2PSOpenGL2$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingGL2PSOpenGL2Python36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingGLtoPSOpenGLIITCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingImage$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingImagePython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingImageTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingLabel$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingLabelPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingLabelTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingLICOpenGL2$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingLICOpenGL2Python36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingLICOpenGLIITCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingLOD$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingLODPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingLODTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingMatplotlib$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingMatplotlibPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingMatplotlibTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingOpenGL2$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingOpenGL2Python36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingOpenGLIITCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingParallel$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingParallelLIC$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingParallelLICPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingParallelLICTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingParallelPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingParallelTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingSceneGraph$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingSceneGraphPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingSceneGraphTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingTkTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingVolume$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingVolumeAMR$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingVolumeAMRPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingVolumeAMRTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingVolumeOpenGL2$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingVolumeOpenGL2Python36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingVolumeOpenGLIITCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingVolumePython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkRenderingVolumeTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtksys$(VTK_LIBRARY_SUFFIX) \
#						-lvtkTestingGenericBridge$(VTK_LIBRARY_SUFFIX) \
#						-lvtkTestingIOSQL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkTestingRendering$(VTK_LIBRARY_SUFFIX) \
#						-lvtkTestingRenderingPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkTestingRenderingTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkverdict$(VTK_LIBRARY_SUFFIX) \
#						-lvtkViewsContext2D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkViewsContext2DPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkViewsContextIIDTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkViewsCore$(VTK_LIBRARY_SUFFIX) \
#						-lvtkViewsCorePython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkViewsCoreTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkViewsGeovis$(VTK_LIBRARY_SUFFIX) \
#						-lvtkViewsGeovisPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkViewsGeovisTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkViewsInfovis$(VTK_LIBRARY_SUFFIX) \
#						-lvtkViewsInfovisPython36D$(VTK_LIBRARY_SUFFIX) \
#						-lvtkViewsInfovisTCL$(VTK_LIBRARY_SUFFIX) \
#						-lvtkVPIC$(VTK_LIBRARY_SUFFIX) \
#						-lvtkWrappingJava$(VTK_LIBRARY_SUFFIX) \
#						-lvtkWrappingPython36Core$(VTK_LIBRARY_SUFFIX) \
#						-lvtkxdmf2$(VTK_LIBRARY_SUFFIX)
#endif
#
#if VTK_LIBRARY_VERSION_8
#VTK_REQUIRED_LIBS     =  -lvtkIOXML$(VTK_LIBRARY_SUFFIX) \
#                         -lvtkCommonExecutionModel$(VTK_LIBRARY_SUFFIX) \
#                         -lvtkCommonCore$(VTK_LIBRARY_SUFFIX) \
#                         -lvtkCommonDataModel$(VTK_LIBRARY_SUFFIX) \
#                         -lvtkFiltersCore$(VTK_LIBRARY_SUFFIX) \
#                         -lvtkFiltersGeneral$(VTK_LIBRARY_SUFFIX) \
#                         -lvtkIOXMLParser$(VTK_LIBRARY_SUFFIX) \
#                         -lvtkIOCore$(VTK_LIBRARY_SUFFIX) \
#                         -lvtkCommonSystem$(VTK_LIBRARY_SUFFIX) \
#                         -lvtkCommonTransforms$(VTK_LIBRARY_SUFFIX) \
#                         -lvtkCommonMisc$(VTK_LIBRARY_SUFFIX) \
#                         -lvtkCommonMath$(VTK_LIBRARY_SUFFIX) \
#                         -lvtksys$(VTK_LIBRARY_SUFFIX) 
#endif
#
#
#if VTK_LIBRARY_VERSION_9
#VTK_REQUIRED_LIBS     =  \
#-lvtkChartsCore$(VTK_LIBRARY_SUFFIX)                   -lvtkexpat$(VTK_LIBRARY_SUFFIX)                   -lvtkFiltersPoints$(VTK_LIBRARY_SUFFIX)        -lvtkhdf5$(VTK_LIBRARY_SUFFIX)                  -lvtkInteractionImage$(VTK_LIBRARY_SUFFIX)    -lvtkIOImport$(VTK_LIBRARY_SUFFIX)       -lvtkIOTecplotTable$(VTK_LIBRARY_SUFFIX)  -lvtknetcdf$(VTK_LIBRARY_SUFFIX)                 -lvtkRenderingLabel$(VTK_LIBRARY_SUFFIX)          -lvtkViewsContext2D$(VTK_LIBRARY_SUFFIX) \
#-lvtkCommonColor$(VTK_LIBRARY_SUFFIX)                  -lvtkFiltersAMR$(VTK_LIBRARY_SUFFIX)              -lvtkFiltersProgrammable$(VTK_LIBRARY_SUFFIX)  -lvtkhdf5_hl$(VTK_LIBRARY_SUFFIX)               -lvtkInteractionStyle$(VTK_LIBRARY_SUFFIX)    -lvtkIOInfovis$(VTK_LIBRARY_SUFFIX)      -lvtkIOVeraOut$(VTK_LIBRARY_SUFFIX)       -lvtkogg$(VTK_LIBRARY_SUFFIX)                    -lvtkRenderingLOD$(VTK_LIBRARY_SUFFIX)            -lvtkViewsCore$(VTK_LIBRARY_SUFFIX) \
#-lvtkCommonComputationalGeometry$(VTK_LIBRARY_SUFFIX)  -lvtkFiltersCore$(VTK_LIBRARY_SUFFIX)             -lvtkFiltersPython$(VTK_LIBRARY_SUFFIX)        -lvtkImagingColor$(VTK_LIBRARY_SUFFIX)          -lvtkInteractionWidgets$(VTK_LIBRARY_SUFFIX)  -lvtkIOLegacy$(VTK_LIBRARY_SUFFIX)       -lvtkIOVideo$(VTK_LIBRARY_SUFFIX)         -lvtkParallelCore$(VTK_LIBRARY_SUFFIX)           -lvtkRenderingOpenGL2$(VTK_LIBRARY_SUFFIX)        -lvtkViewsInfovis$(VTK_LIBRARY_SUFFIX) \
#-lvtkCommonCore$(VTK_LIBRARY_SUFFIX)                   -lvtkFiltersExtraction$(VTK_LIBRARY_SUFFIX)       -lvtkFiltersSelection$(VTK_LIBRARY_SUFFIX)     -lvtkImagingCore$(VTK_LIBRARY_SUFFIX)           -lvtkIOAMR$(VTK_LIBRARY_SUFFIX)               -lvtkIOLSDyna$(VTK_LIBRARY_SUFFIX)       -lvtkIOXML$(VTK_LIBRARY_SUFFIX)           -lvtkParallelDIY$(VTK_LIBRARY_SUFFIX)            -lvtkRenderingSceneGraph$(VTK_LIBRARY_SUFFIX)     -lvtkWrappingPythonCore$(VTK_LIBRARY_SUFFIX) \
#-lvtkCommonDataModel$(VTK_LIBRARY_SUFFIX)              -lvtkFiltersFlowPaths$(VTK_LIBRARY_SUFFIX)        -lvtkFiltersSMP$(VTK_LIBRARY_SUFFIX)           -lvtkImagingFourier$(VTK_LIBRARY_SUFFIX)        -lvtkIOAsynchronous$(VTK_LIBRARY_SUFFIX)      -lvtkIOMINC$(VTK_LIBRARY_SUFFIX)         -lvtkIOXMLParser$(VTK_LIBRARY_SUFFIX)     -lvtkpng$(VTK_LIBRARY_SUFFIX)                    -lvtkRenderingUI$(VTK_LIBRARY_SUFFIX)             -lvtkWrappingTools$(VTK_LIBRARY_SUFFIX) \
#-lvtkCommonExecutionModel$(VTK_LIBRARY_SUFFIX)         -lvtkFiltersGeneral$(VTK_LIBRARY_SUFFIX)          -lvtkFiltersSources$(VTK_LIBRARY_SUFFIX)       -lvtkImagingGeneral$(VTK_LIBRARY_SUFFIX)        -lvtkIOCityGML$(VTK_LIBRARY_SUFFIX)           -lvtkIOMotionFX$(VTK_LIBRARY_SUFFIX)     -lvtkjpeg$(VTK_LIBRARY_SUFFIX)            -lvtkpugixml$(VTK_LIBRARY_SUFFIX)                -lvtkRenderingVolume$(VTK_LIBRARY_SUFFIX)         -lvtkzlib$(VTK_LIBRARY_SUFFIX) \
#-lvtkCommonMath$(VTK_LIBRARY_SUFFIX)                   -lvtkFiltersGeneric$(VTK_LIBRARY_SUFFIX)          -lvtkFiltersStatistics$(VTK_LIBRARY_SUFFIX)    -lvtkImagingHybrid$(VTK_LIBRARY_SUFFIX)         -lvtkIOCore$(VTK_LIBRARY_SUFFIX)              -lvtkIOMovie$(VTK_LIBRARY_SUFFIX)        -lvtkjsoncpp$(VTK_LIBRARY_SUFFIX)         -lvtkPythonContext2D$(VTK_LIBRARY_SUFFIX)        -lvtkRenderingVolumeOpenGL2$(VTK_LIBRARY_SUFFIX) \
#-lvtkCommonMisc$(VTK_LIBRARY_SUFFIX)                   -lvtkFiltersGeometry$(VTK_LIBRARY_SUFFIX)         -lvtkFiltersTexture$(VTK_LIBRARY_SUFFIX)       -lvtkImagingMath$(VTK_LIBRARY_SUFFIX)           -lvtkIOEnSight$(VTK_LIBRARY_SUFFIX)           -lvtkIONetCDF$(VTK_LIBRARY_SUFFIX)       -lvtklibharu$(VTK_LIBRARY_SUFFIX)         -lvtkPythonInterpreter$(VTK_LIBRARY_SUFFIX)      -lvtkRenderingVtkJS$(VTK_LIBRARY_SUFFIX) \
#-lvtkCommonSystem$(VTK_LIBRARY_SUFFIX)                 -lvtkFiltersHybrid$(VTK_LIBRARY_SUFFIX)           -lvtkFiltersTopology$(VTK_LIBRARY_SUFFIX)      -lvtkImagingMorphological$(VTK_LIBRARY_SUFFIX)  -lvtkIOExodus$(VTK_LIBRARY_SUFFIX)            -lvtkIOOggTheora$(VTK_LIBRARY_SUFFIX)    -lvtklibproj$(VTK_LIBRARY_SUFFIX)         -lvtkRenderingAnnotation$(VTK_LIBRARY_SUFFIX)    -lvtksqlite$(VTK_LIBRARY_SUFFIX) \
#-lvtkCommonTransforms$(VTK_LIBRARY_SUFFIX)             -lvtkFiltersHyperTree$(VTK_LIBRARY_SUFFIX)        -lvtkFiltersVerdict$(VTK_LIBRARY_SUFFIX)       -lvtkImagingSources$(VTK_LIBRARY_SUFFIX)        -lvtkIOExport$(VTK_LIBRARY_SUFFIX)            -lvtkIOParallel$(VTK_LIBRARY_SUFFIX)     -lvtklibxml2$(VTK_LIBRARY_SUFFIX)         -lvtkRenderingContext2D$(VTK_LIBRARY_SUFFIX)     -lvtksys$(VTK_LIBRARY_SUFFIX) \
#-lvtkDICOMParser$(VTK_LIBRARY_SUFFIX)                  -lvtkFiltersImaging$(VTK_LIBRARY_SUFFIX)          -lvtkfreetype$(VTK_LIBRARY_SUFFIX)             -lvtkImagingStatistics$(VTK_LIBRARY_SUFFIX)     -lvtkIOExportGL2PS$(VTK_LIBRARY_SUFFIX)       -lvtkIOParallelXML$(VTK_LIBRARY_SUFFIX)  -lvtkloguru$(VTK_LIBRARY_SUFFIX)          -lvtkRenderingCore$(VTK_LIBRARY_SUFFIX)          -lvtkTestingRendering$(VTK_LIBRARY_SUFFIX) \
#-lvtkDomainsChemistry$(VTK_LIBRARY_SUFFIX)             -lvtkFiltersModeling$(VTK_LIBRARY_SUFFIX)         -lvtkGeovisCore$(VTK_LIBRARY_SUFFIX)           -lvtkImagingStencil$(VTK_LIBRARY_SUFFIX)        -lvtkIOExportPDF$(VTK_LIBRARY_SUFFIX)         -lvtkIOPLY$(VTK_LIBRARY_SUFFIX)          -lvtklz4$(VTK_LIBRARY_SUFFIX)             -lvtkRenderingFreeType$(VTK_LIBRARY_SUFFIX)      -lvtktheora$(VTK_LIBRARY_SUFFIX) \
#-lvtkdoubleconversion$(VTK_LIBRARY_SUFFIX)             -lvtkFiltersParallel$(VTK_LIBRARY_SUFFIX)         -lvtkgl2ps$(VTK_LIBRARY_SUFFIX)                -lvtkInfovisCore$(VTK_LIBRARY_SUFFIX)           -lvtkIOGeometry$(VTK_LIBRARY_SUFFIX)          -lvtkIOSegY$(VTK_LIBRARY_SUFFIX)         -lvtklzma$(VTK_LIBRARY_SUFFIX)            -lvtkRenderingGL2PSOpenGL2$(VTK_LIBRARY_SUFFIX)  -lvtktiff$(VTK_LIBRARY_SUFFIX) \
#-lvtkexodusII$(VTK_LIBRARY_SUFFIX)                     -lvtkFiltersParallelImaging$(VTK_LIBRARY_SUFFIX)  -lvtkglew$(VTK_LIBRARY_SUFFIX)                 -lvtkInfovisLayout$(VTK_LIBRARY_SUFFIX)         -lvtkIOImage$(VTK_LIBRARY_SUFFIX)             -lvtkIOSQL$(VTK_LIBRARY_SUFFIX)          -lvtkmetaio$(VTK_LIBRARY_SUFFIX)          -lvtkRenderingImage$(VTK_LIBRARY_SUFFIX)         -lvtkverdict$(VTK_LIBRARY_SUFFIX) \
#-lpython3
#endif

