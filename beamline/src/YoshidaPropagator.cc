/*************************************************************************
**************************************************************************
**************************************************************************
******
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and
******             synchrotrons.
******
******  File:      YoshidaPropagator.cc
******
******  Copyright (c) Fermi Research Alliance
******                Universities Research Association, Inc.
******                Fermilab
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms
******  of the License supplied with this software.
******
******  Software and documentation created under
******  U.S. Department of Energy Contracts No. DE-AC02-76CH03000
******  and No. DE-AC02-07CH11359.
******
******  The U.S. Government retains a world-wide non-exclusive,
******  royalty-free license to publish or reproduce documentation
******  and software for U.S. Government purposes. This software
******  is protected under the U.S. and Foreign Copyright Laws.
******
******
******  Author:    Leo Michelotti
******             Email: michelotti@fnal.gov
******
******
******  ----------------
******  REVISION HISTORY
******  ----------------
******
******  Jun 2013           michelotti@fnal.gov
******  - initial implementation. This is a prototype version, primarily 
******    intended for further testing.  Use cautiously otherwise.
******    Planned future versions will have a different interface
******    (esp. by using templates to expand the application beyond
******    quadrupole to other beamline elements).
******
******  Feb 2015           michelotti@fnal.gov
******  - removed BmlPtr my_beamline_ptr_ from YoshidaPropagator's
******    private data. Combined with lack of virtual destructors
******    for base propagators, it resulted in a memory leak.
******
**************************************************************************
*************************************************************************/

#include <iostream>
#include <beamline/beamline.h>
#include <beamline/drift.h>
#include <beamline/quadrupole.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/YoshidaPropagator.h>

namespace
{
  // Frequently included
  // -------------------
  Particle::PhaseSpaceIndex const& i_x   = Particle::xIndex;
  Particle::PhaseSpaceIndex const& i_y   = Particle::yIndex;
  Particle::PhaseSpaceIndex const& i_cdt = Particle::cdtIndex;
  Particle::PhaseSpaceIndex const& i_npx = Particle::npxIndex;
  Particle::PhaseSpaceIndex const& i_npy = Particle::npyIndex;
  Particle::PhaseSpaceIndex const& i_ndp = Particle::ndpIndex;


template<typename Particle_t>
void propagate( quadrupole& elm, Particle_t& p )
{
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  State_t& state = p.State();

  BmlPtr& bml = bmlnElmnt::core_access::get_BmlPtr(elm);

  for ( beamline::iterator it = bml->begin(); it != bml->end(); ++it ) {
     (*it)->localPropagate( p );
  }

  state[i_cdt] -= elm.getReferenceTime();
}


//----------------------------------------------------------------------------------
// Workaround for gcc < 4.2 mishandling of templates defined in anonymous namespace
//----------------------------------------------------------------------------------

#if (__GNUC__ == 3) ||  ((__GNUC__ == 4) && (__GNUC_MINOR__ < 2 ))

template void propagate( quadrupole& elm,    Particle& p );
template void propagate( quadrupole& elm, JetParticle& p );

#endif

} // namespace


using namespace std;
using FNAL::pcerr;
using FNAL::pcout;


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


YoshidaPropagator::YoshidaPropagator( int n, int steps )
: order_(n),
  steps_(steps),
  accum_az_(0.0),
  accum_kick_(0.0),
  prev_az_(0.0)
{
  if( order_ < 0 ) {
    (*pcerr) << "*** ERROR *** "
             << "\n*** ERROR *** " << __FILE__ << ", " << __LINE__
             << "\nYoshidaPropagator constructor called with negative order argument = "
             << order_
             << endl;

    ostringstream uic;
    uic  << "Constructor called with negative order argument = " << order_;
    throw( GenericException( __FILE__, __LINE__,
           "YoshidaPropagator::YoshidaPropagator( int order, int steps )",
           uic.str().c_str() ) );
  }

  if( steps_ < 0 ) {
    (*pcerr) << "*** ERROR *** "
             << "\n*** ERROR *** " << __FILE__ << ", " << __LINE__
             << "\nYoshidaPropagator constructor called with negative steps argument = "
             << steps_
             << endl;

    ostringstream uic;
    uic  << "Constructor called with negative steps argument = " << steps_;
    throw( GenericException( __FILE__, __LINE__,
           "YoshidaPropagator::YoshidaPropagator( int order, int steps )",
           uic.str().c_str() ) );
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


YoshidaPropagator::YoshidaPropagator( YoshidaPropagator const& x ) // Should not be necessary
: order_(x.order_),
  steps_(x.steps_),
  accum_az_(x.accum_az_),
  accum_kick_(x.accum_kick_),
  prev_az_(x.prev_az_)
{
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void YoshidaPropagator::DriftMap_( double length )
{
   accum_az_ += length;
   return;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void YoshidaPropagator::KickMap_( BmlPtr& my_beamline_ptr, double faux_length, double strength )
{
   my_beamline_ptr->append( DriftPtr   ( new drift   ( "", accum_az_ - prev_az_ ) ) );
   my_beamline_ptr->append( ThinQuadPtr( new thinQuad( "", faux_length*strength ) ) );

   accum_kick_ += faux_length*strength;
   prev_az_ = accum_az_;
   return;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void YoshidaPropagator::generate_map_( BmlPtr& my_beamline_ptr, int n, double t, double strength )
{
  if( 0 == n ) {
    DriftMap_( t/2.0 );
    KickMap_( my_beamline_ptr, t, strength );
    DriftMap_( t/2.0 );

    return;
  }
  else {
    double a, x0, x1;
    a = pow( 2.0, 1.0/(2.0*double(n)+1.0));
    x1 = 1.0/(2.0 - a );
    x0 = - a*x1;

    generate_map_( my_beamline_ptr, n-1, x1*t, strength );
    generate_map_( my_beamline_ptr, n-1, x0*t, strength );
    generate_map_( my_beamline_ptr, n-1, x1*t, strength );

    return;
  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void YoshidaPropagator::setup( quadrupole& arg)
{
  // Apply modified Yoshida algorithm
  // using recursive method generate_map_
  // --------------------------------
  accum_kick_ = 0.0;

  BmlPtr my_beamline_ptr( new beamline );

  for( int step = 0 ; step < steps_ ; ++step ) {
    accum_az_   = 0.0;
    prev_az_    = 0.0;
    generate_map_( my_beamline_ptr, order_, arg.Length()/steps_, arg.Strength() );


    // A final drift must be added to get the particle
    // to the end of the step.
    // --------------------------
    my_beamline_ptr->append( DriftPtr   ( new drift   ( "", accum_az_ - prev_az_ ) ) );
  }
  prev_az_ = accum_az_;


  // Test before returning
  // ---------------------
  if( 1.0e-12 < abs( accum_az_ - arg.Length()/steps_ ) )
  {
    (*pcerr) << "*** ERROR *** "
             << "\n*** ERROR *** " << __FILE__ << ", " << __LINE__
             << "\n*** ERROR *** Accumulated azimuth was " << accum_az_
             << "; expected value is " << arg.Length() << " meters."
             << "\n*** ERROR *** "
             << endl;

     ostringstream uic;
     uic  << "\nAccumulated azimuth was " << accum_az_
          << "; expected value is " << arg.Length() << " meters.";
     throw( GenericException( __FILE__, __LINE__,
            "void YoshidaPropagator::setup( quadrupole& arg)",
            uic.str().c_str() ) );
  }
  if( 1.0e-12 < abs( accum_kick_ - arg.Length()*arg.Strength() ) )
  {
    (*pcerr) << "*** ERROR *** "
             << "\n*** ERROR *** " << __FILE__ << ", " << __LINE__
             << "\n*** ERROR *** Accumulated kick strength was " << accum_kick_
             << "; expected value is " << arg.Length()*arg.Strength() << " Tesla."
             << "\n*** ERROR *** "
             << endl;

     ostringstream uic;
     uic  << "\nAccumulated kick strength was " << accum_kick_
          << "; expected value is " << arg.Length()*arg.Strength() << " Tesla.";
     throw( GenericException( __FILE__, __LINE__,
            "void YoshidaPropagator::setup( quadrupole& arg)",
            uic.str().c_str() ) );
  }


  // Final step: transfer the constructed line
  // -----------------------------------------
  BmlPtr& bml = bmlnElmnt::core_access::get_BmlPtr(arg);
  bml = my_beamline_ptr;

  return;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void YoshidaPropagator::operator()( quadrupole& elm, Particle& p)
{
  ::propagate( elm, p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void YoshidaPropagator::operator()( quadrupole& elm, JetParticle& p)
{
  ::propagate( elm, p);
}
