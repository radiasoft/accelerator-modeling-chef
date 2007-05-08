/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      SolenoidPhysics.cc
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
******  Revision Date: 
******  Original version: April 10, 2007
******                                                                
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

// ??? #include<limits> ???

#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/Solenoid.h>


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Solenoid::localPropagate( Particle& p )
{
  // NOTE: This implementation assumes the magnetic axis of
  // NOTE: the solenoid AND the beam current are in the +z direction.

  if( 0 == strength_ ) {
    bmlnElmnt::localPropagate( p );
    return;
  }

  // Entry edge effect
  // -----------------
  const double edgeKick = strength_ / ( 2.0* p.ReferenceBRho() );
  if( inEdge_ ) {
    p.set_npx( p.get_npx() + edgeKick*(p.get_y()) );
    p.set_npy( p.get_npy() - edgeKick*(p.get_x()) );
  }


  // Body field
  // -----------
  const double dtheta =   ( strength_ * length_ / p.ReferenceBRho() ) 
                        / p.get_npz();
  
  double sn = sin(dtheta);
  double cs = cos(dtheta);

  // ... In what follows, I assume that dtheta is smaller than pi/4.
  const double threshold = 1.0e-14;
  // ??? const double threshold = 4.0*numeric_limits<double>::epsilon();  ???
  int counter = 0;
  double eps = ( 1 - sn*sn - cs*cs ) / (2.0*cs);
  while( threshold < fabs(eps) ) {
    if( counter < 8 ) {
      cs += eps;
      eps = ( 1 - sn*sn - cs*cs ) / (2.0*cs);
      ++counter;
    }
    else {
      throw bmlnElmnt::GenericException( __FILE__, __LINE__, 
            "void Solenoid::localPropagate( Particle& p )",
            "Too many iterations to normalize cosine." );
    }
  }
  
  double x  = p.get_x();
  double y  = p.get_y();
  double px = p.get_npx();
  double py = p.get_npy();

  p.set_npx(    cs*px + sn*py );
  p.set_npy( (-sn)*px + cs*py );
  
  cs -= 1;

  p.set_x( x + ( (    cs*(-py) + sn*px ) / (2.0*edgeKick) ) );
  p.set_y( y + ( ( (-sn)*(-py) + cs*px ) / (2.0*edgeKick) ) );


  // Exit edge effect
  // ----------------
  if( outEdge_ ) {
    p.set_npx( p.get_npx() - edgeKick*(p.get_y()) );
    p.set_npy( p.get_npy() + edgeKick*(p.get_x()) );
  }


  // It's about time
  // Assumes pre-registration via RefRegVisitor
  // 
  // NOTE: p.get_npz() is non-negative by definition.
  // NOTE: I may want to change this some day.
  // -----------------------------------------------
  const double duration =   length_ 
                          / ( p.get_npz() * p.ReferenceMomentum() / p.Energy() );
  p.set_cdt ( p.get_cdt() + ( duration - ctRef_ ) );
}



//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Solenoid::localPropagate( JetParticle& p )
{
  // NOTE: This implementation assumes the magnetic axis of
  // NOTE: the solenoid AND the beam current are in the +z direction.

  if( 0 == strength_ ) {
    bmlnElmnt::localPropagate( p );
    return;
  }

  // Entry edge effect
  // -----------------
  const double edgeKick = strength_ / ( 2.0* p.ReferenceBRho() );
  if( inEdge_ ) {
    p.set_npx( p.get_npx() + edgeKick*(p.get_y()) );
    p.set_npy( p.get_npy() - edgeKick*(p.get_x()) );
  }


  // Body field
  // -----------
  Jet dtheta =   ( strength_ * length_ / p.ReferenceBRho() ) 
                 / p.get_npz();
  
  Jet sn = sin(dtheta);
  Jet cs = cos(dtheta);

  #if 0
  // ... In what follows, I assume that dtheta is smaller than pi/4.
  const double threshold = 1.0e-14;
  // ??? const double threshold = 4.0*numeric_limits<double>::epsilon(); ???
  int counter = 0;
  double eps = ( 1 - sn*sn - cs*cs ) / (2.0*cs);
  while( threshold < fabs(eps) ) {
    if( counter < 8 ) {
      cs += eps;
      eps = ( 1 - sn*sn - cs*cs ) / (2.0*cs);
      ++counter;
    }
    else {
      throw bmlnElmnt::GenericException( __FILE__, __LINE__, 
            "void Solenoid::localPropagate( Particle& p )",
            "Too many iterations to normalize cosine." );
    }
  }
  #endif
  
  Jet x  = p.get_x();
  Jet y  = p.get_y();
  Jet px = p.get_npx();
  Jet py = p.get_npy();

  p.set_npx(    cs*px + sn*py );
  p.set_npy( (-sn)*px + cs*py );
  
  cs = cs - 1.0;

  p.set_x( x + ( (    cs*(-py) + sn*px ) / (2.0*edgeKick) ) );
  p.set_y( y + ( ( (-sn)*(-py) + cs*px ) / (2.0*edgeKick) ) );


  // Exit edge effect
  // ----------------
  if( outEdge_ ) {
    p.set_npx( p.get_npx() - edgeKick*(p.get_y()) );
    p.set_npy( p.get_npy() + edgeKick*(p.get_x()) );
  }


  // It's about time
  // Assumes pre-registration via RefRegVisitor
  // 
  // NOTE: p.get_npz() is non-negative by definition.
  // NOTE: I may want to change this some day.
  // -----------------------------------------------
  Jet duration =   length_ 
                   / ( p.get_npz() * p.ReferenceMomentum() / p.Energy() );
  p.set_cdt ( p.get_cdt() + ( duration - ctRef_ ) );
}
