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
**     October 11, 1995  (modified)
**
*/

#include "beamline.h"
#include "mxyzptlk.h"
#include "EdwardsTeng.h"

char cf( bmlnElmnt* pbe ) {
  if( ( strcasecmp( pbe->Type(), "thinQuad" ) ) == 0 )  return 1;
  else                                                  return 0;
}

main( int argc, char** argv ) {

 if( argc != 7 ) {
  cout << "Usage: " << argv[0]
       << " <length of drift [m]> "
          " <focal length of F [m]> "
          " <focal length of D [m]> "
          " <angle of rolled quad (deg)> "
          " <energy [Gev]> "
          " <number of cells>"
       << endl;
  exit(0);
 }

 Jet::BeginEnvironment( 1 );
 coord x(0.0),  y(0.0),  z(0.0),
      px(0.0), py(0.0), pz(0.0);
 Jet::EndEnvironment();

 double energy    =  atof( argv[5] );
 Proton pr( energy );

 drift    O ( "O",   atof( argv[1] ) );
 thinQuad F ( "F",   pr.ReferenceBRho() / atof( argv[2] ) );
 thinQuad D ( "D", - pr.ReferenceBRho() / atof( argv[3] ) );

 double angle     =  atof( argv[4] );
 int    numCells  =  atoi( argv[6] );

 beamline cell;
          cell.append( F );
          cell.append( O );
          cell.append( D );
          cell.append( O );

 beamline* theLine[3];

 theLine[0] = new beamline;
 theLine[2] = new beamline;

 *( theLine[0] ) = numCells*cell;
    theLine[1]   = (beamline*) ( theLine[0]->flatten()->Clone() );
 *( theLine[2] ) = *( theLine[1] );

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

 bmlnElmnt*   	 lbe;
 ETtunes*     	 tunes;
 ETinfo*      	 infoPtr;
 double          sl = 0.0;

 for( int n = 0; n < 3; n++ ) {
  cout << "Result for line " << n << "******************\n" << endl;
  theLine[n]->peekAt( sl, &pr );

  EdwardsTeng ET( theLine[n] );
  JetProton p( energy );
  argc = ET.doCalc( &p, cf );

  if( argc == 0 ) {
   tunes = (ETtunes*) theLine[n]->dataHook.find( "Tunes" );
   cout << "\nTune: horizontal = " << tunes->hor
        <<        "   vertical = " << tunes->ver
        << endl;

   dlist_iterator getNext( *(dlist*) theLine[n] );

   while( lbe = (bmlnElmnt*) getNext() ) {
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

 }
}
