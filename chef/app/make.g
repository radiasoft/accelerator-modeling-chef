#!/bin/csh
set echo

setenv QMAKESPEC /usr/local/qt/mkspecs/solaris-g++.fnal
setenv QTDIR /usr/local/ap/qt3
/usr/local/ap/qt3/bin/qmake chef.pro
make all

unset echo
