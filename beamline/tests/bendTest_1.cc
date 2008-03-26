////////////////////////////////////////////////////////////
// 
// File:          bendTest_1.cc
// Author:        Leo Michelotti
// 
// Revision date: Tuesday. March 25, 2008.  (original version)
// 
////////////////////////////////////////////////////////////
// 
// Constructs rbend and sbend and propagates a proton through them.
// 
// ------------
// COMMAND LINE
// ------------
// bendTest_1 [options]
// 
// -------
// OPTIONS
// -------
// Note: NNN represents an integer
// 
// -print     prints parameters
// -N  NNN    the bend angle is set to 2 pi / NNN
//            : default = 128
// 
////////////////////////////////////////////////////////////

#include <iostream>

#include <beamline/Particle.h>
#include <beamline/sbend.h>
#include <beamline/rbend.h>


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

  Options( int, char**, int=0 );
};


Options::Options( int argc, char** argv, int lastargs )
:   valid(true)
  , pc(8)
  , bodyLength(2)
  , n(128)
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
          n = atof( argv[i++] );
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
    cout << "Command line: ";
    for( int i = 0; i < argc; i++ ) {
      cout << argv[i] << "  ";
    }
    cout << '\n' << endl;
  }
  else {
    return 1;
  }

  Proton proton( myOptions.energy );

  { // TEST 1: propagation through rbend
  cout << "RBEND TEST" << endl;
  proton.setStateToZero();
  double px = sin(myOptions.bendAngle/2.0);
  proton.set_npx( px );
  cout << "Initial: " << proton.State() << endl;
  rbend magnet( "", myOptions.bodyLength, myOptions.bendField, myOptions.bendAngle );
  magnet.propagate( proton );
  cout << "Final  : " << proton.State() << endl;

  if( 1.0e-14 < std::abs( proton.get_npx() + px ) ) {
    cout <<   "*** FAILED *** "
            "\n*** FAILED *** : " << __FILE__ << ", line " << __LINE__
         << "\n*** FAILED *** : Failed rbend test: 1.0e-14 < "
         << std::abs( proton.get_npx() + px )
         << "\n*** FAILED *** : Proton does not bend through correct angle."
         << "\n*** FAILED *** "
         << endl;
    ret = 2;
  }
  } // End TEST 1

  { // TEST 2: propagation through sbend
  cout << "SBEND TEST" << endl;
  proton.setStateToZero();
  cout << "Initial: " << proton.State() << endl;
  sbend magnet( "", myOptions.arcLength, myOptions.bendField, myOptions.bendAngle );
  magnet.propagate( proton );
  cout << "Final  : " << proton.State() << endl;

  if( 1.0e-14 < std::abs( proton.get_npx() ) ) {
    cout <<   "*** FAILED *** "
            "\n*** FAILED *** : " << __FILE__ << ", line " << __LINE__
         << "\n*** FAILED *** : Failed sbend test: 1.0e-14 < "
         << std::abs( proton.get_npx() )
         << "\n*** FAILED *** : Proton does not bend through correct angle."
         << "\n*** FAILED *** "
         << endl;
    ret = 3;
  }
  } // End TEST 2

  return ret;
}
