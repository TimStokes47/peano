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
#   libtoolize; aclocal; autoconf; autoheader; automake --add-missing
#
#
# Cleanup
# -------
#   rm -rf aclocal.m4 compile config.* configure *sh Makefile Makefile.in missing stamp* ar-lib autom4te.cache *.log libtool depcomp *.scan m4 
#  
#
#
AC_PREREQ([2.69])
AC_INIT(Peano, 4.0)

AM_INIT_AUTOMAKE(foreign -Wall -Werror tar-ustar)

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([src/config.h])

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
# Switches off my multithreading feature if I add it.
#AX_CXX_COMPILE_STDCXX_11()


# Think I do not need a C compiler
#AC_PROG_CC


#
# TBB Options
#
# The result is stored in a variable multithreading
#
AC_ARG_ENABLE(
  multithreading, 
  AC_HELP_STRING([--enable-multithreading], [switch on multithreading [default=false,cpp,tbb,openmp]]),
  [
    AC_MSG_CHECKING(for multithreading)
    case "${enableval}" in
      false)  AC_MSG_RESULT([false (no multithreading)]);            multithreading=false ;;
      cpp)    AC_MSG_RESULT([cpp (C++11)]);                          multithreading=cpp ;;
      tbb)    AC_MSG_RESULT([tbb (Intel Threading Building Blocks]); multithreading=tbb ;;
      *)      AC_MSG_RESULT([default=false (no multithreading). Use cpp or tbb to define target]);            multithreading=false ;;
    esac
  ], 
  [ 
    AC_MSG_CHECKING(for multithreading); 
    AC_MSG_RESULT(false); multithreading=false
  ]
)


if test "$multithreading" == cpp; then
  AC_DEFINE([SharedCPP], ["Shared memory parallelisation"], [SharedCPP])
  AC_SEARCH_LIBS(pthread_create,pthread)
fi

if test "$multithreading" == tbb; then
  AC_CHECK_HEADERS([tbb.h],[],[AC_MSG_ERROR([header <tbb.h> not found])])
  AC_DEFINE([SharedTBB], ["Shared memory parallelisation"], [SharedTBB])
fi




AC_ARG_ENABLE(
  mpi, 
  AC_HELP_STRING([--enable-mpi], [switch on distributed memory parallelisation [default=false, true]]),
  [
    AC_MSG_CHECKING(for mpi)
    case "${enableval}" in
      false)  AC_MSG_RESULT([no (no mpi)]);        mpi=false ;;
      true)   AC_MSG_RESULT([true (with mpi)]);    mpi=true ;;
      *)      AC_MSG_RESULT([true (with mpi)]);    mpi=true ;;
    esac
  ], 
  [ 
    AC_MSG_CHECKING(for mpi); 
    AC_MSG_RESULT(false); mpi=false
  ]
)


if test "$mpi" == true; then
  AC_DEFINE([Parallel], ["Distributed memory parallelisation"], [Parallel])
  #AC_SEARCH_LIBS(MPI_Init,mpi)
  #AX_PROG_CXX_MPI([test x"$mpi" = "true"], [],
  #  [])
fi



#
# Kick out makefiles
#
AC_OUTPUT(src/examples/grid/Makefile src/peano4/Makefile src/Makefile Makefile)

