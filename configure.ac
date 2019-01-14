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
AX_CXX_COMPILE_STDCXX_11(,mandatory)


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
      no)     AC_MSG_RESULT([no multithreading]);                    multithreading=false ;;
      cpp)    AC_MSG_RESULT([cpp (C++11)]);                          multithreading=cpp ;;
      tbb)    AC_MSG_RESULT([tbb (Intel Threading Building Blocks]); multithreading=tbb ;;
      *)      AC_MSG_RESULT([default=no (no multithreading). Use cpp or tbb to define target]);            multithreading=false ;;
    esac
  ], 
  [ 
    AC_MSG_CHECKING(for multithreading); 
    AC_MSG_RESULT(no); multithreading=false
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




AC_ARG_ENABLE(mpi,
  AC_HELP_STRING([--enable-mpi], [switch on distributed memory parallelisation [default=false, true]]),
  [ AC_MSG_CHECKING(for MPI)
    case "${enableval}" in
      no)    AC_MSG_RESULT(no);  mpi=false ;;
      yes)   AC_MSG_RESULT(yes); mpi=true ;;
      *)     AC_MSG_RESULT([default=no (no mpi). Use yes or no]);            mpi=false ;;
    esac ],
  [ AC_MSG_CHECKING(for MPI); AC_MSG_RESULT(no); mpi=false ])


if test "$mpi" == true; then
  AC_DEFINE([Parallel], ["Distributed memory parallelisation"], [Parallel])
  AC_DEFINE([USING__MPI], "1", [using MPI])
  AX_PROG_CXX_MPI([test "$mpi" == true], [use_mpi=yes],
    [AC_MSG_FAILURE([MPI compiler not found.])])
  #AS_AC_EXPAND(LOADMPIFORPY,from mpi4py import MPI)
else
  AC_PROG_CXX
  #AS_AC_EXPAND(LOADMPIFORPY,)
fi
#AM_CONDITIONAL(USING__MPI, test "$mpi" = "true" )


#
# Kick out makefiles
#
AC_OUTPUT(src/examples/grid/Makefile src/peano4/Makefile src/Makefile Makefile)

