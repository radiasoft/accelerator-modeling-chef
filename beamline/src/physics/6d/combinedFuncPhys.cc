/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      combinedFuncPhys.cc
******                                                                
******  Copyright (c) 1991 Universities Research Association, Inc.    
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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "combinedFunction.h"
#include "Particle.h"

void combinedFunction::localPropagate( Particle& p ) 
{
  if( Propagator ) {
    (*Propagator)( this, p );
  }
  else {
    p_bml->propagate( p );
    p.set_cdt( p.get_cdt() - _ctRef );
  }

}

void combinedFunction::localPropagate( JetParticle& p ) 
{
  if( Propagator ) {
    (*Propagator)( this, p );
  }
  else {
    p_bml->propagate( p );
    p.set_cdt( p.get_cdt() - _ctRef );
  }
}
  
