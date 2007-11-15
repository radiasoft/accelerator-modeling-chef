#!/bin/csh

testDet_2 -d 1 2 3 4 5
set return_status = $status
if( 0 != $return_status ) then
  exit $return_status
  endif

testDet_2 -c 1 2 3 4
set return_status = $status
if( 0 != $return_status ) then
  exit $return_status
  endif

testDet_2 -d 0 1 2 3
set return_status = $status
if( 0 != $return_status ) then
  exit $return_status
  endif

exit 0
