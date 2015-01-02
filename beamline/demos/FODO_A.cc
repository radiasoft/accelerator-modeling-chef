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

#include <beamline/beamline.h>
#include <beamline/quadrupole.h>
#include <beamline/drift.h>
#include <beamline/monitor.h>
#include <beamline/Particle.h>

using namespace std;

int main( int argc, char** argv ) 
{
 if( argc != 9 ) {
  cout << "Usage: " << argv[0]
       << " <length of drift [m]> "
          " <focal length [m]> "
          " <energy [Gev]> "
          " x[m] y[m] px/p py/p "
          " n"
       << endl;
  return -1;
 }

 // Instantiate a proton in the specified state.
 // 
 double energy = atof( argv[3] );

 Proton p( energy );
 p.set_x  ( atof( argv[4] ) );
 p.set_y  ( atof( argv[5] ) );
 p.set_npx( atof( argv[6] ) );
 p.set_npy( atof( argv[7] ) );


 // Instantiate the basic drift and quad elements.
 // 
 double length = atof( argv[1] );
 drift OO ( "OO", length );
 drift O  ( "O",  length/2.0 );

 double focalLength = atof( argv[2] );
 double strength = p.ReferenceBRho() / focalLength;
 thinQuad F ( "F",   strength );
 thinQuad D ( "D", - strength );


 // Instantiate monitors within smart pointers
 // for later readout.
 // 
 ofstream hStr( "hor.dat" );
 HMonitorPtr bpmH( new hmonitor( "HORIZONTAL" ) );

 ofstream vStr( "ver.dat" );
 VMonitorPtr bpmV( new vmonitor( "VERTICAL" ) );


 // Create a standard FODO cell
 // (without bends)
 // 
 beamline cell;
          cell.append( F );
          cell.append( OO );
          cell.append( D );
          cell.append( O );
          cell.append( bpmH );
          cell.append( bpmV );
          cell.append( O );


 // Propagate the proton the specified
 // number of times.
 // 
 for( int k = 0; k < atoi( argv[8] ); k++ ) {
   cell.propagate( p );
   hStr << bpmH->hposition() << endl;
   vStr << bpmV->vposition() << endl;
   cout << p.State() << endl;
 }

 hStr.close();
 vStr.close();

 return 0;
}
