////////////////////////////////////////////////////////////
// 
// File:          d2sTest_1.cc
// Author:        Leo Michelotti
// 
// Revision date: Friday. March 28, 2008.  (original version)
// 
////////////////////////////////////////////////////////////
// 
// Does DriftsToSlots work as expected on the simplest
// possible models?
// 
// (Compare this file with beamline/tests/bendTest_2.cc.)
// 
// ------------
// COMMAND LINE
// ------------
// d2sTest_1 [options]
// 
// -------
// OPTIONS
// -------
// Note: NNN represents an integer
// 
// -N   NNN   the bend angle is set to 2 pi / NNN
//            : default = 128
// 
////////////////////////////////////////////////////////////

#include <iostream>

#include <basic_toolkit/VectorD.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/drift.h>
#include <beamline/sbend.h>
#include <beamline/rbend.h>
#include <beamline/beamline.h>
#include <beamline/RefRegVisitor.h>

extern beamline* DriftsToSlots( beamline const& argbml );

using namespace std;

struct Options
{
  bool   valid;
  double n;
  double pc;
  double energy;
  double brho;
  double rho;
  double bodyLength;
  double arcLength;
  double bendAngle;
  double bendField;
  double driftLength;
  int    order;

  Options( int, char**, int=0 );
};


Options::Options( int argc, char** argv, int lastargs )
:   valid(true)
  , n(128)
  , pc(8)
  , bodyLength(2)
  , driftLength(1)
  , order(3)
{
  Proton proton(100);
  proton.SetReferenceMomentum(pc);

  energy     = proton.ReferenceEnergy();
  bendAngle  = M_TWOPI/n;
  brho       = proton.ReferenceBRho();  // Tesla-meters
  rho        = bodyLength / (2.0*sin( bendAngle/2.0 ));
  arcLength  = rho*bendAngle;
  bendField  = brho/rho;

  valid = true;

  int limit = argc-lastargs;
  string s;
  int i = 1;
  while( valid && (i < limit) ) {
    s.assign( argv[i++] );
    if( '-' == s[0] ) {
      s.assign( s.substr(1) );
      if( s == "print" ) {
        cout <<   "Momentum [GeV/c] : " << pc
             << "\nBend angle [rad] : " << bendAngle
             << endl;
      }
      else if( s == "N" ) {
        if( i < limit ) { 
          n = std::abs( atof( argv[i++] ) );
          bendAngle  = M_TWOPI/n;
          rho        = bodyLength / (2.0*sin( bendAngle/2.0 ));
          arcLength  = rho*bendAngle;
          bendField  = brho/rho;
        }
        else {
          cerr << "\n*** ERROR *** Too few parameters." << endl;
          valid = false;
        }
      }
      else {
        cerr << "\n*** ERROR *** Unrecognized option: " << s << endl;
        valid = false;
      }
    }
  }
}


/////////////////////////////////////////////////////////////////////

int main( int argc, char** argv )
{
  int ret = 0;

  Options myOptions( argc, argv );
  if( myOptions.valid ) {
    cout << "\nCommand line: ";
    for( int i = 0; i < argc; i++ ) {
      cout << argv[i] << "  ";
    }
    cout << endl;
  }
  else {
    return 1;
  }

  Proton proton( myOptions.energy );
  JetParticle::createStandardEnvironments( myOptions.order );
  Frame arcFrame;
  Vector fd(3);

  Mapping rbendMap, sbendMap;
  
  // ------------------------------------------------------
  // TEST 1: propagation through rbend
  { 
  cout << "RBEND TEST" << endl;

  rbend magnet( "", myOptions.bodyLength
                  , myOptions.bendField
                  , myOptions.bendAngle );
  drift ooo( "", myOptions.driftLength );
  beamline testline;
           testline.append( ooo );
           testline.append( ooo );
           testline.append( magnet );
           testline.append( ooo );
           testline.append( ooo );

  BmlPtr newlinePtr( DriftsToSlots( testline ) );
  proton.setStateToZero();
  RefRegVisitor rrv( proton );
  newlinePtr->accept( rrv );

  proton.setStateToZero();
  cout << "Initial: " << proton.State() << endl;
  newlinePtr->propagate( proton );
  cout << "Final  : " << proton.State() << endl;

  Vector state( proton.State() );
  for( int i = 0; i < state.Dim(); i++ ) {
    if( 1.0e-14 < std::abs(state[i]) ) { 
      cout <<   "*** FAILED *** "
              "\n*** FAILED *** : " << __FILE__ << ", line " << __LINE__
           << "\n*** FAILED *** : Failed rbend proton propagation:"
              "\n*** FAILED *** : 1.0e-14 < state(" << i << ") = " << state[i]
           << "\n*** FAILED *** "
           << endl;
      ret = 1;
      break;
    }
  }

  JetProton jetproton( myOptions.energy );
  newlinePtr->propagate( jetproton );
  rbendMap = jetproton.State();
  } // End TEST 1
  // ------------------------------------------------------

  // ------------------------------------------------------
  // TEST 2: propagation through sbend
  {
  cout << "SBEND TEST" << endl;

  sbend magnet( "", myOptions.arcLength
                  , myOptions.bendField
                  , myOptions.bendAngle
                  , myOptions.bendAngle/2.0
                  , myOptions.bendAngle/2.0 );
  drift ooo( "", myOptions.driftLength );
  beamline testline;
           testline.append( ooo );
           testline.append( ooo );
           testline.append( magnet );
           testline.append( ooo );
           testline.append( ooo );

  BmlPtr newlinePtr( DriftsToSlots( testline ) );
  proton.setStateToZero();
  RefRegVisitor rrv( proton );
  newlinePtr->accept( rrv );

  proton.setStateToZero();
  cout << "Initial: " << proton.State() << endl;
  newlinePtr->propagate( proton );
  cout << "Final  : " << proton.State() << endl;

  Vector state( proton.State() );
  for( int i = 0; i < state.Dim(); i++ ) {
    if( 1.0e-14 < std::abs(state[i]) ) { 
      cout <<   "*** FAILED *** "
              "\n*** FAILED *** : " << __FILE__ << ", line " << __LINE__
           << "\n*** FAILED *** : Failed sbend proton propagation:"
              "\n*** FAILED *** : 1.0e-14 < state(" << i << ") = " << state[i]
           << "\n*** FAILED *** "
           << endl;
      ret = 2;
      break;
    }
  }

  JetProton jetproton( myOptions.energy );
  newlinePtr->propagate( jetproton );
  sbendMap = jetproton.State();
  } // End TEST 2
  // ------------------------------------------------------

  // ------------------------------------------------------
  // Testing the difference between sbend and rbend maps
  // 
  cout << "\n\n--- BEGIN RBEND MAP ----------\n" << endl;
  rbendMap.printCoeffs();
  cout << "\n--- END   RBEND MAP ----------\n" << endl;
  cout << "\n\n--- BEGIN SBEND MAP ----------\n" << endl;
  sbendMap.printCoeffs();
  cout << "\n--- END   SBEND MAP ----------\n" << endl;

  Mapping diffMap( rbendMap - sbendMap );
  cout << "\n\n--- BEGIN DIFFERENCE MAP ----------\n" << endl;
  diffMap.printCoeffs();
  cout << "\n--- END   DIFFERENCE MAP ----------\n" << endl;


  JLterm termPtr;
  IntArray exps( diffMap[0].getEnvNumVar() );  // Included for illustration
                                               // ; not currently used.
  double ccc = 0;
  for(   int counter = 0
       ; counter < diffMap.Dim()
       ; ++counter ) {
    Jet zlorfik = diffMap[ counter ];
    for(   Jet::iterator iter = zlorfik.begin()
         ; iter != zlorfik.end()
         ; ++iter ) {
      termPtr = *iter;
      exps = termPtr.exponents( zlorfik.Env() );
      ccc  = termPtr.coefficient();

      // NOTE: THIS TEST IS NOT ADEQUATE
      //     : IT SHOULD BE REFINED
      if( 1.0e-14 < std::abs(ccc) ) { 
        cout <<   "*** FAILED *** "
                "\n*** FAILED *** : " << __FILE__ << ", line " << __LINE__
             << "\n*** FAILED *** : Failed difference; at exponent "
             <<                     exps
             <<                     " the coefficient = "
             <<                     ccc
             << "\n*** FAILED *** : rbend and sbend are not equivalent at order "
             <<                     myOptions.order
             << "\n*** FAILED *** "
             << endl;
        ret = 3;
      }
    }
  }

  return ret;
}
