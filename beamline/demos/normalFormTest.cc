/*
**
** Demo.
**
** Uses normalForm utility to Calculate 
** nonresonant normal form of simple
** lattice with sextupoles.
**
** --- Leo Michelotti
** --- October 11, 1993
**
*/

#include "beamline.rsc"
#include "JetUtilities.h"

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

// ==============================================================

main( int argc, char** argv ) {

 // Set up the order of the calculation
 if( argc != 3 ) {
  cout << endl;
  cout << "Usage: " << argv[0] << "d  n                           \n"
       << "where  d  = degree of highest derivative  \n"
       << "       n  = number of cells               \n"
       << endl;
  exit(0);
 }
 int order   = atoi( argv[1] );
 double num  = atof( argv[2] );

 Jet::BeginEnvironment( order );
 coord x(0.0),  y(0.0),  z(0.0),
      px(0.0), py(0.0), pz(0.0);
 Jet__environment* pje = Jet::EndEnvironment();
 JetC::lastEnv = JetC::CreateEnvFrom( pje );

 // Construct the model ring
 int i;

 double energy = 10.0;            // GeV
 JetProton jp( energy );
 double brho      = jp.ReferenceBRho();  // Tesla-meters
 double bendAngle = 2.0*M_PI/num;   // radians
 double arcLength = 1.0;            // meters
 double bendField = bendAngle * brho / arcLength;

 sbend     B ( "B", arcLength, bendField, bendAngle );
 drift     O ( "O",   5.0 );
 thinQuad  F ( "F",   brho / 7.0 );
 thinQuad  D ( "D", - brho / 7.0 );
 thinSextupole S(  "M6", 0.00*brho );
 thinOctupole Oct( "M8", 0.00*brho );
 // srot      r1(   0.1 );
 // srot      r2( - 0.1 );

 Mapping            map;
 MatrixC eigVec( 6, 6 );
 CLieOperator* N = new CLieOperator[ order ];
 CLieOperator* T = new CLieOperator[ order ];

 beamline cell;
 cell.append( O );
 cell.append( D );
 cell.append( O );
 cell.append( B );
 cell.append( O );
 cell.append( F );
 cell.append( O );

 beamline ring;
 ring.append( S );
 // ring.append( r1 );
 ring.append( Oct );
 // ring.append( r2 );
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

 // Get the normal form of the map
 ring.propagate( jp );
 jp.getState( map );   // ??? I don't like this argument.

 // beamline* flatBeamline = ring.flatten();
 // lattFunc W;
 // W =  flatBeamline->twiss();

 // Subtract the image of the origin ...
 // This is a kludge!
 cout << "\nWe are going to subtract ( ";
 for( i = 0; i < 5; i++ ) cout << map(i).standardPart() 
                               << ", ";
 cout << map(5).standardPart() 
      << " )\n" 
      << endl;

 for( i = 0; i < 6; i++ ) map(i) -= map(i).standardPart();


 /* DGN */ map.printCoeffs();


 // Project out the longitudinal components.  
 // This is a kludge!
 // Does it destroy symplecticity?

 // // Project out a 4x4 Mapping
 // Jet::BeginEnvironment( order );
 // coord a(0.0),  b(0.0),  c(0.0), d(0.0);
 // JetC::lastEnv = JetC::CreateEnvFrom( Jet::EndEnvironment() );
 // 
 Mapping projected;
 // static int l[] = { 0, 1, 3, 4 };
 /* DGN */ cout << "DGN> Hang on, everybody. Here we goooo...!" << endl;
 // for( i = 0; i < 4; i++ ) 
 //   projected( i ) = map( l[i] ).filter( fff );
 // 

 static MX_R_FUNCPTR xxx[] = { fff, fff, xx2, fff, fff, xx5 };

 projected = map.filter( xxx );

 // for( i = 0; i < 6; i++ ) 
 //   projected( i ) = map( i ).filter( fff );

 /* DGN */ cout << "DGN> Filtered map:" << endl;
 /* DGN */ projected.printCoeffs();

 /* DGN */ cout << "The original map ______________________________" << endl;
 /* DGN */ map.printCoeffs();

 // normalForm( map, order, &eigVec, N, T );
 normalForm( projected, order, &eigVec, N, T );

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

}
