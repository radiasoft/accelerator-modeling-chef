/*
**
** Demo.
**
** Uses normalForm utility to Calculate 
** nonresonant normal form of simple
** FODO lattice with one octupole.
**
** --- Leo Michelotti
** --- June 6, 1996
**
*/

#include "beamline.h"
#include "EdwardsTeng.h"
#include "JetUtilities.h"

using namespace std;

// Characteristic functions ---------------------------------

bool cf( bmlnElmnt* pbe ) {
  return ( ( strcasecmp( pbe->Name(), "M8" ) ) == 0 );
}

bool fff( const IntArray& index, const double& /* value */ ) {
 return ( ( index(2) == 0 ) && ( index(5) == 0 ) );
}

bool xx2( const IntArray& index, const double& /* value */ ) {
 if (   ( index(0) == 0 ) && ( index(3) == 0 ) &&
        ( index(1) == 0 ) && ( index(4) == 0 ) &&
        ( index(2) == 0 ) && ( index(5) == 0 ) 
    ) return true;
 if ( ( ( index(2) == 1 ) && ( index(5) == 0 ) ) &&
      ( ( index(0) == 0 ) && ( index(3) == 0 )   && 
        ( index(1) == 0 ) && ( index(4) == 0 ) ) 
    ) return true;
 return false;
}

bool xx5( const IntArray& index, const double& /* value */ ) {
 if (   ( index(0) == 0 ) && ( index(3) == 0 ) &&
        ( index(1) == 0 ) && ( index(4) == 0 ) &&
        ( index(2) == 0 ) && ( index(5) == 0 ) 
    ) return true;
 if ( ( ( index(2) == 0 ) && ( index(5) == 1 ) ) &&
      ( ( index(0) == 0 ) && ( index(3) == 0 )   && 
        ( index(1) == 0 ) && ( index(4) == 0 ) ) 
    ) return true;
 return false;
}


// Main program -----------------------------------------

int main( int argc, char** argv ) 
{
 // Set up the order of the calculation
 if( argc != 6 ) {
  cout << endl;
  cout << "Usage: " << argv[0] << "  d  n  l  f  k   \n"
       << "where  d  = degree of highest derivative  \n"
       << "       n  = number of cells               \n"
       << "       l  = drift length      [m]         \n"
       << "       f  = quad focal length [m]         \n"
       << "       k  = kick strength     [m^-3]      \n"
       << endl;
  exit(0);
 }


 cout << "Command line: " 
      << argv[0] << "  "
      << argv[1] << "  "
      << argv[2] << "  "
      << argv[3] << "  "
      << argv[4] << "  "
      << argv[5] << "\n"
      << endl;

 int    order = atoi( argv[1] );
 int    num   = atoi( argv[2] );
 double dl    = atof( argv[3] );
 double fl    = atof( argv[4] );
 double k     = atof( argv[5] );

 JetParticle::createStandardEnvironments( order );

 // Construct the model ring
 int i;

 double energy = 10.0;            // GeV
 JetProton jp( energy );
 double brho  = jp.ReferenceBRho();  // Tesla-meters

 drift     O ( "O",   dl/2.0 );
 thinQuad  F ( "F",   brho / fl );
 thinQuad  D ( "D", - brho / fl );
 thinOctupole Oct( "M8", k*brho );

 Mapping map;
 MatrixC eigVec( 6, 6 );
 CLieOperator* N = new CLieOperator[ order ];
 CLieOperator* T = new CLieOperator[ order ];

 beamline cell;
 cell.append( O );
 cell.append( D );
 cell.append( O );
 cell.append( O );
 cell.append( F );
 cell.append( O );

 beamline ring;
 ring.append( Oct );

 for( i = 0; i < num; i++ ) {
  ring.append( cell );
 }


 /* DGN */ // Some tracking...
 /* DGN */ cout << "DGN> Gonna do some tracking now ..." << endl;
 /* DGN */ Proton p( energy );
 /* DGN */ static double zero[] = { 0., 0., 0., 0., 0., 0. };
 /* DGN */ p.setState( zero );
 /* DGN */ for( i = 0; i < 100; i++ ) {
 /* DGN */   ring.propagate( p );
 /* DGN */   for( int j = 0; j < 6; j++ ) cout << p.State(j) << ",  ";
 /* DGN */   cout << endl;
 /* DGN */ }

 // Precalculate expected answer
 EdwardsTeng ET( &ring );
 argc = ET.doCalc( &jp, cf );

 if( argc != 0 ) {
   cout << "*** ERROR *** ET returned " << argc << endl;
   return -1;
 }
 
 ETinfo* infoPtr;
 if( infoPtr = (ETinfo*) Oct.dataHook.find( "EdwardsTeng" ) ) {
   cout << "beta.hor  " << infoPtr->beta.hor  << endl;
   cout << "beta.ver  " << infoPtr->beta.ver  << endl;
   cout << "alpha.hor " << infoPtr->alpha.hor << endl;
   cout << "alpha.ver " << infoPtr->alpha.ver << endl;
   cout << "phi       " << infoPtr->phi       << endl;
   cout << "D \n"       << infoPtr->D         << endl;

   cout << "Predicted horizontal frequency shift coefficient: "
 	<< 3.0*k*( infoPtr->beta.hor )*( infoPtr->beta.hor ) / 4.0
 	<< endl;
   cout << "Predicted vertical   frequency shift coefficient: "
 	<< 3.0*k*( infoPtr->beta.ver )*( infoPtr->beta.ver ) / 4.0
 	<< endl;
 }
 else cout << "Data not found\n" << endl;

 // Get the normal form of the map
 jp.setState( Mapping( "ident" ) );
 ring.propagate( jp );
 jp.getState( map );   // ??? I don't like this argument.

 // Subtract the image of the origin ...
 // This is a kludge!
 cout << "\nWe are going to subtract ( ";
 for( i = 0; i < 5; i++ ) cout << map(i).standardPart() 
                               << ", ";
 cout << map(5).standardPart() 
      << " )\n" 
      << endl;

 /* DGN */ map.printCoeffs();
 for( i = 0; i < 6; i++ ) map(i) -= map(i).standardPart();
 /* DGN */ map.printCoeffs();


 // Project out the longitudinal components.  
 // This is a kludge!
 // Does it destroy symplecticity?

 Mapping projected;
 /* DGN */ cout << "DGN> Hang on, everybody. Here we goooo...!" << endl;

 static bool (*xxx[]) ( const IntArray&, const double& ) = { fff, fff, xx2, fff, fff, xx5 };

 projected = map.filter( xxx );

 /* DGN */ cout << "DGN> Filtered map:" << endl;
 /* DGN */ projected.printCoeffs();

 /* DGN */ cout << "DGN> Its eigenvalues" << endl;
 /* DGN */ cout << projected.Jacobian().eigenValues();

 /* DGN */ cout << "DGN> The original map ______________________________" << endl;
 /* DGN */ map.printCoeffs();

 normalForm( projected, order, &eigVec, N, T );

 { /* DGN */
 cout << "DGN> After normalForm: the eigenvectors are \n"
      << eigVec
      << endl;
 
 std::complex<double> eta = eigVec(0,0)/abs( eigVec(0,0) );
 std::complex<double> zeta_1 = eigVec(0,0)/eta;
 std::complex<double> zeta_2 = eigVec(3,0)/eta;
 cout << "DGN> zeta_1 = "
      << zeta_1
      << endl;
 cout << "DGN> zeta_2 = "
      << zeta_2
      << endl;
 cout << "DGN> Computed beta = " 
      << 2.0*zeta_1*zeta_1
      << endl;
 cout << "DGN> Computed alpha = " 
      << - real( zeta_2 ) * real( zeta_1 ) * 2.0 
      << endl;
 
 /* DGN */ }

 cout << "The original map ______________________________" << endl;
 map.printCoeffs();

 // Output results
 cout << "The normal Form _______________________________" << endl;
 for( i = 2; i <= order; i++ ) {
  cout << "Terms of degree " << i << "_ _ _ _ _ _ _ _ _ _ _ _ _ _" << endl;
  N[i-2].printCoeffs();
 }
 cout << "The transformation ____________________________" << endl;
 for( i = 2; i <= order; i++ ) {
  cout << "Terms of degree " << i << " _ _ _ _ _ _ _ _ _ _ _ _ _ _" << endl;
  T[i-2].printCoeffs();
 }

 return 0;
}
