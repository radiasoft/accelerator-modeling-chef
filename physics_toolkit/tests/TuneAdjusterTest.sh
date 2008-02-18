#!/bin/csh

./TuneAdjusterTest 10 7 7 800 12 0.01 -0.01 >& TuneAdjusterTest.out
set return_status = $status
if( 0 != $return_status ) then
  exit $return_status
  endif

exit 0
