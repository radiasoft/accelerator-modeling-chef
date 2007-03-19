/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      Sage.cc
******  Version:   1.1
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
******  REVISION HISTORY
******
******  Jan 2007 - Mar 2007  ostiguy@fnal
******    
******  - use new style STL-compatible beamline iterators
******  - added support for reference counted elements/beamlines
******
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

/*
 *  File: Sage.cc
 *  Base class for all sages.
 *  
 *  Leo Michelotti
 *  November 23, 1998
 */


#ifndef NO_RTTI
#include <typeinfo>
#endif

#include <basic_toolkit/GenericException.h>
#include <physics_toolkit/Sage.h>
#include <beamline/FramePusher.h>  // Used by Sage::isRing functions

double Sage::defGapTol_   = 0.005;  // = 5 mm
double Sage::defAngleTol_ = 0.001;  // = 1 mrad

using namespace std;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Sage::Sage( BmlPtr x)
  :  myBeamlinePtr_(x),
     nelms_(0), 
     verbose_(false),
     isRing_(Sage::isRing(*x)),
     errorStreamPtr_( &std::cerr ),
     outputStreamPtr_( &std::cout ),
     ringGapTolerance_(  defGapTol_ ),
     ringAngleTolerance_( defAngleTol_)
{


  if( !x ) {
    throw( GenericException( __FILE__, __LINE__, 
           "Sage::Sage( BmlPtr x, bool doClone )", 
           "Constructor invoked with null pointer." ) );
  }


  if( beamline::unknown == myBeamlinePtr_->getLineMode() ) {
    if( isRing_ ) {
      myBeamlinePtr_->setLineMode( beamline::ring );
    }
    else {
      myBeamlinePtr_->setLineMode( beamline::line );
    }
  }

  nelms_ = myBeamlinePtr_->countHowManyDeeply();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Sage::Sage(beamline const& bml)
  :  myBeamlinePtr_(),
     nelms_(0), 
     verbose_(false),
     isRing_(Sage::isRing( bml ) ),
     errorStreamPtr_( &std::cerr ),
     outputStreamPtr_( &std::cout ),
     ringGapTolerance_(  defGapTol_ ),
     ringAngleTolerance_( defAngleTol_)
{

   
  myBeamlinePtr_ = BmlPtr( bml.Clone() ); 

  if( beamline::unknown == myBeamlinePtr_->getLineMode() ) {
    if( isRing_ ) {
      myBeamlinePtr_->setLineMode( beamline::ring );
    }
    else {
      myBeamlinePtr_->setLineMode( beamline::line );
    }
  }

  nelms_ = myBeamlinePtr_->countHowManyDeeply();
}

 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Sage::~Sage()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Sage::set_verbose()
{
  verbose_ = true;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Sage::unset_verbose()
{
   verbose_ = false;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Sage::setErrorStream( std::ostream* x )
{
  errorStreamPtr_ = x;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Sage::setOutputStream( std::ostream* x )
{
  outputStreamPtr_ = x;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Sage::treatAsRing( bool x )
{
  if(x) {
    myBeamlinePtr_->setLineMode( beamline::ring );
  }
  else {
    myBeamlinePtr_->setLineMode( beamline::line );
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool Sage::isTreatedAsRing() const
{
  return ( beamline::ring == myBeamlinePtr_->getLineMode() );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Sage::setGapTolerance( double x )
{
  ringGapTolerance_ = std::abs(x);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Sage::getGapTolerance() const
{
  return ringGapTolerance_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void Sage::setAngleTolerance( double x )
{
  ringAngleTolerance_ = std::abs(x);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


inline double Sage::getAngleTolerance() const
{
  return ringAngleTolerance_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


bool Sage::no ( ConstElmPtr )
{
  return false;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


bool Sage::yes( ConstElmPtr )
{
  return true;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


bool Sage::isRing( beamline const& bml, double gap_tol, double angle_tol )
{
  // This test will return true for pathologically short lines
  //   like a beamline consisting of a single 1 mm drift.

  gap_tol    = std::abs(gap_tol); 
  angle_tol  = std::abs(angle_tol); 

  // Paranoia in case of excessively stupid user.

  FramePusher fp;
  bml.accept( fp );

  // Check the point of return

  Vector r = fp.getFrame().getOrigin();

  for( int i = 0; i < 3; ++i) {
    if( gap_tol < std::abs(r(i)) ) { return false; }
  }

  // Check the angle of return
  //   Tolerance is hardwired to 1 milliradian
  MatrixD fv = fp.getFrame().getAxes();

  for( int i = 0; i < 3; ++i ) {
    for( int j = 0; j < 3; ++j ) {
      if( i != j ) {
        if( angle_tol < std::abs(fv(i,j)) ) { return false; }
      }
    }
  }
  
  return true;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


bool Sage::isRing( BmlPtr bmlPtr, double gt, double at )
{
  return Sage::isRing( *bmlPtr, gt, at );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


bool Sage::hasRing() const
{
  return Sage::isRing( myBeamlinePtr_ );
}


