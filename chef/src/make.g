#!/bin/csh
set echo

# setenv QMAKESPEC /usr/local/qt/mkspecs/solaris-g++.fnal
setenv QTDIR /usr/local/ap/qt3
$QTDIR/bin/qmake -o Makefile chef.pro
make all

unset echo
