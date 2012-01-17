#!/bin/csh

./test_6and8 -n 256 >& test_6and8.out
set return_status = $status
if( 0 != $return_status ) then
  exit $return_status
  endif

exit 0
