#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "beamline.h"
#include "EdwardsTeng.h"

char criterion( bmlnElmnt* ) { return 1; }

main( int argc, char* argv[] )  
{
 // Process command line
 if( argc != 2 ) {
  cout << "Usage: " << argv[0] << "  <partition size>" << endl;
  exit(0);
 }
 int    ipt( atoi( argv[1] ));
 double npt = ipt;
 double xpt = 1.0/npt;

 // Declare variables
 int i;
 static double zero[]   = { 0., 0., 0., 0., 0., 0. };
 static double scale[]  = { 1.0e-3, 1.0e-3, 1.0e-3, 1.0e-3, 1.0e-3, 1.0e-3 };
 // ??? REMOVE Jet::Setup( 6, 1, 6, zero, scale );
 // ??? REMOVE JetC::Setup( 6, 1, 6 );

 Jet::BeginEnvironment( 1 );
 coord x(0.0),  y(0.0),  z(0.0),
      px(0.0), py(0.0), pz(0.0);
 Jet__environment* pje = Jet::EndEnvironment( scale );
 JetC::lastEnv = JetC::CreateEnvFrom( pje );

 // ====  Lattice description  =================================

 drift*    O;
 drift*    OO;
 drift*    OOO;
 thinQuad* ED;
 thinQuad* EF;
 // srot*     RT;
 // srot*     RW;
 beamline  cell( "cell" );

 // The booster lattice is modelled from a SYNCH input
 // file obtained from Al Russell.    ( 8/22/91 )
 //
 // Magnetic fields are evidently in kG in the SYNCH file
 //
 
 sbend     Dhf ( "Dhf", 0.722403/npt,          // length      [ m ]
                     6.1727/10.,               // field       [ T ]
               xpt*( 6.1727*0.722403 / 296.5 ) // bend angle  [ rad ]
               );  
 sbend     Fhf ( "Fhf", 0.722403/npt, 
                     7.2588/10.,
               xpt*( 7.2588*0.722403 / 296.5 )
               );   

 thinQuad  QDhf( "QDhf", ( -(17.1101/10.)*0.722403 )/( npt - 1.0 ) );
 thinQuad  QFhf( "QFhf", (  (16.0761/10.)*0.722403 )/( npt - 1.0 ) );


 beamline* D = new beamline( "D" );
 beamline* F = new beamline( "F" );
 for( i = 0; i < ipt - 1; i++ ) {
  D->append( Dhf );
  D->append( QDhf );
  F->append( Fhf );
  F->append( QFhf );
 }
 D->append( Dhf );
 F->append( Fhf );
 

 O   = new drift( "O", 0.25 );
 OO  = new drift( "OO", 0.30 );
 OOO = new drift( "OOO", 1.50 );
 ED  = new thinQuad( "ED", - 0.00062639*(296.5/10.) );
 EF  = new thinQuad( "EF", - 0.00086636*(296.5/10.) );
 // RT  = new srot( "RT",   45.0*6.2831853072/360. );
 // RW  = new srot( "RW", - 45.0*6.2831853072/360. );
 
 // .............. Build the booster beamline
 cell.append( *OO  );
 cell.append( *OO  );
 cell.append( *EF  );
 cell.append( *F   );
 cell.append( *F   );
 cell.append( *F   );
 cell.append( *F   );
 cell.append( *EF  );
 cell.append( *O   );
 cell.append( *O   );
 cell.append( *ED  );
 cell.append( *D   );
 cell.append( *D   );
 cell.append( *D   );
 cell.append( *D   );
 cell.append( *ED  );
 cell.append( *OOO );
 cell.append( *OOO );
 cell.append( *OOO );
 cell.append( *OOO );
 cell.append( *ED  );
 cell.append( *D   );
 cell.append( *D   );
 cell.append( *D   );
 cell.append( *D   );
 // cell.append( *RT );    Just a test of EdwardsTeng
 cell.append( *ED  );
 // cell.append( *RW );
 cell.append( *O   );
 cell.append( *O   );
 cell.append( *EF  );
 cell.append( *F   );
 cell.append( *F   );
 cell.append( *F   );
 cell.append( *F   );
 cell.append( *EF  );
 cell.append( *OO  );
 cell.append( *OO  );

 // ============================================================

 EdwardsTeng      ET( &cell );
 bmlnElmnt*   	  lbe;
 ETtunes*     	  tunes;
 ETinfo*      	  infoPtr;

 double energy;
 energy =         PH_CNV_brho_to_p*( 296.5/10. );
 energy =         sqrt(  PH_NORM_mp*PH_NORM_mp + energy*energy );
 JetProton        p( energy );
 
 int ETresult = ET.doCalc( &p, &criterion );
 if( ETresult != 0 ) {
  cout << "Something went wrong: " << ETresult << endl;
  exit(0);
 }
 
 tunes = (ETtunes*) cell.dataHook.find( "Tunes" );
 cout << "\nTune: horizontal = " << tunes->hor 
      <<        "   vertical = " << tunes->ver 
      << endl;
 
 dlist_iterator getNext( (dlist&) cell );
 
 i = 0;
 while( lbe = (bmlnElmnt*) getNext() ) {
  cout << "Element:  " << lbe->Name() << endl;
 
  infoPtr = (ETinfo*) lbe->dataHook.find( "EdwardsTeng" );
  cout << "\n" << i++ 
       << "  --------------------------------------------" << endl;
  cout << "infoPtr->beta.hor  " << infoPtr->beta.hor  << endl;
  cout << "infoPtr->beta.ver  " << infoPtr->beta.ver  << endl;
  cout << "infoPtr->alpha.hor " << infoPtr->alpha.hor << endl;
  cout << "infoPtr->alpha.ver " << infoPtr->alpha.ver << endl;
  cout << "infoPtr->phi       " << infoPtr->phi       << endl;
  cout << "infoPtr->D \n"       << infoPtr->D         << endl;
  lbe->dataHook.eraseFirst( "EdwardsTeng" );
 }
} 
