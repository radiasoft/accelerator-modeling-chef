#!/bin/csh

if (0 == ${?INSTALLDIR}) then
  echo "*** ERROR ***"
  echo "*** ERROR *** Environment variable INSTALLDIR has not been set."
  echo "*** ERROR ***"
  exit 1
endif

if (0 == ${?BOOST_INC}) then
  echo "*** ERROR ***"
  echo "*** ERROR *** Environment variable BOOST_INC has not been set."
  echo "*** ERROR ***"
  exit 1
endif

echo "-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-"
echo "Beginning testing"
echo "in directory `pwd`"
echo "-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-"

foreach w ( `ls *.cc | sed -e "s/.cc//"` ) 
  gmake $w
  set return_status = $status
  if( $return_status ) then
    echo "*** ERROR ***"
    echo "*** ERROR *** File $w.cc failed to build."
    echo "*** ERROR ***"
    exit $return_status
  endif
  if( -e $w.sh ) then
    chmod 755 $w.sh
    echo "  "
    echo "================================"
    echo Running $w.sh
    ./$w.sh
    set return_status = $status
    if( $return_status ) then
      echo "*** ERROR ***"
      echo "*** ERROR *** File $w.sh test failed."
      echo "*** ERROR *** Returned status: $return_status"
      echo "*** ERROR ***"
      exit $return_status
    else
      echo Finished $w.sh: tests passed!
      echo "================================"
      echo "  "
    endif
  else
    echo "*** ERROR ***"
    echo "*** ERROR *** File $w.sh does not exist."
    echo "*** ERROR ***"
    exit 1
  endif
  end

echo "-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-"
echo "Congratulations! All tests passed."
echo "in directory `pwd`"
echo "-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-"
