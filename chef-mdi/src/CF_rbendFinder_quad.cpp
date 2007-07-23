/**************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:      CF_rbend_Finder_quad.cpp                          ****** 
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
#include <CF_rbendFinder_quad.h>
#include <beamline/CF_rbend.h>


// *****************************************
// Implementation: class CF_rbendFinder_quad
// *****************************************

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_rbendFinder_quad::CF_rbendFinder_quad( double q )
: integratedQuad_(q)
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_rbendFinder_quad::~CF_rbendFinder_quad()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void CF_rbendFinder_quad::setQuadrupole( double q )
{
  integratedQuad_ = q;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool CF_rbendFinder_quad::operator()(  bmlnElmnt const& x )
{
  static bool ret;
  ret = false;

  if( 0 == strcmp( x.Type(), "CF_rbend" ) ) {
    if( integratedQuad_ ==  static_cast<CF_rbend const&>(x).getQuadrupole() ) {
      ret = true;
    }
  }

  return ret;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool CF_rbendFinder_quad::operator()(  ElmPtr& x )
{
  return  operator()(*x);
}

