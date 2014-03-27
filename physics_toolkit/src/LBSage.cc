/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      LBSage.cc
******  Version:   1.0
******                                                                
****** Copyright (c) Universities Research Association, Inc.
******                All Rights Reserved
******
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
******                                                                
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
******  REVISION HISTORY
******
******  Dec 2006 - Jean-Francois Ostiguy 
******             ostiguy@fnal
******    
******  - interface based on Particle& rather than ptrs. 
******    Stack allocated local Particle objects.
******  - changes to accomodate new boost::any based Barnacle objects.
******  - use new style STL-compatible beamline iterators
******  - calcs_ array is now an STL vector. LF are now returned by 
******    returning a const reference to the entire vector.
******  - misc cleanup.  
******
****** Mar 2007     ostiguy@fnal.gov
****** - added support for reference counted elements/beamlines
******
******
*************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iomanip>

#include <basic_toolkit/iosetup.h>
#include <physics_toolkit/LBSage.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <basic_toolkit/GenericException.h>

#include <physics_toolkit/BmlUtil.h>

using FNAL::pcout;
using FNAL::pcerr;


using namespace std;

namespace {
 Particle::PhaseSpaceIndex const& i_x     = Particle::xIndex;
 Particle::PhaseSpaceIndex const& i_npx   = Particle::npxIndex;
 Particle::PhaseSpaceIndex const& i_y     = Particle::yIndex;
 Particle::PhaseSpaceIndex const& i_npy   = Particle::npyIndex;
 Particle::PhaseSpaceIndex const& i_cdt   = Particle::cdtIndex;
 Particle::PhaseSpaceIndex const& i_ndp   = Particle::ndpIndex;
}


LBSage::Info::Info() 
  : arcLength(-1.0),
    beta_1x(0.0),
    beta_1y(0.0),
    beta_2x(0.0),
    beta_2y(0.0),
    alpha_1x(0.0),
    alpha_1y(0.0),
    alpha_2x(0.0),
    alpha_2y(0.0),
    u1(0.0),
    u2(0.0),
    u3(0.0),
    u4(0.0),
    nu_1(0.0),
    nu_2(0.0)
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


LBSage::LBSage( BmlPtr x) 
: Sage( x )
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LBSage::LBSage( beamline const& x) 
: Sage( x )
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void LBSage::eraseAll() 
{
  myBeamlinePtr_->dataHook.eraseAll( "LBSage" );

  for (beamline::deep_iterator it  =  myBeamlinePtr_->deep_begin(); 
                              it != myBeamlinePtr_->deep_end();  ++it) {
    (*it)->dataHook.eraseAll( "LBSage" );
  }

  calcs_.clear();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int LBSage::doCalc( JetParticle const& jp)
{
  // PRECONDITIONS:  The JetParticle must be on the closed
  //   orbit with the one-turn  mapping for its state.

  Vector normalizedPhase(3);   // The dimension is 3 in anticipation
                               // of extension to 6D phase space.

  JetParticle jpart( jp);
  Particle    part(  jp);

  const int n   = Particle::PSD;

  //*********************************************************
  // NOTE:
  // ----- 
  //
  // For some reason, MatrixC::eigenvector() will be called if
  // Matrix::eigenvector() is not explicitly specified. It is
  // not clear wether this behavior is compliant or ...
  // a compiler bug. -jfo
  //
  // 
  //*********************************************************
 
  MatrixC E = ( jpart.State() ).Jacobian().Matrix::eigenVectors();
  BmlUtil::normalize( E, normalizedPhase );

  jpart.State() = Mapping( "identity", jpart.State().Env() );


  double lng = 0.0;
  Vector intPhase(3);        // Dimension 3 in anticipation of future upgrade
  Vector fracPhase(3);       // to six-dimensional phase space.
                             // Automatically initialized to zero by constructor.
  calcs_.clear();

  for (beamline::deep_iterator it  =  myBeamlinePtr_->deep_begin(); 
                               it !=  myBeamlinePtr_->deep_end();  ++it) {
    ElmPtr be = (*it);

    calcs_.push_back( LBSage::Info() );

    lng += be->OrbitLength( part );
    calcs_.back().arcLength = lng;
    
    be->propagate( jpart );
    MatrixC  E2 = jpart.State().Jacobian() * E;
    BmlUtil::normalize( E2, normalizedPhase );
    
    // normalized phase advance

    for( int i = 0; i < 2; i++ ) {
      if( normalizedPhase[i] < ( fracPhase[i] + 0.00001 ) ) {
        // The additional 0.00001 is there to offset possible
        // roundoff error in the computation of normalizedPhase.
        intPhase[i] += 1.0;
      }
      fracPhase[i] = normalizedPhase[i];
    }

    calcs_.back().nu_1 = intPhase[0] + fracPhase[0];   // (near) horizontal
    calcs_.back().nu_2 = intPhase[1] + fracPhase[1];   // (near) vertical

    // beta_1x and beta_2y

    std::complex<double> temp = E2(i_x,i_x);
    if( real(temp) > 0.0 ) {
      calcs_.back().beta_1x = 2.0*real(temp)*real(temp);
    }
    else { 
       calcs_.clear(); 
       throw GenericException( __FILE__, __LINE__, 
              "LBSage::doCalc(const JetParticle*, beamline::Criterion&)",
			       "beta_1x is negative !. This error is likely due to an unstable lattice." );
       return 1; 
    }
    temp = real(E2(i_y,i_y));
    if( real(temp) > 0.0 ) {
      calcs_.back().beta_2y = 2.0*real(temp)*real(temp);
    }
    else { 
       calcs_.clear(); 
       throw GenericException( __FILE__, __LINE__, 
             "LBSage::doCalc(const JetParticle*, beamline::Criterion&)",
			       "beta_2y is negative ! This error is likely due to an unstable lattice." );
        return 2; 
     }

    // alpha_1x and alpha_2y
    temp = E2(i_npx,i_x)*E2(i_x,i_x);
    calcs_.back().alpha_1x = -2.0*real(temp);
    calcs_.back().u1       = -2.0*imag(temp);

    temp = E2(i_npy,i_y)*E2(i_y,i_y);
    calcs_.back().alpha_2y = -2.0*real(temp);
    calcs_.back().u4       = -2.0*imag(temp);

    // beta_1y and beta_2x
    temp = E2(i_y,i_x);
    calcs_.back().beta_1y = 2.0*real(temp*conj(temp));
    temp = E2(i_x,i_y);
    calcs_.back().beta_2x = 2.0*real(temp*conj(temp));

    // alpha_1y and alpha_2x

    temp = E2(i_npy,i_x)*E2(i_y,i_x);
    calcs_.back().alpha_1y = -2.0*real(temp);
    calcs_.back().u2       = -2.0*imag(temp);

    temp = E2(i_npx,i_y)*E2(i_x,i_npy);
    calcs_.back().alpha_2x = -2.0*real(temp);
    calcs_.back().u3       = -2.0*imag(temp);

  }

  return 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<LBSage::Info> const& LBSage::getLBArray()
{
  
  return calcs_;
}
