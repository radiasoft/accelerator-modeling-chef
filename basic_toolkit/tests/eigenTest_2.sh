#!/bin/csh

./testEigen_2 2  > testEigen_2.out
set return_status = $status
if( 0 != $return_status ) then
  exit $return_status
  endif

./testEigen_2 3 >> testEigen_2.out
set return_status = $status
if( 0 != $return_status ) then
  exit $return_status
  endif

./testEigen_2 4 >> testEigen_2.out
set return_status = $status
if( 0 != $return_status ) then
  exit $return_status
  endif

./testEigen_2 4 385 >> testEigen_2.out
set return_status = $status
if( 0 != $return_status ) then
  exit $return_status
  endif

./testEigen_2 5 177 >> testEigen_2.out
set return_status = $status
if( 0 != $return_status ) then
  exit $return_status
  endif

./testEigen_2 6 >> testEigen_2.out
set return_status = $status
if( 0 != $return_status ) then
  exit $return_status
  endif
