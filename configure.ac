AC_INIT([finalmetronome], [0.0.8], [raffamaiden@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_PROG_CC
AC_PROG_CXX
AC_LANG_CPLUSPLUS
AC_PROG_RANLIB

# Check for FMOD
FMOD_FOUND=0
#AC_MSG_CHECKING([for FMOD])
AC_CHECK_HEADERS([fmod.h], [FMOD_FOUND=1])
AC_CHECK_HEADERS([fmodex/fmod.h], [FMOD_FOUND=1; CPPFLAGS="$CPPFLAGS -Ifmodex/";])

# Check for header path
#m4_include(ac_path_header.m4)
#AC_PATH_HEADER(FMOD_PA, [fmod.h])
#AC_PATH_HEADER(FMOD_PA, [fmodex/fmod.h])
#AC_MSG_ERROR([FMOD is located at $FMOD_PA])

# Only one '=' needed to test if the strings are equal
if test $FMOD_FOUND = 0; then
	AC_MSG_ERROR([Cannot find fmod.h. You need FMOD installed to compile this program. Value of FMOD_FOUND is $FMOD_FOUND])
fi

# Check for wxWidgets
m4_include(wxwin.m4) # Define wx macros

# AM_OPTIONS_WXCONFIG adds options like --with-wx-config. See options  by calling ./configure --help
AM_OPTIONS_WXCONFIG
reqwx=2.8.10
AM_PATH_WXCONFIG($reqwx, wxWin=1)
if test "$wxWin" != 1; then
	AC_MSG_ERROR([
		wxWidgets must be installed on your system.
 
		Please check that wx-config is in path, the directory
		where wxWidgets libraries are installed (returned by
		'wx-config --libs' or 'wx-config --static --libs' command)
		is in LD_LIBRARY_PATH or equivalent variable and
		wxWidgets version is $reqwx or above.
		])
fi
 
CPPFLAGS="$CPPFLAGS $WX_CPPFLAGS"
CXXFLAGS="$CXXFLAGS $WX_CXXFLAGS_ONLY"
CFLAGS="$CFLAGS $WX_CFLAGS_ONLY"
LIBS="$LIBS $WX_LIBS"

# Check for time.h
AC_HEADER_TIME
#Check for size_t type
AC_TYPE_SIZE_T

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile fmodgetver/Makefile src/Makefile libopentime/Makefile])
AC_OUTPUT
