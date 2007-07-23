/**************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:      RollAccumulator.cpp                                ****** 
******                                                               ******
******  Copyright (c) Universities Research Association, Inc.        ****** 
******                All Rights Reserved                            ****** 
******                                                               ****** 
******  Authors:                                                     ******
******              Leo Michelotti                                   ******
******              Fermilab                                         ******
******              michelotti@fnal.gov                              ****** 
******                                                               ******
******  Usage, modification, and redistribution are subject to terms ******
******  of the License supplied with this software.                  ****** 
******                                                               ******
******  Software and documentation created under                     ****** 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000.    ****** 
******  The U.S. Government retains a world-wide non-exclusive,      ****** 
******  royalty-free license to publish or reproduce documentation   ****** 
******  and software for U.S. Government purposes. This software     ****** 
******  is protected under the U.S. and Foreign Copyright Laws.      ****** 
******  URA/FNAL reserves all rights.                                ****** 
******                                                               ******
****** NOTE: Contents moved from CHEFGUI.cpp                         ******
***************************************************************************
***************************************************************************
**************************************************************************/
#include <RollAccumulator.h>
#include <beamline/Alignment.h>


// *****************************************
// Implementation: class RollAccumulator
// *****************************************

RollAccumulator::RollAccumulator( const char* theType, double theta )
: deltaRoll_(theta), type_(theType), weak_(false)
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RollAccumulator::~RollAccumulator()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void RollAccumulator::setWeakType()
{
  weak_ = true;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void RollAccumulator::setStrongType()
{
  weak_ = false;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int RollAccumulator::operator()( bmlnElmnt& x )
{
  int ret = -1;
  bool doit = false;

  if( weak_ ) {
    doit = ( x.Type() == type_ );
  }
  else {
    doit = ( x.Type() == type_ );
  }

  if( doit ) {
    alignmentData ad( x.Alignment() );
    ad.tilt += deltaRoll_;
    x.setAlignment( ad );
    ret = 0;
  }

  return ret;
}



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int RollAccumulator::operator()( ElmPtr& x )
{
  return operator()(*x);
}
