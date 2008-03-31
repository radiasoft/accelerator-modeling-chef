#!/bin/csh

./d2sTest_1 >& d2sTest_1.out
set return_status = $status
if( 0 != $return_status ) then
  exit $return_status
  endif

exit 0
