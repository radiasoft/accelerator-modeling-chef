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

#include <beamline/beamline.h>
#include <beamline/drift.h>
#include <beamline/quadrupole.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <physics_toolkit/LattFuncSage.h>
#include <physics_toolkit/TuneAdjuster.h>


using namespace std;

char cf( bmlnElmnt* pbe ) {
  if( ( strcasecmp( pbe->Type(), "thinQuad" ) ) == 0 )  return 1;
  else                                                  return 0;
}

int main( int argc, char** argv ) 
{
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


 BmlPtr cellPtr( new beamline );
 for( i = 0; i < N; i++ ) {
   cellPtr->append( F );
   cellPtr->append( O );
   cellPtr->append( D );
   cellPtr->append( O );
 }
 cellPtr->setLineMode( beamline::ring );
 
 TuneAdjuster adj( cellPtr );
 LattFuncSage lfs( cellPtr );

 // Initialize JetProtons for use
 JetParticle::createStandardEnvironments(1);

 JetProton jpr ( energy );
 JetProton jpr2( energy );
 JetProton jpr3( energy );

 ElmPtr q;
 for (   beamline::iterator bi = cellPtr->begin()
       ; bi != cellPtr->end()
       ; ++bi ) {
   q = *bi;
   if( 0 == strcmp( q->Type(), "thinQuad" )) {
     if( q->Strength() > 0.0 ) adj.addCorrector( q, 1.0, 0.0 );
     else                      adj.addCorrector( q, 0.0, 1.0 );
   }
 }

 cellPtr->dataHook.eraseAll( "Tunes" );
 lfs.TuneCalc( jpr3 );

 if( cellPtr->dataHook.begin() == cellPtr->dataHook.end() ) {
   cout << "Whoops" << endl;
   exit(1);
 }
 BarnacleList::iterator bli = cellPtr->dataHook.find( "Tunes" );

 LattFuncSage::tunes* tuneptr = &( boost::any_cast<LattFuncSage::tunes>((*bli).info) );
 cout << "\nTune: horizontal = " << tuneptr->hor
      <<        "   vertical = " << tuneptr->ver
      << endl;
 
 adj.changeTunesBy( delta_H, delta_V, jpr );

 cellPtr->dataHook.eraseAll( "Tunes" );
 lfs.TuneCalc( jpr2 );

 if( cellPtr->dataHook.begin() == cellPtr->dataHook.end() ) {
   cout << "Whoopsie" << endl;
   exit(1);
 }
 bli = cellPtr->dataHook.find( "Tunes" );

 tuneptr = &( boost::any_cast<LattFuncSage::tunes>((*bli).info) );
 cout << "\nTune: horizontal = " << tuneptr->hor
      <<        "   vertical = " << tuneptr->ver
      << endl;


 cout << "The control:\n"
      << adj.getControls()
      << endl;

 return 0;
}
