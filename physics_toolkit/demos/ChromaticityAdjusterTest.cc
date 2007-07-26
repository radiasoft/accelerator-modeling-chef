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

#include <beamline/beamline.h>
#include <beamline/sbend.h>
#include <beamline/quadrupole.h>
#include <beamline/sextupole.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/RefRegVisitor.h>
#include <beamline/beamline_iterators.h>
#include <physics_toolkit/LattFuncSage.h>
#include <physics_toolkit/ChromaticityAdjuster.h>

using namespace std;

int main( int argc, char** argv ) 
{
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
          cell.append(  F );    // NOTE: Current behavior is that
          cell.append( SF );    // the .append function will clone
          cell.append(  O );    // these elements automatically.
          cell.append(  D );
          cell.append( SD );
          cell.append(  O );
          cell.setLineMode( beamline::ring );
 
 // Regrettably, this step must be done.
 // -----------------------------------
 RefRegVisitor registrar(pr);
 cell.accept(registrar);

 ChromaticityAdjuster adj ( cell );
 LattFuncSage         lfs ( cell );

 // Initialize JetProtons for use
 JetParticle::createStandardEnvironments( 1 );

 JetProton jpr( energy );
 JetProton jpr2( energy );
 JetProton jpr3( energy );

 ElmPtr q;
 for (   beamline::iterator bi = cell.begin()
       ; bi != cell.end()
       ; ++bi ) {
   q = *bi;
   if( 0 == strcmp( q->Type(), "thinSextupole" )) 
   {
     if( std::string("SF") == q->Name() ) { adj.addCorrector( q, 1.0, 0.0 ); }
     if( std::string("SD") == q->Name() ) { adj.addCorrector( q, 0.0, 1.0 ); }
   }
 }

 cell.dataHook.eraseAll( "Ring" );
 lfs.Disp_Calc( jpr3 );

 LattFuncSage::lattRing* tuneptr = 0;
 BarnacleList::iterator bli = cell.dataHook.find( "Ring" );
 if( cell.dataHook.end() == bli ) {
   cout << "Whoops" << endl;
   exit(1);
 }
 tuneptr = boost::any_cast<LattFuncSage::lattRing*>((*bli).info);
 cout << "\nTune:         horizontal  = " << ((double) N) * tuneptr->tune.hor
      << "\n              vertical    = " << ((double) N) * tuneptr->tune.ver
      << "\nChromaticity: horizontal  = " << ((double) N) * tuneptr->chromaticity.hor
      << "\n              vertical    = " << ((double) N) * tuneptr->chromaticity.ver
      << endl;

 adj.changeChromaticityBy( delta_H/((double) N), delta_V/((double) N), jpr );

 cell.dataHook.eraseAll( "Ring" );
 lfs.Disp_Calc( jpr2 );

 tuneptr = 0;
 bli = cell.dataHook.find( "Ring" );
 if( cell.dataHook.end() == bli ) {
   cout << "Whoopsie" << endl;
   exit(1);
 }
 tuneptr = boost::any_cast<LattFuncSage::lattRing*>((*bli).info);
 cout << "\nTune:         horizontal  = " << ((double) N)*tuneptr->tune.hor
      << "\n              vertical    = " << ((double) N)*tuneptr->tune.ver
      << "\nChromaticity: horizontal  = " << ((double) N)*tuneptr->chromaticity.hor
      << "\n              vertical    = " << ((double) N)*tuneptr->chromaticity.ver
      << endl;

 cout << "The control:\n"
      << adj.getControls()
      << endl;

 return 0;
}
