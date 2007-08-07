#!/usr/bin/env csh

######################################################
#
# File: test-build.csh
#
# Tests the mxyzptlk/demos files
#
# Original: 
#  version: March 8, 2007
# 
#   Author: Leo Michelotti
#         : michelotti@fnal.gov
#
#  COMMENT: I intend that this script develop organically.
#
#  WARNING: Though this script is very simple, I am more
#           comfortable using C shell than bash, so
#           things may go badly wrong.
#
######################################################

foreach w ( `ls *.cc | sed -e "s/.cc//"` )
  gmake $w >& $w.build.log
  end

## foreach w ( `ls *.cc | sed -e "s/.cc//"` )
##   echo $w
##   $w >& $w.out
##   echo $w
##   end
