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
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>

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

AlignmentDecorator* AlignmentDecorator::clone() const
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

Matrix AlignmentDecorator::rotation( double const& r, double const& y,  double const& p ) 
{
  
//-------------------------     -------------------------       ----------------------	
// pitch			  Yaw		                Roll			
//-------------------------	  -------------------------     ----------------------	
// 1.0        0.0     0.0 	  cos(y) 0.0     sin(y)         cos(r) -sin(r)   0.0 	
// 0.0      cos(p) -sin(p) 	  0.0    1.0       0.0          sin(r)  cos(r)   0.0 	
// 0.0      sin(p)  cos(p) 	 -sin(y) 0.0     cos(y)         0.0        0.0   1.0       
//-------------------------------------------------------------------------- ---------


  TMatrix<double> mtrx_pitch(6,6);
  TMatrix<double>  mtrx_roll(6,6);
  TMatrix<double>   mtrx_yaw(6,6);
   

  mtrx_pitch[0][0] =  1.0;      mtrx_pitch[3][3] =  1.0;    
  mtrx_pitch[1][1] =  cos(p);   mtrx_pitch[4][4] =  cos(p);   
  mtrx_pitch[1][2] =  0.0;      mtrx_pitch[4][5]  =  0.0;       //-sin(p); 
  mtrx_pitch[2][1] =  0.0;      mtrx_pitch[5][3] =  0.0;        // sin(p); 
  mtrx_pitch[2][2] =  1.0;      mtrx_pitch[5][5] =  1.0;        // cos(p); 
  

  mtrx_yaw[0][0]   =  cos(y);   mtrx_yaw[3][3]   =  cos(y); 
  mtrx_yaw[0][2]   =  0.0;      mtrx_yaw[3][5]   =  0.0;        // sin(y); 
  mtrx_yaw[1][1]   =  1.0;      mtrx_yaw[4][4]   =  1.0;    
  mtrx_yaw[2][0]   =  0.0;      mtrx_yaw[5][3]   =  0.0;        // -sin(y); 
  mtrx_yaw[2][2]   =  1.0;      mtrx_yaw[5][5]   =  1.0;        // cos(y); 

  mtrx_roll[0][0]  =  cos(r);   mtrx_roll[3][3]  =  cos(r); 
  mtrx_roll[0][1]  = -sin(r); 	mtrx_roll[3][4]  = -sin(r); 
  mtrx_roll[1][0]  =  sin(r); 	mtrx_roll[4][3]  =  sin(r); 
  mtrx_roll[1][1]  =  cos(r); 	mtrx_roll[4][4]  =  cos(r); 
  mtrx_roll[2][2]  =  1.0;    	mtrx_roll[5][5]  =  1.0;    
  
  return mtrx_roll * mtrx_yaw * mtrx_pitch;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void AlignmentDecorator::setAlignment(Vector const& offsets, Vector const& angles , bool relative)
{
  if (relative) {
   offsets_   += offsets; 
   angles_    += angles; 
  } 
  else {
   offsets_   = offsets; 
   angles_    = angles; 
  }

  rotation_  = rotation( angles_[0], angles_[1], angles_[2] );   
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  AlignmentDecorator::operator()(  BmlnElmnt const& elm,  Particle& p)
{
  
  Vector state = p.state();
 
  state[Particle::i_x]   -= offsets_[0];
  state[Particle::i_y]   -= offsets_[1];

  //   state[i_cdt] -= offset_[2];
 
  p.state() = rotation_*state; 

  (*propagator_)( elm, p); 

} 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  AlignmentDecorator::operator()(  BmlnElmnt const& elm,  JetParticle& p) 
{
  Mapping state = p.state();
 
  state[Particle::i_x]   -= offsets_[0];
  state[Particle::i_y]   -= offsets_[1];
//state[i_cdt] -= offset_[2];
 
  p.state() = rotation_*state; 

  (*propagator_)( elm, p); 
} 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
