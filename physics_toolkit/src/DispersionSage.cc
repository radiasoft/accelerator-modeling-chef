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
******  REVISION HISTORY
******
******  Dec 2006 - Jean-Francois Ostiguy 
******             ostiguy@fnal
******    
******  - interface based on Particle& rather than ptrs. 
******    Stack allocated local Particle objects.
******  - changes to accomodate new style boost::any based Barnacle objects.
******  - use new style STL-compatible beamline iterators
******  - calcs_ array is now an STL vector. LF are now returned by 
******    returning a const reference to the entire vector.
******  - misc cleanups.  
******
******  Mar 2007   ostiguy@fnal.gov
******
******  - eliminated references to slist/dlist
******
******   June 2007 ostiguy@fnal.gov
******   
******  - added new method to compute dispersion using AD
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


extern int filterTransverseTunes( /* const */ MatrixD&, Vector& );

// ... Globals:
const int DispersionSage::DONE             = 0;
const int DispersionSage::TOO_MANY_VECTORS = 7;
const int DispersionSage::IMPOSSIBILITY    = 111;

const DispersionSage::Options DispersionSage::defaultFlags;


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

DispersionSage::Info::Info() {
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
  dpp_( 0.00001), 
  ignoreErrors_( false ),
  calcs_()
  {}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

DispersionSage::DispersionSage( beamline const& x )
: Sage( x ), 
  dpp_( 0.00001 ), 
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

  int i_x   =  Particle::xIndex();
  int i_y   =  Particle::yIndex();
  int i_z   =  Particle::cdtIndex();
  int i_px  =  Particle::npxIndex();
  int i_py  =  Particle::npyIndex();
  int i_dpp =  Particle::ndpIndex();


  // Preliminary steps ...
  ClosedOrbitSage clsg( myBeamlinePtr_ );
  if( verbose_ ) { clsg.set_verbose(); }


  // If the argument is not already declared
  //   to be on the closed orbit, then use a
  //   ClosedOrbitSage to rebuild it.

  JetParticle local_jp(jp);

  if( !flags.onClosedOrbit ) { 
    clsg.setForcedCalc(); 
    ret = clsg.findClosedOrbit( local_jp );
    clsg.unsetForcedCalc();

    // Review the solution
    if( 0 == ret ) {
      if( verbose_ ) {
        *outputStreamPtr_ << "DispersionSage -- Closed orbit successfully calculated." << std::endl;
        outputStreamPtr_->flush();
      }
      jp = local_jp;
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
          outputStreamPtr_->flush();
          errorStreamPtr_->flush();
        }
        jp = local_jp;
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


  // At this point,  jp is on a closed orbit, with
  //   appropriate Jet__environment. 
  //   For purposes of paranoia, I reset Jet::_lastEnv here.

  Jet__environment::setLastEnv( jp.State().Env() );


  // Calculate the closed orbit for an off-momentum particle ...
  if( verbose_ ) {
    *outputStreamPtr_ << "DispersionSage --- Starting calculation of offset closed orbit." << std::endl;
    outputStreamPtr_->flush();
  }


  local_jp = Particle(jp);

  Particle firstParticle(jp);
  MatrixD firstJacobian  = jp.State().Jacobian();

  double energy = firstParticle.ReferenceEnergy();
  double mass   = firstParticle.Mass();

  double momentum = sqrt( energy*energy - mass*mass )*( 1.0 + dpp_ );
  energy          = sqrt( momentum*momentum + mass*mass );

  local_jp.SetReferenceEnergy( energy );

  clsg.setForcedCalc();
  ret = clsg.findClosedOrbit( local_jp );
  clsg.unsetForcedCalc();

  // Review the solution
  if( ret == 0 ) {
    if( verbose_ ) {
      *outputStreamPtr_ << "DispersionSage -- Offset closed orbit successfully calculated." << std::endl;
      outputStreamPtr_->flush();
    }
  }
  else {
    if( ignoreErrors_ ) {
      *outputStreamPtr_ << "DispersionSage -- ClosedOrbitSage error "
                        << ret
                        << " is being ignored for offset orbit." 
                        << std::endl;
      *errorStreamPtr_  << "DispersionSage -- ClosedOrbitSage error "
                        << ret
                        << " is being ignored for offset orbit." 
                        << std::endl;
      outputStreamPtr_->flush();
      errorStreamPtr_->flush();

      ret = 0;
    }
    else {
      if( verbose_ ) {
        *outputStreamPtr_ << "DispersionSage -- Off-momentum closed orbit not successfully calculated." << std::endl;
        *outputStreamPtr_ << "DispersionSage -- Leaving DispersionSage::fullCalc" << std::endl;
        outputStreamPtr_->flush();
      }

      Jet__environment::setLastEnv( storedEnv );
      return ret;
    }
  }

  Particle secondParticle(local_jp);
  MatrixD secondJacobian  = local_jp.State().Jacobian();


  // Attach initial dispersion data to the beamline ...
  Vector d( firstParticle.State().Dim() );


  // Attach dispersion data wherever desired ...
  if( verbose_ ) {
    *outputStreamPtr_ << "DispersionSage --- Attaching dispersion data to the elements." << std::endl;
    outputStreamPtr_->flush();
  }



  double lng = 0.0;
 
  DispersionSage::Info info;
 
  calcs_.clear();

  for (beamline::deep_iterator it = myBeamlinePtr_->deep_begin(); it != myBeamlinePtr_->deep_end(); ++it ) 
  {

    (*it)->propagate( firstParticle );
    (*it)->propagate( secondParticle );

    lng += (*it)->OrbitLength( firstParticle );

    d = ( secondParticle.State()  -  firstParticle.State() ) / dpp_;
  
    info.closedOrbit.hor  = firstParticle.get_x();
    info.closedOrbit.ver = firstParticle.get_y();
    info.dispersion.hor  = d( i_x  );
    info.dPrime.hor      = d( i_px );
    info.dispersion.ver  = d( i_y  );
    info.dPrime.ver      = d( i_py );
    info.arcLength       = lng;
   
    calcs_.push_back(info);

  }  


  // Attach tune and chromaticity to the beamline ........
  Vector firstNu(2), secondNu(2);
  if( ( 0 == filterTransverseTunes( firstJacobian, firstNu   ) ) && 
      ( 0 == filterTransverseTunes( secondJacobian, secondNu ) ) )
  {
    lr_.tune.hor = firstNu(0);
    lr_.tune.ver = firstNu(1);
    lr_.chromaticity.hor = ( secondNu(0) - firstNu(0) ) / dpp_;
    lr_.chromaticity.ver = ( secondNu(1) - firstNu(1) ) / dpp_;
  }
  else {
    *errorStreamPtr_ 
         << "\n*** ERROR ***"
         << "\n*** ERROR *** File: " << __FILE__ << " Line: " << __LINE__
         << "\n*** ERROR *** DispersionSage::fullCalc"
            "\n*** ERROR *** Horrible, inexplicable error occurred"
            "\n*** ERROR *** while trying to filter the tunes."
            "\n*** ERROR ***"
         << std::endl;
    ret = 111;
    Jet__environment::setLastEnv( storedEnv );
    return ret;
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

  const int i_x   =  Particle::xIndex();
  const int i_y   =  Particle::yIndex();
  const int i_z   =  Particle::cdtIndex();
  const int i_px  =  Particle::npxIndex();
  const int i_py  =  Particle::npyIndex();
  const int i_dpp =  Particle::ndpIndex();

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
  for ( beamline::deep_iterator it = myBeamlinePtr_->deep_begin();  it != myBeamlinePtr_->deep_end(); ++it ) {

      (*it)->propagate( firstParticle );
      (*it)->propagate( secondParticle );

      lng += (*it)->OrbitLength( firstParticle );
   
      d = ( secondParticle.State() - firstParticle.State() ) /  (secondParticle.get_ndp()- firstParticle.get_ndp() ) ;
  
      info.dispersion.hor  = d( i_x  );
      info.dPrime.hor      = d( i_px ) / firstParticle.get_npz();
      info.dispersion.ver  = d( i_y  );
      info.dPrime.ver      = d( i_py ) / firstParticle.get_npz();
      info.arcLength       = lng;
      calcs_.push_back(info);

  }

  if( verbose_ ) {
    *outputStreamPtr_ << "DispersionSage -- Leaving DispersionSage::pushCalc" << std::endl;
    outputStreamPtr_->flush();
  }


  // A little useful paranoia:
  // clean out global information if not a ring.
  if( !isTreatedAsRing() ) {
    lr_.tune.hor         = 0.0;
    lr_.tune.ver         = 0.0;
    lr_.chromaticity.hor = 0.0;
    lr_.chromaticity.ver = 0.0;
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

  const int i_x   =  Particle::xIndex();
  const int i_y   =  Particle::yIndex();
  const int i_z   =  Particle::cdtIndex();
  const int i_px  =  Particle::npxIndex();
  const int i_py  =  Particle::npyIndex();
  const int i_dpp =  Particle::ndpIndex();

  double lng = 0.0;

  Particle    particle(p);
  JetParticle jparticle(p);

  Mapping& state = jparticle.State();

  IntArray exp_d(6);
  exp_d[5]  = 1;

  state[i_x ].setTermCoefficient(initialConditions.dispersion.hor, exp_d );
  state[i_px].setTermCoefficient(initialConditions.dPrime.hor,     exp_d );

  state[i_y ].setTermCoefficient(initialConditions.dispersion.ver, exp_d );
  state[i_py].setTermCoefficient(initialConditions.dPrime.ver,     exp_d );
 
  eraseAll();

  const double start_momentum = particle.ReferenceMomentum();

  DispersionSage::Info info;

  for ( beamline::deep_iterator it = myBeamlinePtr_->deep_begin();  it != myBeamlinePtr_->deep_end(); ++it ) {

      (*it)->propagate(jparticle);
     
      lng += (*it)->OrbitLength( particle );

      double scale  =  jparticle.ReferenceMomentum()/start_momentum;
 
      info.dispersion.hor = jparticle.State()[i_x ].getTermCoefficient(exp_d ) * scale;
      info.dPrime.hor     = jparticle.State()[i_px].getTermCoefficient(exp_d ) * scale; 
      info.dispersion.ver = jparticle.State()[i_y ].getTermCoefficient(exp_d ) * scale;
      info.dPrime.ver     = jparticle.State()[i_py].getTermCoefficient(exp_d ) * scale; 
      info.arcLength      = lng;

      calcs_.push_back(info);

  }


  // A little useful paranoia:
  // clean out global information if not a ring.
  if( !isTreatedAsRing() ) {
    lr_.tune.hor         = 0.0;
    lr_.tune.ver         = 0.0;
    lr_.chromaticity.hor = 0.0;
    lr_.chromaticity.ver = 0.0;
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


