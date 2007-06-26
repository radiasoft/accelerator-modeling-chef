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


namespace {

 template <typename Particle_t>
 void propagate( Solenoid& elm, Particle_t& p );

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Solenoid::localPropagate( Particle& p )
{

  if( 0 == strength_ ) {
    bmlnElmnt::localPropagate( p );
    return;
  }
 
  ::propagate( *this, p);

}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Solenoid::localPropagate( JetParticle& p )
{

  if( 0 == strength_ ) {
    bmlnElmnt::localPropagate( p );
    return;
  }

  ::propagate(*this, p);

}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

namespace {

template <typename Particle_t>
void propagate( Solenoid& elm, Particle_t & p )
{


  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
 

  // NOTE: This implementation assumes the magnetic axis of
  // NOTE: the solenoid AND the beam current are in the +z direction.


  // Entry edge effect
  // -----------------

  const double edgeKick = elm.Strength() / ( 2.0* p.ReferenceBRho() );

  if( elm.hasInEdge() ) {

    p.set_npx( p.get_npx() + edgeKick*(p.get_y()) );
    p.set_npy( p.get_npy() - edgeKick*(p.get_x()) );

  }


  // Body field
  // -----------

  Component_t dtheta =   ( elm.Strength() * elm.Length() / p.ReferenceBRho() ) 
                 / p.get_npz();
  
  Component_t sn = sin(dtheta);
  Component_t cs = cos(dtheta);

  #if 0
  //----------------------------------------------------------------------------
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
  //----------------------------------------------------------------------------
  #endif
  
  Component_t x  = p.get_x();
  Component_t y  = p.get_y();
  Component_t px = p.get_npx();
  Component_t py = p.get_npy();

  p.set_npx(    cs*px + sn*py );
  p.set_npy( (-sn)*px + cs*py );
  
  cs = cs - 1.0;

  p.set_x( x + ( (    cs*(-py) + sn*px ) / (2.0*edgeKick) ) );
  p.set_y( y + ( ( (-sn)*(-py) + cs*px ) / (2.0*edgeKick) ) );


  // Exit edge effect
  // ----------------
  if( elm.hasOutEdge() ) {
    p.set_npx( p.get_npx() - edgeKick*(p.get_y()) );
    p.set_npy( p.get_npy() + edgeKick*(p.get_x()) );
  }


  // It's about time
  // Assumes pre-registration via RefRegVisitor
  // 
  // NOTE: p.get_npz() is non-negative by definition.
  // NOTE: I may want to change this some day.
  // -----------------------------------------------

  Component_t duration =   elm.Length() 
                   / ( p.get_npz() * p.ReferenceMomentum() / p.Energy() );

  p.set_cdt ( p.get_cdt() + ( duration - elm.getReferenceTime() ) );
}

} // anonymous namespace
