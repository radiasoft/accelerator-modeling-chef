/*
**
** Demo program:
** 
** This is similar to FODO_E.cc
** 
** --- Leo Michelotti
**     February 10, 1999
**
*/

#include "beamline.h"
#include "mxyzptlk.h"
#include "EdwardsTeng.h"
#include "MatrixCalcVisitor.h"

using namespace std;

// char cf( bmlnElmnt* pbe ) {
//   if( ( strcasecmp( pbe->Type(), "thinQuad" ) ) == 0 )  return 1;
//   else                                                  return 0;
// }

bool cf( bmlnElmnt* ) {
  return true;
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
  exit(0);
 }

 JetParticle::createStandardEnvironments( 1 );

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
  // theLine[n]->peekAt( sl, &pr );

  EdwardsTeng ET( theLine[n] );
  JetProton p( energy );
  argc = ET.doCalc( &p, cf );

  if( argc == 0 ) {
   tunes = (ETtunes*) theLine[n]->dataHook.find( "Tunes" );
   cout << "\nTune: horizontal = " << tunes->hor
        <<        "   vertical = " << tunes->ver
        << endl;

   DeepBeamlineIterator getNext( theLine[n] );

   while( lbe = getNext++ ) {
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


  // Same calculation done using MatrixCalcVisitor

  cout << "\n\n----------------------------------------\n"
          "Begin calculation done using MatrixCalcVisitor\n"
       << endl;

  MatrixCalcVisitor zlorfik( pr );
  theLine[n]->accept( zlorfik );
 
  for( int w = 0; w < zlorfik.NumberOfElements(); w++ ) {
    cout << zlorfik.Beta_h()[w] << "  "
         << zlorfik.Beta_v()[w] << "  "
         << zlorfik.Alpha_h()[w] << "  "
         << zlorfik.Alpha_v()[w] << "  "
         << endl;
  }


  cout << "\n\nEnd calculation done using MatrixCalcVisitor\n"
          "----------------------------------------\n"
       << endl;
 }

 return 0;
}
