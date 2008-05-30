#!/bin/csh

if( -e lpsDemo ) then
  ./lpsDemo >& lpsDemo.out
  set return_status = $status
  if( 0 != $return_status ) then
    exit $return_status
    endif
else
  echo "FAILED: Executable lpsDemo does not exist."
  exit 17
endif

exit 0
