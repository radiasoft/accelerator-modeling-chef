/*
**
** Demo program:
** 
** Simple tracking with a single FODO cell.
** Two monitors are used to write the
** "data" to files hor.dat and ver.dat.
** 
** --- Leo Michelotti
**   - October 11, 1995
**   - Original version
**   -
**   - April 24, 2005
**   - Syntax brought up to 21st century usage.
**
*/

#include <iostream>
#include <fstream>

#include <Particle.h>
#include <quadrupole.h>
#include <drift.h>
#include <monitor.h>

using namespace std;

int main( int argc, char** argv ) 
{
 if( argc != 9) {
  cout << "Usage: " << argv[0]
       << " <length of drift [m]> "
          " <focal length [m]> "
          " <energy [Gev]> "
          " x y px/p py/p n"
       << endl;
  return -1;
 }

 double energy      = atof( argv[3] );
 Proton p( energy );

 double state[6];
 state[0] = atof( argv[4] );
 state[1] = atof( argv[5] );
 state[2] = 0.0;
 state[3] = atof( argv[6] );
 state[4] = atof( argv[7] );
 state[5] = 0.0;
 p.setState( state );

 double length = atof( argv[1] );
 drift OO ( "OO", length );
 drift O  ( "O",  length/2.0 );

 double focalLength = atof( argv[2] );
 double strength = p.ReferenceBRho() / focalLength;
 thinQuad F ( "F",   strength );
 thinQuad D ( "D", - strength );


 ofstream hStr( "hor.dat" );
 hmonitor bpmH;
 bpmH.setOutputStream( hStr );
 bpmH.on();

 ofstream vStr( "ver.dat" );
 vmonitor bpmV;
 bpmV.setOutputStream( vStr );
 bpmV.on();

 beamline cell;
          cell.append( F );
          cell.append( OO );
          cell.append( D );
          cell.append( O );
          cell.append( bpmH );
          cell.append( bpmV );
          cell.append( O );

 for( int k = 0; k < atoi( argv[8] ); k++ ) {
  cell.propagate( p );
 }

 return 0;
}
