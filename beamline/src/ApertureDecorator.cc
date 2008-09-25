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
#include <beamline/bmlnElmnt.h>

namespace {

  Particle::PhaseSpaceIndex const& i_x   = Particle::i_x;
  Particle::PhaseSpaceIndex const& i_y   = Particle::i_y;
  Particle::PhaseSpaceIndex const& i_cdt = Particle::i_cdt;
  Particle::PhaseSpaceIndex const& i_npx = Particle::i_npx;
  Particle::PhaseSpaceIndex const& i_npy = Particle::i_npy;
  Particle::PhaseSpaceIndex const& i_ndp = Particle::i_ndp;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ApertureDecorator::ApertureDecorator( PropagatorPtr p )
  : PropagatorDecorator(p), type_(bmlnElmnt::infinite), hor_(0.0), ver_(0.0)
{}   

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ApertureDecorator::ApertureDecorator( ApertureDecorator const& o)
  : PropagatorDecorator(o)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ApertureDecorator* ApertureDecorator::Clone() const
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

void ApertureDecorator::setAperture( bmlnElmnt::aperture_t type, double const& hor, 
                                                                 double const& ver )
{
  type_ = type;
  hor_  = hor;
  ver_  = ver; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ApertureDecorator::operator()( bmlnElmnt const& elm,         Particle& p)
{
  Vector& state = p.State();
  

  double const x = state[i_x]/hor_;  
  double const y = state[i_y]/ver_;  
  
  bool lost = false;

  switch ( type_ ) {

    case bmlnElmnt::elliptical : lost = ( (x*x) + (y*y) > 1.0 );    
			         break;

    case bmlnElmnt::rectangular: lost = ( (abs(x) > 1.0) || (abs(y) > 1.0) ); 
                                 break; 

    default:                     break;
 
  }

  (*propagator_)(elm,p);   
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ApertureDecorator::operator()( bmlnElmnt const& elm,      JetParticle& p)
{
 
  Mapping& state = p.State();
  
  double const x = state[i_x].standardPart()/hor_;  
  double const y = state[i_y].standardPart()/ver_;  
  
  bool lost = false;

  switch ( type_ ) {

    case bmlnElmnt::elliptical :  lost = ( (x*x) + (y*y) > 1.0 );    
			          break;

    case bmlnElmnt::rectangular:  lost = ( (abs(x) > 1.0) || (abs(y) > 1.0) ); 
                                  break; 

    default:                      break;
 
  }

  (*propagator_)(elm,p);   

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
