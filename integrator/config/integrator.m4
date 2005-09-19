# integrator.m4 - integrator lib local macros            -*-Shell-script-*-
AC_DEFUN([AC_INC_TCLTK],
[
AC_MSG_CHECKING([for TCL-TK headers])
AC_ARG_VAR([TK_INC],    [TCL-TK include directory])dnl
# Check if $TK_INC has been defined and if it exists
if test ${TK_INC} && test -r ${TK_INC}; then
	AC_SUBST(TK_INC)
	AC_MSG_RESULT([${TK_INC}])
else
# Try to guess the location based on the location of tcl
	tcl_tk_inc=`which tclsh | sed "s#bin\/tclsh#include#"`
	if test -r $tcl_tk_inc/tcl.h; then
		TK_INC=$tcl_tk_inc
		AC_SUBST(TK_INC)
		AC_MSG_RESULT([$TK_INC])
	else
		AC_MSG_ERROR([TCL-TK includes not found, please define TK_INC.]);
	fi
fi
])

AC_DEFUN([AC_INC_X],
[
AC_MSG_CHECKING([for X11 headers])
AC_ARG_VAR([X_INC],    [X11 include directory])dnl
# Check if $X_INC has been defined and if it exists
if test ${X_INC} && test -r ${X_INC}; then
	AC_SUBST(X_INC)
	AC_MSG_RESULT([$X_INC])
else
# Try to guess the location based on the location of X
	x_include=`which X | sed "s#bin\/X#include#"`
	if test -r $x_include; then
		X_INC=$x_include
		AC_SUBST(X_INC)
		AC_MSG_RESULT([$X_INC])
	else
		if test -r "/usr/X11R6/include"; then
			X_INC="/usr/X11R6/include"
			AC_SUBST(X_INC)
			AC_MSG_RESULT([$X_INC])
		else		
			AC_MSG_ERROR([X11 headers not found, please specify in arguments.]);
		fi
	fi
fi
])

