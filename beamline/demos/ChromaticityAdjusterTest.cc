/*
**
** Demo program:
** 
** A test of chromaticity adjustment.
** 
** --- Leo Michelotti
** --- January 31, 1999
**
*/

#include "beamline.h"
#include "LattFuncSage.h"
#include "ChromaticityAdjuster.h"


main( int argc, char** argv ) {

 if( argc != 8 ) {
  cout << "Usage: " << argv[0]
       << " <length of sbend [m]>"
          " <focal length of F [m]>"
          " <focal length of D [m]>"
          " <energy [Gev]>"
          " <number of cells>"
          " <horizontal chromaticity shift>"
          " <vertical chromaticity shift>"
       << endl;
  exit(0);
 }
 enum { bendLength=1, focalF, focalD, E, num, dnuH, dnuV };


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
 double bendlength = atof( argv[bendLength] );
 double bendangle  = ( M_TWOPI / ((double)N) )/2.0;
 double rho        = bendlength / bendangle;
 double bendfield  = pr.ReferenceBRho()/rho;

 cout << "The magnetic field is " << bendfield << " Tesla." << endl;

 sbend    O  ( "O", bendlength, bendfield, bendangle );
 thinQuad F  ( "F",     pr.ReferenceBRho() / atof( argv[focalF] ) );
 thinQuad D  ( "D", -   pr.ReferenceBRho() / atof( argv[focalD] ) );
 thinSextupole SF( "SF", 0.0 );
 thinSextupole SD( "SD", 0.0 );

 beamline cell;
 cell.append(  F.Clone() );
 cell.append( SF.Clone() );
 cell.append(  O.Clone() );
 cell.append(  D.Clone() );
 cell.append( SD.Clone() );
 cell.append(  O.Clone() );
 
 ChromaticityAdjuster adj ( cell );
 LattFuncSage         lfs ( cell );

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

 while((  q = bi++  )) 
 {
   if( 0 == strcmp( q->Type(), "thinSextupole" )) 
   {
     if( q->hasName( "SF" ) ) adj.addCorrector( (thinSextupole*) q, 1.0, 0.0 );
     if( q->hasName( "SD" ) ) adj.addCorrector( (thinSextupole*) q, 0.0, 1.0 );
   }
 }

 cell.dataHook.eraseAll( "Ring" );
 // lfs.TuneCalc( &jpr3 );
 lfs.Disp_Calc( &jpr3 );
 LattFuncSage::lattRing* tuneptr = (LattFuncSage::lattRing*) cell.dataHook.find( "Ring" );
 if( tuneptr == 0 ) {
   cout << "Whoops" << endl;
   exit(1);
 }
 cout << "\nTune:         horizontal  = " << ((double) N)*tuneptr->tune.hor
      << "\n              vertical    = " << ((double) N)*tuneptr->tune.ver
      << "\nChromaticity: horizontal  = " << ((double) N)*tuneptr->chromaticity.hor
      << "\n              vertical    = " << ((double) N)*tuneptr->chromaticity.ver
      << endl;
 

 adj.changeChromaticityBy( delta_H/((double) N), delta_V/((double) N), jpr );

 cell.dataHook.eraseAll( "Ring" );
 // lfs.TuneCalc( &jpr2 );
 lfs.Disp_Calc( &jpr2 );
 tuneptr = (LattFuncSage::lattRing*) cell.dataHook.find( "Ring" );
 if( tuneptr == 0 ) {
   cout << "Whoopsie" << endl;
   exit(1);
 }
 cout << "\nTune:         horizontal  = " << ((double) N)*tuneptr->tune.hor
      << "\n              vertical    = " << ((double) N)*tuneptr->tune.ver
      << "\nChromaticity: horizontal  = " << ((double) N)*tuneptr->chromaticity.hor
      << "\n              vertical    = " << ((double) N)*tuneptr->chromaticity.ver
      << endl;


 cout << "The control:\n"
      << adj.getControls()
      << endl;

 // Clean up before exiting
 cell.zap();
}
