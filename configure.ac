#
# Recreate build environment:
# ---------------------------
#   libtoolize
#   aclocal
#   autoconf
#   autoheader
#   automake --add-missing
#
#
# Summary:
#   libtoolize; aclocal; autoconf; autoheader; cp src/config.h.in .; automake --add-missing
#
#
# Cleanup
# -------
#   rm -rf aclocal.m4 compile src/config.* configure *sh Makefile Makefile.in missing stamp* ar-lib autom4te.cache *.log libtool depcomp *.scan  
#  
#
#
AC_INIT(Peano, 4.0)
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE(foreign -Wall -Werror)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_CONFIG_HEADERS([src/config.h])

AC_CANONICAL_BUILD

AC_PROG_INSTALL
AC_PROG_MAKE_SET

#
# This is just a sanity check whether the right files/directories do exist
#
AC_CONFIG_SRCDIR([src/peano4/peano.cpp])

#
# Pipe system-specific findings into a file config.h
#
AC_CONFIG_HEADERS([config.h])
   
#
# I wanna build libraries
#
AM_PROG_AR
LT_INIT
AC_CONFIG_MACRO_DIRS([m4])


# 
# Checks for programs.
#
AC_PROG_CXX


#
# Peano variables
#
#PEANO_CXXFLAGS="-std=c++14 -fconcepts"
PEANO_CXXFLAGS="-std=c++14"
PEANO_LDFLAGS=""

# Think I do not need a C compiler
#AC_PROG_CC


#
# OpenMP Options
# We need those separately from the Peano build as we use OpenMP for 
# postprocessing scripts, e.g.
#
AC_LANG(C++)
AC_OPENMP()


#
# TBB Options
#
# The result is stored in a variable multithreading
#
AC_ARG_WITH(
  multithreading, 
  AC_HELP_STRING([--with-multithreading], [switch on multithreading [default=no,cpp,tbb,openmp]]),
  [
    AC_MSG_CHECKING(for multithreading)
    case "${withval}" in
      no)     AC_MSG_RESULT([no multithreading]);                    multithreading=false ;;
      cpp)    AC_MSG_RESULT([cpp (C++11)]);                          multithreading=cpp ;;
      tbb)    AC_MSG_RESULT([tbb (Intel Threading Building Blocks]); multithreading=tbb ;;
      *)      AC_MSG_FAILURE([default=no (no multithreading). Use cpp or tbb to define target]);            multithreading=false ;;
    esac
  ], 
  [ 
    AC_MSG_CHECKING(for multithreading); 
    AC_MSG_RESULT(no); multithreading=false
  ]
)


if test "$multithreading" == cpp; then
  AC_DEFINE([SharedCPP], [1], [SharedCPP])
  AC_SEARCH_LIBS(pthread_create,pthread)
fi

if test "$multithreading" == tbb; then
  AC_CHECK_HEADERS([tbb.h],[],[AC_MSG_ERROR([header <tbb.h> not found])])
  AC_DEFINE([SharedTBB], [1], [SharedTBB])
fi



#
# MPI  Options
#
AC_ARG_WITH(mpi,
  AC_HELP_STRING([--with-mpi], [switch on distributed memory parallelisation [default=no, mpiCC command]]),
  [ AC_MSG_CHECKING(for mpi)
    case "${withval}" in
      no)    AC_MSG_RESULT(no);                              mpi=false ;;
      *)     AC_MSG_RESULT(use mpi c++ command ${withval});  mpi=${withval}  ;;
    esac ],
  [ AC_MSG_CHECKING(for mpi); AC_MSG_RESULT(no); mpi=false ])
  

if test "$mpi" != false; then
  CXX=$mpi
  AC_DEFINE([Parallel], [1], [Parallel])
fi



#
# VTK Options
#
AC_ARG_WITH(vtk,
  AC_HELP_STRING([--with-vtk], [enable VTK and thus build Peano's conversion tools, too [default=no, vtk include path (such as /usr/include/vtk-8.1)]]),
  [ AC_MSG_CHECKING(for vtk)
    case "${withval}" in
      no)    AC_MSG_RESULT(no);                               vtk=false ;;
      *)     AC_MSG_RESULT(use vtk sources path ${withval});  vtk=${withval}  ;;
    esac ],
  [ AC_MSG_CHECKING(for vtk); AC_MSG_RESULT(no); vtk=false ])
  

if test "$vtk" != false; then
  AC_DEFINE([UseVTK], [1], [UseVTK])
  PEANO_CXXFLAGS+=" -I${vtk}"
fi


#
# HDF5
#
AC_ARG_WITH(hdf5,
  AC_HELP_STRING([--with-hdf5], [enable HDF5 support [default=no, yes]]),
  [ AC_MSG_CHECKING(for hdf5)
    case "${withval}" in
      no)    AC_MSG_RESULT(no);                                hdf5=false ;;
      *)     AC_MSG_RESULT(use hdf5 sources path ${withval});  hdf5=true  ;;
    esac ],
  [ AC_MSG_CHECKING(for hdf5); AC_MSG_RESULT(no); hdf5=false ])
 
if test "$with_hdf5" = "yes"; then
  if test "$mpi" != false; then
    AX_LIB_HDF5([parallel])
  else
    AX_LIB_HDF5([serial])
  fi
  if test "$with_hdf5" = "yes"; then
    AC_DEFINE([UseHDF5], [1], [UseHDF5])
    PEANO_CXXFLAGS+=" ${HDF5_CPPFLAGS}"
    PEANO_LDFLAGS+=" ${HDF5_LDFLAGS} ${HDF5_LIBS}"
  else
    AC_MSG_ERROR([Unable to find HDF5, we need HDF5.])
  fi
fi 



#
# Delta
#
AC_ARG_WITH(delta,
  AC_HELP_STRING([--with-delta], [enable Delta [default=no, path to Delta include files]]),
  [ AC_MSG_CHECKING(for delta)
    case "${withval}" in
      no)    AC_MSG_RESULT(no);                                 delta=false ;;
      *)     AC_MSG_RESULT(use delta sources path ${withval});  delta=${withval}  ;;
    esac ],
  [ AC_MSG_CHECKING(for delta); AC_MSG_RESULT(no); delta=false ])
# This whole autotools thing is not very nice: If I wanna use an if ... endif (
# note the syntax which is different to the present file, then I have to declare
# the variable as I do in the line below. Otherwise, the if thing is not 
# "exported". Seems the autoconf and the makefile generator are completely 
# separate with inconsistent syntax.
AM_CONDITIONAL([UseDelta],[test "$delta" != false])
if test "$delta" != false; then
  AC_DEFINE([UseDelta], [1], [UseDelta])
  PEANO_CXXFLAGS+=" -I${delta}"
fi


#
# Export my Peano-specific flags
#
AC_SUBST([PEANO_CXXFLAGS])
AC_SUBST([PEANO_LDFLAGS])



#
# Kick out makefiles. I'm not sure but it seems that you have to add the directories to 
# Makefile.am in the src subdirectories, too
#
AC_OUTPUT(Makefile src/Makefile src/examples/grid/Makefile src/examples/integerdiffusionthroughfaces/Makefile src/examples/delta/Makefile src/visualisation/Makefile)

