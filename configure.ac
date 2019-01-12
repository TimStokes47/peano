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
# Checks for programs.
#
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX_11

#
# I wanna build libraries
#
AM_PROG_AR
LT_INIT
AC_CONFIG_MACRO_DIRS([m4])


# Think I do not need a C compiler
#AC_PROG_CC


#
# TBB Options
#
# The result is stored in a variable multithreading
#
AC_ARG_ENABLE(
  multithreading, 
  AC_HELP_STRING([--enable-multithreading], [switch on multithreading [default=no,cpp,tbb,openmp]]),
  [
    AC_MSG_CHECKING(for multithreading)
    case "${enableval}" in
      no)  AC_MSG_RESULT([no (no multithreading)]);               multithreading=no ;;
      cpp) AC_MSG_RESULT([cpp (C++11)]);                          multithreading=cpp ;;
      tbb) AC_MSG_RESULT([tbb (Intel Threading Building Blocks]); multithreading=tbb ;;
    esac
  ], 
  [ 
    AC_MSG_CHECKING(for multithreading); 
    AC_MSG_RESULT(no); multithreading=no
  ]
)



#AC_CHECK_HEADERS([limits.h unistd.h])


if test "$multithreading" == cpp; then
  AC_DEFINE([SharedCPP], ["Shared memory parallelisation"], [SharedCPP])
  AC_SEARCH_LIBS(pthread_create,pthread)
  #AC_LDFLAGS += -lpthreads
  #PKG_CHECK_MODULES([GLIB],[])
  #AC_SEARCH_LIBS([], [pthreads], [], [
  #  AC_MSG_ERROR([library pthreads not found]), [])
  #AC_DEFINE([SharedCPP], [], [SharedCPP])
fi

if test "$multithreading" == tbb; then
  AC_CHECK_HEADERS([tbb.h],[],[AC_MSG_ERROR([header <tbb.h> not found])])
  AC_DEFINE([SharedTBB], ["Shared memory parallelisation"], [SharedTBB])
fi

#
# MPI
#
#AC_ARG_ENABLE(mpi,
#  AC_HELP_STRING([--enable-mpi], [Enable MPI]),
#  [ AC_MSG_CHECKING(for MPI)
#    case "${enableval}" in
#      no)  AC_MSG_RESULT(no); mpi=false ;;
#      yes) AC_MSG_RESULT(yes); mpi=true ;;
#    esac ],
#  [ AC_MSG_CHECKING(for MPI); AC_MSG_RESULT(no); mpi=false ])
#if test "$mpi" = "true"; then
#  AC_DEFINE([USING__MPI], "1", [using MPI])
#  AX_PROG_CXX_MPI([test "$mpi" = "true"], [use_mpi=yes],
#    [AC_MSG_FAILURE([MPI compiler not found.])])
#  #AS_AC_EXPAND(LOADMPIFORPY,from mpi4py import MPI)
#else
#  AC_PROG_CXX
#  AS_AC_EXPAND(LOADMPIFORPY,)#
#f#i
#AM_CONDITIONAL(USING__MPI, test "$mpi" = "true" )


#
# Kick out makefiles
#
#AC_OUTPUT(Makefile src/Makefile src/peano4/Makefile src/examples/grid/Makefile)
AC_OUTPUT(src/examples/grid/Makefile src/peano4/Makefile src/Makefile Makefile)

