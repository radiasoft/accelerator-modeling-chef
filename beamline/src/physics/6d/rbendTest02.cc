/*
 * 
 * File: rbendTest02.cc
 *
 * A test to see of zero maps to zero with
 * a rolled rbend.
 * 
 * sbend parameters are the same as in sbendTest.cc
 * Final rbend parameters given by Elliott McCrory.
 * 
 * Author: Leo Michelotti
 * Date:   May 5, 1997
 * 
 */

#include "beamline.rsc"
#include <stream.h>


main( int argc, char** argv ) {

  if( argc != 8 ) {
   cout << "Usage: " << argv[0]
        << " x[m]  y  cdt  x'  y'  dp/p  Energy[GeV]"
        << endl;
   exit(0);
  }

  double zero [6];
  double finalState[6];
  
  zero[0] = atof( argv[1] );
  zero[1] = atof( argv[2] );
  zero[2] = atof( argv[3] );
  zero[3] = atof( argv[4] );
  zero[4] = atof( argv[5] );
  zero[5] = atof( argv[6] );

  double energy = atof( argv[7] );
  Proton p( energy );

  drift O( 0.5 );

  double brho      = p.ReferenceBRho();  // Tesla-meters
  double bendAngle = 2.0*M_PI/774.0;     // radians
  double arcLength = 6.1214;
  double bendField = bendAngle * brho / arcLength;

  cout << "Old sbendTest parameters:\n"
          "Bending field is " << bendField << " Tesla." << endl;

  sbend B ( arcLength, bendField, bendAngle );
  // OLD: sbend B( 6.1214, 4.42351440176, 6.2831853072/774.0 );

  beamline ddd;
           ddd.append( O );
           ddd.append( B );
           ddd.append( O );

  p.setState( zero );
  cout << "Before: " << p.State() << endl;
  ddd.propagate( p );
  cout << "After:  " << p.State() << endl;

  cout << "Introducing a 180 degree roll." << endl;
  alignmentData roll;
  roll.tilt = M_PI;
  B.setAlignment( roll );

  p.setState( zero );
  cout << "Before: " << p.State() << endl;
  ddd.propagate( p );
  cout << "After:  " << p.State() << endl;


  cout << "\nAnalagous parameters for an rbend." << endl;

  double rho = brho/bendField;
  double bodyLength = 2.*rho*sin( bendAngle/2.);
  cout << "Body length is " << bodyLength << " meters." << endl;

  rbend RB ( bodyLength, bendField );

  beamline eee;
           eee.append( O );
           eee.append( RB );
           eee.append( O );

  p.setState( zero );
  cout << "Before: " << p.State() << endl;
  eee.propagate( p );
  cout << "After:  " << p.State() << endl;

  cout << "Introducing a 180 degree roll." << endl;
  RB.setAlignment( roll );

  p.setState( zero );
  cout << "Before: " << p.State() << endl;
  eee.propagate( p );
  cout << "After:  " << p.State() << endl;

  cout << "\nAnd now for Elliott's parameters." << endl;
  cout << "First an sbend." << endl;
  
  bodyLength = 1.1100134904145768644;
  bendField = 3.04800;
  rho = brho/bendField;
  bendAngle = 2.*asin( bodyLength/(2.*rho) );
  arcLength = rho*bendAngle;

  cout << "Bending angle is " << 1000.*bendAngle << " milliradians." << endl;
  cout << "Arc length is " << arcLength << " meters." << endl;

  sbend S_H8021S ( arcLength, bendField, bendAngle );

  beamline fff;
           fff.append( O );
           fff.append( S_H8021S );
           fff.append( O );

  p.setState( zero );
  cout << "Before: " << p.State() << endl;
  fff.propagate( p );
  cout << "After:  " << p.State() << endl;

  cout << "Introducing a 180 degree roll." << endl;
  S_H8021S.setAlignment( roll );

  p.setState( zero );
  cout << "Before: " << p.State() << endl;
  fff.propagate( p );
  cout << "After:  " << p.State() << endl;


  cout << "\nAnd finally the rbend." << endl;
  
  rbend R_H8021S ( bodyLength, bendField );

  beamline ggg;
           ggg.append( O );
           ggg.append( R_H8021S );
           ggg.append( O );

  p.setState( zero );
  cout << "Before: " << p.State() << endl;
  ggg.propagate( p );
  cout << "After:  " << p.State() << endl;

  cout << "Introducing a 180 degree roll." << endl;
  R_H8021S.setAlignment( roll );

  p.setState( zero );
  cout << "Before: " << p.State() << endl;
  ggg.propagate( p );
  cout << "After:  " << p.State() << endl;

}
