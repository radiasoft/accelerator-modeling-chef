#include "beamline.inc"

// **************************************************
//   class Particle
// **************************************************

Particle::Particle( double mass ) {
 q     = 0.0;
 for( int i = 0; i < BMLN_dynDim; i++ ) state[i] = 0.0;
 E     = mass;
 p     = 0.0;
 m     = mass;
 pn    = 0.0;
 pni2  = 1.0e33;       // Actually, infinity
 bRho  = 0.0;
 beta  = 0.0; 
 gamma = 1.0;
}

Particle::Particle( double mass, double energy ) {
 q = 0.0;
 for( int i = 0; i < BMLN_dynDim; i++ ) state[i] = 0.0;
 m = mass;
 E = energy;
 if( E < m ) {
  cout << "\n"
       << "*** ERROR ***  Particle::Particle( double )            \n"
       << "*** ERROR ***  What the hell are you doing???          \n"
       << "*** ERROR ***  The energy must be larger than the      \n"
       << "*** ERROR ***  rest mass!!                             \n"
       << "                                                       \n"
       << endl;
  exit(1);
 }
 p = sqrt( E*E - m*m );
 bRho  = p / PH_CNV_brho_to_p;
 gamma = E / m;
 beta  = sqrt( 1.0 - 1.0 / ( gamma*gamma ) );
 pn = beta*gamma;
 if( pn != 0.0 ) pni2 = 1.0 / ( pn*pn );
 else            pni2 = 1.0e33;
}

Particle::Particle( double mass, double energy, double* s ) {
 q = 0.0;
 for( int i = 0; i < BMLN_dynDim; i++ ) state[i] = s[i];
 m = mass;
 E = energy;
 if( E < m ) {
  cout << "\n"
       << "*** ERROR ***  Particle::Particle( double, double* )   \n"
       << "*** ERROR ***  What the hell are you doing???          \n"
       << "*** ERROR ***  The energy must be larger than the      \n"
       << "*** ERROR ***  rest mass!!                             \n"
       << "                                                       \n"
       << endl;
  exit(1);
 }
 p = sqrt( E*E - m*m );
 bRho  = p / PH_CNV_brho_to_p;
 gamma = E / m;
 beta  = sqrt( 1.0 - 1.0 / ( gamma*gamma ) );
 pn = beta*gamma;
 if( pn != 0.0 ) pni2 = 1.0 / ( pn*pn );
 else            pni2 = 1.0e33;
}

Particle::Particle( const Particle& x ) {
 int i;
 for( i = 0; i < BMLN_dynDim; i++ ) state[i] = x.state[i];
 q = x.q;
 E = x.E;
 p = x.p;
 m = x.m;
 pn = x.pn;
 pni2 = x.pni2;
 bRho = x.bRho;
 beta = x.beta;
 gamma = x.gamma;
}

Particle::~Particle() {
}

void Particle::setState( double* x ) {
 for( int i = 0; i < BMLN_dynDim; i++ )  state[i] = x[i];
} 

void Particle::setState( const Vector& x ) {
 if( x.Dim() != BMLN_dynDim ) {
   cout << "*** ERROR ***                                     \n" 
        << "*** ERROR *** Particle::setState( const Vector& ) \n" 
        << "*** ERROR *** Dimension incorrect ...             \n" 
        << "*** ERROR ***                                     \n" 
        << endl;
   exit(1);
 }
 for( int i = 0; i < BMLN_dynDim; i++ )  state[i] = x(i);
} 

void Particle::getState( double* x ) {
 for( int i = 0; i < BMLN_dynDim; i++ )  x[i] = state[i];
} 


void Particle::SetReferenceEnergy( const double energy ) {
 E = energy;
 if( E < m ) {
  cout << "\n"
       << "*** ERROR ***  Particle::SetReferenceEnergy()          \n"
       << "*** ERROR ***  What the hell are you doing???          \n"
       << "*** ERROR ***  The energy must be larger than the      \n"
       << "*** ERROR ***  rest mass!!                             \n"
       << "                                                       \n"
       << endl;
  exit(1);
 }
 p = sqrt( E*E - m*m );
 bRho  = p / PH_CNV_brho_to_p;
 gamma = E / m;
 beta  = sqrt( 1.0 - 1.0 / ( gamma*gamma ) );
 pn = beta*gamma;
 if( pn != 0.0 ) pni2 = 1.0 / ( pn*pn );
 else            pni2 = 1.0e33;
}

Vector Particle::VectorBeta()
{
 Vector ret(3);
 double nrg = Energy();
 double pz  = Momentum();
 pz = sqrt( pz*pz - ((p*p)*( state[3]*state[3] + state[4]*state[4] )) );
 ret(0) = p*state[3] / nrg;
 ret(1) = p*state[4] / nrg;
 ret(2) = pz         / nrg;
 return ret;
}

Particle& Particle::operator=( const Particle& x ) {
 int i;
 if(&x != this) {
   for( i = 0; i < BMLN_dynDim; i++ ) state[i] = x.state[i];
   q = x.q;
   E = x.E;
   p = x.p;
   m = x.m;
   pn = x.pn;
   pni2 = x.pni2;
   bRho = x.bRho;
   beta = x.beta;
   gamma = x.gamma;
 }
 return *this;
}


// **************************************************
//   class Proton
// **************************************************


Proton::Proton() : Particle( PH_NORM_mp ) {
 q = PH_MKS_e;
}

Proton::Proton( double energy ) : Particle( PH_NORM_mp, energy ){
 q = PH_MKS_e;
}

Proton::Proton( double energy, double* s ) : Particle( PH_NORM_mp, energy, s ){
 q = PH_MKS_e;
}

Proton::Proton( const Proton& x ) : Particle( x ) {
}

Proton::~Proton() {
}


// **************************************************
//   class AntiProton
// **************************************************


AntiProton::AntiProton() : Particle( PH_NORM_mp ) {
 q = - PH_MKS_e;
}

AntiProton::AntiProton( double energy ) : Particle( PH_NORM_mp, energy ){
 q = - PH_MKS_e;
}

AntiProton::AntiProton( double energy, double* s ) : Particle( PH_NORM_mp, energy, s ){
 q = - PH_MKS_e;
}

AntiProton::AntiProton( const AntiProton& x ) : Particle( x ) {
}

AntiProton::~AntiProton() {
}


// **************************************************
//   class Electron
// **************************************************

Electron::Electron() : Particle ( PH_NORM_me ){
 q = - PH_MKS_e;
}

Electron::Electron( double W ) : Particle( PH_NORM_me, W ){
 q = - PH_MKS_e;
}

Electron::Electron( double W, double* s ) : Particle( PH_NORM_me, W, s ){
 q = - PH_MKS_e;
}

Electron::Electron( const Electron& x ) : Particle( x ) {
}

Electron::~Electron() {
}


// **************************************************
//   class Positron
// **************************************************

Positron::Positron() : Particle ( PH_NORM_me ){
 q =  PH_MKS_e;
}

Positron::Positron( double W ) : Particle( PH_NORM_me, W ){
 q =  PH_MKS_e;
}

Positron::Positron( double W, double* s ) : Particle( PH_NORM_me, W, s ){
 q =  PH_MKS_e;
}

Positron::Positron( const Positron& x ) : Particle( x ) {
}

Positron::~Positron() {
}

// **************************************************
//   class Muon
// **************************************************


Muon::Muon() : Particle( PH_NORM_mmu ) {
 q = -PH_MKS_e;
}

Muon::Muon( double energy ) : Particle( PH_NORM_mmu, energy ){
 q = -PH_MKS_e;
}

Muon::Muon( double energy, double* s ) : Particle( PH_NORM_mmu, energy, s ){
 q = -PH_MKS_e;
}

Muon::Muon( const Muon& x ) : Particle( x ) {
}

Muon::~Muon() {
}


// **************************************************
//   class AntiMuon
// **************************************************


AntiMuon::AntiMuon() : Particle( PH_NORM_mmu ) {
 q = PH_MKS_e;
}

AntiMuon::AntiMuon( double energy ) : Particle( PH_NORM_mmu, energy ){
 q = PH_MKS_e;
}

AntiMuon::AntiMuon( double energy, double* s ) : Particle( PH_NORM_mmu, energy, s ){
 q = PH_MKS_e;
}

AntiMuon::AntiMuon( const AntiMuon& x ) : Particle( x ) {
}

AntiMuon::~AntiMuon() {
}



// **************************************************
//   class JetParticle
// **************************************************

JetParticle::JetParticle( double mass ) {
 q     = 0.0;
 // ??? REMOVE for( int i = 0; i < BMLN_dynDim; i++ )  state.SetComponent( i, 0.0 );
 state = Mapping( "id", Jet::lastEnv );
 E     = mass;
 p     = 0.0;
 m     = mass;
 pn    = 0.0;
 pni2  = 1.0e33;       // Actually, infinity
 bRho  = 0.0;
 beta  = 0.0; 
 gamma = 1.0;
}

JetParticle::JetParticle( double mass, double energy ) {
 q = 0.0;
 // ??? REMOVE for( int i = 0; i < BMLN_dynDim; i++ )  state.SetComponent( i, 0.0 );
 state = Mapping( "id", Jet::lastEnv );
 m = mass;
 E = energy;
 if( E < m ) {
  cout << "\n"
       << "*** ERROR ***  JetParticle::JetParticle( double, double ) \n"
       << "*** ERROR ***  What the hell are you doing???          \n"
       << "*** ERROR ***  The energy must be larger than the      \n"
       << "*** ERROR ***  rest mass!!                             \n"
       << "                                                       \n"
       << endl;
  exit(1);
 }
 p = sqrt( E*E - m*m );
 bRho  = p / PH_CNV_brho_to_p;
 gamma = E / m;
 beta  = sqrt( 1.0 - 1.0 / ( gamma*gamma ) );
 pn = beta*gamma;
 if( pn != 0.0 ) pni2 = 1.0 / ( pn*pn );
 else            pni2 = 1.0e33;
}

JetParticle::JetParticle( double mass, double energy, double* s ) {
 q = 0.0;
 state = Mapping( "id", Jet::lastEnv );
 for( int i = 0; i < state.Dim(); i++ )  
   state(i) = state(i) + ( s[i] - Jet::lastEnv->refPoint[i] );
 // ??? REMOVE for( int i = 0; i < BMLN_dynDim; i++ )  state.SetComponent( i, s[i] );
 m = mass;
 E = energy;
 if( E < m ) {
  cout << "\n"
       << "*** ERROR ***  JetParticle::JetParticle( double )      \n"
       << "*** ERROR ***  What the hell are you doing???          \n"
       << "*** ERROR ***  The energy must be larger than the      \n"
       << "*** ERROR ***  rest mass!!                             \n"
       << "                                                       \n"
       << endl;
  exit(1);
 }
 p = sqrt( E*E - m*m );
 bRho  = p / PH_CNV_brho_to_p;
 gamma = E / m;
 beta  = sqrt( 1.0 - 1.0 / ( gamma*gamma ) );
 pn = beta*gamma;
 if( pn != 0.0 ) pni2 = 1.0 / ( pn*pn );
 else            pni2 = 1.0e33;
}

JetParticle::JetParticle( const Particle& x ) {
 q = x.Charge();
 state = Mapping( "id", Jet::lastEnv );
 for( int i = 0; i < state.Dim(); i++ )  
   state(i) = state(i) + ( x.State(i) - Jet::lastEnv->refPoint[i] );
 // ??? REMOVE for( int i = 0; i < BMLN_dynDim; i++ )  state.SetComponent( i, x.State(i) );
 m = x.Mass();
 E = x.ReferenceEnergy();
 if( E < m ) {
  cout << "\n"
       << "*** ERROR ***  JetParticle::JetParticle( double )      \n"
       << "*** ERROR ***  What the hell are you doing???          \n"
       << "*** ERROR ***  The energy must be larger than the      \n"
       << "*** ERROR ***  rest mass!!                             \n"
       << "                                                       \n"
       << endl;
  exit(1);
 }
 p = sqrt( E*E - m*m );
 bRho  = p / PH_CNV_brho_to_p;
 gamma = E / m;
 beta  = sqrt( 1.0 - 1.0 / ( gamma*gamma ) );
 pn = beta*gamma;
 if( pn != 0.0 ) pni2 = 1.0 / ( pn*pn );
 else            pni2 = 1.0e33;
}


JetParticle::JetParticle(const JetParticle& x) {
  state = x.state;
  q = x.q;
  E = x.E;
  p = x.p;
  m = x.m;
  pn = x.pn;
  pni2 = x.pni2;
  bRho = x.bRho;
  beta = x.beta;
  gamma = x.gamma;
}

JetParticle::~JetParticle() {
}

void JetParticle::setState( double* x ) {
 Jet__environment* pje = state.Env();
 state = Mapping( "id", pje );
 for( int i = 0; i < state.Dim(); i++ )  
   state(i) = state(i) + ( x[i] - pje->refPoint[i] );
 // ??? REMOVE for( int i = 0; i < BMLN_dynDim; i++ )  state.SetComponent( i, x[i] );
 // ??? REMOVE state.fixReference(x);
} 

void JetParticle::setState( const Mapping& x ) {
 state = x;
} 

void JetParticle::getState( Mapping& x ) {
 x = state;
} 

void JetParticle::getState( Jet* x ) {
 for( int i = 0; i < BMLN_dynDim; i++ ) x[i] = state(i);
} 

void JetParticle::SetReferenceEnergy( const double energy ) {
 E = energy;
 if( E < m ) {
  cout << "\n"
       << "*** ERROR ***  JetParticle::SetReferenceEnergy()       \n"
       << "*** ERROR ***  What the hell are you doing???          \n"
       << "*** ERROR ***  The energy must be larger than the      \n"
       << "*** ERROR ***  rest mass!!                             \n"
       << "                                                       \n"
       << endl;
  exit(1);
 }
 p = sqrt( E*E - m*m );
 bRho  = p / PH_CNV_brho_to_p;
 gamma = E / m;
 beta  = sqrt( 1.0 - 1.0 / ( gamma*gamma ) );
 pn = beta*gamma;
 if( pn != 0.0 ) pni2 = 1.0 / ( pn*pn );
 else            pni2 = 1.0e33;
}

JetVector JetParticle::VectorBeta()
{
 JetVector ret(3);
 Jet nrg = Energy();
 Jet pz  = Momentum();
 pz = sqrt( pz*pz - ((p*p)*( state(3)*state(3) + state(4)*state(4) )) );
 ret(0) = p*state(3) / nrg;
 ret(1) = p*state(4) / nrg;
 ret(2) = pz         / nrg;
 return ret;
}

// **************************************************
//   class JetProton
// **************************************************

JetProton::JetProton() : JetParticle( PH_NORM_mp ){
 q = PH_MKS_e;
}

JetProton::JetProton( double energy ) : JetParticle( PH_NORM_mp, energy ){
 q = PH_MKS_e;
}

JetProton::JetProton( const Proton& x ) : JetParticle( (Particle&) x ) {
}

JetProton::JetProton( const JetProton& x ) : JetParticle( (JetParticle&) x ) {
}

JetProton::~JetProton() {
}

Particle* JetProton::ConvertToParticle() {
 static int i;
 double* x = new double[ BMLN_dynDim ];
 for( i = 0; i < BMLN_dynDim; i++ ) x[i] = state(i).standardPart();
 Particle* prt = new Proton( E );
 prt->setState( x );
 delete [] x;
 return prt;
}

JetParticle& JetParticle::operator=(const JetParticle& x) {
  if(&x != this) {
    state = x.state;
    q = x.q;
    E = x.E;
    p = x.p;
    m = x.m;
    pn = x.pn;
    pni2 = x.pni2;
    bRho = x.bRho;
    beta = x.beta;
    gamma = x.gamma;
  }
  return *this;
}

// **************************************************
//   class JetAntiProton
// **************************************************

JetAntiProton::JetAntiProton() : JetParticle( PH_NORM_mp ){
 q = - PH_MKS_e;
}

JetAntiProton::JetAntiProton( double energy ) : JetParticle( PH_NORM_mp, energy ){
 q = - PH_MKS_e;
}

JetAntiProton::JetAntiProton( const AntiProton& x ) : JetParticle( (Particle&) x ) {
}

JetAntiProton::JetAntiProton( const JetAntiProton& x ) : JetParticle( (JetParticle&) x ) {
}

JetAntiProton::~JetAntiProton() {
}

Particle* JetAntiProton::ConvertToParticle() {
 static int i;
 double* x = new double[ BMLN_dynDim ];
 for( i = 0; i < BMLN_dynDim; i++ ) x[i] = state(i).standardPart();
 Particle* prt = new AntiProton( E );
 prt->setState( x );
 delete [] x;
 return prt;
}


// **************************************************
//   class JetElectron
// **************************************************

JetElectron::JetElectron() : JetParticle( PH_NORM_me ) {
 q = - PH_MKS_e;
}

JetElectron::JetElectron( double energy ) : JetParticle( PH_NORM_me, energy ) {
 q = - PH_MKS_e;
}

JetElectron::JetElectron( const Electron& x ) : JetParticle( (Particle&) x ) {
}

JetElectron::JetElectron( const JetElectron& x ) : JetParticle( (JetParticle&) x ) {
}

JetElectron::~JetElectron() {
}

Particle* JetElectron::ConvertToParticle() {
 static int i;
 double* x = new double[ BMLN_dynDim ];
 for( i = 0; i < BMLN_dynDim; i++ ) x[i] = state(i).standardPart();
 Particle* prt = new Electron( E );
 prt->setState( x );
 delete [] x;
 return prt;
}


// **************************************************
//   class JetPositron
// **************************************************

JetPositron::JetPositron() : JetParticle( PH_NORM_me ) {
 q = PH_MKS_e;
}

JetPositron::JetPositron( double energy ) : JetParticle( PH_NORM_me, energy ) {
 q = PH_MKS_e;
}

JetPositron::JetPositron( const Positron& x ) : JetParticle( (Particle&) x ) {
}

JetPositron::JetPositron( const JetPositron& x ) : JetParticle( (JetParticle&) x ) {
}

JetPositron::~JetPositron() {
}

Particle* JetPositron::ConvertToParticle() {
 static int i;
 double* x = new double[ BMLN_dynDim ];
 for( i = 0; i < BMLN_dynDim; i++ ) x[i] = state(i).standardPart();
 Particle* prt = new Positron( E );
 prt->setState( x );
 delete [] x;
 return prt;
}

// **************************************************
//   class JetMuon
// **************************************************

JetMuon::JetMuon() : JetParticle( PH_NORM_mmu ){
 q = - PH_MKS_e;
}

JetMuon::JetMuon( double energy ) : JetParticle( PH_NORM_mmu, energy ){
 q = - PH_MKS_e;
}

JetMuon::JetMuon( const Muon& x ) : JetParticle( (Particle&) x ) {
}

JetMuon::JetMuon( const JetMuon& x ) : JetParticle( (JetParticle&) x ) {
}

JetMuon::~JetMuon() {
}

Particle* JetMuon::ConvertToParticle() {
 static int i;
 double* x = new double[ BMLN_dynDim ];
 for( i = 0; i < BMLN_dynDim; i++ ) x[i] = state(i).standardPart();
 Particle* prt = new Muon( E );
 prt->setState( x );
 delete [] x;
 return prt;
}

// **************************************************
//   class JetAntiMuon
// **************************************************

JetAntiMuon::JetAntiMuon() : JetParticle( PH_NORM_mmu ){
 q = PH_MKS_e;
}

JetAntiMuon::JetAntiMuon( double energy ) : JetParticle( PH_NORM_mmu, energy ){
 q = PH_MKS_e;
}

JetAntiMuon::JetAntiMuon( const AntiMuon& x ) : JetParticle( (Particle&) x ) {
}

JetAntiMuon::JetAntiMuon( const JetAntiMuon& x ) : JetParticle( (JetParticle&) x ) {
}

JetAntiMuon::~JetAntiMuon() {
}

Particle* JetAntiMuon::ConvertToParticle() {
 static int i;
 double* x = new double[ BMLN_dynDim ];
 for( i = 0; i < BMLN_dynDim; i++ ) x[i] = state(i).standardPart();
 Particle* prt = new AntiMuon( E );
 prt->setState( x );
 delete [] x;
 return prt;
}

