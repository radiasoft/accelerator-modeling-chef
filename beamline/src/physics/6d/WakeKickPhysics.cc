/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      WakeKickPhysics.cc
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
//       For the convolution finctor to return a sensible result (no aliasing), the sum of the widths 
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
  
    interval_ *= 1.25 * interval_;

    (*pcout ) << " *** WARNING ***: WakeKickPhysics: aliasing   detected in wake computation. \n"  
              << " *** WARNING ***: WakeKickPhysics: Increasing the size of the sampling interval.\n"
              << " *** WARNING ***: bunch length    = " << bunch_length  << "\n"
              << " *** WARNING ***: old interval length = " << interval_ << "\n"
              << " *** WARNING ***: new interval length = " << interval_ << "\n"
              << std::endl;

    interval_has_changed = true;
  }      

  if ( interval_has_changed ) {

    boost::function<double(int)> lwake = boost::bind<double>(ShortRangeLWakeFunction(),  _1,  interval_/(nsamples_-1),  0.5*interval_ );
    boost::function<double(int)> twake = boost::bind<double>(ShortRangeLWakeFunction(),  _1,  interval_/(nsamples_-1),  0.5*interval_ );

    std::vector<double> lwake_lhs(nsamples_);  
    std::vector<double> twake_lhs(nsamples_);  
    
    for (int i=0; i<nsamples_; ++i) { lwake_lhs[i] = lwake(i); } 
    for (int i=0; i<nsamples_; ++i) { twake_lhs[i] = twake(i); } 
   
    lwake_.resetLHS( lwake_lhs );
    twake_.resetLHS( twake_lhs ); 
  }

  //-------------------------------------------------------------------------- 
  // NOTE: the result of the convolution needs to be scaled by binsize. 
  //       This is is done below, when the kicks are actually applied
  //--------------------------------------------------------------------------

  std::vector<double> const dpx_vec =  twake_( projector.dipoleHorLineDensity() );   
  std::vector<double> const dpy_vec =  twake_( projector.dipoleVerLineDensity() );   
  std::vector<double> const dpz_vec =  lwake_( projector.monopoleLineDensity()  );   

  //------------------------------------------------------------------
  // For each particle in the bunch, apply appropriate wakefield kicks 
  // -----------------------------------------------------------------  

  const double binsize  = interval_/(nsamples_-1);

  // ------------------------------------------------------------------------------
  // At this point, the bunch has been longitudinally sorted by the BunchProjector. 
  // We get the min value of cdt from the first particle.
  //
  // NOTE: particle reference momentum in expressed internally in  GeV, 
  //       The wake is assumed to be expressed in V/pC/m   longitudinal 
  //                                              V/pC/m/m transverse 
  // --------------------------------------------------------------------------------------------

  double  cdt_min =  projector.cdt_min(); 
  double  p0      =  bunch.begin()->ReferenceMomentum(); // in 1.0e9 eV 

  double npart = ( bunch.Intensity() * PH_MKS_e );       // in 1.0e9 pC 

  double const coeff = npart * binsize / p0; // normalization 
                                             // THIS SHOULD BE MULTIPLIED BY CAVITY EFFECTIVE LENTGH (for ILC, l = 1.036m)   

  for ( ParticleBunch::iterator it = bunch.begin(); it != bunch.end(); ++it )
  {
      Vector& state         =  it->State();

      int ibin = int( (state[2] - cdt_min) / binsize ); 
       
      if ( ibin > (nsamples_-1) ) { 

	(*pcerr) << "*** WARNING *** "                                                                << std::endl; 
	(*pcerr) << "*** WARNING ***   WakeKickPhysics.cc : Histogram bin index out of range  "       << std::endl; 
	(*pcerr) << "*** WARNING ***   WakeKickPhysics.cc : Total bunch length : "    << bunch_length << std::endl;
	(*pcerr) << "*** WARNING ***   WakeKickPhysics.cc : Sampling region width : " << interval_    << std::endl;

         break;  //  just a sanity check ... this should never happen !
      }

      state[3]  +=  coeff* dpx_vec[ibin];
      state[4]  +=  coeff* dpy_vec[ibin]; 

      //------------------------------------------------------
      // **** the longitudinal wake is disabled for the moment
      //------------------------------------------------------

      // double npz  =  it->get_npz() + npart*(dpz_vec[ibin] * binsize)/p0;   
         double npz  =  it->get_npz();

      state[5] =  sqrt( npz*npz + state[3]*state[3] + state[4]*state[4] );
  }   

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

