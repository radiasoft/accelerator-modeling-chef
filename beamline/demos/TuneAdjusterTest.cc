/*
**
** Demo program:
** 
** A test of tune adjustment.
** 
** --- Leo Michelotti
** --- January 14, 1999
**
*/

#include "beamline.h"
#include "LattFuncSage.h"
#include "TuneAdjuster.h"
#include "EdwardsTeng.h"    // for ETtunes


char cf( bmlnElmnt* pbe ) {
  if( ( strcasecmp( pbe->Type(), "thinQuad" ) ) == 0 )  return 1;
  else                                                  return 0;
}

main( int argc, char** argv ) {

 if( argc != 8 ) {
  cout << "Usage: " << argv[0]
       << " <length of drift [m]>"
          " <focal length of F [m]>"
          " <focal length of D [m]>"
          " <energy [Gev]>"
          " <number of cells>"
          " <horizontal tune shift>"
          " <vertical tune shift>"
       << endl;
  exit(0);
 }
 enum { driftLength=1, focalF, focalD, E, num, dnuH, dnuV };


 double delta_H = atof( argv[dnuH] );
 double delta_V = atof( argv[dnuV] );

 double energy    =  atof( argv[E] );
 Proton pr( energy );

 int N = atoi( argv[num] );
 if( N <= 0 ) {
   cout << "N = " << N << "; this should be positive." << endl;
   exit(0);
 }
 int i, j;


 // Construct a FODO cell
 drift    O ( "O",   atof( argv[driftLength] ) );
 thinQuad F ( "F",   pr.ReferenceBRho() / atof( argv[focalF] ) );
 thinQuad D ( "D", - pr.ReferenceBRho() / atof( argv[focalD] ) );


 beamline cell;
 for( i = 0; i < N; i++ ) {
   cell.append( F.Clone() );
   cell.append( O.Clone() );
   cell.append( D.Clone() );
   cell.append( O.Clone() );
 }
 
 TuneAdjuster adj( cell );
 LattFuncSage lfs( &cell );

 // Initialize JetProtons for use
 Jet::BeginEnvironment( 1 );
 coord x(0.0),  y(0.0),  z(0.0),
      px(0.0), py(0.0), pz(0.0);
 Jet::EndEnvironment();

 JetProton jpr( energy );
 JetProton jpr2( energy );
 JetProton jpr3( energy );

 BeamlineIterator bi( cell );
 bmlnElmnt* q;
 while((  q = bi++  )) {
   if( 0 == strcmp( q->Type(), "thinQuad" )) {
     if( q->Strength() > 0.0 ) adj.addCorrector( (thinQuad*) q, 1.0, 0.0 );
     else                      adj.addCorrector( (thinQuad*) q, 0.0, 1.0 );
   }
 }

 cell.dataHook.eraseAll( "Tunes" );
 lfs.TuneCalc( &jpr3 );
 ETtunes* tuneptr = (ETtunes*) cell.dataHook.find( "Tunes" );
 if( tuneptr == 0 ) {
   cout << "Whoops" << endl;
   exit(1);
 }
 cout << "\nTune: horizontal = " << tuneptr->hor
      <<        "   vertical = " << tuneptr->ver
      << endl;
 
 adj.changeTunesBy( delta_H, delta_V, jpr );

 cell.dataHook.eraseAll( "Tunes" );
 lfs.TuneCalc( &jpr2 );
 tuneptr = (ETtunes*) cell.dataHook.find( "Tunes" );
 if( tuneptr == 0 ) {
   cout << "Whoops" << endl;
   exit(1);
 }
 cout << "\nTune: horizontal = " << tuneptr->hor
      <<        "   vertical = " << tuneptr->ver
      << endl;


 // Clean up before exiting
 cell.zap();
}
