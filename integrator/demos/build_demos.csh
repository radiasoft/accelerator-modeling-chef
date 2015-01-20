#!/bin/csh

#######################################################
#
# File: integrator/demos/build_demos.csh
#
# Original: 
#  version: December 17, 2014
# 
# Three environment variables are required to be
# predefined: INSTALLDIR, BOOST_INC and BOOST_LIB
# 
#   Author: Leo Michelotti
#         : michelotti@fnal.gov
#
#######################################################

if (0 == ${?INSTALLDIR}) then
  echo "*** ERROR ***"
  echo "*** ERROR *** Environment variable INSTALLDIR has not been set."
  echo "*** ERROR ***"
  exit 1
endif

if (0 == ${?BOOST_INC}) then
  echo "*** ERROR ***"
  echo "*** ERROR *** Environment variable BOOST_INC has not been set."
  echo "*** ERROR ***"
  exit 1
endif

if (0 == ${?BOOST_LIB}) then
  echo "*** ERROR ***"
  echo "*** ERROR *** Environment variable BOOST_LIB has not been set."
  echo "*** ERROR ***"
  exit 1
endif

foreach w ( `ls *.cc | sed -e "s/.cc//"` )
  echo Building $w
  make -f makefile.local $w
  echo "  "
end

echo ==========================

foreach w ( `ls *.cc | sed -e "s/.cc//"` )
  if( ! -e $w ) then
    echo $w failed to build.
  endif
end
