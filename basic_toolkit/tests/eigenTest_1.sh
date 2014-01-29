#!/bin/csh

./eigenTest_1  > eigenTest_1.out
set return_status = $status
if( 0 != $return_status ) then
  exit $return_status
  endif
