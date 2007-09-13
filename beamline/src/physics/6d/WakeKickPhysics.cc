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
//       For the convolution to return a sensible result (no aliasing), the sum of width of the
//       non-zero support of the two functions must be < length. Here, we just assume that 
//       length was set a-priori in such a way that this condition is met.  
//----------------------------------------------------------------------------------------------

void WakeKickPropagator::operator()(  ParticleBunch& bunch )
{

  BunchProjector projector(bunch, interval_, nsamples_);

  const double bunch_length = ( (--bunch.end() )->get_cdt() - bunch.begin()->get_cdt() );
  if (bunch_length > 0.5*interval_ ) {
    (*pcout ) << " *** WARNING ***: WakeKickPhysics: aliasing detected in wake computation. \n"  
              << " *** WARNING ***: WakeKickPhysics: Increase the size of the sampling interval.\n"
              << " *** WARNING ***: bunch length    = " << bunch_length << "\n"
              << " *** WARNING ***: interval length = " << interval_ 
              << std::endl;
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
  // ------------------------------------------------------------------------------

  double  cdt_min =  projector.cdt_min(); 
  double  p0      =  bunch.begin()->ReferenceMomentum(); 

  double npart = bunch.Population(); 

  for ( ParticleBunch::iterator it = bunch.begin(); it != bunch.end(); ++it )
  {
      Vector& state         =  it->State();

      int ibin = int( (state[5] - cdt_min) / binsize ); 
       
      if ( ibin > (nsamples_-1) ) break;  //  this happens if histogramming has truncated the bunch profile  

      state[3]  +=  npart *( dpx_vec[ibin] * binsize ) /p0;
      state[4]  +=  npart *( dpy_vec[ibin] * binsize )/ p0; 

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

