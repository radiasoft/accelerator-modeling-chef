#!/bin/csh

./SolenoidTest01
set return_status = $status
if( 0 != $return_status ) then
  exit $return_status
  endif

exit 0
