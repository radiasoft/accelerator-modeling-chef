////////////////////////////////////////////////////////////
// 
// File:          bendTest_2.cc
// Author:        Leo Michelotti
// 
// Revision date: Tuesday. March 27, 2008.  (original version)
// 
////////////////////////////////////////////////////////////
// 
// With no error fields, an rbend should behave like 
// an sbend with parallel faces.
// 
// 
// ------------
// COMMAND LINE
// ------------
// bendTest_2 [options]
// 
// -------
// OPTIONS
// -------
// Note: NNN represents an integer
// 
// -print     prints parameters
// -N   NNN   the bend angle is set to 2 pi / NNN
//            : default = 128
// 
////////////////////////////////////////////////////////////

#include <iostream>

#include <basic_toolkit/VectorD.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/Slot.h>
#include <beamline/sbend.h>
#include <beamline/rbend.h>
#include <beamline/RefRegVisitor.h>

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

  { // TEST 1: propagation through rbend
  cout << "RBEND TEST" << endl;

  rbend magnet( "", myOptions.bodyLength
                  , myOptions.bendField
                  , myOptions.bendAngle );

  proton.set_npx( sin(myOptions.bendAngle/2.0) );
  RefRegVisitor rrv( proton );
  magnet.accept(rrv);
  proton.setStateToZero();

  double entryAngle = magnet.getEntryAngle();
  cout << "entryAngle = " << entryAngle << ";  bendAngle/2 = " << (myOptions.bendAngle/2.0) << endl;
  arcFrame.reset();
  arcFrame.rotate( - entryAngle, arcFrame.getyAxis() );
  fd(0) = 0.0;
  fd(1) = 0.0;
  fd(2) = myOptions.driftLength;
  arcFrame.translate(fd);
  Slot upstream_slot( "", arcFrame );

  double exitAngle = magnet.getExitAngle();
  cout << "exitAngle = " << exitAngle << ";  bendAngle/2 = " << (myOptions.bendAngle/2.0) << endl;
  arcFrame.reset(); 
  arcFrame.translate(fd);
  arcFrame.rotate( exitAngle, arcFrame.getyAxis() );
  Slot downstream_slot( "", arcFrame );

  proton.setStateToZero();
  cout << "Initial: " << proton.State() << endl;
  upstream_slot.propagate( proton );
  magnet.propagate( proton );
  downstream_slot.propagate( proton );
  cout << "Final  : " << proton.State() << endl;

  Vector state( proton.State() );
  int const i_cdt = Particle::cdtIndex;
  for( int i = 0; i < state.Dim(); i++ ) {
    if( i_cdt != i ) {
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
  }

  JetProton jetproton( myOptions.energy );
  upstream_slot.propagate( jetproton );
  magnet.propagate( jetproton );
  downstream_slot.propagate( jetproton );
  jetproton.State().printCoeffs();
  } // End TEST 1

  { // TEST 2: propagation through sbend
  cout << "SBEND TEST" << endl;

  sbend magnet( "", myOptions.arcLength
                  , myOptions.bendField
                  , myOptions.bendAngle
                  , myOptions.bendAngle/2.0
                  , myOptions.bendAngle/2.0 );

  double entryAngle = magnet.getEntryAngle();
  cout << "entryAngle = " << entryAngle << ";  bendAngle/2 = " << (myOptions.bendAngle/2.0) << endl;
  arcFrame.reset();
  arcFrame.rotate( - entryAngle, arcFrame.getyAxis() );
  fd(0) = 0.0;
  fd(1) = 0.0;
  fd(2) = myOptions.driftLength;
  arcFrame.translate(fd);
  Slot upstream_slot( "", arcFrame );

  double exitAngle = magnet.getExitAngle();
  cout << "exitAngle = " << exitAngle << ";  bendAngle/2 = " << (myOptions.bendAngle/2.0) << endl;
  arcFrame.reset(); 
  arcFrame.translate(fd);
  arcFrame.rotate( exitAngle, arcFrame.getyAxis() );
  Slot downstream_slot( "", arcFrame );

  proton.setStateToZero();
  cout << "Initial: " << proton.State() << endl;
  upstream_slot.propagate( proton );
  magnet.propagate( proton );
  downstream_slot.propagate( proton );
  cout << "Final  : " << proton.State() << endl;

  Vector state( proton.State() );
  int const i_cdt = Particle::cdtIndex;
  for( int i = 0; i < state.Dim(); i++ ) {
    if( i_cdt != i ) {
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
  }

  JetProton jetproton( myOptions.energy );
  upstream_slot.propagate( jetproton );
  magnet.propagate( jetproton );
  downstream_slot.propagate( jetproton );
  jetproton.State().printCoeffs();

  } // End TEST 2

  return ret;
}
