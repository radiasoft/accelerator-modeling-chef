#include "Particle.h"
#include "PhysicsConstants.h"

// **************************************************
//   class Particle
// **************************************************

const short int Particle::PSD = 6;



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

Particle::Particle( const Particle& u ) {
 int i;
 for( i = 0; i < BMLN_dynDim; i++ ) state[i] = u.state[i];
 q = u.q;
 E = u.E;
 p = u.p;
 m = u.m;
 pn = u.pn;
 pni2 = u.pni2;
 bRho = u.bRho;
 beta = u.beta;
 gamma = u.gamma;
}

Particle::~Particle() {
}

void Particle::setState( double* u ) {
 memcpy( (void *) this->state, 
         (const void *) u, 
         BMLN_dynDim * sizeof(double) );
} 

void Particle::setState( const Vector& u ) {
 if( u.Dim() != BMLN_dynDim ) {
   cout << "*** ERROR ***                                     \n" 
        << "*** ERROR *** Particle::setState( const Vector& ) \n" 
        << "*** ERROR *** Dimension incorrect ...             \n" 
        << "*** ERROR ***                                     \n" 
        << endl;
   exit(1);
 }
 for( int i = 0; i < BMLN_dynDim; i++ )  state[i] = u(i);
} 

void Particle::setStateToZero() {
 for( int i = 0; i < BMLN_dynDim; i++ )  state[i] = 0.0;
} 

void Particle::getState( double* u ) {
 memcpy( (void *) u, 
         (const void *) this->state, 
         BMLN_dynDim * sizeof(double) );
} 


void Particle::getState( Vector& u ) {
  if( u.Dim() == BMLN_dynDim ) {
    for( int i = 0; i < BMLN_dynDim; i++ ) {
      u(i) = state[i];
    }
  }
  else {
    cerr << "*** ERROR ***                                      \n"
            "*** ERROR *** Particle::getState( Vector& )        \n"
            "*** ERROR *** Dimension is not correct.            \n"
            "*** ERROR ***                                      \n"
         << endl;
    exit(1);
  }
} 


void Particle::getState( Vector* u ) {
  if( u->Dim() == BMLN_dynDim ) {
    for( int i = 0; i < BMLN_dynDim; i++ ) {
      (*u)(i) = state[i];
    }
  }
  else {
    cerr << "*** ERROR ***                                      \n"
            "*** ERROR *** Particle::getState( Vector* )        \n"
            "*** ERROR *** Dimension is not correct.            \n"
            "*** ERROR ***                                      \n"
         << endl;
    exit(1);
  }
} 


void Particle::SetReferenceEnergy( double energy ) {
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

Vector Particle::VectorBeta() const
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

Vector Particle::VectorMomentum() const
{
 Vector ret(3);
 double px, py;
 double pz  = Momentum();

 ret(0) = px = p*state[3];
 ret(1) = py = p*state[4];
 ret(2) = sqrt( pz*pz - px*px - py*py );

 return ret;
}

Vector Particle::NormalizedVectorMomentum() const
{
 // ??? This assumes ret(2) > 0
 Vector ret(3);
 double px, py;
 double pz  = NormalizedMomentum();

 ret(0) = px = state[3];
 ret(1) = py = state[4];
 ret(2) = sqrt( pz*pz - px*px - py*py );

 return ret;
}

Particle& Particle::operator=( const Particle& u ) {
 int i;
 if(&u != this) {
   for( i = 0; i < BMLN_dynDim; i++ ) state[i] = u.state[i];
   q = u.q;
   E = u.E;
   p = u.p;
   m = u.m;
   pn = u.pn;
   pni2 = u.pni2;
   bRho = u.bRho;
   beta = u.beta;
   gamma = u.gamma;
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

Proton::Proton( const Proton& u ) : Particle( u ) {
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

AntiProton::AntiProton( const AntiProton& u ) : Particle( u ) {
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

Electron::Electron( const Electron& u ) : Particle( u ) {
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

Positron::Positron( const Positron& u ) : Particle( u ) {
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

Muon::Muon( const Muon& u ) : Particle( u ) {
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

AntiMuon::AntiMuon( const AntiMuon& u ) : Particle( u ) {
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

JetParticle::JetParticle( const Particle& u ) {
 q = u.Charge();
 state = Mapping( "id", Jet::lastEnv );
 for( int i = 0; i < state.Dim(); i++ )  
   state(i) = state(i) + ( u.State(i) - Jet::lastEnv->refPoint[i] );
 // ??? REMOVE for( int i = 0; i < BMLN_dynDim; i++ )  state.SetComponent( i, u.State(i) );
 m = u.Mass();
 E = u.ReferenceEnergy();
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


JetParticle::JetParticle(const JetParticle& u) {
  state = u.state;
  q = u.q;
  E = u.E;
  p = u.p;
  m = u.m;
  pn = u.pn;
  pni2 = u.pni2;
  bRho = u.bRho;
  beta = u.beta;
  gamma = u.gamma;
}

JetParticle::~JetParticle() {
}

void JetParticle::setState( const Vector& u ) {
 if( u.Dim() != BMLN_dynDim ) {
   cout << "*** ERROR ***                                     \n" 
        << "*** ERROR *** JetParticle::setState( const Vector& ) \n" 
        << "*** ERROR *** Dimension incorrect ...             \n" 
        << "*** ERROR ***                                     \n" 
        << endl;
   exit(1);
 }

 Jet__environment* pje = state.Env();
 state = Mapping( "id", pje );
 for( int i = 0; i < state.Dim(); i++ )  
   state(i) = state(i) + ( u(i) - pje->refPoint[i] );
 // ??? REMOVE for( int i = 0; i < BMLN_dynDim; i++ )  state.SetComponent( i, u[i] );
 // ??? REMOVE state.fixReference(u);
} 

void JetParticle::setState( double* u ) {
 Jet__environment* pje = state.Env();
 state = Mapping( "id", pje );
 for( int i = 0; i < state.Dim(); i++ )  
   state(i) = state(i) + ( u[i] - pje->refPoint[i] );
 // ??? REMOVE for( int i = 0; i < BMLN_dynDim; i++ )  state.SetComponent( i, u[i] );
 // ??? REMOVE state.fixReference(u);
} 

void JetParticle::setState( const Mapping& u ) {
 state = u;
} 

void JetParticle::getState( Mapping& u ) {
 u = state;
} 

void JetParticle::getState( Jet* u ) {
 for( int i = 0; i < BMLN_dynDim; i++ ) u[i] = state(i);
} 


MatrixD JetParticle::SymplecticTest() {
 // Note: this assumes a 6x6 state: ( x, y, cdt; px/p, py/p, dp/p )
  MatrixD J( "J", 6 );
  MatrixD M( 6, 6 );
  MatrixD T( "I", 6 ), Ti( "I", 6 );

  T( 2, 2 ) = 1.0 / PH_MKS_c;
  T( 3, 3 ) = this->ReferenceMomentum() / PH_MKS_c;
  T( 4, 4 ) = this->ReferenceMomentum() / PH_MKS_c;
  T( 5, 5 ) = - ( this->ReferenceMomentum() 
                * ( this->Momentum() ).standardPart() )
                / ( this->Energy() ).standardPart();
  Ti( 2, 2 ) = 1.0 / T( 2, 2 );
  Ti( 3, 3 ) = 1.0 / T( 3, 3 );
  Ti( 4, 4 ) = 1.0 / T( 4, 4 );
  Ti( 5, 5 ) = 1.0 / T( 5, 5 );
  M = (this->state) .Jacobian();
  M = T*M*Ti;
  return  - M*J*M.transpose()*J;
}


void JetParticle::SetReferenceEnergy( double energy ) {
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

JetVector JetParticle::VectorBeta() const
{
 // ??? This assumes ret(2) > 0
 JetVector ret(3);
 Jet nrg = Energy();
 Jet pz  = Momentum();
 pz = sqrt( pz*pz - ((p*p)*( state(3)*state(3) + state(4)*state(4) )) );
 ret(0) = p*state(3) / nrg;
 ret(1) = p*state(4) / nrg;
 ret(2) = pz         / nrg;
 return ret;
}

JetVector JetParticle::VectorMomentum() const
{
 // ??? This assumes ret(2) > 0
 JetVector ret(3);
 Jet px, py;
 Jet pz  = Momentum();

 ret(0) = px = p*state(3);
 ret(1) = py = p*state(4);
 ret(2) = sqrt( pz*pz - px*px - py*py );

 return ret;
}

JetVector JetParticle::NormalizedVectorMomentum() const
{
 // ??? This assumes ret(2) > 0
 JetVector ret(3);
 Jet px, py;
 Jet pz  = NormalizedMomentum();

 ret(0) = px = state(3);
 ret(1) = py = state(4);
 ret(2) = sqrt( pz*pz - px*px - py*py );

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

JetProton::JetProton( const Proton& u ) : JetParticle( (Particle&) u ) {
}

JetProton::JetProton( const JetProton& u ) : JetParticle( (JetParticle&) u ) {
}

JetProton::~JetProton() {
}

Particle* JetProton::ConvertToParticle() {
 static int i;
 double* u = new double[ BMLN_dynDim ];
 for( i = 0; i < BMLN_dynDim; i++ ) u[i] = state(i).standardPart();
 Particle* prt = new Proton( E );
 prt->setState( u );
 delete [] u;
 return prt;
}

JetParticle& JetParticle::operator=(const JetParticle& u) {
  if(&u != this) {
    state = u.state;
    q = u.q;
    E = u.E;
    p = u.p;
    m = u.m;
    pn = u.pn;
    pni2 = u.pni2;
    bRho = u.bRho;
    beta = u.beta;
    gamma = u.gamma;
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

JetAntiProton::JetAntiProton( const AntiProton& u ) : JetParticle( (Particle&) u ) {
}

JetAntiProton::JetAntiProton( const JetAntiProton& u ) : JetParticle( (JetParticle&) u ) {
}

JetAntiProton::~JetAntiProton() {
}

Particle* JetAntiProton::ConvertToParticle() {
 static int i;
 double* u = new double[ BMLN_dynDim ];
 for( i = 0; i < BMLN_dynDim; i++ ) u[i] = state(i).standardPart();
 Particle* prt = new AntiProton( E );
 prt->setState( u );
 delete [] u;
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

JetElectron::JetElectron( const Electron& u ) : JetParticle( (Particle&) u ) {
}

JetElectron::JetElectron( const JetElectron& u ) : JetParticle( (JetParticle&) u ) {
}

JetElectron::~JetElectron() {
}

Particle* JetElectron::ConvertToParticle() {
 static int i;
 double* u = new double[ BMLN_dynDim ];
 for( i = 0; i < BMLN_dynDim; i++ ) u[i] = state(i).standardPart();
 Particle* prt = new Electron( E );
 prt->setState( u );
 delete [] u;
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

JetPositron::JetPositron( const Positron& u ) : JetParticle( (Particle&) u ) {
}

JetPositron::JetPositron( const JetPositron& u ) : JetParticle( (JetParticle&) u ) {
}

JetPositron::~JetPositron() {
}

Particle* JetPositron::ConvertToParticle() {
 static int i;
 double* u = new double[ BMLN_dynDim ];
 for( i = 0; i < BMLN_dynDim; i++ ) u[i] = state(i).standardPart();
 Particle* prt = new Positron( E );
 prt->setState( u );
 delete [] u;
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

JetMuon::JetMuon( const Muon& u ) : JetParticle( (Particle&) u ) {
}

JetMuon::JetMuon( const JetMuon& u ) : JetParticle( (JetParticle&) u ) {
}

JetMuon::~JetMuon() {
}

Particle* JetMuon::ConvertToParticle() {
 static int i;
 double* u = new double[ BMLN_dynDim ];
 for( i = 0; i < BMLN_dynDim; i++ ) u[i] = state(i).standardPart();
 Particle* prt = new Muon( E );
 prt->setState( u );
 delete [] u;
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

JetAntiMuon::JetAntiMuon( const AntiMuon& u ) : JetParticle( (Particle&) u ) {
}

JetAntiMuon::JetAntiMuon( const JetAntiMuon& u ) : JetParticle( (JetParticle&) u ) {
}

JetAntiMuon::~JetAntiMuon() {
}

Particle* JetAntiMuon::ConvertToParticle() {
 static int i;
 double* u = new double[ BMLN_dynDim ];
 for( i = 0; i < BMLN_dynDim; i++ ) u[i] = state(i).standardPart();
 Particle* prt = new AntiMuon( E );
 prt->setState( u );
 delete [] u;
 return prt;
}

