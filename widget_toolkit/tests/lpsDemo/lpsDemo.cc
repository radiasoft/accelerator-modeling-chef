////////////////////////////////////////////////////////////
// 
// File:          lpsDemo.cc
//                                                                
// -------------------------------------------------------------
// 
// Copyright Universities Research Association, Inc./ Fermilab    
//           All Rights Reserved                             
//
// Usage, modification, and redistribution are subject to terms          
// of the License supplied with this software.
// 
// Software and documentation created under 
// U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
// The U.S. Government retains a world-wide non-exclusive, 
// royalty-free license to publish or reproduce documentation 
// and software for U.S. Government purposes. This software 
// is protected under the U.S. and Foreign Copyright Laws.
// 
// -------------------------------------------------------------
// 
// Author:        Leo Michelotti
// 
// REVISION HISTORY
// 
// Wednesday. October 24, 2001.  (original version)
// * Written as demonstration of perturbation theory in
//   longitudinal phase space.
// 
// Thursday.  May 29, 2008.      (this file)
// 
////////////////////////////////////////////////////////////
// 
// Test and demonstration of two-dimensional normal form calculations
// applied to a simple RF transfer map.
// 
// The original form of this program was written in October, 2001 to
// demonstrate how to populate form invariant distributions in
// longitudinal phase space using normal form coordinates.  At that time
// it was called macdemo.cc, mostly because it was written as a
// demonstration for Jim Maclachlan.
// 
////////////////////////////////////////////////////////////
// 
// ------------
// COMMAND LINE
// ------------
// lpsDemo [options]
// 
// -------
// OPTIONS
// -------
// Note: N represents an integer
//       D            a  double
// 
// -ord  N    order in perturbation theory to which
//            normal form coordinates are calculated.
//            (Can be changed at runtime.)
//            : default = 1
//            : accepted values = [ 1 3 5 7 ]
// 
// -max  N    maximum number of iterates retained within
//            an orbit.
//            : default = 1024
// 
// -mul  D    the RF synchronous phase advance is set to
//            D x pi/2. 
//            : default = 0.3
// 
////////////////////////////////////////////////////////////

#include <string>
#include <qapplication.h>

#include <Orbit.h>
#include <RFMap.h>
#include <Tracker.h>

#define I_PHI 0
#define I_E   1
#define DIM   2

using namespace std;
using namespace AESOP_domain;

struct Options
{
  int order;
  double multiplier;
  int maxInOrbit;

  Options( int, char**, int );
};

Options::Options( int argc, char** argv, int lastargs )
:   order(1)
  , multiplier(0.3)
  , maxInOrbit(1024)
{
  int limit = argc-lastargs;
  string s;
  int i = 1;
  while( i < limit ) {
    s.assign( argv[i++] );
    if( '-' == s[0] ) {
      s.assign( s.substr(1) );
      if( s == "ord" ) {
        if( i < limit ) { 
          order = atoi(argv[i++]); 
          if( order < 0 )  { order = - order; }
          if( 0 == order ) { order = 1;       }
        }
      }
      else if( s == "mul" ) {
        if( i < limit ) { 
          multiplier = atof(argv[i++]); 
          if( multiplier < 0 )  { multiplier = - multiplier; }
          if( 0 == multiplier ) { multiplier = 0.3;          }
        }
      }
      else if( s == "max" ) {
        if( i < limit ) { 
          int j = atoi(argv[i++]);
          maxInOrbit = ( 0 <= j ? j : -j );
        }
      }
      else {
        cerr << "\n*** ERROR *** Unrecognized option: " << s << endl;
      }
    }
    else {
      cerr << "\n*** ERROR *** Unable to interpret command line argument: " << s << endl;
    }
  }
}


int main( int argc, char** argv )
{
  cout << "Command line: ";
  for( int i = 0; i < argc; i++ ) { cout << argv[i] << "  "; }
  cout << endl;

  Options myOptions( argc, argv, 0 );

  RFMap rf;
  rf.setMultiplier( myOptions.multiplier );

  Orbit::set_maxNumberOfOrbitsDefault( myOptions.maxInOrbit );

  argc = 1;
  QApplication appl( argc, argv );

  Tracker trackWidget( &rf, myOptions.order );
  appl.setMainWidget( &trackWidget );
  trackWidget.show();
  return appl.exec();
}
