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

#
# I wanna build libraries
#
AM_PROG_AR
LT_INIT
AC_CONFIG_MACRO_DIRS([m4])


# Think I do not need a C compiler
#AC_PROG_CC

# 
# Checks for libraries.
#
AC_CHECK_LIB([pthread], [main])


#
# Kick out makefiles
#
#AC_OUTPUT(Makefile src/Makefile src/peano4/Makefile src/examples/grid/Makefile)
AC_OUTPUT(src/examples/grid/Makefile src/peano4/Makefile src/Makefile Makefile)

