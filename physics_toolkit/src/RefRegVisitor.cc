#if HAVE_CONFIG_H
#include <config.h>
#endif
/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   1.0                    
******                                    
******  File:      RefRegVisitor.cc
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
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
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


#include "RefRegVisitor.h"
#include "CF_rbend.h"
#include "CF_sbend.h"
#include "rbend.h"
#include "Slot.h"
#include "Particle.h"

// Static error codes

const int RefRegVisitor::OKAY          = 0;

using namespace std;

// Constructors

RefRegVisitor::RefRegVisitor( const Particle& p )
: _errorCode(OKAY)
{
  _prtnPtr = p.Clone();
}


RefRegVisitor::RefRegVisitor( const RefRegVisitor& x )
: _errorCode(x._errorCode)
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
  x->acceptInner( *this );
}


void RefRegVisitor::visitCF_sbend( CF_sbend* x ) 
{
  x->acceptInner( *this );
}


/*
void RefRegVisitor::visitRbend( rbend* x )
{
  x->setReferenceTime(0.0);
  _prtnPtr->set_cdt(0.0);
  x->propagate( *_prtnPtr );
  x->setReferenceTime( _prtnPtr->get_cdt() );
}


void RefRegVisitor::visitSlot( Slot* x )
{
  x->setReferenceTime(0.0);
  _prtnPtr->set_cdt(0.0);
  x->propagate( *_prtnPtr );
  x->setReferenceTime( _prtnPtr->get_cdt() );

  // x->setReferenceTime( *_prtnPtr );
  // x->propagate( *_prtnPtr );
}
*/
