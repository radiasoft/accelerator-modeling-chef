/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Beamline:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      TBunch.cc
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
******  Author:    Jean-Francois Ostiguy                                     
******             ostiguy@fnal.gov
******
******                                                                
**************************************************************************
*************************************************************************/

#include <beamline/Particle.h>
#include <beamline/TBunch.h>

#include<boost/random.hpp>

using namespace std;

using FNAL::pcout;
using FNAL::pcerr;


namespace {

typedef  boost::rand48    base_generator_type;

base_generator_type generator1(static_cast<boost::int32_t>(117u) );
base_generator_type generator2(static_cast<boost::int32_t>(137u) );

boost::variate_generator<base_generator_type&, boost::uniform_real<> > srnd(generator1,   boost::uniform_real<>(0.0, 1.0 )   );
boost::variate_generator<base_generator_type&, boost::uniform_real<> > trnd(generator2,   boost::uniform_real<>(0.0, 2*M_PI) );

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
std::vector<double>  TBunch<Particle>::emittances() const 
{
  //-----------------------------------------------------------------------------------
  // compute the *geometric* transverse rms emittance or the longitudinal rms emittance  
  //
  // 
  //  emittance is defined as using Sacharer's convention.
  //
  //  sqrt( <x^2><x'^2> - <x x'>^2 )
  //
  //  Multiply by 4 for Lapostolle's definition  
  //  
  //-----------------------------------------------------------------------------------

  

  std::vector<double> u_2 (3, 0.0);
  std::vector<double> up_2(3, 0.0);
  std::vector<double> u_up(3, 0.0);


  for (TBunch<Particle >::const_iterator it = begin(); it != end(); ++it ) { 

    Vector const& state = it->State();

    double const npz = it->get_npz();
     
     u_2[0] +=  (state[0] * state[0]);  
    up_2[0] +=  (state[3] * state[3])/( npz*npz);
    u_up[0] +=  (state[0] * state[3])/ npz;  

     u_2[1] +=  (state[1] * state[1]);  
    up_2[1] +=  (state[4] * state[4])/( npz*npz);
    u_up[1] +=  (state[1] * state[4])/ npz;  

     u_2[2] +=  (state[2] * state[2]);  
    up_2[2] +=  (state[5] * state[5]);
    u_up[2] +=  (state[2] * state[5]);

  }

  int const n = size();

  for ( int i=0; i<3; ++i ) {
     u_2[i] /= n; 
    up_2[i] /= n; 
    u_up[i] /= n;
  }

  std::vector<double> eps(3);

  for (int i=0; i<3; ++i ) {
    eps[i] = sqrt( u_2[i]*up_2[i] - u_up[i]*u_up[i] ); 
  }

  return eps;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <>
void  TBunch<Particle>::populateGaussian( PhaseSpaceProjection psid, double sigma_x, double sigma_px, double r_12 )
{

  int M = 10; //  Strictly speaking, the limit M --> infty is the Gaussian, but 30 is more than good enough
              //  Cutoff will be at sqrt(10.5) = 3.2 sigma
    
  double x_lim  = sqrt(M + 0.5) *  sigma_x;
  double px_lim = sqrt(M + 0.5) *  sigma_px;
 
  populateBinomial ( psid,  M,  x_lim,  px_lim, r_12 ); 


}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <>
void  TBunch<Particle>::populateWaterBag ( PhaseSpaceProjection psid, double x_rms, double px_rms,  double r_12 )
{

  // Water Bag = uniform in phase space 

 double M = 1.0;

 double x_lim  = sqrt(M + 0.5) *  x_rms;
 double px_lim = sqrt(M + 0.5) *  px_rms;

 populateBinomial ( psid,  M,  x_lim,  px_lim, r_12 ); 

}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <>
void  TBunch<Particle>::populateParabolic( PhaseSpaceProjection psid, double x_rms, double px_rms,  double r_12 )
{

  double M = 2.0; 

  double x_lim  = sqrt(M + 0.5) *  x_rms;
  double px_lim = sqrt(M + 0.5) *  px_rms;

  populateBinomial ( psid,  M,  x_lim,  px_lim, r_12 ); 



}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <>
void  TBunch<Particle>::populateKV       ( PhaseSpaceProjection psid, double x_rms, double px_rms,  double r_12 )
{
  // KV = uniform projection  (KV in 2N phase space ==> uniform projection in N (momentum or coordinates) subspace    
  
  double M = 1.0e-9; // Stricly speaking, KV is the limit  M-->0, but this is pretty good. 
  populateBinomial ( psid,  M,  x_rms,  px_rms, r_12 ); 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <>
void TBunch<Particle>::populateBinomial ( PhaseSpaceProjection psid, double M, double x_lim, double px_lim, double r_12)
{

 double chi    = asin(r_12);

 double a     = 0.0;
 double alpha = 0.0;
 double u     = 0.0;
 double v     = 0.0;
 double x     = 0.0;
 double px    = 0.0;
  
 std::vector<std::pair<double,double> > phase_space;

 

 for ( iterator it  = bunch_.begin(); it != bunch_.end(); ++it ) {

    a     = sqrt(1.0 - pow( srnd(), 1.0/M ));

    alpha = trnd();

    u     = a*cos(alpha);
    v     = a*sin(alpha);

    Vector& state = it->State(); 
 
    state[0+psid]    += x_lim*u;
    state[3+psid]    += px_lim*(u*sin(chi) + v*cos(chi)); 

 }

 return;
}
