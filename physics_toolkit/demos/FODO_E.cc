/*
**
** Demo program:
** 
** A test of the EdwardsTeng object.
** Calculates lattice functions for a hierarchical
** beamline and its flattened version.  A third
** line contains an extra cell with a rotated quad.
** 
** --- Leo Michelotti
** --- August   5, 1994
**     - Modified from old FODO.cc demo
**     October 11, 1995  
**     -  Modified slightly.
**     April   21, 2006
**     -  Brought into compliance with current
**        versions of our libraries and of g++.
**
*/


#include <iostream>

#include <mxyzptlk/mxyzptlk.h>
#include <beamline/Particle.h>
#include <beamline/quadrupole.h>
#include <beamline/drift.h>
#include <beamline/BeamlineIterator.h>

// The following is from the physics_toolkit layer.
#include <physics_toolkit/EdwardsTeng.h>

using namespace std;


// A utility function
bool cf( bmlnElmnt* pbe ) 
{ 
  return (pbe->isType("thinQuad")); 
}


int main( int argc, char** argv ) 
{
 if( argc != 7 ) {
  cout << "Usage: " << argv[0]
       << " <length of drift [m]> "
          " <focal length of F [m]> "
          " <focal length of D [m]> "
          " <angle of rolled quad (deg)> "
          " <energy [Gev]> "
          " <number of cells>"
       << endl;
  return -1;
 }
 else {
   cout << "Command line: ";
   for( int i = 0; i < argc; i++ ) {
     cout << argv[i] << "  ";
   }
   cout << endl;
 }


 // Process command line arguments to get
 // physical parameters and instantiate
 // the drift and quads that will be used.
 double energy    =  atof( argv[5] );
 Proton pr( energy );

 drift    O ( "O",   atof( argv[1] ) );
 thinQuad F ( "F",   pr.ReferenceBRho() / atof( argv[2] ) );
 thinQuad D ( "D", - pr.ReferenceBRho() / atof( argv[3] ) );

 double angle     =  atof( argv[4] );
 int    numCells  =  atoi( argv[6] );


 // Create one standard FODO cell.
 beamline cell;
          cell.append( F );
          cell.append( O );
          cell.append( D );
          cell.append( O );


 // This array will contain three beamlines
 // built up from the FODO cell.
 beamline* theLine[3];

 theLine[0] = new beamline;
 theLine[2] = new beamline;

 beamline clonedCell = cell.Clone();
 *( theLine[0] ) = numCells*clonedCell;
    theLine[1]   = (beamline*) ( theLine[0]->flatten()->Clone() );
 *( theLine[2] ) = theLine[1]->Clone();

 alignmentData roll;
 roll.tilt = angle*M_PI/180.0;
 thinQuad Q( D );
 Q.setAlignment( roll );

 theLine[2]->append( F );
 theLine[2]->append( O );
 theLine[2]->append( Q );
 theLine[2]->append( O );
 theLine[2]->append( cell );
 theLine[2]->append( F );
 theLine[2]->append( O );
 theLine[2]->append( Q );
 theLine[2]->append( O );


 // Anticipating calculations, a standard 
 // environment is created for Jet
 // objects: the void argument means
 // only first order derivatives will be
 // calculated.
 JetParticle::createStandardEnvironments();


 // Now we use an old standby, the EdwardsTeng object,
 // to calculate the lattice functions of those lines.
 bmlnElmnt*   	 lbe;
 ETtunes*     	 tunes;
 ETinfo*      	 infoPtr;
 // REMOVE: double          sl = 0.0;

 for( int n = 0; n < 3; n++ ) {
  cout << "\n\nResult for line " << n << "******************\n" << endl;
  // REMOVE: theLine[n]->peekAt( sl, pr );

  EdwardsTeng ET( theLine[n] );
  JetProton p( energy );

  // Note: lattice functions will be attached only to
  // elements that satisfy the criterion indicated by
  // the second argument.
  argc = ET.doCalc( &p, cf );    // Waste not, want not.

  if( argc == 0 ) {
   tunes = (ETtunes*) theLine[n]->dataHook.find( "Tunes" );
   cout << "\nTune: horizontal = " << tunes->hor
        <<        "   vertical = " << tunes->ver
        << endl;

   DeepBeamlineIterator dbi( theLine[n] );

   while((  lbe = dbi++  )) {
    cout << "Element:  " << lbe->Name() << endl;

    if( infoPtr = (ETinfo*) lbe->dataHook.find( "EdwardsTeng" ) ) {
      cout << "infoPtr->beta.hor  " << infoPtr->beta.hor  << endl;
      cout << "infoPtr->beta.ver  " << infoPtr->beta.ver  << endl;
      cout << "infoPtr->alpha.hor " << infoPtr->alpha.hor << endl;
      cout << "infoPtr->alpha.ver " << infoPtr->alpha.ver << endl;
      cout << "infoPtr->phi       " << infoPtr->phi       << endl;
      cout << "infoPtr->D \n"       << infoPtr->D         << endl;
      lbe->dataHook.eraseFirst( "EdwardsTeng" );
    }
    else cout << "Data not found\n" << endl;
   }
  }
  else {
    cout << "SORRY. EdwardsTeng exited with error " << argc << endl;
  }
 }

 return 0;
}
