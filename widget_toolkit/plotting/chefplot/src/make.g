#!/bin/csh
set echo

# setenv QMAKESPEC /usr/local/qt/mkspecs/solaris-g++.fnal
setenv QTDIR /usr/local/ap/qt3

$QTDIR/bin/qmake widget_toolkit.pro
make

unset echo
