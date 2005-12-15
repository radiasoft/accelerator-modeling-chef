/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      RefRegVisitor.cc
******  Version:   1.1
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

#include "RefRegVisitor.h"
#include "PhysicsConstants.h"
#include "CF_rbend.h"
#include "CF_sbend.h"
#include "rbend.h"
#include "sbend.h"
#include "Slot.h"
#include "rfcavity.h"
#include "Particle.h"
#include "BeamlineIterator.h"

// Static error codes

const int RefRegVisitor::OKAY          = 0;

using namespace std;

// Constructors

RefRegVisitor::RefRegVisitor( const Particle& p )
:   _prtnPtr(0)
  , _errorCode(OKAY)
  , _revolutionFrequency(-1.0)
{
  _prtnPtr = p.Clone();
}


RefRegVisitor::RefRegVisitor( const RefRegVisitor& x )
:   _prtnPtr(0)
  , _errorCode(OKAY)
  , _revolutionFrequency(x._revolutionFrequency)
{
  _prtnPtr = x._prtnPtr->Clone();
}


RefRegVisitor::~RefRegVisitor()
{
  delete _prtnPtr;
}


int RefRegVisitor::getErrorCode() const
{
  return _errorCode;
}


// Visiting functions

void RefRegVisitor::visitBeamline( beamline* x )
{
  x->setEnergy( _prtnPtr->Energy() );  // not just reference energy

  // Preliminary traversal in case there are
  // RF cavities in the line.
  if( _revolutionFrequency < 0 ) {
    Particle* copy = _prtnPtr->Clone();
    copy->set_cdt(0);
    bmlnElmnt* q;
    DeepBeamlineIterator dbi( x );
    while((  q = dbi++  )) {
      q->setReferenceTime(0);
      q->propagate( *copy );
    }
    _revolutionFrequency = PH_MKS_c /( copy->get_cdt() );
    delete copy;
  }

  this->BmlVisitor::visitBeamline( x );
}


void RefRegVisitor::visitBmlnElmnt( bmlnElmnt* x )
{
  x->setReferenceTime(0.0);
  _prtnPtr->set_cdt(0.0);
  x->propagate( *_prtnPtr );
  x->setReferenceTime( _prtnPtr->get_cdt() );
  // x->propagate( *_prtnPtr );
}


void RefRegVisitor::visitCF_rbend( CF_rbend* x ) 
{
  // This does not nullify the edge focussing from 
  // the inner propagators before setting the entry
  // and exit angles. It may be that these attributes
  // are not even used.
  x->setEntryAngle( *_prtnPtr );
  x->acceptInner( *this );
  x->setExitAngle( *_prtnPtr );
}


void RefRegVisitor::visitCF_sbend( CF_sbend* x ) 
{
  // This does not nullify the edge focussing from 
  // the inner propagators before setting the entry
  // and exit angles. It may be that these attributes
  // are not even used.
  x->setEntryAngle( *_prtnPtr );
  x->acceptInner( *this );
  x->setExitAngle( *_prtnPtr );
}


// REMOVE: void RefRegVisitor::visitSbend( sbend* x )
// REMOVE: {
// REMOVE:   x->setEntryAngle( *_prtnPtr );
// REMOVE:   this->visitBmlnElmnt( ((bmlnElmnt*) x) );
// REMOVE:   x->setExitAngle( *_prtnPtr );
// REMOVE: }


void RefRegVisitor::visitSbend( sbend* x )
{
  const bmlnElmnt::PropFunc* propPtr = x->getPropFunction();
  x->setPropFunction( &sbend::NoEdge );
  x->setEntryAngle( *_prtnPtr );
  this->visitBmlnElmnt( ((bmlnElmnt*) x) );
  x->setExitAngle( *_prtnPtr );
  x->setPropFunction( propPtr );
}


void RefRegVisitor::visitRbend( rbend* x )
{
  const bmlnElmnt::PropFunc* propPtr = x->getPropFunction();
  x->setPropFunction( &rbend::NoEdge );
  x->setEntryAngle( *_prtnPtr );
  this->visitBmlnElmnt( ((bmlnElmnt*) x) );
  x->setExitAngle( *_prtnPtr );
  x->setPropFunction( propPtr );
}


void RefRegVisitor::visitThinrfcavity( thinrfcavity* x ) 
{
  if( x->getFrequency() < 1.0e-9 ) {
    if( 0 < x->getHarmonicNumber() ) {
      x->setFrequencyRelativeTo( _revolutionFrequency );
    }
  }
}


// REMOVE: void RefRegVisitor::visitSlot( Slot* x )
// REMOVE: {
// REMOVE:   x->setReferenceTime(0.0);
// REMOVE:   _prtnPtr->set_cdt(0.0);
// REMOVE:   x->propagate( *_prtnPtr );
// REMOVE:   x->setReferenceTime( _prtnPtr->get_cdt() );
// REMOVE: 
// REMOVE:   // x->setReferenceTime( *_prtnPtr );
// REMOVE:   // x->propagate( *_prtnPtr );
// REMOVE: }
