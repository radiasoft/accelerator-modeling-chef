/*
 *  File: ClosedOrbitSage.cc
 *  The "Sage" for using FPSolver.
 *  
 *  Leo Michelotti
 *  November 23, 1998
 */


#include "ClosedOrbitSage.h"
#include "FPSolver.h"


ClosedOrbitSage::ClosedOrbitSage( const beamline* x )
: Sage( x ), _forced(0)
{
}

ClosedOrbitSage::~ClosedOrbitSage()
{
}

  
int ClosedOrbitSage::findClosedOrbit( JetParticle* p_jp )
{
  if( this->_verbose ) {
    cout << "ClosedOrbitSage -- Entering ClosedOrbitSage::findClosedOrbit" << endl;
  }

  // Preliminary steps ...
  beamline* bml = this->_myBeamlinePtr;
  Jet__environment* storedEnv = Jet::lastEnv;   // Just precautionary
  Jet::lastEnv = (p_jp->State()).Env();


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
  short int i_x   =  p_prt->xIndex();
  short int i_y   =  p_prt->yIndex();
  short int i_z   =  p_prt->cdtIndex();
  short int i_px  =  p_prt->npxIndex();
  short int i_py  =  p_prt->npyIndex();
  short int i_dpp =  p_prt->ndpIndex();

  Vector inState(  p_prt->State() );
  inState( i_dpp ) = 0.0;
  p_prt->setState( inState );

  Vector co     (  inState );
  

  // Turn off all RF ...
  bmlnElmnt* q = 0;
  DeepBeamlineIterator getNext( bml );

  while((  q = getNext++  )) 
  {
    if( 0 == strcmp( "thinrfcavity", q->Type() ) ) 
    {
      sd = new strengthData;
      sd->address = q;
      sd->strength = q->Strength();
      cavityStrengths.append( sd );
      q->setStrength( 0.0 );
    }
  }


  // Set up an FPSolver ...
  int i;
  double jumpScale [] = { 1.0e-3, 1.0e-3, 1.0e-4, 1.0e-4 };
  double zeroScale [] = { 1.0e-9, 1.0e-9, 1.0e-9, 1.0e-9 };

  FPSolver fp( bml );
  for( i = 0; i < 6; i++ ) fp.ZeroScale(i) = zeroScale[i];
  for( i = 0; i < 6; i++ ) fp.JumpScale(i) = jumpScale[i];


  // If the calculation is forced, calculate the closed orbit.
  if( this->_forced ) 
  {
    p_prt->setState( inState );
    fp( (JetProton*) p_jp, "transverse", Sage::no );  // ???
              // ??? The recast is a kludge which must be changed.
    
    // Test the new closed orbit ...
    if( p_prt ) delete p_prt;
    p_prt =  p_jp->ConvertToParticle();
    co = p_prt->State();
    bml->propagate( *p_prt );
    if( ( fabs( co(i_x)  - p_prt->State(i_x)  ) > 1.0e-6 ) ||
        ( fabs( co(i_y)  - p_prt->State(i_y)  ) > 1.0e-6 ) ||
        ( fabs( co(i_px) - p_prt->State(i_px) ) > 1.0e-6 ) ||
        ( fabs( co(i_py) - p_prt->State(i_py) ) > 1.0e-6 )
    ){
      cerr << "*** ERROR ***                                   \n"
              "*** ERROR *** ClosedOrbitSage::findClosedOrbit            \n"
              "*** ERROR *** Closed orbit not correct.         \n"
              "*** ERROR ***                                   \n"
           << endl;
      if( this->_verbose ) {
        cout << "ClosedOrbitSage -- Leaving ClosedOrbitSage::findClosedOrbit" << endl;
      }
  
      return 1;
    }
  }

  // If not, check to see if the Particle is on a transverse closed orbit.
  else 
  {
    bml->propagate( *p_prt );
    if( ( fabs( inState(i_x)  - p_prt->State(i_x)  ) > 1.0e-6 ) ||
        ( fabs( inState(i_y)  - p_prt->State(i_y)  ) > 1.0e-6 ) ||
        ( fabs( inState(i_px) - p_prt->State(i_px) ) > 1.0e-6 ) ||
        ( fabs( inState(i_py) - p_prt->State(i_py) ) > 1.0e-6 )
    ){
  
      if( this->_verbose ) {
        cout << "ClosedOrbitSage --- Starting calculation of closed orbit." << endl;
      }
  
      // If not, solve for the closed orbit ...
      p_prt->setState( inState );
      fp( (JetProton*) p_jp, "transverse", Sage::no );  // ???
                // ??? The recast is a kludge which must be changed.
      
      // Test the new closed orbit ...
      if( p_prt ) delete p_prt;
      p_prt =  p_jp->ConvertToParticle();
      co = p_prt->State();
      bml->propagate( *p_prt );
      if( ( fabs( co(i_x)  - p_prt->State(i_x)  ) > 1.0e-6 ) ||
          ( fabs( co(i_y)  - p_prt->State(i_y)  ) > 1.0e-6 ) ||
          ( fabs( co(i_px) - p_prt->State(i_px) ) > 1.0e-6 ) ||
          ( fabs( co(i_py) - p_prt->State(i_py) ) > 1.0e-6 )
      ){
        cerr << "*** ERROR ***                                   \n"
                "*** ERROR *** ClosedOrbitSage::findClosedOrbit            \n"
                "*** ERROR *** Closed orbit not correct.         \n"
                "*** ERROR ***                                   \n"
             << endl;
        if( this->_verbose ) {
          cout << "ClosedOrbitSage -- Leaving ClosedOrbitSage::findClosedOrbit" << endl;
        }
  
        return 1;  // ??? Memory leak!!
      }
    }
  }

  if( this->_verbose ) {
    cout << "ClosedOrbitSage --- Closed orbit successfully calculated." << endl;
  }


  // Final operations ....................................

  if( p_prt ) delete p_prt;

  // Return RF to original settings before leaving ...
  while((  sd = (strengthData*) cavityStrengths.get()  )) 
  {
    ( sd->address )->setStrength( sd->strength );
    delete sd;
  }

  // Reset the Jet environment ...
  Jet::lastEnv = storedEnv;


  // ??? REMOVE: Put the argument on the closed orbit
  // ??? REMOVE: p_jp->setState( co );
  // 
  // 
  if( this->_verbose ) {
    cout << "ClosedOrbitSage -- Leaving ClosedOrbitSage::findClosedOrbit" << endl;
  }

  return 0;
}


void ClosedOrbitSage::eraseAll()
{
  cout << "ClosedOrbitSage::eraseAll() does nothing." << endl;
}
