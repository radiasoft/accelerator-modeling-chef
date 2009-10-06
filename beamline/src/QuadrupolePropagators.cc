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
#include <beamline/Drift.h>

namespace {

 typedef PhaseSpaceIndexing::index index;

 index const i_x   = PhaseSpaceIndexing::i_x;
 index const i_y   = PhaseSpaceIndexing::i_y;
 index const i_cdt = PhaseSpaceIndexing::i_cdt;
 index const i_npx = PhaseSpaceIndexing::i_npx;
 index const i_npy = PhaseSpaceIndexing::i_npy;
 index const i_ndp = PhaseSpaceIndexing::i_ndp;

template<typename Component_t>
inline double toDouble( Component_t const& val );

template<>
inline double toDouble( Jet const& val ) { return val.standardPart(); }

template<>
inline double toDouble( double const& val ) { return val; }



} // namespace

//******************************************************************************************
//
//   ThinQuad::Propagator
//
//******************************************************************************************

namespace { 

template <typename Particle_t>
void propagate( thinQuad const& elm, Particle_t& p ) 
{

 // "Strength" is B'l in Tesla

 typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
 typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

 State_t& state = p.state();

 if( elm.Strength() == 0.0 ) return; 

 double const k = elm.Strength() / p.refBrho();
 
 state[i_npx]  += - k * state[i_x]; 
 state[i_npy]  +=   k * state[i_y];

}
} // namespace


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinQuad::Propagator::Propagator::Propagator()
 : BasePropagator()
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinQuad::Propagator::Propagator( thinQuad const& elm)
 : BasePropagator(elm)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinQuad::Propagator::Propagator( thinQuad::Propagator const& p)
: BasePropagator(p)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinQuad::Propagator::operator()( BmlnElmnt const& elm, Particle& p ) 
{
  ::propagate( static_cast<thinQuad const&>(elm),p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinQuad::Propagator::operator()( BmlnElmnt const& elm, JetParticle& p ) 
{
  ::propagate( static_cast<thinQuad const&>(elm),p);
}

//******************************************************************************************
//
//   Quadrupole::Propagator
//
//******************************************************************************************

namespace { 

template<typename Particle_t>
void propagate( quadrupole const& elm, Particle_t& p,  BmlPtr bml )
{
   
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  State_t& state = p.state();

  for ( beamline::const_iterator it = bml->begin(); it != bml->end(); ++it ) { 
     (*it)->localPropagate( p );
  }
  
  state[i_cdt] -= elm.getReferenceTime();
}

} // namespace

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

quadrupole::Propagator::Propagator(int n)
  : n_(n),BasePropagator()
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

quadrupole::Propagator::Propagator(quadrupole const& elm, int n) 
  : n_(n), BasePropagator(elm)
{
  ctor(elm);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

quadrupole::Propagator::Propagator(quadrupole::Propagator const& p)
  : n_(p.n_), BasePropagator(p)
{}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

quadrupole::Propagator*  quadrupole::Propagator::clone() const
{
 return new Propagator(*this);
} 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void quadrupole::Propagator::ctor( BmlnElmnt const& arg )
{
  quadrupole const& elm = static_cast< quadrupole const&>(arg); 

  double const lng  = arg.Length();
  double const str  = arg.Strength();
 
  double frontLength     = 6.0*(lng/4.0)/15.0;
  double sepLength       = ( lng - 2.0*frontLength ) / 3.0;
  double quarterStrength = str*lng/4.0;
 
  bml_ = BmlPtr( new beamline("QUADRUPOLE_PRIVATE") );

  //-----------------------------
  // One thin element ..........
  //-----------------------------

  if( n_ == 1 ) {           
    bml_->append( DriftPtr( new Drift( "", lng / 2.0 ) ));
    bml_->append( ThinQuadPtr( new thinQuad( "", str*lng ) ));
    bml_->append( DriftPtr( new Drift( "", lng / 2.0 ) ) );
  }
  
  //-----------------------------
  // TEAPOT-like schema .........
  //-----------------------------

  else if( n_ == 4 ) {      
    bml_->append( DriftPtr( new Drift( "", frontLength ) ));
    bml_->append( ThinQuadPtr( new thinQuad( "", quarterStrength ) ) );
 
   for( int i = 0; i<3; ++i) {
      bml_->append( DriftPtr( new Drift( "", sepLength ) ));
      bml_->append( ThinQuadPtr( new thinQuad( "", quarterStrength ) ) );
    }

    bml_->append( DriftPtr( new Drift( "", frontLength ) ) );
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
      bml_->append( DriftPtr( new Drift( "", frontLength ) ) );
      bml_->append( ThinQuadPtr( new thinQuad( "", quarterStrength ) ) );

      for( int i=0; i<3; ++i) {
        bml_->append( DriftPtr( new Drift( "", sepLength ) ));
        bml_->append( ThinQuadPtr( new thinQuad( "", quarterStrength ) ) );
      }
      bml_->append( DriftPtr( new Drift( "", frontLength ) ) );
    }
  }

  //-----------------------------
  // Equal spacing ...............
  //-----------------------------

  else {                   
    bml_->append( DriftPtr( new Drift( "", lng / ( 2.0*n_ ) )  ));
    bml_->append( ThinQuadPtr( new thinQuad( "", str*lng/ n_ ) ) );

    for( int i=0; i<n_-1; ++i ) {
      bml_->append( DriftPtr( new Drift( "", lng / n_ ) ));
      bml_->append( ThinQuadPtr( new thinQuad( "", str*lng/ n_ ) ));
    }
    bml_->append( DriftPtr( new Drift( "", lng / ( 2.0*n_ ) ) ) );
  }

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  quadrupole::Propagator::setAttribute( BmlnElmnt& elm, std::string const& name, boost::any const& value )
{ 
  ctor(elm);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void quadrupole::Propagator::operator()( BmlnElmnt const& elm, Particle& p ) 
{
  ::propagate( static_cast<quadrupole const&>(elm),p, bml_);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void quadrupole::Propagator::operator()( BmlnElmnt const& elm, JetParticle& p ) 
{
  ::propagate( static_cast<quadrupole const&>(elm),p, bml_);
}

//******************************************************************************************
//
//   Quadrupole::MADPropagator
//
//******************************************************************************************

namespace { 

template<typename Particle_t>
void mad_propagate( quadrupole const& elm, Particle_t& p )
{

  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  State_t& state = p.state();

  double const length = elm.Length();

  double q0divp0 = 1.0 + toDouble(p.ndp()); 
  double K1      = elm.Strength() / p.refBrho() / q0divp0;
  double Beta    = toDouble(p.beta() );   
  double Gamma   = toDouble(p.gamma());

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

    Component_t npz = p.npz();  

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

} //namespace

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

quadrupole::MADPropagator::MADPropagator()
 : BasePropagator()
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

quadrupole::MADPropagator::MADPropagator( quadrupole const& elm)
 : BasePropagator(elm)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

quadrupole::MADPropagator::MADPropagator( quadrupole::MADPropagator const& p)
 : BasePropagator(p)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void quadrupole::MADPropagator::ctor( BmlnElmnt const& elm)  
{}
  
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void quadrupole::MADPropagator::operator()(  BmlnElmnt const& elm, Particle& p)
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

void quadrupole::MADPropagator::operator()(  BmlnElmnt const& elm, JetParticle& p)
{  
  ::mad_propagate(static_cast<quadrupole const&>(elm), p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  quadrupole::MADPropagator::setAttribute( BmlnElmnt& elm, std::string const& name, boost::any const& value )
{ 
  ctor(elm);
}

