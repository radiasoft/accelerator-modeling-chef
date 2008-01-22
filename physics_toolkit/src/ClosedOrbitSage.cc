/*************************************************************************
**************************************************************************
**************************************************************************
******
******  PHYSICS TOOLKIT: Library of utilites and Sage classes
******             which facilitate calculations with the
******             BEAMLINE class library.
******
******  File:      ClosedOrbitSage.cc
******
******  Copyright (c) 2001  Universities Research Association, Inc.
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
******  Mar 2007        ostiguy@fnal.gov
******  -reference counted elements
******  -use new-style beamline iterators
******  -eliminated references to slist/dlist
******
******  Jan 2008        ostiguy@fnal.gov
****** - modified to compute closed orbit using non-zero dp/p state component
******
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

/*
 *  File: ClosedOrbitSage.cc
 *  The "Sage" for using FPSolver.
 *
 *  November 23, 1998
 *  Leo Michelotti
 *
 *  Modified:
 *  April 29, 2004
 *  Leo Michelotti
 */

#include <physics_toolkit/ClosedOrbitSage.h>
#include <physics_toolkit/FPSolver.h>
#include <beamline/RefRegVisitor.h>
#include <mxyzptlk/Jet__environment.h>
#include <beamline/rfcavity.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <list>

using namespace std;

namespace {
  Particle::PhaseSpaceIndex const& i_x   = Particle::xIndex;
  Particle::PhaseSpaceIndex const& i_y   = Particle::yIndex;
  Particle::PhaseSpaceIndex const& i_z   = Particle::cdtIndex;
  Particle::PhaseSpaceIndex const& i_px  = Particle::npxIndex;
  Particle::PhaseSpaceIndex const& i_py  = Particle::npyIndex;
  Particle::PhaseSpaceIndex const& i_dpp = Particle::ndpIndex;
  
  int const BMLN_dynDim = 6;

 }

  int ClosedOrbitSage::ERR_NOTRING = -17;

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ClosedOrbitSage::ClosedOrbitSage( BmlPtr x )
: Sage( x ), forced_(false), ignoreErrors_(false)
{ }


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void ClosedOrbitSage::setForcedCalc()
{
  forced_ = true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ClosedOrbitSage::unsetForcedCalc()
{
  forced_ = false;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ClosedOrbitSage::setIgnoreErrors( bool x )
{
  ignoreErrors_ = x;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


int ClosedOrbitSage::findClosedOrbit( JetParticle& jp )
{
  if( verbose_ ) {
    *outputStreamPtr_ << "ClosedOrbitSage -- Entering ClosedOrbitSage::findClosedOrbit"
                      << endl;
  }

  // If the line is not to be treated as a ring, abort the calculation.
  if( !isTreatedAsRing() ) {
    if( !ignoreErrors_ ) { return ERR_NOTRING; }
  }

  // Preliminary steps ...

  Jet__environment_ptr  storedEnv  = Jet__environment::getLastEnv();
  JetC__environment_ptr storedEnvC = JetC__environment::getLastEnv();

  Jet__environment::setLastEnv( jp.State().Env() );


  // Helper particle ...

  Particle prt(jp);

  Vector inState( prt.State() );

  // Turn off all RF ...

  std::list<strengthData> cavityStrengths;

  for ( beamline::deep_iterator it  = myBeamlinePtr_->deep_begin(); 
                                it != myBeamlinePtr_->deep_end(); ++it )
  {
    if(         ( dynamic_cast<rfcavity*>( (*it).get() ) )
	     || ( dynamic_cast<rfcavity*>( (*it).get() ) ) )
    {
      strengthData sd;
      sd.address = (*it);
      sd.strength = (*it)->Strength();
      cavityStrengths.push_back(sd);
      (*it)->setStrength( 0.0 );
    }
  }


  int i, ret;

  // If the calculation is forced, calculate the closed orbit.
  if( forced_ )
  {
    if( 0 != ( ret = invokeFPSolver( jp ) ) ) {
      if( !ignoreErrors_ ) { return ret; }
    }
  }

  // If not, check to see if the Particle is on a transverse closed orbit.
  else
  {
    myBeamlinePtr_->propagate( prt );
    // *** CHANGE ***
    // *** CHANGE *** See above re tolerances
    // *** CHANGE ***
    if( ( std::abs( inState(i_x)  - prt.State()[i_x]  ) > 1.0e-6 ) ||
        ( std::abs( inState(i_y)  - prt.State()[i_y]  ) > 1.0e-6 ) ||
        ( std::abs( inState(i_px) - prt.State()[i_px] ) > 1.0e-6 ) ||
        ( std::abs( inState(i_py) - prt.State()[i_py] ) > 1.0e-6 )
    ){
      if( 0 != ( ret = invokeFPSolver( jp ) ) ) {
        if( !ignoreErrors_ ) { return ret; }
      }
    }
  }

  if( verbose_ ) {
    *outputStreamPtr_ << "ClosedOrbitSage --- Closed orbit successfully calculated."
         << endl;
  }

  // Register the closed orbit with the beamline.
  // NOTE: this alters the line.

  if( verbose_ ) {
    *outputStreamPtr_ << "ClosedOrbitSage --- Registering closed orbit: will change line."
         << endl;
  }

  prt = Particle(jp);

  RefRegVisitor registrar( prt );
  myBeamlinePtr_->accept( registrar );


  // Useful Particle and Vector
  // containing the closed orbit.

  prt = Particle(jp);
  prt.set_cdt(0.0);

  Jet__environment::setLastEnv( Jet__environment::getApproxJetEnvironment(Jet__environment::getLastEnv()->maxWeight(), prt.State() ));

  bool newcoords = false;
  coord*  tmpcoord[BMLN_dynDim];

  if ( !Jet__environment::getLastEnv() ) {  // true if there was no suitable approximate Environment  
 
    Jet__environment::BeginEnvironment( jp.State().Env()->maxWeight() );
    for( i = 0; i < BMLN_dynDim; ++i) { 
      tmpcoord[i] = new coord(prt.State()[i]); 
    }
    Jet__environment::EndEnvironment();
    //    Jet::_lastEnv  = Jet__environment::EndEnvironment();
    //    JetC::_lastEnv = Jet::_lastEnv;

    newcoords = true;
  }

  // Instantiate a JetParticle with the new environment
  // and propagate it once around the ring.

  if( verbose_ ) {
    *outputStreamPtr_ << "ClosedOrbitSage --- Propagating JetParticle around ring final time."
         << endl;
  }

  JetParticle jpr2Ptr(prt, Jet__environment::getLastEnv() );

  myBeamlinePtr_->propagate( jpr2Ptr );

  // Reset the argument to this second JetParticle

  jp = jpr2Ptr;

  // Final operations ....................................

  // ... Return RF to original settings before leaving ...

  for ( std::list<strengthData>::iterator it  = cavityStrengths.begin();
	it != cavityStrengths.end(); ++it )
  {
    ( it->address )->setStrength( it->strength );
  }


  // ... Reset the Jet environment to its incoming state

  Jet__environment::setLastEnv(storedEnv);
  JetC__environment::setLastEnv(storedEnvC);

  // ... delete temporary coordinates 
  if( newcoords ) {
    for( i=0; i<BMLN_dynDim; i++ ) { 
      delete tmpcoord[i]; 
    }
  }

  if( verbose_ ) {
    *outputStreamPtr_ << "ClosedOrbitSage -- Leaving ClosedOrbitSage::findClosedOrbit"
         << endl;
  }

  

  return 0;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int ClosedOrbitSage::invokeFPSolver( JetParticle& jp )
{
  if( verbose_ ) {
    *outputStreamPtr_
      << "ClosedOrbitSage --- Starting calculation of closed orbit."
      << endl;
  }


  FPSolver fp( myBeamlinePtr_ );

#if 0

  double jumpScale [] = { 1.0e-6, 1.0e-6, 1.0e-6, 1.0e-6, 1.0e-6, 1.0e-6 };
  double zeroScale [] = { 1.0e-9, 1.0e-9, 1.0e-9, 1.0e-9, 1.0e-9, 1.0e-9 };

  for( int i = 0; i < BMLN_dynDim; i++ ) fp.ZeroScale(i) = zeroScale[i];
  for( int i = 0; i < BMLN_dynDim; i++ ) fp.JumpScale(i) = jumpScale[i];

#endif

  int fpError = fp( jp, "transverse", Sage::no );

  if(fpError)
  {
    *errorStreamPtr_
         << "\n*** ERROR ***"
            "\n*** ERROR *** File: " << __FILE__ << " Line: " << __LINE__
         << "\n*** ERROR *** ClosedOrbitSage::invokeFPSolver"
            "\n*** ERROR *** The FPSolver failed to find a solution."
            "\n*** ERROR *** Return error code = " << fpError
         << "\n*** ERROR *** "
         << endl;
    return 2;
  }


  // Test the new closed orbit ...

  Particle prt(jp);
  Vector co = prt.State();

  myBeamlinePtr_->propagate( prt );

  // *** CHANGE ***
  // *** CHANGE *** Why one micron?
  // *** CHANGE *** Shouldn't the stopping criterion be the same
  // *** CHANGE *** as was used by the FPSolver fp?

  if( ( std::abs( co(i_x)  - prt.State()[i_x]  ) > 2.0e-6 ) ||
      ( std::abs( co(i_y)  - prt.State()[i_y]  ) > 2.0e-6 ) ||
      ( std::abs( co(i_px) - prt.State()[i_px] ) > 2.0e-6 ) ||
      ( std::abs( co(i_py) - prt.State()[i_py] ) > 2.0e-6 )
  ){
    *errorStreamPtr_
         << "\n*** WARNING ***"
            "\n*** WARNING *** File: " << __FILE__ << " Line: " << __LINE__
         << "\n*** WARNING *** ClosedOrbitSage::invokeFPSolver"
            "\n*** WARNING *** Closed orbit not correct."
            "\n*** WARNING *** delta x = "
         << (std::abs(co(i_x)  - prt.State()[i_x]))
         << "\n*** WARNING *** delta y = "
         << (std::abs(co(i_y)  - prt.State()[i_y]))
         << "\n*** WARNING *** delta p_x/p = "
         << (std::abs(co(i_px)  - prt.State()[i_px]))
         << "\n*** WARNING *** delta p_y/p = "
         << (std::abs(co(i_py)  - prt.State()[i_py]))
         << "\n*** WARNING ***"
         << endl;

    if( verbose_ ) {
      *outputStreamPtr_ << "ClosedOrbitSage -- Leaving ClosedOrbitSage::invokeFPSolver with error"
                        << endl;
    }

    return 1;  
  }

  return 0;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ClosedOrbitSage::eraseAll()
{
  *outputStreamPtr_ << "ClosedOrbitSage::eraseAll() does nothing." << endl;
}
