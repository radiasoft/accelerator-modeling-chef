#include "beamline.rsc"

main( int, char* argv [] ) {

  int i;
  static double scale[]  = { 1.0e-3, 1.0e-3, 1.0e-3, 1.0e-3, 1.0e-3, 1.0e-3 };

  Jet::BeginEnvironment( 1 );
  coord x(0.0),  y(0.0),  z(0.0),
       px(0.0), py(0.0), pz(0.0);
  Jet__environment* pje = Jet::EndEnvironment( scale );
  JetC::lastEnv = JetC::CreateEnvFrom( pje );

  double energy = atof( argv[7] );
  JetProton p( energy );
  Proton   xp( energy );

  double zero[6], zzero[6];
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

  for( i = 0; i < 6; i++ ) zzero[i] = zero[i];

  double dpop = zero[5];

  double length( 6.1214 );
  double strength( 4.42351440176 );
  double angle( 6.2831853072/774.0 );
  sbend Sbend( length, strength, angle );

  p.setState( zero );
  xp.setState( zero );

  Sbend.propagate( p );
  Sbend.propagate( xp );

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

  // -------------------------------------------------------------

  cout << "\nA second test ..............\n" << endl;
  for( i = 0; i < 6; i++ ) zero[i] = 0.0;
  zero[5] = dpop;
  double rho = xp.BRho() / 4.42351440176;
  zero[0] = rho*dpop;

  cout << "Initial state: ";
  for( i = 0; i < 6; i++ ) cout << zero[i] << "  ";
  cout << endl;

  p.setState( zero );
  xp.setState( zero );

  Sbend.propagate( p );
  Sbend.propagate( xp );

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


  // Test of concatenation of sbends
  cout << "\nBeginning concatenation test:" << endl;

  int j, k;
  for( j = 1; j <= 10; j++ ) {
    double xj = j;
    sbend XXX( length/xj, strength, angle/xj );
    beamline TTT;
    for( k = 0; k < j; k++ ) TTT.append( XXX );
    cout << "\n\n==========================\nj = " << j << endl;

    xp.setState( zzero );
    cout << xp.State();
    TTT.propagate( xp );
    cout << xp.State() << endl;

    xp.setState( zzero );
    cout << xp.State();
    for( k = 0; k < j; k++ ) XXX.propagate( xp );
    cout << xp.State() << endl;
  }

}
