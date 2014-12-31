/*
**
** Demo program:
** 
** A test of tune adjustment.
** 
** --- Leo Michelotti
**     January  14, 1999  (original)
**     February 16, 2008  (current)
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
    return -1;
  }
  enum { driftLength=1, focalF, focalD, E, num, dnuH, dnuV };


  double delta_H = atof( argv[dnuH] );
  double delta_V = atof( argv[dnuV] );

  double initialTune_H = -1.0;
  double initialTune_V = -1.0;
  double   finalTune_H = -2.0;
  double   finalTune_V = -2.0;



  double energy    =  atof( argv[E] );
  Proton pr( energy );

  int N = atoi( argv[num] );
  if( N <= 0 ) {
    cout <<   "*** ERROR *** "
            "\n*** ERROR *** : " << __FILE__ << ", line " << __LINE__
         << "\n*** ERROR *** : N = " << N << "; this should be positive."
            "\n*** ERROR *** "
         << endl;
    return 1;
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
    cout <<   "*** ERROR *** "
            "\n*** ERROR *** : " << __FILE__ << ", line " << __LINE__
         << "\n*** ERROR *** : First tune calculation failed. (LattFuncSage::TuneCalc)"
            "\n*** ERROR *** "
         << endl;
    return 2;
  }
  BarnacleList::iterator bli = cellPtr->dataHook.find( "Tunes" );

  LattFuncSage::tunes the_tunes( boost::any_cast<LattFuncSage::tunes>((*bli).info) );
  cout << "\nTune: horizontal = " << ( initialTune_H = the_tunes.hor )
       <<        "   vertical = " << ( initialTune_V = the_tunes.ver )
       << endl;
  

  adj.changeTunesBy( delta_H, delta_V, jpr );


  cellPtr->dataHook.eraseAll( "Tunes" );
  lfs.TuneCalc( jpr2 );

  if( cellPtr->dataHook.begin() == cellPtr->dataHook.end() ) {
    cout <<   "*** ERROR *** "
            "\n*** ERROR *** : " << __FILE__ << ", line " << __LINE__
         << "\n*** ERROR *** : Second tune calculation failed. (LattFuncSage::TuneCalc)"
            "\n*** ERROR *** "
         << endl;
    return 3;
  }
  bli = cellPtr->dataHook.find( "Tunes" );

  LattFuncSage::tunes the_tunes2( boost::any_cast<LattFuncSage::tunes>((*bli).info) );
  cout << "\nTune: horizontal = " << ( finalTune_H = the_tunes2.hor )
       <<        "   vertical = " << ( finalTune_V = the_tunes2.ver )
       << endl;


  if(    ( 0.0001 < abs(initialTune_H + delta_H - finalTune_H) )
      || ( 0.0001 < abs(initialTune_V + delta_V - finalTune_V) ) ) 
  {
    cout <<   "*** ERROR *** "
            "\n*** ERROR *** : " << __FILE__ << ", line " << __LINE__
         << "\n*** ERROR *** : Tune adjustment failed."
            "\n*** ERROR *** : Requested: ( " << delta_H << ", " << delta_V << "  )"
            "\n*** ERROR *** : Achieved:  ( " << ( finalTune_H - initialTune_H )
         <<                                                  ", " 
                                              << ( finalTune_V - initialTune_V )
         <<                                                                    "  )"
            "\n*** ERROR *** "
         << endl;
    return 4;
  }

  cout << "The control:\n"
       << adj.getControls()
       << endl;

  return 0;
}
