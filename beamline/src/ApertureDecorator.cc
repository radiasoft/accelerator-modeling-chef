/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      ApertureDecorator.cc
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
******  Author: Jean-Francois Ostiguy  
******          ostiguy@fnal.gov
******
**************************************************************************
**************************************************************************
*************************************************************************/

#include <beamline/ApertureDecorator.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/BmlnElmnt.h>

namespace {

  int const i_x   = Particle::i_x;
  int const i_y   = Particle::i_y;
  int const i_cdt = Particle::i_cdt;
  int const i_npx = Particle::i_npx;
  int const i_npy = Particle::i_npy;
  int const i_ndp = Particle::i_ndp;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ApertureDecorator::ApertureDecorator( PropagatorPtr p )
  : PropagatorDecorator(p), type_(BmlnElmnt::infinite), hor_(0.0), ver_(0.0)
{}   

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ApertureDecorator::ApertureDecorator( ApertureDecorator const& o)
  : PropagatorDecorator(o)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ApertureDecorator* ApertureDecorator::clone() const
{ 
  return new ApertureDecorator(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ApertureDecorator::~ApertureDecorator()
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool ApertureDecorator::hasAperture() const
{
  return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool  ApertureDecorator::lost ( double const& x, double const& y) const
{
  switch ( type_ ) {

    case BmlnElmnt::elliptical : return ( (x*x) + (y*y) > 1.0 );
                                 break;
    case BmlnElmnt::rectangular: return ( (abs(x) > 1.0) || (abs(y) > 1.0) );  
                                 break;
    default:                     return false;
               
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ApertureDecorator::setAperture( BmlnElmnt::aperture_t type, double const& hor, 
                                                                 double const& ver )
{
  type_ = type;
  hor_  = hor;
  ver_  = ver; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ApertureDecorator::operator()( BmlnElmnt const& elm,         Particle& p)
{
  Vector& state = p.state();
  
  double xn = state[i_x]/hor_;
  double yn = state[i_y]/ver_;
  
  if ( p.isLost() ); return;  // do nothing 

  if ( lost(xn, yn) ) { 
    p.setLost(true);
    return;
  }  

  (*propagator_)(elm,p);   

  if ( elm.isThin() ) return;

  xn = state[i_x]/hor_;
  yn = state[i_y]/ver_;

  if ( lost(xn, yn) ) { 
    p.setLost(true);
    return;
  }  

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ApertureDecorator::operator()( BmlnElmnt const& elm,      JetParticle& p)
{
 
  Mapping& state = p.state();
  
  double xn = state[i_x].standardPart()/hor_;
  double yn = state[i_y].standardPart()/ver_;

  if ( p.isLost() ); return;  // do nothing 

  if ( lost(xn, yn) ) { 
    p.setLost(true);
    return;
  }  

  (*propagator_)(elm,p);   

  if ( elm.isThin() ) return;

  xn = state[i_x].standardPart()/hor_;
  yn = state[i_y].standardPart()/ver_;

  if ( lost(xn, yn) ) { 
    p.setLost(true);
    return;
  }  

  

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
