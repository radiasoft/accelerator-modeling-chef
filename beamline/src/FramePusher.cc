/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   2.0
******                                    
******  File:      FramePusher.cc
******                                                                
******  Copyright (c) 2001  Universities Research Association, Inc.   
******                All Rights Reserved                             
******                                                                
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******                                                                
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

/*
 * File: FramePusher.cc
 * 
 * class FramePusher
 * 
 * Moves a Frame around a beamline, 
 * in anticipation of drawing the footprint.
 *
 * Leo Michelotti
 * May 11, 2001
 *
 */


#include <beamline/FramePusher.h>
#include <beamline/bmlnElmnt.h>
#include <beamline/sbend.h>
#include <beamline/CF_sbend.h>
#include <beamline/rbend.h>
#include <beamline/CF_rbend.h>
#include <beamline/Slot.h>

// Static error codes

const int FramePusher::OKAY          = 0;
const int FramePusher::SECTORVISITED = 1;

using namespace std;

// Constructors

FramePusher::FramePusher()
: frame_()
  , errorCode_(OKAY)
{
}

FramePusher::FramePusher( const Frame& f )
: frame_(f)
  , errorCode_(OKAY)
{
}

FramePusher::FramePusher( const FramePusher& x )
: frame_( x.frame_ )
  , errorCode_(x.errorCode_)
{
}

FramePusher::~FramePusher()
{
}


// Visiting functions

void FramePusher::visit( bmlnElmnt const& x )
{
  if( x.Length() > 0.0 ) {
    frame_.translate( (x.Length())*frame_.getzAxis() );
  }
}


void FramePusher::visit( rbend const& x )
{
  // Note: the lower bound of 1/2 nanoradian was
  // taken from file: rbend.cc
  //        function: bool rbend::hasStandardFaces
  // 
  double edgeAngle = x.getEntryEdgeAngle();
  if( 0.5e-9 < std::abs(edgeAngle) ) {
    frame_.rotate( - edgeAngle, frame_.getyAxis(), false );
  }

  frame_.translate( (x.Length())*frame_.getzAxis() );

  edgeAngle = x.getExitEdgeAngle();
  if( 0.5e-9 < std::abs(edgeAngle) ) {
    frame_.rotate(   edgeAngle, frame_.getyAxis(), false );
  }
}


void FramePusher::visit( CF_rbend const& x )
{
  // Note: the lower bound of 1/2 nanoradian was
  // taken from file: rbend.cc
  //        function: bool rbend::hasStandardFaces
  // 
  double edgeAngle = x.getEntryEdgeAngle();
  if( 0.5e-9 < std::abs(edgeAngle) ) {
    frame_.rotate( - edgeAngle, frame_.getyAxis(), false );
  }

  frame_.translate( (x.Length())*frame_.getzAxis() );

  edgeAngle = x.getExitEdgeAngle();
  if( 0.5e-9 < std::abs(edgeAngle) ) {
    frame_.rotate(   edgeAngle, frame_.getyAxis(), false );
  }
}


void FramePusher::visit( sbend const& x    )
{
  double angle;
  double displacement;
  double rho;

  angle = x.getAngle();
  rho = x.Length() / angle;
  angle /= 2.0;
  displacement = 2.0*rho*sin(angle);

  double edgeAngle = x.getEntryEdgeAngle();
  if( 0.5e-9 < std::abs(angle - edgeAngle) ) {
    frame_.rotate( - (angle - edgeAngle), frame_.getyAxis(), false );
  }

  frame_.translate( displacement*frame_.getzAxis() );

  edgeAngle = x.getExitEdgeAngle();
  if( 0.5e-9 < std::abs(angle - edgeAngle) ) {
    frame_.rotate( - (angle - edgeAngle), frame_.getyAxis(), false );
  }
}


void FramePusher::visit( CF_sbend const& x )
{
  double angle;
  double displacement;
  double rho;

  angle = x.getAngle();
  rho = x.Length() / angle;
  angle /= 2.0;
  displacement = 2.0*rho*sin(angle);

  double edgeAngle = x.getEntryEdgeAngle();
  if( 0.5e-9 < std::abs(angle - edgeAngle) ) {
    frame_.rotate( - (angle - edgeAngle), frame_.getyAxis(), false );
  }

  frame_.translate( displacement*frame_.getzAxis() );

  edgeAngle = x.getExitEdgeAngle();
  if( 0.5e-9 < std::abs(angle - edgeAngle) ) {
    frame_.rotate( - (angle - edgeAngle), frame_.getyAxis(), false );
  }
}


void FramePusher::visit( sector const&)
{
  cerr << "*** WARNING ***                                \n"
       << "*** WARNING *** FramePusher::visitSector       \n"
       << "*** WARNING ***                                \n"
       << "*** WARNING *** Sectors are not handled        \n"
       << "*** WARNING *** properly. Errorcode is         \n"
       << "*** WARNING *** being set. Results will be     \n"
       << "*** WARNING *** unreliable.                    \n"
       << "*** WARNING ***                                \n"
       << endl;
  errorCode_ = SECTORVISITED;
}


void FramePusher::visit( Slot const& x     )
{
  frame_ = ( ( x.getOutFrame() ).relativeTo( x.getInFrame() ) ) 
           .patchedOnto( frame_ );
}
