#!/bin/csh
set echo

setenv LD_LIBRARY_PATH /home/michelotti/briefcase/first_order_jl/installed/lib:/usr/local/ap/qt3/lib:/home/michelotti/software/installed/lib:/usr/local/ap/qt3/lib:/usr/local/ap/X11R6/lib:/usr/local/ap/X11R6/lib-sunos-4.x:/usr/openwin/lib:/usr/dt/lib:/usr/local/TCL-TK/lib:/usr/local/ap/parasoft/lib.solaris
setenv PATH /usr/local/ap/qt3/bin:/bin:/usr/bin:/usr/ccs/bin:/usr/local/ap/bin:/usr/local/bin:/opt/SUNWspro/bin:/opt/SUNWwabi/bin:/opt/acad13/bin:/usr/sbin:/usr/ucb:/usr/local/ap/bin/X11:/usr/local/ap/TeX/bin:/usr/local/ap/metamail/bin:/usr/local/ap/X11R6/bin:/usr/openwin/bin:/usr/local/TCL-TK/bin:/usr/local/ap/lattice_tools/bin:/usr/local/ap/em_tools/bin:/usr/local/ap/AcroRead/bin:/usr/local/ap/parasoft/bin.solaris:.:/home/michelotti/.:/usr/local/ap/cern/v97a/bin

setenv QMAKESPEC /usr/local/qt/mkspecs/solaris-g++.fnal
setenv QTDIR /usr/local/ap/qt3

'mv' chefplotmainbase.cpp chefplotmainbase.cpp.~1~
sed -e "s/..\/chefplotmainbase.ui.h/chefplotmainbase.ui.h/" chefplotmainbase.cpp.~1~ > chefplotmainbase.cpp

rehash

$QTDIR/bin/qmake widget_toolkit.pro
make

unset echo
