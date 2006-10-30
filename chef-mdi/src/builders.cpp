/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
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


// 
// Header files.
// 

#include <fstream>

#include "beamline.h"
#include "BeamlineContext.h"
#include "bmlfactory.h"
#include "QtMonitor.h"
#include <beamline/drift.h>
#include <beamline/quadrupole.h>
#include <beamline/sextupole.h>


// FIX THIS:
// External variable that has to be defined for the MAD parser
madparser* mp = 0;

#include "CHEFGUI.h"

using namespace std;

void CHEFGUI::_makeFODO()
{
  beamline* bmlPtr = new beamline( "5_FODO_cells" );

  const double f =  7.0;  /*[m]*/                 // focal length of thin quads
  const double l = 10.0;  /*[m]*/                 // length of drift
  const double E = 100.0 - PH_NORM_mp; /*[Gev]*/  // proton's kinetic energy (arbitrary)

  double energy = E + PH_NORM_mp;
  Proton pr( energy );

  drift    O ( "O",   l/2.0 );
  thinQuad F ( "F",    pr.ReferenceBRho() / f );
  thinQuad D ( "D",  - pr.ReferenceBRho() / f );
 
  QtMonitor M( "Qt_monitor" );

  bmlPtr->append( M.Clone() );
  for( int i = 0; i < 5; i++ ) {
    bmlPtr->append( O.Clone() );
    bmlPtr->append( F.Clone() );
    bmlPtr->append( M.Clone() );
    bmlPtr->append( O.Clone() );
    bmlPtr->append( M.Clone() );
    bmlPtr->append( O.Clone() );
    bmlPtr->append( D.Clone() );
    bmlPtr->append( M.Clone() );
    bmlPtr->append( O.Clone() );
    bmlPtr->append( M.Clone() );
  }
   
  bmlPtr->setEnergy( energy );
  
  _p_currBmlCon = new BeamlineContext( Proton(energy), bmlPtr, false );
  _p_currBmlCon->setClonedFlag( true );
  _contextList.insert( _p_currBmlCon );

  _p_currBmlCon->handleAsRing();

  emit _new_beamline();
}


void CHEFGUI::_makeSingSext()
{
  beamline* bmlPtr = new beamline( "Henon lattice" );

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

  bmlPtr->append( M.Clone() );
  bmlPtr->append( O.Clone() );
  bmlPtr->append( F.Clone() );
  bmlPtr->append( M.Clone() );
  bmlPtr->append( O.Clone() );
  bmlPtr->append( M.Clone() );
  bmlPtr->append( S.Clone() );
  bmlPtr->append( O.Clone() );
  bmlPtr->append( D.Clone() );
  bmlPtr->append( M.Clone() );
  bmlPtr->append( O.Clone() );
  bmlPtr->append( M.Clone() );
   
  bmlPtr->setEnergy( energy );
  
  _p_currBmlCon = new BeamlineContext( Proton(energy), bmlPtr, false );
  _p_currBmlCon->setClonedFlag( true );
  _contextList.insert( _p_currBmlCon );

  _p_currBmlCon->handleAsRing();

  emit _new_beamline();
}


