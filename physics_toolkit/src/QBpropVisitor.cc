/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   1.0                    
******                                    
******  File:      QBpropVisitor.cc
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
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


#include "beamline.h"
#include "QBpropVisitor.h"

using namespace std;

QBpropVisitor::QBpropVisitor( const Particle& x )
: particleVisitor( x )
{
}


QBpropVisitor::QBpropVisitor( const QBpropVisitor& x )
: particleVisitor( (const particleVisitor&) x )
{
}

QBpropVisitor::~QBpropVisitor()
{
}



void QBpropVisitor::visitBmlnElmnt( bmlnElmnt* x )
{
  static drift OO( 1. );
  static double lng;
  if( 0.0 != ( lng = x->Length() ) )  {
    OO.setLength( lng );
    OO.propagate( *particle );
  }
}


void QBpropVisitor::visitMarker( marker* x )
{
  // Do nothing.
}


void QBpropVisitor::visitDrift( drift* x )
{
  x->propagate( *particle );
}


void QBpropVisitor::visitRbend( rbend* x )
{
  x->propagate( *particle );
}


void QBpropVisitor::visitSbend( sbend* x )
{
  x->propagate( *particle );
}


void QBpropVisitor::visitSector( sector* x )
{
  x->propagate( *particle );
  cerr << "*** WARNING ***                              \n"
          "*** WARNING *** QBpropVisitor::visitSector   \n"
          "*** WARNING *** Using 1st order part of map  \n"
          "*** WARNING ***                              \n"
       << endl;
  //exit(1);
}


void QBpropVisitor::visitQuadrupole( quadrupole* x )
{
  x->propagate( *particle );
}


void QBpropVisitor::visitJetQuadrupole( JetQuadrupole* x )
{
  x->propagate( *particle );
}


void QBpropVisitor::visitThinQuad( thinQuad* x )
{
  x->propagate( *particle );
}


void QBpropVisitor::visitSlot( Slot* x )
{
  x->propagate( *particle );
}


void QBpropVisitor::visitCF_rbend( CF_rbend* x )
{
  x->propagate( *particle );
}


void QBpropVisitor::getState( Vector& x )
{
  particle->getState( x );
}


void QBpropVisitor::getState( Vector* x )
{
  particle->getState( x );
}


void QBpropVisitor::getState( double* x )
{
  particle->getState( x );
}

Vector QBpropVisitor::State()
{
  return particle->State();
}
