#!/bin/csh

./eigenTest_2 2  > eigenTest_2.out
set return_status = $status
if( 0 != $return_status ) then
  exit $return_status
  endif

./eigenTest_2 3 >> eigenTest_2.out
set return_status = $status
if( 0 != $return_status ) then
  exit $return_status
  endif

./eigenTest_2 4 >> eigenTest_2.out
set return_status = $status
if( 0 != $return_status ) then
  exit $return_status
  endif

./eigenTest_2 4 385 >> eigenTest_2.out
set return_status = $status
if( 0 != $return_status ) then
  exit $return_status
  endif

./eigenTest_2 5 177 >> eigenTest_2.out
set return_status = $status
if( 0 != $return_status ) then
  exit $return_status
  endif

./eigenTest_2 6 >> eigenTest_2.out
set return_status = $status
if( 0 != $return_status ) then
  exit $return_status
  endif
