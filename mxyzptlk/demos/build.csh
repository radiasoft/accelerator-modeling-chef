#!/bin/tcsh
set echo

## 
## CHANGE THESE LINES IF NECESSARY
## 
set INSTALLDIR=../../RUNTIME
set BOOST_INC=/usr/local/ap/include/boost-1_33
set LIBDIR=/usr/local/ap/lib

g++ -c -o $1.o \
    -I$BOOST_INC \
    -I$INSTALLDIR/include \
    $1.cc

## THIS DOESN'T WORK, BUT SHOULD
## g++ \
##   -Wl,-rpath,$LIBDIR \
##   -Wl,-rpath,$INSTALLDIR/lib \
##   -o $1 $1.o \
##   -L$LIBDIR \
##   -L$INSTALLDIR/lib \
##   -lmxyzptlk \
##   -lbasic_toolkit

## THIS DOES WORK, BUT USAGE OF libphysics_toolkit
## AND libbeamline SHOULD NOT BE NECESSARY!!
g++ \
  -Wl,-rpath,$LIBDIR \
  -Wl,-rpath,$INSTALLDIR/lib \
  -o $1 $1.o \
  -L$LIBDIR \
  -L$INSTALLDIR/lib \
  -lphysics_toolkit \
  -lbeamline \
  -lmxyzptlk \
  -lbasic_toolkit

unset echo
