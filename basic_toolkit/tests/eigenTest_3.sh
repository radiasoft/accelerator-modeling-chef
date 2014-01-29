#!/bin/csh

./eigenTest_3 2  > eigenTest_3.out
set return_status = $status
if( 0 != $return_status ) then
  exit $return_status
  endif

./eigenTest_3 3 >> eigenTest_3.out
set return_status = $status
if( 0 != $return_status ) then
  exit $return_status
  endif

./eigenTest_3 4 >> eigenTest_3.out
set return_status = $status
if( 0 != $return_status ) then
  exit $return_status
  endif

./eigenTest_3 4 385 >> eigenTest_3.out
set return_status = $status
if( 0 != $return_status ) then
  exit $return_status
  endif

./eigenTest_3 5 177 >> eigenTest_3.out
set return_status = $status
if( 0 != $return_status ) then
  exit $return_status
  endif

./eigenTest_3 6 >> eigenTest_3.out
set return_status = $status
if( 0 != $return_status ) then
  exit $return_status
  endif
