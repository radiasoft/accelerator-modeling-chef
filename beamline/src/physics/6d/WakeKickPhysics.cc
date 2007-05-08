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
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <basic_toolkit/iosetup.h>
#include <beamline/Particle.h>
#include <beamline/ParticleBunch.h>
#include <beamline/JetParticle.h>
#include <beamline/BunchProjector.h>
#include <beamline/WakeKick.h>
#include <vector>
#include <cmath>

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;



//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void WakeKick::localPropagate( ParticleBunch& bunch )
{

  BunchProjector projector(bunch);
  
  //-------------------------------------------------------------------------- 
  // NOTE: the result of the convolution need to be scaled by binsize. 
  //       This is is done below, when the kicks are actually applied
  //--------------------------------------------------------------------------

  std::vector<double> const& dpx_vec =  hor_twake_( projector.dipoleHorLineDensity() );   
  std::vector<double> const& dpy_vec =  ver_twake_( projector.dipoleVerLineDensity() );   

//std::vector<double> dpz_vec =  lwake_( projector.monopoleLineDensity() );   


  // For each particle in the bunch, apply the appropriate wakefield kicks 
  // ----------------------------------------------------------------------  

  double binsize  = interval_/(nsamples_-1);
  int    ibin = 0;

  // At this point, the bunch has been longitudinally sorted by the BunchProjector. 
  // We get the min value of cdt from the first particle.

  double cdt_min = ( *bunch.begin() )->get_cdt(); 

  for ( ParticleBunch::iterator it = bunch.begin(); it != bunch.end(); ++it )
  {
 
      Vector& state = (*it)->getState();

      ibin = int( (state[5] - cdt_min) / binsize ); 
       
      state[3] += dpx_vec[ibin] * binsize;
      state[4] += dpy_vec[ibin] * binsize; 
    
      // add code for longitudinal wake kick here.

  }   

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


