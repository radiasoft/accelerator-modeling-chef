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

  while(1) {
  p.setState( zero );
  xp.setState( zero );

  Sbend.propagate( p );
  Sbend.propagate( xp );
  }

}
