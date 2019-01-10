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

AM_INIT_AUTOMAKE(foreign -Wall -Werror)


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
# @todo Replace main with a function in `-lpthread':
AC_CHECK_LIB([pthread], [main])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
#AC_CHECK_HEADER_STDBOOL

#AC_CONFIG_FILES([src/Makefile Makefile])

AC_OUTPUT(Makefile src/Makefile)
