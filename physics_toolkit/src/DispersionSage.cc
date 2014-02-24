/*************************************************************************
**************************************************************************
**************************************************************************
******
******  PHYSICS TOOLKIT: Library of utilites and Sage classes
******             which facilitate calculations with the
******             BEAMLINE class library.
******
******  File:      DispersionSage.cc
******
******  Copyright (c) 2004  Universities Research Association, Inc.
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
******  Author:    Leo Michelotti
******             Email: michelotti@fnal.gov
******
******
******  REVISION HISTORY
******
******  Dec 2006           Jean-Francois Ostiguy
******                     ostiguy@fnal.gov
******  - interface based on Particle& rather than ptrs.
******    Stack allocated local Particle objects.
******  - changes to accomodate new style boost::any based Barnacle objects.
******  - use new style STL-compatible beamline iterators
******  - calcs_ array is now an STL vector. LF are now returned by
******    returning a const reference to the entire vector.
******  - misc cleanups.
******
******  Mar 2007           ostiguy@fnal.gov
******  - eliminated references to slist/dlist
******
******  Jun 2007           ostiguy@fnal.gov
******  - added new method to compute dispersion using AD
******
******  Jan 2007           ostiguy@fnal.gov
******  - modified to compute ring dispersion using 2nd particle with
******    non-zero dp/p state component
******
******  Feb 2014           michelotti@fnal.gov
******  - modified member function DispersionSage::doCalc(...)
******    to use a four-point algorithm written by James Amundson
******    to evaluate dispersions and chromaticities. Compared to the
******    two-point algorithm, error is reduced from O((dp/p)^2) to
******    O((dp/p)^5).
******
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <physics_toolkit/ClosedOrbitSage.h>
#include <physics_toolkit/DispersionSage.h>
#include <physics_toolkit/DispersionSage.h>

#define DEFAULT_DPP 0.0005

extern int filterTransverseTunes( /* const */ MatrixD&, Vector& );

namespace {
  Particle::PhaseSpaceIndex const& i_x    =  Particle::xIndex;
  Particle::PhaseSpaceIndex const& i_y    =  Particle::yIndex;
  Particle::PhaseSpaceIndex const& i_z    =  Particle::cdtIndex;
  Particle::PhaseSpaceIndex const& i_npx  =  Particle::npxIndex;
  Particle::PhaseSpaceIndex const& i_npy  =  Particle::npyIndex;
  Particle::PhaseSpaceIndex const& i_ndp  =  Particle::ndpIndex;
}


// ... Globals:
const int DispersionSage::DONE             = 0;
const int DispersionSage::TOO_MANY_VECTORS = 7;
const int DispersionSage::IMPOSSIBILITY    = 111;

const DispersionSage::Options DispersionSage::defaultFlags;


using namespace std;


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

DispersionSage::Info::Info() 
{
  arcLength       = -1.0;
  closedOrbit.hor = 0.0;
  closedOrbit.ver = 0.0;
  dispersion.hor  = 0.0;
  dispersion.ver  = 0.0;
  dPrime.hor      = 0.0;
  dPrime.ver      = 0.0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

DispersionSage::GlobalInfo::GlobalInfo()
{
  tune.hor         = 0.0;
  tune.ver         = 0.0;
  chromaticity.hor = 0.0;
  chromaticity.ver = 0.0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

DispersionSage::Options::Options()
: onClosedOrbit(false)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

DispersionSage::DispersionSage( BmlPtr x )
: Sage( x ),
  dpp_( DEFAULT_DPP ),
  ignoreErrors_( false ),
  calcs_()
  {}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

DispersionSage::DispersionSage( beamline const& x )
: Sage( x ),
  dpp_( DEFAULT_DPP ),
  ignoreErrors_( false ),
  calcs_()
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

DispersionSage::GlobalInfo DispersionSage::getGlobalInfo() const
{
 return lr_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double DispersionSage::get_dpp() const
{
  return dpp_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void DispersionSage::set_dpp( double x )
{
  if( x > 0.0 ) { dpp_ = x; }
  else {
    *errorStreamPtr_ << "\n*** WARNING ***                         "
                        "\n*** WARNING *** DispersionSage::set_dpp( "
                     << x
                     << " )"
                        "\n*** WARNING *** Non-positive argument changed."
                        "\n*** WARNING ***                         "
                     << std::endl;
    if( x != 0.0 ) { dpp_ = -x; }
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void DispersionSage::setIgnoreErrors( bool x )
{
  ignoreErrors_ = x;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

DispersionSage::Options DispersionSage::get_options()
{
 return flags;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void DispersionSage::set_options( const DispersionSage::Options& x )
{
 flags = x;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int DispersionSage::doCalc( JetParticle& jp )
{
  if( verbose_ ) {
    *outputStreamPtr_ << "DispersionSage -- Entering DispersionSage::doCalc" << std::endl;
    outputStreamPtr_->flush();
  }

  // Preserve the current Jet environment
  Jet__environment_ptr storedEnv = Jet__environment::getLastEnv();

  int ret = 0;


  // Preliminary steps ...
  ClosedOrbitSage clsg( myBeamlinePtr_ );
  if( verbose_ ) { clsg.set_verbose(); }


  // If the argument is not already declared
  //   to be on the closed orbit, then use a
  //   ClosedOrbitSage to rebuild it.

  JetParticle jp_co(jp);

  if( !flags.onClosedOrbit ) {
    clsg.setForcedCalc();
    ret = clsg.findClosedOrbit( jp_co );
    clsg.unsetForcedCalc();

    // Review the solution
    if( ret == 0 ) {
      if( verbose_ ) {
        *outputStreamPtr_ << "DispersionSage -- Closed orbit successfully calculated." << std::endl;
        outputStreamPtr_->flush();
      }
      jp = jp_co;
    }
    else {
      if( ignoreErrors_ ) {
        if( verbose_ ) {
          *outputStreamPtr_ << "DispersionSage -- ClosedOrbitSage error "
                            << ret
                            << " is being ignored."
                            << std::endl;
          *errorStreamPtr_  << "DispersionSage -- ClosedOrbitSage error "
                            << ret
                            << " is being ignored."
                            << std::endl;
        }
        jp = jp_co;
        ret = 0;
      }
      else {
        if( verbose_ ) {
          *errorStreamPtr_ << "DispersionSage -- Closed orbit not successfully calculated." << std::endl;
          *errorStreamPtr_ << "DispersionSage -- Leaving DispersionSage::fullCalc" << std::endl;
          errorStreamPtr_->flush();
        }
        Jet__environment::setLastEnv( storedEnv );
        return ret;
      }
    }
  }

  // ---------------------------------------------------------
  // At this point,  jp is on a closed orbit, with
  //   appropriate Jet__environment.
  //   For purposes of paranoia, I reset Jet::_lastEnv here.
  //----------------------------------------------------------

  Jet__environment::setLastEnv( jp.State().Env() );


  // Calculate the closed orbit for an off-momentum particle ...
  
   if( verbose_ ) {
    *outputStreamPtr_ << "DispersionSage --- Starting calculation of offset closed orbit." << std::endl;
    outputStreamPtr_->flush();
  }
  

  // Instantiate five particles of the same
  // species as the argument and on the closed orbit.
  // -----------------------------------------------

  Particle firstParticle(jp_co);

  Particle probes [] = { firstParticle,    // NOTE: all these are on the closed
                         firstParticle,    //       orbit for dp/p = 0.  These
                         firstParticle,    //       will be recalculated below.
                         firstParticle,
                         firstParticle  };

  MatrixD firstJacobian  = jp_co.State().Jacobian();

  MatrixD jacobians [] = { firstJacobian,
                           firstJacobian,
                           firstJacobian,
                           firstJacobian,
                           firstJacobian };


  // Calculate the closed orbit for off-momentum particles ...
  if( verbose_ ) {
    (*pcout) << "LattFuncSage --- Starting calculation of offset closed orbit." << endl;
    (*pcout).flush();
  }

  double const dpp_scale  = get_dpp();
  double const dpp_center = probes[2].get_ndp();     // Usually, this is zero.
  probes[0].set_ndp( dpp_center - 2.0*dpp_scale );
  probes[1].set_ndp( dpp_center -     dpp_scale );
  probes[3].set_ndp( dpp_center +     dpp_scale );
  probes[4].set_ndp( dpp_center + 2.0*dpp_scale );


  for( int i = 0; i < 5; ++i )
  {
    if( 2 != i )
    {
      JetParticle jp = JetParticle(probes[i]);

      clsg.setForcedCalc();
      ret = clsg.findClosedOrbit( jp );
      clsg.unsetForcedCalc();

      if( ret == 0 ) {
        if( verbose_ ) {
          (*pcout) << "LattFuncSage -- Offset closed orbit successfully calculated." << endl;
          (*pcout).flush();
        }
      }
      else {
        if( verbose_ ) {
          (*pcout) << "LattFuncSage -- Off-momentum closed orbit not successfully calculated." << endl;
          (*pcout) << "LattFuncSage -- Leaving LattFuncSage::FourPointDisp_Calc" << endl;
          (*pcout).flush();
        }
        return ret;
      }

      probes[i] = Particle(jp);
      jacobians[i] = jp.State().Jacobian();
    }
  }

  // Reset environment to initial value
  Jet__environment::setLastEnv( storedEnv );



  // Attach dispersion data wherever desired ...
  if( verbose_ ) {
    (*pcout) << "LattFuncSage --- Attaching dispersion data to the elements." << endl;
    (*pcout).flush();
  }


  // Attach initial dispersion data to the beamline ...

  double lng = 0.0;

  for( beamline::deep_iterator it  = myBeamlinePtr_->deep_begin();
                               it != myBeamlinePtr_->deep_end();
                             ++it )
  {
    for( int i = 0; i < 5; ++i ) {
      (*it)->propagate( probes[i] );
    }

    lng += (*it)->OrbitLength( firstParticle );  // DANGER HERE

    Vector d_a = ( probes[3].State() - probes[1].State() ) / (2.0*dpp_scale);
    Vector d_b = ( probes[4].State() - probes[0].State() ) / (4.0*dpp_scale);
    Vector d   = ( 4.0*d_a - d_b )/3.0;

    DispersionSage::Info info;
    info.closedOrbit.hor  = firstParticle.get_x();
    info.closedOrbit.ver = firstParticle.get_y();
    info.dispersion.hor  = d( i_x  );
    info.dPrime.hor      = d( i_npx );
    info.dispersion.ver  = d( i_y  );
    info.dPrime.ver      = d( i_npy );
    info.arcLength       = lng;

    calcs_.push_back(info);
  }


  // Attach tune and chromaticity to the beamline ........

  Vector tunes(2);
  Vector nu [] = { tunes,
                   tunes,
                   tunes,
                   tunes,
                   tunes };

  for( int i = 0; i < 5; ++i )
  {
    if( 0 != filterTransverseTunes( jacobians[i], nu[i] ) )
    {
      (*pcerr) << "*** ERROR ***                                        \n"
                  "*** ERROR *** File: " << __FILE__ << "; Line " << __LINE__ << "\n"
                  "*** ERROR *** LattFuncSage::FourPointDisp_Calc                \n"
                  "*** ERROR ***                                        \n"
                  "*** ERROR *** Horrible error occurred while trying   \n"
                  "*** ERROR *** to filter the tunes for dpp = "
               << probes[i].get_ndp() << "\n"
                  "*** ERROR ***                                        \n"
               << endl;
      ret = 111;

      return ret;
    }
  }


  lr_.tune.hor = nu[2][0];
  lr_.tune.ver = nu[2][1];

  { // horizontal
    double a = ( nu[3][0] - nu[1][0] ) / (2.0*dpp_scale);
    double b = ( nu[4][0] - nu[0][0] ) / (4.0*dpp_scale);

    lr_.chromaticity.hor = ( 4.0*a - b )/3.0;
  }

  { // vertical
    double a = ( nu[3][1] - nu[1][1] ) / (2.0*dpp_scale);
    double b = ( nu[4][1] - nu[0][1] ) / (4.0*dpp_scale);

    lr_.chromaticity.ver = ( 4.0*a - b )/3.0;
  }


  if( verbose_ ) {
    (*pcout) << "LattFuncSage -- Leaving LattFuncSage::FourPointDisp_Calc" << endl;
    (*pcout).flush();
  }


  // Final operations ....................................
  if( verbose_ ) {
    *outputStreamPtr_ << "DispersionSage -- Leaving DispersionSage::fullCalc" << std::endl;
    outputStreamPtr_->flush();
  }

  Jet__environment::setLastEnv( storedEnv );

  return ret;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int DispersionSage::pushCalc( Particle const& prt, Info const& initialConditions )
{

  //------------------------------------------------------------------------------
  // This method computes the dispersion by subtracting two particle trajectories
  // The momentum offset is dpp_
  //------------------------------------------------------------------------------

  if( verbose_ ) {
    *outputStreamPtr_ << "DispersionSage -- Entering DispersionSage::pushCalc" << std::endl;
    outputStreamPtr_->flush();
  }

  int ret = 0;

  Vector d( prt.State().Dim() );

  double lng = 0.0;

  Particle firstParticle(prt);
  Particle secondParticle(prt);

  secondParticle.set_ndp( firstParticle.get_ndp() + dpp_  );

  secondParticle.set_x   ( initialConditions.dispersion.hor  * dpp_ );
  secondParticle.set_npx ( initialConditions.dPrime.hor      * dpp_ );

  secondParticle.set_y   ( initialConditions.dispersion.ver  * dpp_ );
  secondParticle.set_npy ( initialConditions.dPrime.ver      * dpp_ );

  eraseAll();

  double start_momentum = firstParticle.Momentum();

  DispersionSage::Info info;
  for ( beamline::deep_iterator it = myBeamlinePtr_->deep_begin();
                                it != myBeamlinePtr_->deep_end();
                              ++it )
  {
      (*it)->propagate( firstParticle );
      (*it)->propagate( secondParticle );

      lng += (*it)->OrbitLength( firstParticle );

      d = ( secondParticle.State() - firstParticle.State() ) /  (secondParticle.get_ndp()- firstParticle.get_ndp() ) ;

      info.dispersion.hor  = d( i_x  );
      info.dPrime.hor      = d( i_npx ) / firstParticle.get_npz();
      info.dispersion.ver  = d( i_y  );
      info.dPrime.ver      = d( i_npy ) / firstParticle.get_npz();
      info.arcLength       = lng;
      calcs_.push_back(info);

  }

  if( verbose_ ) {
    *outputStreamPtr_ << "DispersionSage -- Leaving DispersionSage::pushCalc" << std::endl;
    outputStreamPtr_->flush();
  }

  return ret;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int DispersionSage::pushCalc( JetParticle const& p, Info const& initialConditions )
{

  //--------------------------------------------------------------------
  // This method computes the dispersion using automatic differentiation
  //--------------------------------------------------------------------

  int ret = 0;

  double lng = 0.0;

  Particle    particle(p);
  JetParticle jparticle(p);

  Mapping& state = jparticle.State();

  IntArray exp_d(6);
  exp_d[5]  = 1;

  state[i_x ].setTermCoefficient(initialConditions.dispersion.hor, exp_d );
  state[i_npx].setTermCoefficient(initialConditions.dPrime.hor,     exp_d );

  state[i_y ].setTermCoefficient(initialConditions.dispersion.ver, exp_d );
  state[i_npy].setTermCoefficient(initialConditions.dPrime.ver,     exp_d );

  eraseAll();

  const double start_momentum = particle.ReferenceMomentum();

  DispersionSage::Info info;

  for ( beamline::deep_iterator it = myBeamlinePtr_->deep_begin();  it != myBeamlinePtr_->deep_end(); ++it ) {

      (*it)->propagate(jparticle);

      lng += (*it)->OrbitLength( particle );

      double scale  =  jparticle.ReferenceMomentum()/start_momentum;

      info.dispersion.hor = jparticle.State()[i_x ].getTermCoefficient(exp_d ) * scale;
      info.dPrime.hor     = jparticle.State()[i_npx].getTermCoefficient(exp_d ) * scale;
      info.dispersion.ver = jparticle.State()[i_y ].getTermCoefficient(exp_d ) * scale;
      info.dPrime.ver     = jparticle.State()[i_npy].getTermCoefficient(exp_d ) * scale;
      info.arcLength      = lng;

      calcs_.push_back(info);

  }

  return ret;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

DispersionSage::Info const& DispersionSage::getInfo() const
{
  return calcs_.back();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<DispersionSage::Info> const& DispersionSage::getDispersionArray() const
{
 return calcs_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void DispersionSage::eraseAll()
{

  myBeamlinePtr_->dataHook.eraseAll( "Dispersion" );

  for ( beamline::deep_iterator it  = myBeamlinePtr_->deep_begin();
                                it != myBeamlinePtr_->deep_end(); ++it ) {

    (*it)->dataHook.eraseAll( "Dispersion" );
  }

  calcs_.clear();

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


