/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      QuadrupolePropagators.cc
******                                                                
******  Copyright Fermi Research Alliance / Fermilab    
******            All Rights Reserved                             
*****
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-07CH11359 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws.
******                                                                
******                                                                
******  Authors:   Leo Michelotti         michelotti@fnal.gov
******             Jean-Francois Ostiguy  ostiguy@fnal.gov
******
****** REVISION HISTORY:
******
****** May 2008 ostiguy@fnal.gov
******  - propagator moved backed to base class. Use static downcast 
******    in operator()() implementation.
******
**************************************************************************
**************************************************************************
*************************************************************************/

#include <beamline/QuadrupolePropagators.h>
#include <beamline/beamline.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/ParticleBunch.h>
#include <beamline/quadrupole.h>
#include <beamline/drift.h>

namespace {

  Particle::PhaseSpaceIndex const& i_x   = Particle::i_x;
  Particle::PhaseSpaceIndex const& i_y   = Particle::i_y;
  Particle::PhaseSpaceIndex const& i_cdt = Particle::i_cdt;
  Particle::PhaseSpaceIndex const& i_npx = Particle::i_npx;
  Particle::PhaseSpaceIndex const& i_npy = Particle::i_npy;
  Particle::PhaseSpaceIndex const& i_ndp = Particle::i_ndp;

template<typename Component_t>
double toDouble( Component_t const& val );

template<>
double toDouble( Jet const& val ) { return val.standardPart(); }

template<>
double toDouble( double const& val ) { return val; }


// ------------------------------------------------------------------------------

template<typename Particle_t>
void propagate( quadrupole const& elm, Particle_t& p )
{
   
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  State_t& state = p.State();

  BmlPtr const& bml = bmlnElmnt::core_access::get_BmlPtr(elm);

  for ( beamline::const_iterator it = bml->begin(); it != bml->end(); ++it ) { 
     (*it)->localPropagate( p );
  }
  
  state[i_cdt] -= elm.getReferenceTime();
}

// ------------------------------------------------------------------------------

template <typename Particle_t>
void propagate( thinQuad const& elm, Particle_t& p ) 
{

 // "Strength" is B'l in Tesla

 typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
 typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

 State_t& state = p.State();

 if( elm.Strength() == 0.0 ) return; 

 double const k = elm.Strength() / p.ReferenceBRho();
 
 state[i_npx]  += - k * state[i_x]; 
 state[i_npy]  +=   k * state[i_y];

}

// ------------------------------------------------------------------------------

template<typename Particle_t>
void mad_propagate( quadrupole const& elm, Particle_t& p )
{

  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  State_t& state = p.State();

  double const length = elm.Length();

  double q0divp0 = 1.0 + toDouble(p.get_ndp()); 
  double K1      = elm.Strength() / p.ReferenceBRho() / q0divp0;
  double Beta    = toDouble(p.Beta() );   
  double Gamma   = toDouble(p.Gamma());

  double kxsqr =  K1;
  double kysqr = -K1;

  double factor = ( K1 > 0.0 )? sqrt( K1 ): sqrt( -K1 );

  double arg = factor * elm.Length();
 
  double  cn_x = 0.0; 
  double  sn_x = 0.0; 
  double  cn_y = 0.0; 
  double  sn_y = 0.0; 

  if ( elm.Strength() > 0.0 )  {            // Focussing horizontally
      cn_x  = cos( arg );
      sn_x  = sin( arg )/factor;
      cn_y  = cosh( arg );
      sn_y  = sinh( arg )/factor;
   } else {                                 // Defocussing horizontally
      cn_x  = cosh( arg );
      sn_x  = sinh( arg )/factor;
      cn_y  = cos( arg );
      sn_y  = sin( arg )/factor;
   } 
  
    double T200  =   ( length - sn_x * cn_x );
    double T203  = - ( sn_x * sn_x          );
    double T233  =   ( length + sn_x * cn_x );
    double T211  = - ( length - sn_y * cn_y );
    double T214  =   ( sn_y * sn_y          );        
    double T244  =   ( length + sn_y * cn_y );

    State_t instate =  state; 

    instate[i_npx] /= q0divp0;   // p_x/p_0 * p_0/q_0 = p_x/q_0 
    instate[i_npy] /= q0divp0;   // p_y/p_0 * p_0/q_0 = p_y/q_0 

    Component_t npz = p.get_npz();  

    state[i_x  ] =          cn_x * instate[i_x] +  sn_x * instate[i_npx];
    state[i_npx] = -kxsqr * sn_x * instate[i_x] +  cn_x * instate[i_npx];
    state[i_y  ] =          cn_y * instate[i_y] +  sn_y * instate[i_npy];
    state[i_npy] = -kysqr * sn_y * instate[i_y] +  cn_y * instate[i_npy];

    state[i_npx] *= q0divp0;          
    state[i_npy] *= q0divp0;          
    
  // second order correction for the path length (as per MAD manual )

    Component_t Dist = length + ( 
	      ( T200 * instate[i_x  ] + 2.0 * T203 * instate[i_npx] ) * K1 * instate[i_x]
	    +   T233 * instate[i_npx] * instate[i_npx]
	    + ( T211 * instate[i_y  ] + 2.0 * T214 * instate[i_npy] ) * K1 * instate[i_y]
	    +   T244 * instate[i_npy] * instate[i_npy] )/4.0;
  
    state[i_cdt] +=  Dist/Beta - elm.getReferenceTime();

  }


//----------------------------------------------------------------------------------
// Workaround for gcc < 4.2 mishandling of templates defined in anonymous namespace
//----------------------------------------------------------------------------------

#if (__GNUC__ == 3) ||  ((__GNUC__ == 4) && (__GNUC_MINOR__ < 2 ))

template void propagate(     quadrupole const& elm,    Particle& p );
template void propagate(     quadrupole const& elm, JetParticle& p );
template void propagate(       thinQuad const& elm,    Particle& p );
template void propagate(       thinQuad const& elm, JetParticle& p );

#endif

} // namespace

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void quadrupole::Propagator::setup( bmlnElmnt& arg)
{
 
  BmlPtr& bml = bmlnElmnt::core_access::get_BmlPtr(arg);
  ElmPtr& elm = bmlnElmnt::core_access::get_ElmPtr(arg);

  const double lng  = arg.Length();
  const double str  = arg.Strength();
 
  double frontLength     = 6.0*(lng/4.0)/15.0;
  double sepLength       = ( lng - 2.0*frontLength ) / 3.0;
  double quarterStrength = str*lng/4.0;
 
  bml = BmlPtr( new beamline("QUADRUPOLE_PRIVATE") );

  //-----------------------------
  // One thin element ..........
  //-----------------------------

  if( n_ == 1 ) {           
    bml->append( DriftPtr( new drift( "", lng / 2.0 ) ));
    bml->append( elm = ThinQuadPtr( new thinQuad( "", str*lng ) ));
    bml->append( DriftPtr( new drift( "", lng / 2.0 ) ) );
  }
  
  //-----------------------------
  // TEAPOT-like schema .........
  //-----------------------------

  else if( n_ == 4 ) {      
    bml->append( DriftPtr( new drift( "", frontLength ) ));
    bml->append( ThinQuadPtr( new thinQuad( "", quarterStrength ) ) );
 
   for( int i = 0; i<3; ++i) {
      bml->append( DriftPtr( new drift( "", sepLength ) ));
      bml->append( ThinQuadPtr( new thinQuad( "", quarterStrength ) ) );
    }

    bml->append( DriftPtr( new drift( "", frontLength ) ) );
  }
  
  //-----------------------------
  // TEAPOT tandem ...............
  //-----------------------------

  else if( (n_ % 4) == 0 ) { 
    int    u         = n_/4;
    double xu        = u;
    frontLength     /= xu;
    sepLength       /= xu;
    quarterStrength /= xu;

    for( int i=0; i<u; ++i) {
      bml->append( DriftPtr( new drift( "", frontLength ) ) );
      bml->append( ThinQuadPtr( new thinQuad( "", quarterStrength ) ) );

      for( int i=0; i<3; ++i) {
        bml->append( DriftPtr( new drift( "", sepLength ) ));
        bml->append( ThinQuadPtr( new thinQuad( "", quarterStrength ) ) );
      }
      bml->append( DriftPtr( new drift( "", frontLength ) ) );
    }
  }

  //-----------------------------
  // Equal spacing ...............
  //-----------------------------

  else {                   
    bml->append( DriftPtr( new drift( "", lng / ( 2.0*n_ ) )  ));
    bml->append( ThinQuadPtr( new thinQuad( "", str*lng/ n_ ) ) );

    for( int i=0; i<n_-1; ++i ) {
      bml->append( DriftPtr( new drift( "", lng / n_ ) ));
      bml->append( ThinQuadPtr( new thinQuad( "", str*lng/ n_ ) ));
    }
    bml->append( DriftPtr( new drift( "", lng / ( 2.0*n_ ) ) ) );
  }

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  quadrupole::Propagator::setAttribute( bmlnElmnt& elm, std::string const& name, boost::any const& value )
{ 
  setup(elm);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void quadrupole::Propagator::operator()( bmlnElmnt const& elm, Particle& p ) 
{
  ::propagate( static_cast<quadrupole const&>(elm),p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void quadrupole::Propagator::operator()( bmlnElmnt const& elm, JetParticle& p ) 
{
  ::propagate( static_cast<quadrupole const&>(elm),p);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinQuad::Propagator::operator()( bmlnElmnt const& elm, Particle& p ) 
{
  ::propagate( static_cast<thinQuad const&>(elm),p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinQuad::Propagator::operator()( bmlnElmnt const& elm, JetParticle& p ) 
{
  ::propagate( static_cast<thinQuad const&>(elm),p);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void quadrupole::MADPropagator::setup( bmlnElmnt& elm)  
{}
  
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void quadrupole::MADPropagator::operator()(  bmlnElmnt const& elm, Particle& p)
{  
  static bool show_message = true; 
  if ( show_message) { 
    std::cout << "using   quadrupole::MADPropagator" << std::endl;
    show_message = false;
  }
  
 ::mad_propagate(static_cast<quadrupole const&>(elm), p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void quadrupole::MADPropagator::operator()(  bmlnElmnt const& elm, JetParticle& p)
{  
  ::mad_propagate(static_cast<quadrupole const&>(elm), p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  quadrupole::MADPropagator::setAttribute( bmlnElmnt& elm, std::string const& name, boost::any const& value )
{ 
  setup(elm);
}

