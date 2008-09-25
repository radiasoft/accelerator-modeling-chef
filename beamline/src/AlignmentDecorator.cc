/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      AlignmentDecorator.cc
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

#include <beamline/AlignmentDecorator.h>

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

AlignmentDecorator::AlignmentDecorator( PropagatorPtr p )
  : PropagatorDecorator(p)
{}   

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

AlignmentDecorator::AlignmentDecorator( AlignmentDecorator const& o)
  : PropagatorDecorator(o)

{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

AlignmentDecorator* AlignmentDecorator::Clone() const
{ 
  return new AlignmentDecorator(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

AlignmentDecorator::~AlignmentDecorator()
{}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool AlignmentDecorator::hasAlignment() const
{
  return true;
}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void AlignmentDecorator::setAlignment(Vector const& translation, Vector const& rotation)
{
  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  AlignmentDecorator::operator()(  bmlnElmnt const& elm,  Particle& p)
{
  
#if  0
  double cs = align_->cos_roll();
  double sn = align_->sin_roll();

  Vector& state = p.State();

  state[0] -= align_->x_offset();
  state[1] -= align_->y_offset();

  if( align_->roll() != 0.0) {

    double temp  = state[0] * cs + state[1] * sn;
    state[1]     = state[1] * cs - state[0] * sn;
    state[0]     = temp;

    temp       = state[3] * cs + state[4] * sn;
    state[4]   = state[4] * cs - state[3] * sn;
    state[3]   = temp;
  }
 
  }
#endif

  (*propagator_)( elm, p); 

#if 0
  double cs = align_->cos_roll();
  double sn = align_->sin_roll();

  Vector& state = p.State();

  if( align_->roll() != 0.0) {
    double temp   = state[0] * cs - state[1] * sn;
    state[1]      = state[1] * cs + state[0] * sn;
    state[0]      = temp;

    temp        = state[3] * cs - state[4] * sn;
    state[4]    = state[4] * cs + state[3] * sn;
    state[3]    = temp;
  }

  state[0] += align_->x_offset();
  state[1] += align_->y_offset();

#endif

} 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  AlignmentDecorator::operator()(  bmlnElmnt const& elm,  JetParticle& p) 
{
  (*propagator_)( elm, p); 
} 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
