/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Chef: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                                                
******                                    
******  File:      CHEF.builders.cc
******  Version:   3.1
******                                                                
******  Copyright (c) 2004  Universities Research Association, Inc.   
******                All Rights Reserved                             
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
******  URA/FNAL reserves all rights.                        
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
****** REVISION HISTORY
****** Mar 2007 ostiguy@fnal.gov
****** - changes to support new semantics of reference counted beamlines
******                                                                
**************************************************************************
*************************************************************************/


// 
// Header files.
// 

#include <fstream>

#include <basic_toolkit/PhysicsConstants.h>
#include <beamline/beamline.h>
#include <beamline/drift.h>
#include <beamline/quadrupole.h>
#include <beamline/sextupole.h>
#include <bmlfactory/bmlfactory.h>
#include <physics_toolkit/BeamlineContext.h>

#include "QtMonitor.h"
#include "CHEFGUI.h"

using namespace std;

void CHEFGUI::makeFODO()
{
  BmlPtr bmlPtr( new beamline( "5_FODO_cells" ) );

  const double f =  7.0;  /*[m]*/                 // focal length of thin quads
  const double l = 10.0;  /*[m]*/                 // length of drift
  const double E = 100.0 - PH_NORM_mp; /*[Gev]*/  // proton's kinetic energy (arbitrary)

  double energy = E + PH_NORM_mp;
  Proton pr( energy );

  drift    O ( "O",   l/2.0 );
  thinQuad F ( "F",    pr.ReferenceBRho() / f );
  thinQuad D ( "D",  - pr.ReferenceBRho() / f );
 
  QtMonitor M( "Qt_monitor" );

  bmlPtr->append( M );
  for( int i = 0; i < 5; i++ ) {
    bmlPtr->append( O );
    bmlPtr->append( F );
    bmlPtr->append( M );
    bmlPtr->append( O );
    bmlPtr->append( M );
    bmlPtr->append( O );
    bmlPtr->append( D );
    bmlPtr->append( M );
    bmlPtr->append( O );
    bmlPtr->append( M );
  }
   
  bmlPtr->setEnergy( energy );
  
  p_currBmlCon_ = BmlContextPtr( new BeamlineContext( Proton(energy), bmlPtr) );
  contextList_.push_front( p_currBmlCon_ );

  p_currBmlCon_->handleAsRing();

  emit new_beamline();
}


void CHEFGUI::makeSingSext()
{
  BmlPtr bmlPtr( new beamline( "Henon lattice" ) );

  const double f =  7.0;  /*[m]*/                 // focal length of thin quads
  const double l = 10.0;  /*[m]*/                 // length of drift
  const double E = 100.0 - PH_NORM_mp; /*[Gev]*/  // proton's kinetic energy (arbitrary)

  double energy = E + PH_NORM_mp;
  Proton pr( energy );

  drift         O ( "O",   l/2.0 );
  thinQuad      F ( "F",    pr.ReferenceBRho() / f );
  thinQuad      D ( "D",  - pr.ReferenceBRho() / f );
  thinSextupole S ( "S", 100.0*F.Strength() );
 
  QtMonitor M( "Qt monitor" );

  bmlPtr->append( M );
  bmlPtr->append( O );
  bmlPtr->append( F );
  bmlPtr->append( M );
  bmlPtr->append( O );
  bmlPtr->append( M );
  bmlPtr->append( S );
  bmlPtr->append( O );
  bmlPtr->append( D );
  bmlPtr->append( M );
  bmlPtr->append( O );
  bmlPtr->append( M );
   
  bmlPtr->setEnergy( energy );
  
  p_currBmlCon_ = BmlContextPtr( new BeamlineContext( Proton(energy), bmlPtr) );
  contextList_.push_front( p_currBmlCon_ );

  p_currBmlCon_->handleAsRing();

  emit new_beamline();
}


