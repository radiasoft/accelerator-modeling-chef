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
****** Mar 2007       ostiguy@fnal.gov
****** - interface based on Particle/JetParticle references
****** - use stack variables for local Particles/JetParticles
****** - visitor interface takes advantage of (reference) dynamic type
******                                                                
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <physics_toolkit/QBpropVisitor.h>
#include <beamline/beamline_elements.h>

using namespace std;

QBpropVisitor::QBpropVisitor( Particle const& p )
: ParticleVisitor( p )
{}


QBpropVisitor::QBpropVisitor( QBpropVisitor const& x )
: ParticleVisitor(x)
{}


QBpropVisitor::~QBpropVisitor()
{}



void QBpropVisitor::visit( bmlnElmnt& x )
{
  static drift OO( 1. );
  static double lng;
  if( 0.0 != ( lng = x.Length() ) )  {
    OO.setLength( lng );
    OO.propagate( particle_ );
  }
}


void QBpropVisitor::visit( marker& x )
{
  // Do nothing.
}


void QBpropVisitor::visit( drift& x )
{
  x.propagate( particle_ );
}


void QBpropVisitor::visit( rbend& x )
{
  x.propagate( particle_ );
}


void QBpropVisitor::visit( sbend& x )
{
  x.propagate( particle_ );
}


void QBpropVisitor::visit( sector& x )
{
  x.propagate( particle_ );
  cerr << "*** WARNING ***                              \n"
          "*** WARNING *** QBpropVisitor::visitSector   \n"
          "*** WARNING *** Using 1st order part of map  \n"
          "*** WARNING ***                              \n"
       << endl;
  //exit(1);
}


void QBpropVisitor::visit( quadrupole& x )
{
  x.propagate( particle_ );
}


void QBpropVisitor::visit( thinQuad& x )
{
  x.propagate( particle_ );
}


void QBpropVisitor::visit( Slot& x )
{
  x.propagate( particle_ );
}


void QBpropVisitor::visit( CF_rbend& x )
{
  x.propagate( particle_ );
}


Vector& QBpropVisitor::getState()
{
  return particle_.getState();
}


Vector QBpropVisitor::State()
{
  return particle_.State();
}
