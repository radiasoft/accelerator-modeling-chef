#include "beamline.rsc"

main() {

  Jet::Setup( 6, 1, 6 );
  JetC::Setup( 6, 1, 6 );
  
  JetProton p( 1000.0 );
  Proton xp( 1000.0 );
  double zero [] = { 0., 0., 0., 0., 0., 0. };
  Map state;
  MatrixD J( "J", 6 );
  MatrixD M( 6, 6 );
  MatrixD T( "I", 6 ), Ti( "I", 6 );

  T( 2, 2 ) = 1.0 / PH_MKS_c;
  T( 3, 3 ) = xp.ReferenceMomentum() / PH_MKS_c;
  T( 4, 4 ) = xp.ReferenceMomentum() / PH_MKS_c;
  T( 5, 5 ) = - ( xp.ReferenceMomentum()*xp.Momentum() ) / xp.Energy();
  Ti( 2, 2 ) = 1.0 / T( 2, 2 );
  Ti( 3, 3 ) = 1.0 / T( 3, 3 );
  Ti( 4, 4 ) = 1.0 / T( 4, 4 );
  Ti( 5, 5 ) = 1.0 / T( 5, 5 );

  drift  Drift( 1.0 );
  p.setState( zero );
  Drift.propagate( p );
  p.getState( state );
  cout << "\n\nElement " << Drift.Type() << endl;
  cout << ( M = state.Jacobian() );
  cout << endl;
  M = T*M*Ti;
  cout << - M*J*M.transpose()*J << endl;
  
  hkick  Hkick;
  p.setState( zero );
  Hkick.propagate( p );
  p.getState( state );
  cout << "\n\nElement " << Hkick.Type() << endl;
  cout << ( M = state.Jacobian() );
  cout << endl;
  M = T*M*Ti;
  cout << - M*J*M.transpose()*J << endl;
  
  hmonitor  Hmonitor;
  p.setState( zero );
  Hmonitor.propagate( p );
  p.getState( state );
  cout << "\n\nElement " << Hmonitor.Type() << endl;
  cout << ( M = state.Jacobian() );
  cout << endl;
  M = T*M*Ti;
  cout << - M*J*M.transpose()*J << endl;
  
  marker  Marker;
  p.setState( zero );
  Marker.propagate( p );
  p.getState( state );
  cout << "\n\nElement " << Marker.Type() << endl;
  cout << ( M = state.Jacobian() );
  cout << endl;
  M = T*M*Ti;
  cout << - M*J*M.transpose()*J << endl;
  
  monitor  Monitor;
  p.setState( zero );
  Monitor.propagate( p );
  p.getState( state );
  cout << "\n\nElement " << Monitor.Type() << endl;
  cout << ( M = state.Jacobian() );
  cout << endl;
  M = T*M*Ti;
  cout << - M*J*M.transpose()*J << endl;
  
  octupole  Octupole( 1.0, 1.0 ) ;
  p.setState( zero );
  Octupole.propagate( p );
  p.getState( state );
  cout << "\n\nElement " << Octupole.Type() << endl;
  cout << ( M = state.Jacobian() );
  cout << endl;
  M = T*M*Ti;
  cout << - M*J*M.transpose()*J << endl;
  
  quadrupole  Quadrupole( 0.635, -6.4297 ) ;
  p.setState( zero );
  Quadrupole.propagate( p );
  p.getState( state );
  cout << "\n\nElement " << Quadrupole.Type() << endl;
  cout << ( M = state.Jacobian() );
  cout << endl;
  M = T*M*Ti;
  cout << - M*J*M.transpose()*J << endl;
  
  rbend Rbend( 6.1214, 4.42351440176 );
  p.setState( zero );
  Rbend.propagate( p );
  p.getState( state );
  cout << "\n\nElement " << Rbend.Type() << endl;
  cout << ( M = state.Jacobian() );
  cout << endl;
  M = T*M*Ti;
  cout << - M*J*M.transpose()*J << endl;
  
  sbend Sbend( 6.1214, 4.42351440176, 6.2831853072/774.0 );
  p.setState( zero );
  Sbend.propagate( p );
  p.getState( state );
  cout << "\n\nElement " << Sbend.Type() << endl;
  cout << ( M = state.Jacobian() );
  cout << endl;
  M = T*M*Ti;
  cout << - M*J*M.transpose()*J << endl;
  
  sextupole  Sextupole( 1.0, 0.1 ) ;
  p.setState( zero );
  Sextupole.propagate( p );
  p.getState( state );
  cout << "\n\nElement " << Sextupole.Type() << endl;
  cout << ( M = state.Jacobian() );
  cout << endl;
  M = T*M*Ti;
  cout << - M*J*M.transpose()*J << endl;
  
  srot  Srot( 0.0 ) ;
  p.setState( zero );
  Srot.propagate( p );
  p.getState( state );
  cout << "\n\nElement " << Srot.Type() << endl;
  cout << ( M = state.Jacobian() );
  cout << endl;
  M = T*M*Ti;
  cout << - M*J*M.transpose()*J << endl;
  
  thinDecapole  Thindecapole( 0.1 ) ;
  p.setState( zero );
  Thindecapole.propagate( p );
  p.getState( state );
  cout << "\n\nElement " << Thindecapole.Type() << endl;
  cout << ( M = state.Jacobian() );
  cout << endl;
  M = T*M*Ti;
  cout << - M*J*M.transpose()*J << endl;
  
  thinMultipole  Thinmultipole( 0.1 ) ;
  p.setState( zero );
  Thinmultipole.propagate( p );
  p.getState( state );
  cout << "\n\nElement " << Thinmultipole.Type() << endl;
  cout << ( M = state.Jacobian() );
  cout << endl;
  M = T*M*Ti;
  cout << - M*J*M.transpose()*J << endl;
  
  thinOctupole  Thinoctupole( 0.1 ) ;
  p.setState( zero );
  Thinoctupole.propagate( p );
  p.getState( state );
  cout << "\n\nElement " << Thinoctupole.Type() << endl;
  cout << ( M = state.Jacobian() );
  cout << endl;
  M = T*M*Ti;
  cout << - M*J*M.transpose()*J << endl;
  
  thinQuad  Thinquad( 0.1 ) ;
  p.setState( zero );
  Thinquad.propagate( p );
  p.getState( state );
  cout << "\n\nElement " << Thinquad.Type() << endl;
  cout << ( M = state.Jacobian() );
  cout << endl;
  M = T*M*Ti;
  cout << - M*J*M.transpose()*J << endl;
  
  thinSextupole  Thinsextupole( 0.1 ) ;
  p.setState( zero );
  Thinsextupole.propagate( p );
  p.getState( state );
  cout << "\n\nElement " << Thinsextupole.Type() << endl;
  cout << ( M = state.Jacobian() );
  cout << endl;
  M = T*M*Ti;
  cout << - M*J*M.transpose()*J << endl;
  
  thinrfcavity  Thinrfcavity( 60.0, 500.0, 0.0, 0.0, 0.0 ) ;
  p.setState( zero );
  Thinrfcavity.propagate( p );
  p.getState( state );
  cout << "\n\nElement " << Thinrfcavity.Type() << endl;
  cout << ( M = state.Jacobian() );
  cout << endl;
  M = T*M*Ti;
  cout << - M*J*M.transpose()*J << endl;
  
  vkick  Vkick;
  p.setState( zero );
  Vkick.propagate( p );
  p.getState( state );
  cout << "\n\nElement " << Vkick.Type() << endl;
  cout << ( M = state.Jacobian() );
  cout << endl;
  M = T*M*Ti;
  cout << - M*J*M.transpose()*J << endl;
  
  vmonitor  Vmonitor;
  p.setState( zero );
  Vmonitor.propagate( p );
  p.getState( state );
  cout << "\n\nElement " << Vmonitor.Type() << endl;
  cout << ( M = state.Jacobian() );
  cout << endl;
  M = T*M*Ti;
  cout << - M*J*M.transpose()*J << endl;

}
