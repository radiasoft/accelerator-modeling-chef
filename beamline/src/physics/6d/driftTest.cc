#include "beamline.rsc"

main( int, char* argv [] ) {

  int i;
  Jet::Setup( 6, 1, 6 );
  JetC::Setup( 6, 1, 6 );
  
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

  double dpop = zero[5];

  sbend Sbend( 6.1214, 4.42351440176, 6.2831853072/774.0 );

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

}
