#include <iostream>

#include <beamline.h>
#include <EdwardsTeng.h>

bool criterion( bmlnElmnt* )
{ return true; }

using namespace std;

int main( int argc, char* argv[] )
{
 // Process command line
 if( argc != 2 ) {
  cout << "Usage: " << argv[0] << "  <partition size>" << endl;
  exit(0);
 }
 int    ipt( atoi( argv[1] ));
 double npt = ipt;
 double xpt = 1.0/npt;


 // Declare a standard Jet environment
 JetParticle::createStandardEnvironments( 1 );


 // ====  Lattice description  =================================
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


 beamline D = beamline( "D" );
 beamline F = beamline( "F" );
 for( int i = 0; i < ipt - 1; i++ ) {
  D.append( Dhf );
  D.append( QDhf );
  F.append( Fhf );
  F.append( QFhf );
 }
 D.append( Dhf );
 F.append( Fhf );
 

 drift    O   ( "O", 0.25 );
 drift    OO  ( "OO", 0.30 );
 drift    OOO ( "OOO", 1.50 );
 thinQuad ED  ( "ED", - 0.00062639*(296.5/10.) );
 thinQuad EF  ( "EF", - 0.00086636*(296.5/10.) );
 
 // .............. Build the booster beamline
 beamline  cell( "cell" );
 	   cell.append( OO  );
 	   cell.append( OO  );
 	   cell.append( EF  );
 	   cell.append( F   );
 	   cell.append( F   );
 	   cell.append( F   );
 	   cell.append( F   );
 	   cell.append( EF  );
 	   cell.append( O   );
 	   cell.append( O   );
 	   cell.append( ED  );
 	   cell.append( D   );
 	   cell.append( D   );
 	   cell.append( D   );
 	   cell.append( D   );
 	   cell.append( ED  );
 	   cell.append( OOO );
 	   cell.append( OOO );
 	   cell.append( OOO );
 	   cell.append( OOO );
 	   cell.append( ED  );
 	   cell.append( D   );
 	   cell.append( D   );
 	   cell.append( D   );
 	   cell.append( D   );
 	   cell.append( ED  );
 	   cell.append( O   );
 	   cell.append( O   );
 	   cell.append( EF  );
 	   cell.append( F   );
 	   cell.append( F   );
 	   cell.append( F   );
 	   cell.append( F   );
 	   cell.append( EF  );
 	   cell.append( OO  );
 	   cell.append( OO  );

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
  return -1;
 }
 
 tunes = (ETtunes*) cell.dataHook.find( "Tunes" );
 cout << "\nTune: horizontal = " << tunes->hor 
      <<        "   vertical = " << tunes->ver 
      << endl;
 
 BeamlineIterator bi( cell );
 int i = 0;
 while( lbe = bi++ ) {
  cout << "Element:  " << lbe->Name() << endl;
  cout << "\n" << i++ 
       << "  --------------------------------------------" << endl;

  infoPtr = (ETinfo*) lbe->dataHook.find( "EdwardsTeng" );
  if( 0 != infoPtr ) {
    cout << "infoPtr->beta.hor  " << infoPtr->beta.hor  << endl;
    cout << "infoPtr->beta.ver  " << infoPtr->beta.ver  << endl;
    cout << "infoPtr->alpha.hor " << infoPtr->alpha.hor << endl;
    cout << "infoPtr->alpha.ver " << infoPtr->alpha.ver << endl;
    cout << "infoPtr->phi       " << infoPtr->phi       << endl;
    cout << "infoPtr->D \n"       << infoPtr->D         << endl;
    lbe->dataHook.eraseFirst( "EdwardsTeng" );
  }
  else {
    cout << "*** DATA NOT FOUND ***" << endl;
  }
 }

} 
