#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(topographic toolkit, 1.2.0, jake(dot)askeland(at)gmail(dot)com)
AM_INIT_AUTOMAKE(topographic toolkit, 1.2.0)
AC_OUTPUT(Makefile src/Makefile)
AC_CONFIG_SRCDIR([src/taspa.cc])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
dnl Check for SDL
SDL_VERSION=1.2.0
AM_PATH_SDL($SDL_VERSION,:,)
CFLAGS="$CFLAGS $SDL_CFLAGS"
LIBS="$LIBS $SDL_LIBS"

# Check for SDL_image library
AC_CHECK_HEADERS(SDL_image.h, [
  AC_CHECK_LIB(SDL_image, IMG_Load, [
    LIBS="-lSDL_image $LIBS"
    AC_DEFINE(HAVE_SDL_IMAGE, 1, [SDL_image support is enabled])
  ])
])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([string.h sys/time.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM
AC_TYPE_UINT16_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_CHECK_FUNCS([gettimeofday pow sqrt])
#AC_CHECK_LIB(SDL_image, SDL_FreeSurface,, AC_MSG_ERROR([Need SDL_image]))

# Don't compile debug commands
AC_DEFINE([NDEBUG], [1], [Assertions disabled.])

AC_OUTPUT
