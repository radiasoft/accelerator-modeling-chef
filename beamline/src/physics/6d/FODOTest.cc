#include "beamline.rsc"

main( int, char* argv [] ) {

  int i;

  Jet::BeginEnvironment( 3 );
  coord x(0.0),  y(0.0),  z(0.0),
       px(0.0), py(0.0), pz(0.0);
  Jet__environment* pje = Jet::EndEnvironment();
  JetC::lastEnv = JetC::CreateEnvFrom( pje );

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

 double brho  = p.ReferenceBRho();  // Tesla-meters
 double dl = 5.0;
 double fl = 7.0;
 drift     O ( "O",   dl/2.0 );
 thinQuad  F ( "F",   brho / fl );
 thinQuad  D ( "D", - brho / fl );

 beamline cell;
 cell.append( O );
 cell.append( D );
 cell.append( O );
 cell.append( O );
 cell.append( F );
 cell.append( O );

  p.setState( Map( "ident" ) );
  xp.setState( zero );

  cell.propagate( p );
  cell.propagate( xp );

  p.getState( state );
  xp.getState( finalState );

  for( i = 0; i < 6; i++ ) cout << finalState[i] << "  ";
  cout << "\n" << endl;
  for( i = 0; i < 6; i++ ) cout << state(i).standardPart() << "  ";
  cout << "\n" << endl;

  cout << "The JetProton final state is written here:" << endl;
  state.printCoeffs();

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

  p.setState( Map( "ident" ) );
  xp.setState( zero );

  cell.propagate( p );
  cell.propagate( xp );

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

}
