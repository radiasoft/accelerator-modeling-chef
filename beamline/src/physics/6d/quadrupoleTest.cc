#include "beamline.rsc"

int Translate( const int x )
{
  if( x < 1 || x > 6 ) exit(137);
  if( x == 1 ) return 0;
  if( x == 2 ) return 3;
  if( x == 3 ) return 1;
  if( x == 4 ) return 4;
  if( x == 5 ) return 2;
  if( x == 6 ) return 5;
}


main( int, char* argv [] ) {

  static int TMC[15][3] = {
    1, 1, 6,
    2, 2, 6,
    3, 3, 6,
    4, 4, 6,
    5, 1, 1,
    5, 3, 3,
    1, 2, 6,
    3, 4, 6,
    5, 1, 2,
    5, 3, 4,
    5, 6, 6,
    2, 1, 6,
    4, 3, 6,
    5, 2, 2,
    5, 4, 4
  };

  int i;
  Jet::Setup  ( 6, 2, 6 );
  JetC::Setup ( 6, 2, 6 );
  
  double energy = atof( argv[7] );
  JetProton p( energy );
  Proton   xp( energy );

  double zero [6];
  double finalState[6];
  Map state;
  MatrixD J( "J", 6 );
  MatrixD M( 6, 6 );
  MatrixD T( "I", 6 ), Ti( "I", 6 );

  
  zero[0] = atof( argv[1] );
  zero[1] = atof( argv[2] );
  zero[2] = atof( argv[3] );
  zero[3] = atof( argv[4] );
  zero[4] = atof( argv[5] );
  zero[5] = atof( argv[6] );

  double dpop = zero[5];

  quadrupole CAQ0( "C:AQ0", 0.635, 6.4297 );

  p.setState( zero );
  xp.setState( zero );

  CAQ0.propagate( p );
  CAQ0.propagate( xp );

  p.getState( state );
  xp.getState( finalState );

  for( i = 0; i < 6; i++ ) cout << finalState[i] << "  ";
  cout << "\n" << endl;
  for( i = 0; i < 6; i++ ) cout << state(i).standardPart() << "  ";
  cout << "\n" << endl;

  state.printCoeffs();
  cout << "Transport Coefficients:" << endl;
  int ind[6];
  for( i = 0; i < 15; i++ ) {
    int k;
    for( k = 0; k < 6; k++ ) ind[k] = 0;
    cout << "TMC ";
    for( k = 0; k < 3; k++ ) cout << TMC[i][k] << "  ";
    cout << ":  ";

    int j = Translate( TMC[i][0] );
    ind[ Translate( TMC[i][1] ) ]++;
    ind[ Translate( TMC[i][2] ) ]++;

    cout << state[j].weightedDerivative( ind ) << endl;
  }
  cout << endl;

  T( 2, 2 ) = 1.0 / PH_MKS_c;
  T( 3, 3 ) = xp.ReferenceMomentum() / PH_MKS_c;
  T( 4, 4 ) = xp.ReferenceMomentum() / PH_MKS_c;
  T( 5, 5 ) = - ( xp.ReferenceMomentum()*xp.Momentum() ) / xp.Energy();
  Ti( 2, 2 ) = 1.0 / T( 2, 2 );
  Ti( 3, 3 ) = 1.0 / T( 3, 3 );
  Ti( 4, 4 ) = 1.0 / T( 4, 4 );
  Ti( 5, 5 ) = 1.0 / T( 5, 5 );

  cout << "Original M\n" << ( M = state.Jacobian() ) << endl;

  double cx, sx, cy, sy;
  double kx2, ky2, K1;
  double L, b, g;

  cx = M(0,0);
  sx = M(0,3);
  kx2 = K1 = - M(3,0)/sx;
  if( fabs( M(0,0) - M(3,3) ) > 1.0e-10 ) 
    cout << "*** ERROR *** M(0,0) = " << M(0,0) 
                     << "  M(3,3) = " << M(3,3) << endl;

  cy = M(1,1);
  sy = M(1,4);
  ky2 = - M(4,1)/sy;
  if( fabs( M(1,1) - M(4,4) ) > 1.0e-10 ) 
    cout << "*** ERROR *** M(1,1) = " << M(1,1) 
                     << "  M(4,4) = " << M(4,4) << endl;

  b = xp.ReferenceBeta();
  g = xp.ReferenceGamma();
  L = - (b*b*g*g)*M( 2, 5 );  // The negative sign is introduced
                              // because MAD uses - cdt as a coordinate,
                              // not cdt.

  cout << "kx2 = " << kx2 << endl;
  cout << "ky2 = " << ky2 << endl;
  cout << "cx  = " << cx  << endl;
  cout << "sx  = " << sx  << endl;
  cout << "cy  = " << cy  << endl;
  cout << "sy  = " << sy  << endl;
  cout << "L   = " << L   << endl;
  cout << "B'L = " << K1*p.ReferenceBRho()*L << endl;

  cout << "\n" << endl;

  cout << "T_116 = " <<   K1*L*sx         /(4.0*b) << endl;
  cout << "T_336 = " << - K1*L*sy         /(4.0*b) << endl;
  cout << "T_511 = " << - K1*( L - sx*cx )/(4.0*b) << endl;
  cout << "T_533 = " <<   K1*( L - sy*cy )/(4.0*b) << endl;
  cout << "T_126 = " << -    ( sx + L*cx )/(4.0*b) << endl;
  cout << "T_346 = " << -    ( sy + L*cy )/(4.0*b) << endl;
  cout << "T_512 = " <<   K1*sx*sx        /(4.0*b) << endl;
  cout << "T_534 = " << - K1*sy*sy        /(4.0*b) << endl;
  cout << "T_566 = " << - 3.0*L /( 2.0*b*b*b*g*g ) << endl;
  cout << "T_216 = " << - K1*( sx - L*cx )/(4.0*b) << endl;
  cout << "T_436 = " << + K1*( sy - L*cy )/(4.0*b) << endl;
  cout << "T_522 = " << -    ( L + sx*cx )/(4.0*b) << endl;
  cout << "T_544 = " << -    ( L + sy*cy )/(4.0*b) << endl;

  cout << "\n" << endl;

  M = T*M*Ti;
  cout << "Transformed M\n" << M << endl;
  cout << "Symplectic test: \n" << - M*J*M.transpose()*J << endl;

}
