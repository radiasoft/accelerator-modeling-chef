#include "beamline.rsc"

main( int, char* argv [] ) {

  int i;

  Jet::BeginEnvironment(1);
  coord j1(0.0), j2(0.0), j3(0.0), j4(0.0), j5(0.0), j6(0.0);
  Jet::EndEnvironment();

  JetC::BeginEnvironment(1);
  Complex z( 0.0, 0.0 );
  coordC zj1(z), zj2(z), zj3(z), zj4(z), zj5(z), zj6(z);
  JetC::EndEnvironment();

  JetProton p( 1000.0 );
  Proton   xp( 1000.0 );
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

  double length( 6.1214 );
  double strength( 4.42351440176 );
  rbend Rbend( length, strength );

  p.setState( zero );
  xp.setState( zero );

  Rbend.propagate( p );
  Rbend.propagate( xp );

  p.getState( state );
  xp.getState( finalState );

  for( i = 0; i < 6; i++ ) cout << finalState[i] << "  ";
  cout << "\n" << endl;
  for( i = 0; i < 6; i++ ) cout << state(i).standardPart() << "  ";
  cout << "\n" << endl;

  T( 2, 2 ) = 1.0 / PH_MKS_c;
  T( 3, 3 ) = xp.ReferenceMomentum() / PH_MKS_c;
  T( 4, 4 ) = xp.ReferenceMomentum() / PH_MKS_c;
  T( 5, 5 ) = - ( xp.ReferenceMomentum()*xp.Momentum() ) / xp.Energy();
  Ti( 2, 2 ) = 1.0 / T( 2, 2 );
  Ti( 3, 3 ) = 1.0 / T( 3, 3 );
  Ti( 4, 4 ) = 1.0 / T( 4, 4 );
  Ti( 5, 5 ) = 1.0 / T( 5, 5 );
  cout << "Original M\n" << ( M = state.Jacobian() ) << endl;
  M = T*M*Ti;
  cout << "Transformed M\n" << M << endl;
  cout << "Symplectic test: \n" << - M*J*M.transpose()*J << endl;

  double sn = Rbend.Strength()*Rbend.Length()/( 2.0*xp.BRho() );
  double cs = sqrt( 1.0 - sn*sn );
  // zero[3] = xp.Momentum()*cs;
  zero[3] = sn;

  int j, k;
  for( j = 1; j <= 10; j++ ) {
    double xj = j;
    rbend XXX( length/xj, strength );
    beamline TTT;
    for( k = 0; k < j; k++ ) TTT.append( XXX );
    cout << "\n\n==========================\nj = " << j << endl;

    xp.setState( zero );
    cout << xp.State();
    TTT.propagate( xp );
    cout << xp.State() << endl;

    xp.setState( zero );
    cout << xp.State();
    for( k = 0; k < j; k++ ) XXX.propagate( xp );
    cout << xp.State() << endl;
  }

  for( j = 1; j <= 10; j++ ) {
    double xj = j;
    rbend XXX( length/xj, strength );
    beamline TTT;
    for( k = 0; k < j; k++ ) TTT.append( XXX );
    cout << "\n\n==========================\nj = " << j << endl;

    xp.setState( zero );
    cout << xp.State();
    for( k = 0; k < j; k++ ) XXX.P_NoEdge( &XXX, xp );
    cout << xp.State() << endl;
  }

}
