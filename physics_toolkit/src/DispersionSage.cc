/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                                                
******  File:      DispersionSage.cc
******  Version:   1.0
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
******  - changes to accomodate new boost::any based Barnacle objects.
******  - use new style STL-compatible beamline iterators
******  - calcs_ array is now an STL vector. LF are now returned by 
******    returning a const reference to the entire vector.
******  - misc cleanup.  
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


DispersionSage::GlobalInfo::GlobalInfo() {
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


DispersionSage::DispersionSage( const beamline* x, bool doClone )
: Sage( x, doClone ), 
  dpp_( 0.00005 ), 
  ignoreErrors_( false ),
  calcs_()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

DispersionSage::DispersionSage( const beamline& x, bool doClone )
: Sage( &x, doClone ), 
  dpp_( 0.00005 ), 
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
    *_errorStreamPtr << "\n*** WARNING ***                         "
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

int DispersionSage::doCalc( JetParticle & jp, beamline::Criterion& crit )
{
  // DispersionSage::doCalc maintained for backwards compatability.
  return fullCalc( jp, crit );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int DispersionSage::fullCalc( JetParticle& jp, beamline::Criterion& )
{
  // 
  // This is copied and modified from 
  // int LattFuncSage::NewDisp_Calc( const JetParticle* arg_jp, 
  //                                 bool onClosedOrbit )
  // 

  if( _verbose ) {
    *_outputStreamPtr << "DispersionSage -- Entering DispersionSage::fullCalc" << std::endl;
    _outputStreamPtr->flush();
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
  ClosedOrbitSage clsg( _myBeamlinePtr );
  if( _verbose ) { clsg.set_verbose(); }


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
      if( _verbose ) {
        *_outputStreamPtr << "DispersionSage -- Closed orbit successfully calculated." << std::endl;
        _outputStreamPtr->flush();
      }
      jp = local_jp;
    }
    else {
      if( ignoreErrors_ ) {
        if( _verbose ) {
          *_outputStreamPtr << "DispersionSage -- ClosedOrbitSage error "
                            << ret
                            << " is being ignored." 
                            << std::endl;
          *_errorStreamPtr  << "DispersionSage -- ClosedOrbitSage error "
                            << ret
                            << " is being ignored." 
                            << std::endl;
          _outputStreamPtr->flush();
          _errorStreamPtr->flush();
        }
        jp = local_jp;
        ret = 0;
      }
      else {
        if( _verbose ) {
          *_errorStreamPtr << "DispersionSage -- Closed orbit not successfully calculated." << std::endl;
          *_errorStreamPtr << "DispersionSage -- Leaving DispersionSage::fullCalc" << std::endl;
          _errorStreamPtr->flush();
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
  if( _verbose ) {
    *_outputStreamPtr << "DispersionSage --- Starting calculation of offset closed orbit." << std::endl;
    _outputStreamPtr->flush();
  }


  local_jp = Particle(jp);

  Particle firstParticle(jp);
  MatrixD firstJacobian  = jp.State().Jacobian();

  double energy = firstParticle.ReferenceEnergy();
  double mass   = firstParticle.Mass();

  double momentum = sqrt( energy*energy - mass*mass )*( 1.0 + dpp_ );
  energy   = sqrt( momentum*momentum + mass*mass );

  local_jp.SetReferenceEnergy( energy );

  clsg.setForcedCalc();
  ret = clsg.findClosedOrbit( local_jp );
  clsg.unsetForcedCalc();

  // Review the solution
  if( ret == 0 ) {
    if( _verbose ) {
      *_outputStreamPtr << "DispersionSage -- Offset closed orbit successfully calculated." << std::endl;
      _outputStreamPtr->flush();
    }
  }
  else {
    if( ignoreErrors_ ) {
      *_outputStreamPtr << "DispersionSage -- ClosedOrbitSage error "
                        << ret
                        << " is being ignored for offset orbit." 
                        << std::endl;
      *_errorStreamPtr  << "DispersionSage -- ClosedOrbitSage error "
                        << ret
                        << " is being ignored for offset orbit." 
                        << std::endl;
      _outputStreamPtr->flush();
      _errorStreamPtr->flush();

      ret = 0;
    }
    else {
      if( _verbose ) {
        *_outputStreamPtr << "DispersionSage -- Off-momentum closed orbit not successfully calculated." << std::endl;
        *_outputStreamPtr << "DispersionSage -- Leaving DispersionSage::fullCalc" << std::endl;
        _outputStreamPtr->flush();
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
  if( _verbose ) {
    *_outputStreamPtr << "DispersionSage --- Attaching dispersion data to the elements." << std::endl;
    _outputStreamPtr->flush();
  }



  double lng = 0.0;
  bmlnElmnt* q = 0;
 
  DispersionSage::Info info;
 
  calcs_.clear();

  for (beamline::deep_iterator it = _myBeamlinePtr->deep_begin(); it != _myBeamlinePtr->deep_end(); ++it ) 
  {

    q = (*it);
 
    q->propagate( firstParticle );
    q->propagate( secondParticle );

    lng += q->OrbitLength( firstParticle );

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
    *_errorStreamPtr 
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
  if( _verbose ) {
    *_outputStreamPtr << "DispersionSage -- Leaving DispersionSage::fullCalc" << std::endl;
    _outputStreamPtr->flush();
  }

  Jet__environment::setLastEnv( storedEnv );
  return ret;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int DispersionSage::pushCalc( const Particle& prt, const Info& initialConditions )
{
  if( _verbose ) {
    *_outputStreamPtr << "DispersionSage -- Entering DispersionSage::pushCalc" << std::endl;
    _outputStreamPtr->flush();
  }

  int ret = 0;
  Vector d( prt.State().Dim() );

  int i_x   =  Particle::xIndex();
  int i_y   =  Particle::yIndex();
  int i_z   =  Particle::cdtIndex();
  int i_px  =  Particle::npxIndex();
  int i_py  =  Particle::npyIndex();
  int i_dpp =  Particle::ndpIndex();

  double lng = 0.0;

  Particle firstParticle(prt);
  double energy   = firstParticle.ReferenceEnergy();
  double mass     = firstParticle.Mass();
  double momentum = sqrt( energy*energy - mass*mass )*( 1.0 + dpp_ );
         energy   = sqrt( momentum*momentum + mass*mass );

  Particle secondParticle(prt);
  secondParticle.SetReferenceEnergy( energy );

  secondParticle.set_x   (prt.get_x()   + (initialConditions.dispersion.hor)*dpp_);
  secondParticle.set_y   (prt.get_y()   + (initialConditions.dispersion.ver)*dpp_);
  secondParticle.set_npx (prt.get_npx() + (initialConditions.dPrime.hor)*dpp_);
  secondParticle.set_npy (prt.get_npy() + (initialConditions.dPrime.ver)*dpp_);
 
  eraseAll();

  DispersionSage::Info info;
  for ( beamline::deep_iterator it = _myBeamlinePtr->deep_begin();  it != _myBeamlinePtr->deep_end(); ++it ) {

      (*it)->propagate( firstParticle );
      (*it)->propagate( secondParticle );

      lng += (*it)->OrbitLength( firstParticle );

      d = ( secondParticle.State() - firstParticle.State() ) / dpp_;
  
      info.dispersion.hor  = d( i_x  );
      info.dPrime.hor      = d( i_px );
      info.dispersion.ver  = d( i_y  );
      info.dPrime.ver      = d( i_py );
      info.arcLength       = lng;

      calcs_.push_back(info);

  }

  // Clean up before leaving .............................................


  if( _verbose ) {
    *_outputStreamPtr << "DispersionSage -- Leaving DispersionSage::pushCalc" << std::endl;
    _outputStreamPtr->flush();
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

  _myBeamlinePtr->dataHook.eraseAll( "Dispersion" );

  for ( beamline::deep_iterator it = _myBeamlinePtr->deep_begin();  it != _myBeamlinePtr->deep_end(); ++it ) {

    (*it)->dataHook.eraseAll( "Dispersion" );
  }

  calcs_.clear();

}


