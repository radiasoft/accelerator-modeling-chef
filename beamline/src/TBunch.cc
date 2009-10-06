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
****** For more information about the algorithm used to populate 
****** the bunches see:
******
******  W. Joho
******  "Representation of Beam Ellipses for Transport Calculations" 
******  Swiss Institute for Nuclear Research
******  SIN-REPORT TM-11-14 (8.5.1980)
******                                                               
******
****** The alternate technique used for populating Gaussian 2D
****** phase space is based on quadtaric forms. Assuming
****** X is a random vector with normally distributed components   
******
******    X  [ R ] X =  X [ R ]^{1/2} [ R ]^{1/2}  X = epsilon  
******
****** The entries of the desired correlation matrix R can be 
****** expressed in terms of the parameter of the beam ellipse. 
****** Then, by forming the vector [ R ]^{1/2} X  
****** ( X is set by a random number generator),
****** a distribution with the desired statistics is obtained.
******
****** Note on emittance
****** -----------------
******
****** The emittance is the  *geometric* emittances. 
****** state[i_npx] and  state[i_npy] are converted into angles
****** when the emittance is computed.
******
****** For the angle coordinates to be accurately converted when a bunch 
****** is populated the ctd - ndp coordinates must be set first.    
******
**************************************************************************
*************************************************************************/

#include <beamline/Particle.h>
#include <beamline/TBunch.h>


using namespace std;

using FNAL::pcout;
using FNAL::pcerr;


namespace {

  typedef PhaseSpaceIndexing::index index;

  index const   i_x     = Particle::i_x; 
  index const   i_y     = Particle::i_y; 
  index const   i_cdt   = Particle::i_cdt; 
  index const   i_npx   = Particle::i_npx; 
  index const   i_npy   = Particle::i_npy; 
  index const   i_ndp   = Particle::i_ndp;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
std::vector<double>  TBunch<Particle>::emittances() const 
{ 
  std::vector<double> dispersion(4, 0.0); // null dispersion 
  return emittances(dispersion);
}  

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
Matrix  TBunch<Particle>::sigmas( std::vector<double> const&  dispersion) const 
{
  Matrix sigmas(6,6);

  vector<double> x(6);
  vector<double> xbar(6);

 for ( TBunch<Particle>::const_iterator it = begin(); it != end(); ++it ) { 

     Vector state = it->state();

     double const npz = it->npz();
   
    // correct for correlation due to dispersive effect 

    state[i_x]   -= dispersion[0] * state[i_ndp]; 
    state[i_npx] -= dispersion[1] * state[i_ndp]* npz; 
 
    state[i_x]   -= dispersion[2] * state[i_ndp];  
    state[i_npy] -= dispersion[3] * state[i_ndp] * npz; 

    
    x[0]= state[i_x  ]; 
    x[1]= state[i_npx]; 
    x[2]= state[i_y  ]; 
    x[3]= state[i_npy]; 
    x[4]= state[i_cdt]; 
    x[5]= state[i_ndp]; 

    for (int i=0; i<6; ++i ) { xbar[i] += x[i]; }
     
 }
  
 for (int i=0; i<6; ++i ) { xbar[i] /= size(); }
 

 for ( TBunch<Particle>::const_iterator it = begin(); it != end(); ++it ) { 

     Vector state = it->state();

     double const npz = it->npz();
   
    // correct for correlation due to dispersive effect 

    state[i_x]   -= dispersion[0] * state[i_ndp]; 
    state[i_npx] = state[i_npx]/npz + dispersion[1] * state[i_ndp]; 
 
    state[i_x]   -= dispersion[2] * state[i_ndp];  
    state[i_npy] = state[i_npy]/npz + dispersion[3] * state[i_ndp]; 

     
    x[0]= state[i_x  ]; 
    x[1]= state[i_npx]; 
    x[2]= state[i_y  ]; 
    x[3]= state[i_npy]; 
    x[4]= state[i_cdt]; 
    x[5]= state[i_ndp]; 

    for (int i=0; i<6; ++i ) {
      for (int j=0; j<=i; ++j ) {
        sigmas[i][j] +=  (x[i]-xbar[i])*(x[j]-xbar[j]);  
      }
    }
 }

 for (int i=0; i<6; ++i ) {
    for (int j=0; j<=i; ++j ) {
      sigmas[j][i] =  sigmas[i][j];
     }
 }

 return sigmas; 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
std::vector<double>  TBunch<Particle>::emittances( std::vector<double> const&  dispersion) const 
{

  //-----------------------------------------------------------------------------------
  // Computes the *geometric* dispersion (optinally) CORRECTED transverse rms emittance and the 
  // longitudinal rms emittance  
  // 
  //  Emittance is defined as using Sacharer's convention.
  //
  //  sqrt( <x^2><x'^2> - <x x'>^2 )
  //
  //  Multiply by 4 for Lapostolle's definition  
  //  
  // 
  //-----------------------------------------------------------------------------------

  std::vector<double>  u_2 (3, 0.0);
  std::vector<double>  up_2(3, 0.0);
  std::vector<double>  u_up(3, 0.0);
  std::vector<double>  ubar(3, 0.0);
  std::vector<double> upbar(3, 0.0);


 int const n   = size();

  for ( TBunch<Particle>::const_iterator it = begin(); it != end(); ++it ) { 

     Vector state = it->state();

     double const npz = it->npz();
   
    // correct for correlation due to dispersive effect 

    state[i_x]   -= dispersion[0] * state[i_ndp]; 
    state[i_npx] -= dispersion[1] * state[i_ndp]* npz; 
 
    state[i_x]   -= dispersion[2] * state[i_ndp];  
    state[i_npy] -= dispersion[3] * state[i_ndp] * npz; 

     
    for (int i=0; i<3; ++i ) {
      ubar[i] += state[i]; 
     upbar[i] += (i+3 != 5) ? state[3+i] / npz : state[3+i];
    } 

     u_2[0] +=  (state[i_x]   * state[i_x]   );
    up_2[0] +=  (state[i_npx] * state[i_npx] ) / (npz*npz);
    u_up[0] +=  (state[i_x]   * state[i_npx] ) / npz;

     u_2[1] +=  (state[i_y]   * state[i_y]   );
    up_2[1] +=  (state[i_npy] * state[i_npy] ) / (npz*npz);
    u_up[1] +=  (state[i_y]   * state[i_npy] ) / npz;

     u_2[2] +=  (state[i_cdt]*state[i_cdt]);
    up_2[2] +=  (state[i_ndp]*state[i_ndp]);
    u_up[2] +=  (state[i_cdt]*state[i_ndp]);

 }

   
   u_2[0] -= (  ubar[0] *  ubar[0]  )/n;
  up_2[0] -= ( upbar[0] * upbar[0]  )/n;  
  u_up[0] -= (  ubar[0] * upbar[0]  )/n;

   u_2[1] -= (  ubar[1] *  ubar[1]  )/n;
  up_2[1] -= ( upbar[1] * upbar[1]  )/n; 
  u_up[1] -= (  ubar[1] * upbar[1]  )/n;

   u_2[2] -= (  ubar[2] *  ubar[2]  )/n;
  up_2[2] -= ( upbar[2] * upbar[2]  )/n;
  u_up[2] -= (  ubar[2] * upbar[2]  )/n;


 std::vector<double> eps(3);

 for (int i=0; i<3; ++i ) {
   eps[i] = sqrt( u_2[i]*up_2[i] - u_up[i]*u_up[i] )/n;
  }

  return eps;

}  


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <>
void  TBunch<Particle>::populateParGaussian( PhaseSpaceProjection psid, double sigma_x, double sigma_px, double r_12,  boost::function<double(void)>& rnd, double cutoff )
{

  double xlim          = sigma_x;
  double xplim         = sigma_px;

  double sn = r_12;
  double cs = sqrt(1.0 - r_12*r_12); 

  for ( iterator it  = bunch_.begin(); it != bunch_.end(); ++it ) {

    Vector& state = it->state(); 

    double a     = sqrt( -2.0 * log( rnd()) );

    if ( cutoff > 0.0 ) {
      while (  abs(a)  > cutoff ) { a = sqrt( -2.0 * log( rnd()) ); }
    }
    double alpha = 2.0*M_PI* rnd();

    double u     = a*cos(alpha);
    double v     = a*sin(alpha);
    
    state[0+psid]   = xlim  *  u;
    state[3+psid]   = xplim * ( psid == cdt_ndp ? 1.0 : (1.0 + state[i_ndp]) ) * (u*sn + v*cs); 

  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <>
void TBunch<Particle>::populateParGaussianAlt( PhaseSpaceProjection psid, double sigmax, double sigmaxp, double r12,  boost::function<double(void)>& nrnd, double cutoff )
{
  //-----------------------------------------------------------------------------------
  // Populate 2D phase space using correlation matrix method rather than 
  // Joho binomial scheme
  // 
  // nrnd() must return a normally distributed random number x, with 
  //        sigma_x = 1 <x> = 0.0
  // cutoff is expressed in units of sigma. if cutoff <= 0.0, it is ignored.   
  //-----------------------------------------------------------------------------------

  double sigma1 = sigmax;
  double sigma2 = sigmaxp;

  double r21    = r12;

  for ( iterator it  = bunch_.begin(); it != bunch_.end(); ++it ) {

    Vector& state = it->state(); 

    double u0 = nrnd();
    double v0 = nrnd();

    if ( cutoff > 0.0 ) {
      while (  abs(u0) > cutoff ) { u0 = nrnd(); }
      while ( (abs(v0) * (psid == cdt_ndp ? 1.0 : (1.0 + state[i_ndp]) )) > cutoff ) { v0 = nrnd(); }
    }

    double x  = sigma1 * u0;
    double xp = sigma2 * v0 + r21 * x;

 
    state[0+psid]   = x;
    state[3+psid]   = xp * (1.0 + state[i_ndp]) ; 
 
  }

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <>
void TBunch<Particle>::populateCSGaussian ( PhaseSpaceProjection psid, double beta, double alpha, double epsilon,  boost::function<double(void)>& rnd, double cutoff )
{

  double x_m         = sqrt(epsilon*beta);
  double gamma       = (1.0+alpha*alpha)/beta; 
  double theta_m     = sqrt(epsilon*gamma);
  double sin_chi     = -alpha/sqrt(beta*gamma); // r_12
  
  populateParGaussian(psid, x_m, theta_m, sin_chi, rnd, cutoff); 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <>
void TBunch<Particle>::populateCSGaussianAlt( PhaseSpaceProjection psid, double beta, double alpha, double epsilon,  boost::function<double(void)>& nrnd, double cutoff )
{
  //-----------------------------------------------------------------------------------
  // Populate 2D phase space using correlation matrix method rather than 2D Joho scheme
  // 
  // nrnd() must return a normally distributed random number x, with 
  //        sigma_x = 1 <x> = 0.0
  // cutoff is expressed in units of sigma. if cutoff <= 0.0, it is ignored.   
  //-----------------------------------------------------------------------------------

  double sigma1 = sqrt (epsilon* beta);
  double sigma2 = sqrt (epsilon/ beta);

  double r21    = -alpha/beta;

  populateParGaussianAlt( psid, sigma1, sigma2, r21, nrnd, cutoff);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <>
void  TBunch<Particle>::populateParWaterBag ( PhaseSpaceProjection psid, double x_rms, double px_rms,  double r_12,   boost::function<double(void)>& rnd)
{

  // Water Bag = uniform in phase space 

 double M = 1.0;

 populateParBinomial ( psid,  M,  x_rms,  px_rms, r_12, rnd ); 

}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <>
void  TBunch<Particle>::populateCSWaterBag( PhaseSpaceProjection psid, double beta, double alpha,  double epsilon,  boost::function<double(void)>& rnd)
{
  double x_m         = sqrt(epsilon*beta);
  double gamma       = (1.0+alpha*alpha)/beta; 
  double theta_m     = sqrt(epsilon*gamma);
  double sin_chi     = -alpha/ sqrt(beta*gamma); // r_12
  
  populateParWaterBag(psid, x_m, theta_m, sin_chi, rnd); 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <>
void  TBunch<Particle>::populateParElliptic( PhaseSpaceProjection psid, double x_rms, double px_rms,  double r_12, boost::function<double(void)>& rnd )
{
  double M = 2.0; 

  populateParBinomial ( psid,  M,  x_rms,  px_rms, r_12, rnd); 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <>
void  TBunch<Particle>::populateCSElliptic( PhaseSpaceProjection psid, double beta, double alpha,  double epsilon,  boost::function<double(void)>& rnd )
{
  double x_m         = sqrt(epsilon*beta);
  double gamma       = (1.0+alpha*alpha)/beta; 
  double theta_m     = sqrt(epsilon*gamma);
  double sin_chi     = -alpha/ sqrt(beta*gamma); // r_12
  
  populateParElliptic(psid, x_m, theta_m, sin_chi, rnd ); 

}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <>
void  TBunch<Particle>::populateParKV       ( PhaseSpaceProjection psid, double x_rms, double px_rms,  double r_12, boost::function<double(void)>& rnd )
{
  // KV = uniform projection  (KV in 2N phase space ==> uniform projection in N (momentum or coordinates) subspace    
  
  double M = 1.0e-9; // Stricly speaking, KV is the limit  M-->0, but this is pretty good. 
  populateParBinomial ( psid,  M,  x_rms,  px_rms, r_12 , rnd ); 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <>
void  TBunch<Particle>::populateCSKV( PhaseSpaceProjection psid, double beta, double alpha, double epsilon, boost::function<double(void)>& rnd )
{
  double x_m         = sqrt(epsilon*beta);
  double gamma       = (1.0+alpha*alpha)/beta; 
  double theta_m     = sqrt(epsilon*gamma);
  double sin_chi     = -alpha/ sqrt(beta*gamma); // r_12
  
  populateParKV(psid, x_m, theta_m, sin_chi, rnd); 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <>
void TBunch<Particle>::populateParBinomial (PhaseSpaceProjection psid, double M, double sigmax, double sigmaxp, double r_12,  boost::function<double(void)>& rnd)
{

  std::vector<std::pair<double,double> > phase_space;

  double  xlim = sqrt( 2.0*(M+1.0) )*sigmax;
  double xplim = sqrt( 2.0*(M+1.0) )*sigmaxp;
 
  double sn = r_12;
  double cs = sqrt(1.0 - r_12*r_12); 

  for ( iterator it  = bunch_.begin(); it != bunch_.end(); ++it ) {

    double a     = sqrt(1.0 - pow( rnd(), 1.0/M ));

    double alpha = 2.0*M_PI*rnd();

    double u     = a*cos(alpha);
    double v     = a*sin(alpha);

    Vector& state = it->state(); 
 
    state[0+psid]   = xlim*u;
    state[3+psid]   = xplim*(u*sn + v*cs ); 

 }

 return;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <>
void TBunch<Particle>::populateCSBinomial ( PhaseSpaceProjection psid, double M, double beta, double alpha, 
						       double epsilon,  boost::function<double(void)>& rnd )
{
  double x_m         = sqrt(epsilon*beta);
  double gamma       = (1.0+alpha*alpha)/beta; 
  double theta_m     = sqrt(epsilon*gamma);
  double sin_chi     = -alpha/ sqrt(beta*gamma); // r_12
  
  populateParBinomial( psid, M, x_m, theta_m, sin_chi, rnd ); 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
