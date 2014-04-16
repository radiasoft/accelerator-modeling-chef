/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.      
******                
******  File:      combinedFuncPhys.cc
******  Version:   2.0                    
******                                    
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
*****
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
**************************************************************************
*************************************************************************/


#include <beamline/beamline.h>
#include <beamline/combinedFunction.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void combinedFunction::localPropagate( Particle& p ) 
{
  if( propfunc_ ) {
    (*propfunc_)( this, p );
  }
  else {
    p_bml_->propagate( p );
    p.set_cdt( p.get_cdt() - ctRef_ );
  }

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void combinedFunction::localPropagate( JetParticle& p ) 
{
  if( propfunc_ ) {
    (*propfunc_)( this, p );
  }
  else {
    p_bml_->propagate( p );
    p.set_cdt( p.get_cdt() - ctRef_ );
  }
}
  
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
