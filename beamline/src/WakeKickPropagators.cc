/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      WakeKickPropagators.cc
******
******  Copyright (c) Fermi Research Alliance LLC
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-07CH11359.
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******
******                                                                
******  Author:    Jean-Francois Ostiguy                                     
******             ostiguy@fnal.gov      
******                                                                
**************************************************************************
**************************************************************************
*************************************************************************/
#include <boost/bind.hpp>
#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/PhysicsConstants.h>
#include <beamline/Particle.h>
#include <beamline/ParticleBunch.h>
#include <beamline/JetParticle.h>
#include <beamline/BunchProjector.h>
#include <beamline/WakeKickPropagator.h>
#include <beamline/WakeFunctions.h>
#include <algorithm>
#include <vector>
#include <cmath>

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//----------------------------------------------------------------------------------------------------
// NOTE: In the code below, the ConvolutionFunctor uses the wakefunction sampled at nsample locations.
//       The wakefunction is *truncated* at 0.5*interval, i.e. for z > 0.5*interval the samples are 0. 
//----------------------------------------------------------------------------------------------------
 
WakeKickPropagator::WakeKickPropagator( int nsamples, double const& interval)
  :  nsamples_(nsamples), 
     interval_(interval),
     lwake_( nsamples_,  boost::bind<double>( ShortRangeLWakeFunction(),  _1,  interval_/(nsamples_-1),  0.5*interval_ ), true), 
     twake_( nsamples_,  boost::bind<double>( ShortRangeTWakeFunction(),  _1,  interval_/(nsamples_-1),  0.5*interval_ ), true)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


WakeKickPropagator::WakeKickPropagator( WakeKickPropagator const& other ) 
 : nsamples_(other.nsamples_), 
   interval_(other.interval_),
   lwake_(other.lwake_), 
   twake_(other.twake_) 
{}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//----------------------------------------------------------------------------------------------
// NOTE: both the bunch and the wakefunction are sampled over an interval of length "interval_".
//       For the convolution functor to return a sensible result (no aliasing), the sum of the widths 
//       of the non-zero support of the two functions must be < interval_. Here, we just assume that 
//       length was set a-priori in such a way that this condition is met.  
//----------------------------------------------------------------------------------------------

void WakeKickPropagator::operator()(  ParticleBunch& bunch )
{

  BunchProjector projector(bunch, interval_, nsamples_);

  const double bunch_length = ( (--bunch.end() )->get_cdt() - bunch.begin()->get_cdt() );

  bool interval_has_changed = false;

  //-------------------------------------------------------------------------------- 
  // Detect aliasing. If necessary, increase the sampling interval size
  // Then, reset the LHS in the convolution function object. 
  //--------------------------------------------------------------------------------

  while ( bunch_length > 0.5*interval_ ) {
  
    double old_interval = interval_;
    interval_ *= 1.25;

    (*pcout ) << " *** WARNING ***: WakeKickPhysics: aliasing   detected in wake computation. \n"  
              << " *** WARNING ***: WakeKickPhysics: Increasing the size of the sampling interval.\n"
              << " *** WARNING ***: bunch length    = "     << bunch_length     << "\n"
              << " *** WARNING ***: old interval length = " << old_interval << "\n"
              << " *** WARNING ***: new interval length = " << interval_    << "\n"
              << std::endl;

    interval_has_changed = true;
  }      

  if ( interval_has_changed ) {

    boost::function<double(int)> lwake = boost::bind<double>(ShortRangeLWakeFunction(),  _1,  interval_/(nsamples_-1),  0.5*interval_ );
    boost::function<double(int)> twake = boost::bind<double>(ShortRangeTWakeFunction(),  _1,  interval_/(nsamples_-1),  0.5*interval_ );

    std::vector<double> lwake_lhs(nsamples_);  
    std::vector<double> twake_lhs(nsamples_);  
    
    for (int i=0; i<nsamples_; ++i) { lwake_lhs[i] = lwake(i); } 
    for (int i=0; i<nsamples_; ++i) { twake_lhs[i] = twake(i); } 
   
    lwake_.resetLHS( lwake_lhs );
    twake_.resetLHS( twake_lhs ); 
  }

  //-------------------------------------------------------------------------- 
  // NOTE: the result of the convolution will need to be scaled by binsize. 
  //       This is is done below, when the kicks are actually applied
  //--------------------------------------------------------------------------

  std::vector<double> dpz_vec =  lwake_( projector.monopoleLineDensity()  );   
  std::vector<double> dpx_vec =  twake_( projector.dipoleHorLineDensity() );   
  std::vector<double> dpy_vec =  twake_( projector.dipoleVerLineDensity() );   

  //------------------------------------------------------------------
  // For each particle in the bunch, apply appropriate wakefield kicks 
  // -----------------------------------------------------------------  

  // -------------------------------------------------------------------------------------------------------------------------------
  // 
  // At this point, the bunch has been longitudinally sorted by the BunchProjector. 
  // We get the min value of cdt from the first particle.
  //
  // NOTES: 
  // ====== 
  //      - particle reference momentum in expressed internally in  GeV, 
  //      - The wake is assumed to be expressed in V/pC    =  longitudinal  (already integrated over length of the cavity)
  //                                               V/pC/m  =  transverse    (already integrated over length of the cavity)
  //      - the mono/dipole distributions are normalized w/r the total no of pseudo-particles.
  //        so  integral monopoleLineDensity() = 1.0 
  //            integral dipoleLineDensity()   = integral [ w(z) x(z) dz  ] where w(z) is the particle
  //                                             density at position z and x(z) is the transverse offset 
  //                                               
  // -----------------------------------------------------------------------------------------------------------------------------------

  double  const binsize  = interval_/(nsamples_-1);
  double  const cdt_min =  projector.cdt_min(); 
  double  const p0      =  bunch.begin()->ReferenceMomentum();      // in  [GeV/c] 
  double  const charge  =  bunch.begin()->Charge();                 // particle charge in C

  double const bunch_charge = ( bunch.Intensity() * charge * 1.0e12 );     // in pC. wake is assumed to be in V/pC/m [ integrated     
  double const coeff        = ( 1.0e-9 * bunch_charge /p0 )* binsize;      // converts kick to [GeV/c] / p0 
                                                                           // binsize is needed (see above comment)                      

  std::transform( dpx_vec.begin(), dpx_vec.end(), dpx_vec.begin(), std::bind2nd( multiplies<double>(), coeff) );
  std::transform( dpy_vec.begin(), dpy_vec.end(), dpy_vec.begin(), std::bind2nd( multiplies<double>(), coeff) );
  std::transform( dpz_vec.begin(), dpz_vec.end(), dpz_vec.begin(), std::bind2nd( multiplies<double>(), coeff) );

  for ( ParticleBunch::iterator it = bunch.begin(); it != bunch.end(); ++it )
   {
      Vector& state         =  it->State();

      int ibin = int( (state[2] - cdt_min) / binsize ); 
       
      if ( ibin > (nsamples_-1) ) { 

	(*pcerr) << "*** WARNING *** "                                                                << std::endl; 
	(*pcerr) << "*** WARNING ***   WakeKickPhysics.cc : Histogram bin index out of range  "       << std::endl; 
	(*pcerr) << "*** WARNING ***   WakeKickPhysics.cc : Total bunch length : "    << bunch_length << std::endl;
	(*pcerr) << "*** WARNING ***   WakeKickPhysics.cc : Sampling region width : " << interval_     << std::endl;

         break;  //  just a sanity check ... this should never happen !
      }

      state[3]  +=  dpx_vec[ibin] ;
      state[4]  +=  dpy_vec[ibin] ; 

      //------------------------------------------------------
      // **** apply the longitudinal wake 
      //------------------------------------------------------

      double npz =  it->get_npz() + dpz_vec[ibin];   
      state[5]   =  sqrt( npz*npz + state[3]*state[3] + state[4]*state[4] ) - 1.0 ;
  }   

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void WakeKickPropagator::operator()(  JetParticleBunch& bunch )
{

  // empty for the moment

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void WakeKickPropagator::debug(  ParticleBunch& bunch )
{
  const BunchProjector projector(bunch);
  
  double binsize  = interval_/(nsamples_-1);
  int    ibin = 0;

  //-------------------------------------------------------------------------- 
  // NOTE: the result of the convolution need to be scaled by binsize. 
  //       This is is done below, when the kicks are actually applied
  //--------------------------------------------------------------------------

  std::vector<double> const dpx_vec =  twake_( projector.dipoleHorLineDensity() );   
  std::vector<double> const dpy_vec =  twake_( projector.dipoleVerLineDensity() );   
  std::vector<double> const dpz_vec =  lwake_( projector.monopoleLineDensity()  );   

  std::vector<double>::const_iterator it_dipole_x =  projector.dipoleHorLineDensity().begin(); 
  std::vector<double>::const_iterator it_dipole_y =  projector.dipoleVerLineDensity().begin();
  std::vector<double>::const_iterator it_monopole =  projector.monopoleLineDensity().begin();

  std::vector<double>::const_iterator it_dpx = dpx_vec.begin();  
  std::vector<double>::const_iterator it_dpy = dpy_vec.begin();
  std::vector<double>::const_iterator it_dpz = dpz_vec.begin();

  for (int i=0; i < dpx_vec.size(); ++i ) { 
     std::cout <<  std::scientific 
               <<  setw(12)  << *it_monopole   << "   " <<  setw(12) <<  *it_dpz  
               <<  setw(12)  << *it_dipole_x   << "   " <<  setw(12) <<  *it_dpx 
               <<  setw(12)  << *it_dipole_y   << "   " <<  setw(12) <<  *it_dpy   
               <<  std::endl;

        ++it_monopole; ++it_dpz; ++it_dipole_x; ++it_dpx; ++it_dipole_y; ++it_dpy;
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

