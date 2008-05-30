#!/bin/csh

######################################################
#
# File: test_all.sh
#
# Original: 
#  version: May 29, 2008
# 
#   Author: Leo Michelotti
#         : michelotti@fnal.gov
#
######################################################

echo "-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-"
echo "Beginning testing"
echo "in directory `pwd`"
echo "-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-"

pushd lpsDemo >& /dev/null
./test_all.sh
popd
