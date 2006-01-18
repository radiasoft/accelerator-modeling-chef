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


#include <ClosedOrbitSage.h>
#include <FPSolver.h>
#include <RefRegVisitor.h>
#include <Jet__environment.h>


using namespace std;

int ClosedOrbitSage::ERR_NOTRING = -17;

ClosedOrbitSage::ClosedOrbitSage( const beamline* x, bool doClone )
: Sage( x, doClone ), _forced(false), _ignoreErrors(false)
{
}

ClosedOrbitSage::~ClosedOrbitSage()
{
}


int ClosedOrbitSage::findClosedOrbit( JetParticle* p_jp )
{
  if( _verbose ) {
    *_outputStreamPtr << "ClosedOrbitSage -- Entering ClosedOrbitSage::findClosedOrbit"
                      << endl;
  }

  // If the line is not to be treated as a ring, abort the calculation.
  if( !(this->isTreatedAsRing()) ) {
    if( !_ignoreErrors ) { return ERR_NOTRING; }
  }

  // Preliminary steps ...
  Jet__environment_ptr storedEnv = Jet::_lastEnv;
  Jet::_lastEnv = p_jp->State().Env();


  // Data structures for handling RF ...
  struct strengthData
  {
    bmlnElmnt*    address;
    double        strength;
  };
  strengthData* sd;

  slist cavityStrengths;


  // Helper particle ...
  Particle* p_prt =  p_jp->ConvertToParticle();
  Vector inState(  p_prt->State() );

  short int i_x   = Particle::_x();
  short int i_y   = Particle::_y();
  short int i_z   = Particle::_cdt();
  short int i_px  = Particle::_xp();
  short int i_py  = Particle::_yp();
  short int i_dpp = Particle::_dpop();


  // Turn off all RF ...
  bmlnElmnt* q = 0;
  DeepBeamlineIterator getNext( _myBeamlinePtr );

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
  if( _forced )
  {
    if( 0 != ( ret = _invokeFPSolver( p_jp ) ) ) {
      if( !_ignoreErrors ) { return ret; }
    }
  }

  // If not, check to see if the Particle is on a transverse closed orbit.
  else
  {
    _myBeamlinePtr->propagate( *p_prt );
    // *** CHANGE ***
    // *** CHANGE *** See above re tolerances
    // *** CHANGE ***
    if( ( std::abs( inState(i_x)  - p_prt->State(i_x)  ) > 1.0e-6 ) ||
        ( std::abs( inState(i_y)  - p_prt->State(i_y)  ) > 1.0e-6 ) ||
        ( std::abs( inState(i_px) - p_prt->State(i_px) ) > 1.0e-6 ) ||
        ( std::abs( inState(i_py) - p_prt->State(i_py) ) > 1.0e-6 )
    ){
      if( 0 != ( ret = _invokeFPSolver( p_jp ) ) ) {
        if( !_ignoreErrors ) { return ret; }
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
  if( p_prt ) delete p_prt;
  p_prt =  p_jp->ConvertToParticle();
  RefRegVisitor registrar( *p_prt );
  _myBeamlinePtr->accept( registrar );


  // Useful Particle and Vector
  // containing the closed orbit.
  if( p_prt ) delete p_prt;
  p_prt =  p_jp->ConvertToParticle();
  p_prt->set_cdt(0.0);
  Vector z( BMLN_dynDim );
  z = p_prt->State();

  Jet::_lastEnv = TJetEnvironment<double>::getApproxJetEnvironment(Jet::_lastEnv->maxWeight(), z);

  bool newcoords = false;
  coord*  tmpcoord[BMLN_dynDim];
  if ( Jet::_lastEnv.get() == 0 ) {  // true if there was no suitable approximate Environment  
    Jet__environment::BeginEnvironment( p_jp->State().Env()->maxWeight() );
    for( i = 0; i < BMLN_dynDim; i++ ) { 
      tmpcoord[i] = new coord(z(i)); 
    }
    Jet__environment::EndEnvironment();
    newcoords = true;
  }

  // Instantiate a JetParticle with the new environment
  // and propagate it once around the ring.
  if( _verbose ) {
    *_outputStreamPtr << "ClosedOrbitSage --- Propagating JetParticle around ring final time."
         << endl;
  }
  JetParticle* jpr2Ptr = p_prt->ConvertToJetParticle();
  _myBeamlinePtr->propagate( *jpr2Ptr );


  // Reset the argument to this second JetParticle
  *p_jp = *jpr2Ptr;


  // Final operations ....................................

  // ... Return RF to original settings before leaving ...
  while((  sd = (strengthData*) cavityStrengths.get()  ))
  {
    ( sd->address )->setStrength( sd->strength );
    delete sd;
  }


  // ... Reset the Jet environment to its incoming state
  Jet::_lastEnv = storedEnv;

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

  if( p_prt ) {
    delete p_prt;
    p_prt = 0;
  }
  if( jpr2Ptr ) {
    delete jpr2Ptr;
    jpr2Ptr = 0;
  }

  return 0;
}


int ClosedOrbitSage::_invokeFPSolver( JetParticle* p_jp )
{
  if( _verbose ) {
    *_outputStreamPtr
      << "ClosedOrbitSage --- Starting calculation of closed orbit."
      << endl;
  }

  int i;

  short int i_x   = Particle::_x();
  short int i_y   = Particle::_y();
  short int i_z   = Particle::_cdt();
  short int i_px  = Particle::_xp();
  short int i_py  = Particle::_yp();
  short int i_dpp = Particle::_dpop();


  // Set up an FPSolver ...
  FPSolver fp( _myBeamlinePtr );

  // *** CHANGE ***
  // *** CHANGE *** jumpScale and zeroScale should be
  // *** CHANGE *** member data, set by the invoking program
  // *** CHANGE ***
  double jumpScale [] = { 1.0e-3, 1.0e-3, 1.0e-3, 1.0e-4, 1.0e-4, 1.0e-4 };
  double zeroScale [] = { 1.0e-9, 1.0e-9, 1.0e-9, 1.0e-9, 1.0e-9, 1.0e-9 };
  for( i = 0; i < BMLN_dynDim; i++ ) fp.ZeroScale(i) = zeroScale[i];
  for( i = 0; i < BMLN_dynDim; i++ ) fp.JumpScale(i) = jumpScale[i];

  // *** CHANGE ***
  // *** CHANGE *** fp should know nothing about beamlines
  // *** CHANGE ***
  // ??? Also: the recast is a kludge which must be changed.
  int fpError = fp( dynamic_cast<JetProton*>(p_jp), "transverse", Sage::no );
  if( 0 != fpError )
  {
    *_errorStreamPtr
         << "\n*** ERROR ***"
            "\n*** ERROR *** File: " << __FILE__ << " Line: " << __LINE__
         << "\n*** ERROR *** ClosedOrbitSage::_invokeFPSolver"
            "\n*** ERROR *** The FPSolver failed to find a solution."
            "\n*** ERROR *** Return error code = " << fpError
         << "\n*** ERROR *** "
         << endl;
    return 2;
  }

  // Test the new closed orbit ...
  Particle* p_prt =  p_jp->ConvertToParticle();
  Vector co( BMLN_dynDim );
  co = p_prt->State();
  _myBeamlinePtr->propagate( *p_prt );
  // *** CHANGE ***
  // *** CHANGE *** Why one micron?
  // *** CHANGE *** Shouldn't the stopping criterion be the same
  // *** CHANGE *** as was used by the FPSolver fp?
  if( ( std::abs( co(i_x)  - p_prt->State(i_x)  ) > 1.0e-6 ) ||
      ( std::abs( co(i_y)  - p_prt->State(i_y)  ) > 1.0e-6 ) ||
      ( std::abs( co(i_px) - p_prt->State(i_px) ) > 1.0e-6 ) ||
      ( std::abs( co(i_py) - p_prt->State(i_py) ) > 1.0e-6 )
  ){
    *_errorStreamPtr
         << "\n*** WARNING ***"
            "\n*** WARNING *** File: " << __FILE__ << " Line: " << __LINE__
         << "\n*** WARNING *** ClosedOrbitSage::_invokeFPSolver"
            "\n*** WARNING *** Closed orbit not correct."
            "\n*** WARNING *** delta x = "
         << (std::abs(co(i_x)  - p_prt->State(i_x)))
         << "\n*** WARNING *** delta y = "
         << (std::abs(co(i_y)  - p_prt->State(i_y)))
         << "\n*** WARNING *** delta p_x/p = "
         << (std::abs(co(i_px)  - p_prt->State(i_px)))
         << "\n*** WARNING *** delta p_y/p = "
         << (std::abs(co(i_py)  - p_prt->State(i_py)))
         << "\n*** WARNING ***"
         << endl;

    if( _verbose ) {
      *_outputStreamPtr << "ClosedOrbitSage -- Leaving ClosedOrbitSage::_invokeFPSolver with error"
                        << endl;
    }

    delete p_prt;
    return 1;  // ??? Memory leak!!
  }

  delete p_prt;
  return 0;
}


void ClosedOrbitSage::eraseAll()
{
  *_outputStreamPtr << "ClosedOrbitSage::eraseAll() does nothing." << endl;
}
