/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      RefRegVisitor.cc
******
******                                                                
******  Copyright (c) 2003  Universities Research Association, Inc.   
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
****** REVISION HISTORY
******
******  Mar 2007        ostiguy@fnal.gov
******
****** -use new style STL-compatible iterators
****** -use locally defined particles allocated on the stack
****** -visitor public interface taking advantage of dynamic type
****** -RefRegVisitor now makes no implicit assumptions about flat beamlines.
******  
****** May 2007 ostiguy@fnal.gov
******
****** - eliminated uncessary two-step registration process. 
******   Registration now done in a single loop.
****** 
**************************************************************************
*************************************************************************/

/*
 * File: RefRegVisitor.cc
 * 
 * Source file for class RefRegVisitor, 
 * a "reference registration visitor."
 * 
 * Registers a user-defined reference proton with a bmlnElmnt.
 * 
 * Leo Michelotti
 * April 10, 2003
 * 
 */


#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <beamline/RefRegVisitor.h>
#include <basic_toolkit/PhysicsConstants.h>
#include <beamline/beamline.h>
#include <beamline/CF_rbend.h>
#include <beamline/CF_sbend.h>
#include <beamline/rbend.h>
#include <beamline/sbend.h>
#include <beamline/Slot.h>
#include <beamline/rfcavity.h>
#include <beamline/LinacCavity.h>
#include <beamline/Particle.h>

#include <iomanip>

// Static error codes

const int RefRegVisitor::OKAY          = 0;

using namespace std;

// Constructors

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RefRegVisitor::RefRegVisitor( Particle const& p )
  :    particle_(p)
  ,    errorCode_(OKAY)
  ,    revolutionFrequency_(-1.0)
  ,    initialMomentum_( p.ReferenceMomentum() )
{}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RefRegVisitor::RefRegVisitor( RefRegVisitor const& x )
:   particle_(x.particle_)
  , errorCode_(OKAY)
  , revolutionFrequency_(x.revolutionFrequency_)
  , initialMomentum_(x.initialMomentum_)
{}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RefRegVisitor::~RefRegVisitor()
{}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int RefRegVisitor::getErrorCode() const
{
  return errorCode_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double RefRegVisitor::getCdt()
{
  return particle_.get_cdt();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void RefRegVisitor::setCdt( double x )
{
   particle_.set_cdt(x);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//
// Visiting functions

void RefRegVisitor::visit( beamline& x )
{

  x.setEnergy( particle_.ReferenceEnergy() );

 //------------------------------------------------------------------------------------
 // *** visit individual elements to set reference time  **** 
 //-------------------------------------------------------------------------------------

     double cumulativeCdt = 0.0;


     for (beamline::deep_iterator it  = x.deep_begin();
                              it != x.deep_end(); ++it) {
    
        double momentum = particle_.ReferenceMomentum();
       
        if( (initialMomentum_ != momentum) && ( (*it)->isMagnet() ) ) { 
         (*it)->setStrength( ((*it)->Strength())*(momentum/initialMomentum_) );
        }

        (*it)->accept( *this ); 

        cumulativeCdt +=(*it)->getReferenceTime();          

     }

    //-------------------------------------------------------------
    // Store the total time of traversal as a frequency. 
    // Not used if the beamline is not a ring. 
    // NOTE: revolutionFrequency_ is meaningless if rfcavity 
    // elements with an accelerating phase are present.
    //---------------------------------------------------------------

    revolutionFrequency_ = PH_MKS_c/cumulativeCdt;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void RefRegVisitor::visit( bmlnElmnt& x )
{
  x.setReferenceTime(0.0);
  particle_.set_cdt(0.0);
  x.propagate( particle_ );
  x.setReferenceTime( particle_.get_cdt() );

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void RefRegVisitor::visit( CF_rbend& x ) 
{
  // This does not nullify the edge focussing from 
  // the inner propagators before setting the entry
  // and exit angles. It may be that these attributes
  // are not even used.
  x.setEntryAngle( particle_ );
  x.acceptInner( *this );
  x.setExitAngle( particle_ );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void RefRegVisitor::visit( CF_sbend& x ) 
{
  // This does not nullify the edge focussing from 
  // the inner propagators before setting the entry
  // and exit angles. It may be that these attributes
  // are not even used.
  x.setEntryAngle( particle_ );
  x.acceptInner( *this );
  x.setExitAngle( particle_ );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void RefRegVisitor::visit( sbend& x )
{

  bmlnElmnt::PropFunc const* pf = x.getPropFunction();
 
  if ( pf == &sbend::LikeMAD ) {     
    visit( static_cast<bmlnElmnt&>(x) );
    return;
  }

  x.setPropFunction( &sbend::NoEdge );
  x.setEntryAngle( particle_ );
  visit( static_cast<bmlnElmnt&>(x) );
  x.setExitAngle( particle_ );
  x.setPropFunction( pf );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void RefRegVisitor::visit( rbend& x )
{
  bmlnElmnt::PropFunc const* pf = x.getPropFunction();

  x.setPropFunction( &rbend::NoEdge );
  x.setEntryAngle( particle_ );
  visit( static_cast<bmlnElmnt&>(x) );
  x.setExitAngle( particle_ );
  x.setPropFunction( pf );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void RefRegVisitor::visit( rfcavity& x ) 
{
 x.acceptInner( *this );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void RefRegVisitor::visit( LinacCavity& x ) 
{
  x.acceptInner( *this );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void RefRegVisitor::visit( thinrfcavity& x ) 
{

  if( x.getRadialFrequency() < 1.0e-9 ) {
    if( x.getHarmonicNumber() > 0 ) {
      x.setFrequencyRelativeTo( revolutionFrequency_ );
    }
  }

  visit( static_cast<bmlnElmnt&>(x) );
 
}

