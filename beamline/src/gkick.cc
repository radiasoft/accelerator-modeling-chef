/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      gkick.cc
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
******  Author:    Jean-Francois Ostiguy                                     
******             ostiguy@fnal.gov                         
******
****** 
**************************************************************************
*************************************************************************/

/**************************************************************************
This element is meant to emulate the GKICK type as specified in DIMAD.

  This element is a general kick.
         l        is the length.
         dx       is the change in x.
         dxp      is the change in x'.
         dy       is the change in y.
         dyp      is the change in y'.
         dl       is the change in path length.
         dp       is the change in dp/p.
         angle    is  the  angle  through  which  the  coordinates  are
                  rotated about the longitudinal axis.
         dz       is the longitudinal displacement.
         v        is the  extrance-exit parameter  of the  kick.  v  is
                  positive for an  entrance kick,  and negative  for an
                  exit kick.   The absolute value of v is used to force
                  the  kick to  be applied  every  abs(v)  turns.   The
                  default value of v is 1.
         t        is the momentum dependence  parameter.  The kicks dx'
                  and dy' can  be thought of as  misalignment errors or
                  as angle kicks of orbit correctors. In the first case
                  (t=0)  they  are momentum independent.  When  t=1 the
                  kicks  dx'  and  dy' vary  inversely  with  momentum.
                  When t is set to a negative integer value -n the kick
                  is applied every turn and  the momentum of a particle
                  with  initial momentum  p will  oscillate around  the
                  nominal  momentum p0  with  amplitude  (p-p0)  and  a
                  period equal to n turns.  More than one such kick may
                  be put in the line  (all identical though)  the phase
                  of  the cosine  oscillation  is  proportional to  the
                  pathlength of the reference trajectory.


************************************************************************/

#include <iomanip>
#include <beamline/gkick.h>
#include <beamline/BmlVisitor.h>

using namespace std;


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

gkick::gkick() 
  : bmlnElmnt(), 
  dx_   ( 0.0 ),     
  dxp_  ( 0.0 ),   
  dy_   ( 0.0 ),    
  dyp_  ( 0.0 ),     
  dl_   ( 0.0 ),   
  dp_   ( 0.0 ),   
  angle_( 0.0 ),  
  dz_   ( 0.0 ),    
  v_    ( 0.0 ),  
  t_    ( 0.0 )
{
  setStrength(1.0); // the strength is used here only as a momentum dependent 
                    // scaling factor for linacs.  
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

gkick::gkick( gkick const& x )
  : bmlnElmnt( x ),
    dx_   ( x.dx_  ),     
    dxp_  ( x.dxp_ ),   
    dy_   ( x.dy_  ),    
    dyp_  ( x.dyp_ ),     
    dl_   ( x.dl_  ),   
    dp_   ( x.dp_  ),   
    angle_( x.angle_ ),  
    dz_   ( x.dx_  ),    
    v_    ( x.v_   ),  
    t_    ( x.t_   )

{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

gkick&  gkick::operator=( gkick const& rhs) {

  if ( &rhs == this ) return *this;  
  bmlnElmnt::operator=(rhs);

  dx_   = rhs.dx_;     
  dxp_  = rhs.dxp_;   
  dy_   = rhs.dy_;    
  dyp_  = rhs.dyp_;     
  dl_   = rhs.dl_;   
  dp_   = rhs.dp_;   
  angle_= rhs.angle_;  
  dz_   = rhs.dx_;    
  v_    = rhs.v_;  
  t_    = rhs.t_;

  return *this; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

gkick::~gkick() 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void   gkick::setStrength( double const& strength ) 
{

  strength_ = strength;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void gkick::set_dx( double const& dx)
{
  dx_ = dx;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  gkick::set_dxp(double const& dxp )
{
  dxp_ = dxp;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  gkick::set_dy( double const& dy )
{
  dy_ = dy;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  gkick::set_dyp(  double const& dyp)
{
  dyp_ = dyp;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  gkick::set_dl( double const& dl)
{
    dl_ = dl;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  gkick::set_dp( double const& dp)
{
    dp_ = dp;

}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  gkick::set_angle( double const& angle)
{
    angle_ = angle;
  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
void  gkick::set_dz( double const& dz)
{

    dz_ = dz;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  gkick::set_v( double const& v)
{
     v_ = v;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  gkick::set_t( double const& t)
{
     t_ = t;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* gkick::Type() const 
{ 
  return "gkick"; 
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool gkick::isMagnet() const 
{ 
  return true;  // strength_  will be rescaled with momentum.  
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void gkick::accept( BmlVisitor& v )
{ 
  v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void gkick::accept( ConstBmlVisitor& v ) const
{
   v.visit( *this ); 
}

