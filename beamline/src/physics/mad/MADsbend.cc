/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      MADsbend.cc
******                                                                
******  Copyright  Universities Research Association, Inc.    
******             All Rights Reserved                             
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
******  Jan 2008 ostiguy@fnal.gov
******  
******  - refactored version                        
******  - templated implementation
******  - Propagator is now appropriate for a generic wedge 
******    bend magnet and optinally includes vertical focusing, arbitrary edge 
******    angles and finite body gradient. 
******  - added comments to document the impact of the choice of cdt
******    rather than ds (arc length) as a long phase space coordinate.  
******  - combined support for sbend CF_sbend 
******                                      
*************************************************************************
*************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <basic_toolkit/GenericException.h>
#include <beamline/sbend.h>
#include <beamline/CF_sbend.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <algorithm>

sbend::MAD_Prop sbend::LikeMAD;

namespace { 
 int const i_x   = Particle::xIndex();     
 int const i_y   = Particle::yIndex();     
 int const i_cdt = Particle::cdtIndex();   
 int const i_npx = Particle::npxIndex();   
 int const i_npy = Particle::npyIndex();   
 int const i_ndp = Particle::ndpIndex();   


template <typename T>
inline double standardPart( T const& );

template<>
inline double standardPart( Jet const& value)    { return value.standardPart(); }

template<>
inline double standardPart( double const& value) { return value; }
 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template < typename Element_t, typename Particle_t>
int propagate( Element_t& elm, Particle_t& p, double const& rho, double const& n )
{

  if( BMLN_dynDim != 6 ) {
      throw(  GenericException( __FILE__, __LINE__,
             "int sbend::MAD_Prop::operator()( bmlnElmnt* p_be, Particle& p ) {", 
             "This was written for a phase space dimension of 6." ) );
  }

  typedef typename PropagatorTraits<Particle_t>::State_t     State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t Component_t;

  State_t& state = p.State();
  
  double const h      = 1/rho;                    // curvature
  double const length = elm.Length();         
  double const gamma  = standardPart( p.Gamma() );
  double const beta   = standardPart( p.Beta()  );

  double const angle   = elm.getAngle();

  if(angle == 0.0) {
    state[i_x  ] +=   length * state[i_npx];
    state[i_y  ] +=   length * state[i_npy];
    state[i_cdt] +=  -length /(beta*gamma*gamma) * state[i_ndp];
    return 0;
  }  

  double  matrix [ BMLN_dynDim ][BMLN_dynDim];
  
  for   ( int i=0; i < BMLN_dynDim; ++i) {
    for ( int j=0; j < BMLN_dynDim; ++j){
      matrix[i][j] = ( i==j) ? 1.0 : 0.0;
    }
  }
  
  double const kx     = h * sqrt(1.0 - n ); 
  double const ky     = h * sqrt( n ); 
  
  
  // entrance focusing 
  
  state[i_npx] +=  tan(elm.getEntryEdgeAngle())/rho *  state[i_x]; 
  state[i_npy] -=  tan(elm.getEntryEdgeAngle())/rho *  state[i_y]; 

  matrix[i_x  ][i_x  ] =           cos(kx*length);
  matrix[i_x  ][i_npx] =  (1.0/kx)*sin(kx*length);
  matrix[i_x  ][i_ndp] =  h/(kx*kx)*(1.0 - cos(kx*length));

  matrix[i_npx][i_x   ] = -kx*sin(kx*length);
  matrix[i_npx][i_npx ] =  cos(kx*length);
  matrix[i_npx][i_ndp ] =  h/kx * sin(kx*length); 

  matrix[i_y]  [i_y   ] =  cos(ky*length);
  matrix[i_y]  [i_npy ] = ( ky == 0.0 ) ? length : (1.0/ky)*sin(ky*length);

  matrix[i_npy][i_y   ] = -ky*sin(ky*length);
  matrix[i_npy][i_npy ] =  cos(ky*length);

  const double cnv =  - 1.0/beta;  

  //----------------------------------------------------------------------------------
  // Note: the std optical matrix formalism assumes the arc length s as the independent 
  //       coordinate. CHEF uses cdt with t > 0 when the particle is *late* w/r to 
  //       a reference particle. 
  //       A conversion factor cnv from ds to cdt is introduced to convert from cdt   
  //       to ds. A subtle difference is that cdt depends on dp/p not only 
  //       through the change in path length, but also through the change in velocity
  //       associated with a change in momentum. This latter effect is the origin
  //       of the extra term   - length/(beta*gamma*gamma);   
  // ----------------------------------------------------------------------------------

  matrix[i_cdt][i_x]    =  cnv * (h/kx)*sin(kx*length);
  matrix[i_cdt][i_npx]  =  cnv * (h/kx*kx)*(1.0-cos(kx*length));
  matrix[i_cdt][i_ndp]  =  cnv * (h*h)/(kx*kx*kx)*(kx*length - sin(kx*length) ) - length/(beta*gamma*gamma); 

  // Finally .. the mapping ...............................

  State_t const& inState  = p.State();
  State_t        outState( p.State().Dim() ); 

  for(  int i=0; i<BMLN_dynDim; ++i  ) {
    outState[i] = 0.0;
    for( int j=0; j<BMLN_dynDim; ++j  ) {
      outState[i] += matrix[i][j]*inState[j];
    }
  }

  std::copy( outState.begin(), outState.end(), state.begin() ); 

  // exit focusing ......................... 

  state[i_npx] +=  tan( elm.getExitEdgeAngle())/rho *  state[i_x]; 
  state[i_npy] -=  tan( elm.getExitEdgeAngle())/rho *  state[i_y]; 

  return 0;
}

} // anonymous namespace

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int sbend::MAD_Prop::operator()( bmlnElmnt* p_be, Particle& p ) 
{  

  sbend* pbe       = dynamic_cast<sbend*>(p_be);
  double const rho = p.BRho()/pbe->Strength();
  return ::propagate( *pbe, p, rho, 0.0);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int sbend::MAD_Prop::operator()( bmlnElmnt* p_be, JetParticle& p ) 
{  
  sbend* pbe       = dynamic_cast<sbend*>(p_be);
  double const rho = standardPart(p.BRho())/pbe->Strength();
  return ::propagate( *pbe,p, rho, 0.0);
}

