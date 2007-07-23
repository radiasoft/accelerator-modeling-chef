/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      LayoutVisitor.cc
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
****** July 2007 ostiguy@fnal.gov
****** - take advantage of dynamic type resolution for visit functions
****** - element predicate based on general function object   
******                                                               
******                                                                
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif


/*
 * File: LayoutVisitor.cc
 * 
 * class LayoutVisitor
 * 
 * Provides coordinates for simple 2D diagram
 * of the beamline. FramePusher should be used
 * for more detailed site maps.
 *
 * Leo Michelotti
 * May 7, 2002
 * 
 * Version 1.1  June 27, 2003
 *
 */


#include <basic_toolkit/iosetup.h>
#include <physics_toolkit/LayoutVisitor.h>
#include <beamline/bmlnElmnt.h>
#include <beamline/quadrupole.h>
#include <beamline/rbend.h>
#include <beamline/sbend.h>
#include <beamline/CF_rbend.h>
#include <beamline/CF_sbend.h>
#include <beamline/sextupole.h>
#include <beamline/sector.h>


using namespace std;
using FNAL::pcerr;
using FNAL::pcout;


// Static error codes

const int LayoutVisitor::OKAY          = 0;
const int LayoutVisitor::SECTORVISITED = 1;
const int LayoutVisitor::NOFILEOPENED  = 2;


// Constructors

LayoutVisitor::LayoutVisitor( double bsl, double bh, double qh, double sh )
: s_(0.0),
  baseline_(bsl),
  bendHeight_(bh),
  quadHeight_(qh),
  sextHeight_(sh),
  specialHeight_(0.0),
  streamPtr_(0),
  errorCode_(OKAY)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LayoutVisitor::~LayoutVisitor()
{
  if( streamPtr_ ) {
    this->closeFile();
  }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LayoutVisitor::setDiscriminator( boost::function< bool( bmlnElmnt&) > dsc, 
                                      double hght )
{
  specialHeight_   = hght;
  discriminator_   = dsc;
}


// Visiting functions

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LayoutVisitor::visit ( sector& )
{
  (*pcerr) << "*** WARNING ***                                \n"
       << "*** WARNING *** LayoutVisitor::visitSector       \n"
       << "*** WARNING ***                                \n"
       << "*** WARNING *** Sectors are not handled        \n"
       << "*** WARNING *** properly. Errorcode is         \n"
       << "*** WARNING *** being set. Results will be     \n"
       << "*** WARNING *** unreliable.                    \n"
       << "*** WARNING ***                                \n"
       << endl;
  errorCode_ = SECTORVISITED;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LayoutVisitor::visit( bmlnElmnt& x )
{

  if( !streamPtr_ ) { 
     errorCode_ = NOFILEOPENED;
     return;
  }

  if( !streamPtr_ ) return;

  if( discriminator_ ) {
     if(  discriminator_(x) ) {
        processSpecialElement( x );
        return;
     }
  }
  
  if( x.Length() > 0.0 ) {
      s_ += x.Length();
      (*streamPtr_) << s_ << "  " << baseline_ << endl;
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 
void LayoutVisitor::visit( quadrupole& x )
{
  if( !streamPtr_ ) { 
     errorCode_ = NOFILEOPENED;
     return;
  }

  if( !streamPtr_ ) return;

  if( discriminator_ ) {
      if( discriminator_(x) ) {
        processSpecialElement( x );
        return;
      }
  }

   if( 0 == x.Strength() ) {
      this->visit(x);
   }
   else if ( 0 < x.Strength() ) {
      (*streamPtr_) << s_ << "  " << baseline_ + quadHeight_ << endl;
      s_ += x.Length();
      (*streamPtr_) << s_ << "  " << baseline_ + quadHeight_ << endl;
      (*streamPtr_) << s_ << "  " << baseline_ << endl;
   }
   else {
      (*streamPtr_) << s_ << "  " << baseline_ - quadHeight_ << endl;
      s_ += x.Length();
      (*streamPtr_) << s_ << "  " << baseline_ - quadHeight_ << endl;
      (*streamPtr_) << s_ << "  " << baseline_ << endl;
   }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LayoutVisitor::visit_bend( bmlnElmnt& x )
{

  if( !streamPtr_ ) { 
     errorCode_ = NOFILEOPENED;
     return;
  }


  if( discriminator_ ) {
      if( discriminator_(x) ) {
        processSpecialElement( x );
        return;
      }
  }

  if( 0 == x.Strength() ) {
      this->visit(x);
  }
  else if ( 0 < x.Strength() ) {
     (*streamPtr_) << s_ << "  " << baseline_ + bendHeight_ << endl;
     s_ += x.Length();
     (*streamPtr_) << s_ << "  " << baseline_ + bendHeight_ << endl;
     (*streamPtr_) << s_ << "  " << baseline_ << endl;
    }
    else {
      (*streamPtr_) << s_ << "  " << baseline_ - bendHeight_ << endl;
      s_ += x.Length();
      (*streamPtr_) << s_ << "  " << baseline_ - bendHeight_ << endl;
      (*streamPtr_) << s_ << "  " << baseline_ << endl;
    }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LayoutVisitor::visit( rbend& x )
{
  visit_bend(x);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LayoutVisitor::visit( sbend& x )
{
  visit_bend(x);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LayoutVisitor::visit( CF_rbend& x )
{
  visit_bend(x);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LayoutVisitor::visit( CF_sbend& x )
{
   visit_bend(x);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LayoutVisitor::visit( sextupole& x )
{
  if( !streamPtr_ ) { 
     errorCode_ = NOFILEOPENED;
     return;
  }

  if( discriminator_ ) {
    if( discriminator_(x) ) {
     processSpecialElement( x );
     return;
    }
  }

   if( x.Strength() == 0.0 ) {
      this->visit(x);
   }
   else if ( 0 < x.Strength() ) {
     (*streamPtr_) << s_ << "  " << baseline_ + sextHeight_ << endl;
     s_ += x.Length();
     (*streamPtr_) << s_ << "  " << baseline_ + sextHeight_ << endl;
     (*streamPtr_) << s_ << "  " << baseline_ << endl;
    }
    else {
     (*streamPtr_) << s_ << "  " << baseline_ - sextHeight_ << endl;
      s_ += x.Length();
      (*streamPtr_) << s_ << "  " << baseline_ - sextHeight_ << endl;
      (*streamPtr_) << s_ << "  " << baseline_ << endl;
    }

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// File manipulation

int LayoutVisitor::openFile( const char* fileName )
{
  if( streamPtr_ ) {
    this->closeFile();
  }
  streamPtr_ = new ofstream( fileName );

  // Record first point
  (*streamPtr_) << s_ << "  " << baseline_ << endl;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int LayoutVisitor::closeFile()
{
  int ret;

  if( streamPtr_ ) {
    (*streamPtr_) << endl;
    (*streamPtr_) << 0.0 << "  " << baseline_ << endl;
    (*streamPtr_) << s_  << "  " << baseline_ << endl;
    streamPtr_->close();
    delete streamPtr_;
    streamPtr_ = 0;
    ret = OKAY;
  }

  else {
    ret = NOFILEOPENED;
  }

  return ret;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int LayoutVisitor::getErrorCode() const
{
  return errorCode_;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LayoutVisitor::processSpecialElement( bmlnElmnt& x )
{
  (*streamPtr_) << s_ << "  " << baseline_ + specialHeight_ << endl;
  s_ += x.Length();
  (*streamPtr_) << s_ << "  " << baseline_ + specialHeight_ << endl;
  (*streamPtr_) << s_ << "  " << baseline_ << endl;
}
