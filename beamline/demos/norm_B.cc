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

#include "beamline.rsc"
#include "EdwardsTeng.h"
#include "JetUtilities.h"


// Characteristic functions ---------------------------------

char cf( bmlnElmnt* pbe ) {
  return ( ( strcasecmp( pbe->Name(), "M8" ) ) == 0 );
}

char fff( const IntArray& index, const double& /* value */ ) {
 return ( ( index(2) == 0 ) && ( index(5) == 0 ) );
}

char xx2( const IntArray& index, const double& /* value */ ) {
 if (   ( index(0) == 0 ) && ( index(3) == 0 ) &&
        ( index(1) == 0 ) && ( index(4) == 0 ) &&
        ( index(2) == 0 ) && ( index(5) == 0 ) 
    ) return 1;
 if ( ( ( index(2) == 1 ) && ( index(5) == 0 ) ) &&
      ( ( index(0) == 0 ) && ( index(3) == 0 )   && 
        ( index(1) == 0 ) && ( index(4) == 0 ) ) 
    ) return 1;
 return 0;
}

char xx5( const IntArray& index, const double& /* value */ ) {
 if (   ( index(0) == 0 ) && ( index(3) == 0 ) &&
        ( index(1) == 0 ) && ( index(4) == 0 ) &&
        ( index(2) == 0 ) && ( index(5) == 0 ) 
    ) return 1;
 if ( ( ( index(2) == 0 ) && ( index(5) == 1 ) ) &&
      ( ( index(0) == 0 ) && ( index(3) == 0 )   && 
        ( index(1) == 0 ) && ( index(4) == 0 ) ) 
    ) return 1;
 return 0;
}



// Main program -----------------------------------------

main( int argc, char** argv ) 
{
 int    order;
 int    num;
 double dl;
 double fl;
 double k;
  
 // Command line processing .........
 if( argc != 6 ) {
   cout << endl;
   cout << "Usage: " << argv[0] << "  d  n  l  f  k   \n"
  	<< "where  d  = degree of highest derivative; must be >= 3  \n"
  	<< "       n  = number of cells               \n"
  	<< "       l  = drift length      [m]         \n"
  	<< "       f  = quad focal length [m]         \n"
  	<< "       k  = kick strength     [m^-3]      \n"
  	<< "                                          \n"
  	<< "I will substitute: d = 4                  \n"
  	<< "                   n = 1                  \n"
  	<< "                   l = 5                  \n"
  	<< "                   f = 7                  \n"
  	<< "                   k = 0.01               \n"
  	<< endl;
   order = 4;
   num   = 1;
   dl    = 5.0;
   fl    = 7.0;
   k     = 0.01;
 }
 else {
   cout << "Command line: " 
 	<< argv[0] << "  "
 	<< argv[1] << "  "
 	<< argv[2] << "  "
 	<< argv[3] << "  "
 	<< argv[4] << "  "
 	<< argv[5] << "\n"
 	<< endl;
  
   order = atoi( argv[1] );
   num   = atoi( argv[2] );
   dl    = atof( argv[3] );
   fl    = atof( argv[4] );
   k     = atof( argv[5] );
  
   if( order < 3 ) {
     cout << "The order must be at least 3." << endl;
     exit(1);
   }
 }

 // Create environment of the job ......
 Jet::BeginEnvironment( order );
 coord x(0.0),  y(0.0),  z(0.0),
      px(0.0), py(0.0), pz(0.0);
 Jet__environment* pje = Jet::EndEnvironment();
 JetC::lastEnv = JetC::CreateEnvFrom( pje );


 // Construct the model ring
 int i;
 double energy = 10.0;               // GeV
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


 // Precalculate expected answer  ...................
 EdwardsTeng ET( &ring );
 argc = ET.doCalc( &jp, cf );

 if( argc != 0 ) 
   cout << "*** ERROR *** ET returned " << argc << endl;
 
 ETinfo* infoPtr;
 if( infoPtr = (ETinfo*) Oct.dataHook.find( "EdwardsTeng" ) ) {
   cout << "As computed by EdwardsTeng"       << endl;
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


 // Propagate the JetProton and obtain its state .......
 jp.setState( Mapping( "ident" ) );
 ring.propagate( jp );
 jp.getState( map );   // ??? I don't like this argument.


 // Subtract the image of the origin ...
 // This is a kludge!!!
 cout << "\nWe are going to subtract ( ";
 for( i = 0; i < 5; i++ ) cout << map(i).standardPart() 
                               << ", ";
 cout << map(5).standardPart() 
      << " )\n" 
      << endl;

 // Project out the longitudinal components.  
 // This is a kludge!
 // Does it destroy symplecticity?

 Mapping projected;
 static MX_R_FUNCPTR xxx[] = { fff, fff, xx2, fff, fff, xx5 };
 projected = map.filter( xxx );


 // Use the normal form algorithm on the projected map .....
 normalForm( projected, order, &eigVec, N, T );


 Complex eta = eigVec(0,0)/abs( eigVec(0,0) );
 Complex zeta_1 = eigVec(0,0)/eta;
 Complex zeta_2 = eigVec(3,0)/eta;
 cout << "Computed horizontal beta = " 
      << 2.0*zeta_1*zeta_1
      << endl;
 cout << "Computed horizontal alpha = " 
      << - real( zeta_2 ) * real( zeta_1 ) * 2.0 
      << endl;
 

 // cout << "The original map ______________________________" << endl;
 // map.printCoeffs();

 // Output results
 int w [] = { 2, 0, 0, 1, 0, 0 };
 cout << "Calculated horizontal frequency shift coefficient: "
      << imag( N[1](0).weightedDerivative( w ) )
      << endl;

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

}
