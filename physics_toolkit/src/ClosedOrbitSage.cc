/*************************************************************************
**************************************************************************
**************************************************************************
******
******  PHYSICS TOOLKIT: Library of utilites and Sage classes
******             which facilitate calculations with the
******             BEAMLINE class library.
******
******  File:      ClosedOrbitSage.cc
******  Version:   2.0
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
#include <beamline/BeamlineIterator.h>
#include <mxyzptlk/Jet__environment.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>


using namespace std;

int ClosedOrbitSage::ERR_NOTRING = -17;

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ClosedOrbitSage::ClosedOrbitSage( beamline const* x, bool doClone )
: Sage( x, doClone ), forced_(false), ignoreErrors_(false)
{}


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
  if( _verbose ) {
    *_outputStreamPtr << "ClosedOrbitSage -- Entering ClosedOrbitSage::findClosedOrbit"
                      << endl;
  }

  // If the line is not to be treated as a ring, abort the calculation.
  if( !(this->isTreatedAsRing()) ) {
    if( !ignoreErrors_ ) { return ERR_NOTRING; }
  }

  // Preliminary steps ...

  Jet__environment_ptr  storedEnv  = Jet__environment::getLastEnv();
  JetC__environment_ptr storedEnvC = JetC__environment::getLastEnv();

  Jet__environment::setLastEnv( jp.State().Env() );


  // Data structures for handling RF ...
  struct strengthData
  {
    bmlnElmnt*    address;
    double        strength;
  };
  strengthData* sd;

  slist cavityStrengths;


  // Helper particle ...

  Particle prt(jp);

  Vector inState( prt.State() );

  int i_x   = Particle::xIndex();
  int i_y   = Particle::yIndex();
  int i_z   = Particle::cdtIndex();
  int i_px  = Particle::npxIndex();
  int i_py  = Particle::npyIndex();
  int i_dpp = Particle::ndpIndex();


  // Turn off all RF ...

  bmlnElmnt* q = 0;
  DeepBeamlineIterator getNext( *_myBeamlinePtr );

  while((  q = getNext++  ))
  {
    if(    ( 0 == strcmp( "rfcavity",     q->Type() ) )
        || ( 0 == strcmp( "thinrfcavity", q->Type() ) ) )
    {
      sd = new strengthData;
      sd->address = q;
      sd->strength = q->Strength();
      cavityStrengths.append( sd );
      q->setStrength( 0.0 );
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
    _myBeamlinePtr->propagate( prt );
    // *** CHANGE ***
    // *** CHANGE *** See above re tolerances
    // *** CHANGE ***
    if( ( std::abs( inState(i_x)  - prt.State(i_x)  ) > 1.0e-6 ) ||
        ( std::abs( inState(i_y)  - prt.State(i_y)  ) > 1.0e-6 ) ||
        ( std::abs( inState(i_px) - prt.State(i_px) ) > 1.0e-6 ) ||
        ( std::abs( inState(i_py) - prt.State(i_py) ) > 1.0e-6 )
    ){
      if( 0 != ( ret = invokeFPSolver( jp ) ) ) {
        if( !ignoreErrors_ ) { return ret; }
      }
    }
  }

  if( _verbose ) {
    *_outputStreamPtr << "ClosedOrbitSage --- Closed orbit successfully calculated."
         << endl;
  }


  // Register the closed orbit with the beamline.
  // NOTE: this alters the line.
  if( _verbose ) {
    *_outputStreamPtr << "ClosedOrbitSage --- Registering closed orbit: will change line."
         << endl;
  }

  prt = Particle(jp);

  RefRegVisitor registrar( prt );
 _myBeamlinePtr->accept( registrar );


  // Useful Particle and Vector
  // containing the closed orbit.

  prt = Particle(jp);
  prt.set_cdt(0.0);

  Vector z( BMLN_dynDim );
  z = prt.State();

  Jet__environment::setLastEnv( Jet__environment::getApproxJetEnvironment(Jet__environment::getLastEnv()->maxWeight(), z));

  bool newcoords = false;
  coord*  tmpcoord[BMLN_dynDim];

  if ( !Jet__environment::getLastEnv() ) {  // true if there was no suitable approximate Environment  
    Jet__environment::BeginEnvironment( jp.State().Env()->maxWeight() );
    for( i = 0; i < BMLN_dynDim; i++ ) { 
      tmpcoord[i] = new coord(z(i)); 
    }
    Jet__environment::EndEnvironment();
    //    Jet::_lastEnv  = Jet__environment::EndEnvironment();
    //    JetC::_lastEnv = Jet::_lastEnv;

    newcoords = true;
  }

  // Instantiate a JetParticle with the new environment
  // and propagate it once around the ring.

  if( _verbose ) {
    *_outputStreamPtr << "ClosedOrbitSage --- Propagating JetParticle around ring final time."
         << endl;
  }

  JetParticle jpr2Ptr(prt);

  _myBeamlinePtr->propagate( jpr2Ptr );

  // Reset the argument to this second JetParticle
  jp = jpr2Ptr;


  // Final operations ....................................

  // ... Return RF to original settings before leaving ...

  while((  sd = (strengthData*) cavityStrengths.get()  ))
  {
    ( sd->address )->setStrength( sd->strength );
    delete sd;
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

  if( _verbose ) {
    *_outputStreamPtr << "ClosedOrbitSage -- Leaving ClosedOrbitSage::findClosedOrbit"
         << endl;
  }

  return 0;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int ClosedOrbitSage::invokeFPSolver( JetParticle& jp )
{
  if( _verbose ) {
    *_outputStreamPtr
      << "ClosedOrbitSage --- Starting calculation of closed orbit."
      << endl;
  }


  int i_x   = Particle::xIndex();
  int i_y   = Particle::yIndex();
  int i_z   = Particle::cdtIndex();
  int i_px  = Particle::npxIndex();
  int i_py  = Particle::npyIndex();
  int i_dpp = Particle::ndpIndex();


  // Set up an FPSolver ...
  FPSolver fp( _myBeamlinePtr );

  // *** CHANGE ***
  // *** CHANGE *** jumpScale and zeroScale should be
  // *** CHANGE *** member data, set by the invoking program
  // *** CHANGE ***

  double jumpScale [] = { 1.0e-3, 1.0e-3, 1.0e-3, 1.0e-4, 1.0e-4, 1.0e-4 };
  double zeroScale [] = { 1.0e-9, 1.0e-9, 1.0e-9, 1.0e-9, 1.0e-9, 1.0e-9 };

  for( int i = 0; i < BMLN_dynDim; i++ ) fp.ZeroScale(i) = zeroScale[i];
  for( int i = 0; i < BMLN_dynDim; i++ ) fp.JumpScale(i) = jumpScale[i];

  // *** CHANGE ***
  // *** CHANGE *** fp should know nothing about beamlines
  // *** CHANGE ***


  int fpError = fp( jp, "transverse", Sage::no );

  if( 0 != fpError )
  {
    *_errorStreamPtr
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

  _myBeamlinePtr->propagate( prt );

  // *** CHANGE ***
  // *** CHANGE *** Why one micron?
  // *** CHANGE *** Shouldn't the stopping criterion be the same
  // *** CHANGE *** as was used by the FPSolver fp?

  if( ( std::abs( co(i_x)  - prt.State(i_x)  ) > 1.0e-6 ) ||
      ( std::abs( co(i_y)  - prt.State(i_y)  ) > 1.0e-6 ) ||
      ( std::abs( co(i_px) - prt.State(i_px) ) > 1.0e-6 ) ||
      ( std::abs( co(i_py) - prt.State(i_py) ) > 1.0e-6 )
  ){
    *_errorStreamPtr
         << "\n*** WARNING ***"
            "\n*** WARNING *** File: " << __FILE__ << " Line: " << __LINE__
         << "\n*** WARNING *** ClosedOrbitSage::invokeFPSolver"
            "\n*** WARNING *** Closed orbit not correct."
            "\n*** WARNING *** delta x = "
         << (std::abs(co(i_x)  - prt.State(i_x)))
         << "\n*** WARNING *** delta y = "
         << (std::abs(co(i_y)  - prt.State(i_y)))
         << "\n*** WARNING *** delta p_x/p = "
         << (std::abs(co(i_px)  - prt.State(i_px)))
         << "\n*** WARNING *** delta p_y/p = "
         << (std::abs(co(i_py)  - prt.State(i_py)))
         << "\n*** WARNING ***"
         << endl;

    if( _verbose ) {
      *_outputStreamPtr << "ClosedOrbitSage -- Leaving ClosedOrbitSage::_invokeFPSolver with error"
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
  *_outputStreamPtr << "ClosedOrbitSage::eraseAll() does nothing." << endl;
}
