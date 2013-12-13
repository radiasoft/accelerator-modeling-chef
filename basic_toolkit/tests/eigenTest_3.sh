#!/bin/csh

./testEigen_3 2  > testEigen_3.out
set return_status = $status
if( 0 != $return_status ) then
  exit $return_status
  endif

./testEigen_3 3 >> testEigen_3.out
set return_status = $status
if( 0 != $return_status ) then
  exit $return_status
  endif

./testEigen_3 4 >> testEigen_3.out
set return_status = $status
if( 0 != $return_status ) then
  exit $return_status
  endif

./testEigen_3 4 385 >> testEigen_3.out
set return_status = $status
if( 0 != $return_status ) then
  exit $return_status
  endif

./testEigen_3 5 177 >> testEigen_3.out
set return_status = $status
if( 0 != $return_status ) then
  exit $return_status
  endif

./testEigen_3 6 >> testEigen_3.out
set return_status = $status
if( 0 != $return_status ) then
  exit $return_status
  endif
