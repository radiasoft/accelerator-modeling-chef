/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      JetParticle.cc
******  Version:   2.4
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
*****
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws.
******                                                                
******                                                                
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
******  REVISION HISTORY:
******
******  Dec 2006  Jean-Francois Ostiguy
******            ostiguy@fnal.gov
******
******  Major revision 
****** 
******  - use covariant return types for Clone()
******  - eliminated ConvertToXXX() type conversion functions; 
******    use explicit mixed type constructors instead.
******  - take max advantage of constructor member initialization (useful 
******    for bunches, since a lot of particles may be instantiated)
******  - streamlined public interface. Eliminated get/set functions
******    with raw ptr as argument(s).
******  - use Vector and Mapping to store state info instead of raw
*****     arrays.    
******  - elements no longer declared friends. Doing so breaks encapsulation 
******    with no real benefit. Propagators should use access function to 
******    change state. There is no significant penalty, if any.       
******  - use empty core_access class as access control mechanism 
******                                                                
******                                                               
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <basic_toolkit/PhysicsConstants.h>
#include <basic_toolkit/GenericException.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <mxyzptlk/Jet__environment.h>


using namespace std;
using FNAL::pcerr;
using FNAL::pcout;


// **************************************************
//   class JetParticle
// **************************************************

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetParticle::JetParticle( double const& mass, double const& energy )
  : tag_(""),       
    q_(0.0),
    E_(energy),         
    m_(mass),         
    p_( sqrt( E_*E_ - m_*m_ ) ),         
gamma_(E_/m_),     
 beta_(sqrt( 1.0 - 1.0 / ( gamma_*gamma_ ) ) ),      
   pn_( beta_*gamma_),        
 bRho_(p_ / PH_CNV_brho_to_p ),      
 pni2_( (pn_ > 0.0) ? (1.0 / ( pn_*pn_) ) : 1.0e33 ),      
  wgt_(1.0)       
{                          

  state_ = Mapping( "id", Jet__environment::getLastEnv() );    

  if( energy < mass ) {
  ostringstream uic;
  uic  << "Energy, " << energy << " GeV, is less than mass, " << mass << " GeV.";
  throw( GenericException( __FILE__, __LINE__, 
         "JetParticle::JetParticle( double, double )",
         uic.str().c_str() ) );
  }
 

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetParticle::JetParticle( double const& mass, double const& energy, Mapping const& state) 
  : tag_(""),       
    q_(0.0),
    E_(energy),         
    m_(mass),         
    p_( sqrt( E_*E_ - m_*m_ ) ),         
gamma_(E_/m_),     
 beta_(sqrt( 1.0 - 1.0 / ( gamma_*gamma_ ) ) ),      
   pn_( beta_*gamma_),        
 bRho_(p_ / PH_CNV_brho_to_p ),      
 pni2_(pn_ > 0.0 ? ( 1.0 / ( pn_*pn_) ) : 1.0e33),      
  wgt_(1.0),       
 state_(state_)
{                          

 if( energy < mass ) {
  ostringstream uic;
  uic  << "Energy, " << energy << " GeV, is less than mass, " << mass << " GeV.";
  throw(  GenericException( __FILE__, __LINE__, 
         "JetParticle::JetParticle( double, double, double* )",
         uic.str().c_str() ) );
 }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetParticle::JetParticle( Particle const& u , EnvPtr<double> const& pje) 
 : tag_(u.tag_),       
    q_(u.q_),
    E_(u.E_),         
    m_(u.m_),         
    p_(u.p_),         
gamma_(u.gamma_),     
 beta_(u.beta_),      
   pn_(u.pn_),        
 bRho_(u.bRho_),      
 pni2_(u.pni2_),      
 wgt_(u.wgt_),
 state_(  u.state_.Dim() )
{
   int dim =  u.state_.Dim();       
 
   for ( int i = 0; i < dim; ++i ) { 
     Jet component;
     component.setVariable( u.state_[i] ,i,  pje);
     state_[i] = component;   
   }

}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetParticle::JetParticle(JetParticle const& u)
 : tag_(u.tag_),       
    q_(u.q_),
    E_(u.E_),         
    m_(u.m_),         
    p_(u.p_),         
gamma_(u.gamma_),     
 beta_(u.beta_),      
   pn_(u.pn_),        
 bRho_(u.bRho_),      
 pni2_(u.pni2_),      
  wgt_(u.wgt_),       
state_(u.state_) {}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetParticle::~JetParticle() {

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void JetParticle::setState( Mapping const& u ) {
 state_ = u;
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void   JetParticle::setState( Vector  const& u ) {    // sets the state to the identity Map. Vector elements define ref point. 

 if( u.Dim() != BMLN_dynDim ) {
  ostringstream uic;
  uic  << "Dimension of argument, " << u.Dim() << ", does not match "
          "phase space dimension, " << BMLN_dynDim << "." << endl;
  throw( GenericException( __FILE__, __LINE__, 
         "void JetParticle::setState( const Vector& )", 
         uic.str().c_str() ) );
 }

 state_ = Mapping( "id", state_.Env() );

 for( int i = 0; i < state_.Dim(); ++i ) { 
   state_[i] = state_[i] +  u[i];
   /// state_[i] = state_[i] + ( u[i] - pje->refPoint()[i] ); 
 }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


 Mapping& JetParticle::getState() {
  return state_;
} 


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet& JetParticle::State( int i )
{
  if( (0 <= i) && (i < 6) ) {
    return state_(i);
  }
  else { 
    ostringstream uic;
    uic  << "Argument = " << i << ": out of range.";
    throw( GenericException( __FILE__, __LINE__, 
           "inline Jet State( int ) const", 
           uic.str().c_str() ) );
  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JetParticle::createStandardEnvironments( int deg )
{
  // Create an initial Jet environment 
  double scale[]  = { 1.0e-3, 1.0e-3, 1.0e-3, 1.0e-3, 1.0e-3, 1.0e-3 };
  Jet__environment::BeginEnvironment( deg );
  coord x(0.0),  y(0.0),  z(0.0),
       px(0.0), py(0.0), pz(0.0);
  JetC__environment::setLastEnv( Jet__environment::EndEnvironment(scale) ); // implicit conversion
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

MatrixD JetParticle::SymplecticTest() {

 // Note: this assumes a 6x6 state_: ( x, y, cdt; px/p, py/p, dp/p )

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
  M = (this->state_) .Jacobian();
  M = T*M*Ti;
  return  - M*J*M.transpose()*J;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JetParticle::SetReferenceEnergy( double const& energy ) {

 if( energy < m_ ) {
  ostringstream uic;
  uic  << "Energy, " << E_ << " GeV, is less than mass, " << m_ << " GeV.";
  throw(  GenericException( __FILE__, __LINE__, 
         "void JetParticle::SetReferenceEnergy( double )", 
         uic.str().c_str() ) );
 }

 E_     = energy;
 p_     = sqrt( E_*E_ - m_*m_ );
 bRho_  = p_ / PH_CNV_brho_to_p;
 gamma_ = E_ / m_;
 beta_  = sqrt( 1.0 - 1.0 / ( gamma_*gamma_ ) );
 pn_    = beta_*gamma_;
 pni2_  = (pn_ > 0.0 ) ? 1.0 / ( pn_*pn_ ) : 1.0e33;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JetParticle::SetReferenceMomentum( double const& new_p ) 
{
 p_       = new_p;
 E_       = sqrt( p_*p_ + m_*m_ );
 bRho_    = p_ / PH_CNV_brho_to_p;
 gamma_   = E_ / m_;
 beta_    = sqrt( 1.0 - 1.0 / ( gamma_*gamma_ ) );
 pn_      = beta_*gamma_;
 pni2_    = ( pn_ > 0.0 ) ? 1.0 / ( pn_*pn_ ) : 1.0e33;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double JetParticle::setWeight( double const& w )
{
  double ret = wgt_;
  wgt_ = ( w >= 0.0 ) ? w : 1.0;
  return ret;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetVector JetParticle::VectorBeta() const
{
 // ??? This assumes ret(2) > 0
 JetVector  ret(3, state_.Env() );

 Jet nrg( Energy()  );
 Jet  pz( Momentum()); 

 pz         = sqrt( pz*pz - ((p_*p_)*( state_(3)*state_(3) + state_(4)*state_(4) )) );

 ret(0)     = p_*state_(3) / nrg;
 ret(1)     = p_*state_(4) / nrg;
 ret(2)     = pz           / nrg;
 return ret;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetVector JetParticle::VectorMomentum() const
{
 // ??? This assumes ret(2) > 0
 JetVector ret(3, state_.Env());

 Jet px, py;

 Jet pz( Momentum() ) ;

 ret(0) = px = p_*state_(3);  // px
 ret(1) = py = p_*state_(4);  
 ret(2) = sqrt( pz*pz - px*px - py*py );

 return ret;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetVector JetParticle::NormalizedVectorMomentum() const
{
 // ??? This assumes ret(2) > 0
 JetVector ret(3, state_.Env());

 Jet px, py;
 Jet pz( NormalizedMomentum() ); 

 ret(0) = px = state_(3);
 ret(1) = py = state_(4);
 ret(2) = sqrt( pz*pz - px*px - py*py );

 return ret;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// **************************************************
//   class JetProton
// **************************************************

JetProton::JetProton() : JetParticle( PH_NORM_mp,  PH_NORM_mp ){
 q_ = PH_MKS_e;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetProton::JetProton( double const& energy ) : JetParticle( PH_NORM_mp, energy ){
 q_ = PH_MKS_e;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetProton::JetProton( JetProton const& u ): JetParticle(u) {}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


JetProton::JetProton( Proton const& u,   EnvPtr<double> const& pje ): JetParticle(u, pje) {} 
 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetProton::~JetProton() {
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetParticle& JetParticle::operator=(JetParticle const& u) {
  
 if(&u == this)  return *this;

 tag_   = u.tag_;
 q_     = u.q_;
 E_     = u.E_;
 p_     = u.p_;
 m_     = u.m_;
 pn_    = u.pn_;
 pni2_  = u.pni2_;
 bRho_  = u.bRho_;
 beta_  = u.beta_;
 gamma_ = u.gamma_;
 state_ = u.state_;

 return *this;
}

// **************************************************
//   class JetAntiProton
// **************************************************

JetAntiProton::JetAntiProton() : JetParticle( PH_NORM_mp, PH_NORM_mp){
 q_ = - PH_MKS_e;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetAntiProton::JetAntiProton( double const& energy ) : JetParticle( PH_NORM_mp, energy ){
 q_ = - PH_MKS_e;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetAntiProton::JetAntiProton( JetAntiProton const & u ) : JetParticle(u) { }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


JetAntiProton::JetAntiProton( AntiProton const& u,  EnvPtr<double> const& pje ) : JetParticle(u, pje) {}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetAntiProton::~JetAntiProton() {
}

// **************************************************
//   class JetElectron
// **************************************************

JetElectron::JetElectron() : JetParticle( PH_NORM_me, PH_NORM_me ) {
 q_ = - PH_MKS_e;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetElectron::JetElectron( double const& energy ) : JetParticle( PH_NORM_me, energy ) {
 q_ = - PH_MKS_e;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetElectron::JetElectron( JetElectron const& u ) : JetParticle( u ) {}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


JetElectron::JetElectron( Electron const& u,   EnvPtr<double> const& pje) : JetParticle( u, pje ) {}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetElectron::~JetElectron() {
}

// **************************************************
//   class JetPositron
// **************************************************

JetPositron::JetPositron() : JetParticle( PH_NORM_me, PH_NORM_me ) {
 q_ = PH_MKS_e;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetPositron::JetPositron( double const& energy ) : JetParticle( PH_NORM_me, energy ) {
 q_ = PH_MKS_e;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


JetPositron::JetPositron( JetPositron const& u ) : JetParticle( u ) {}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetPositron::JetPositron( Positron const& u,  EnvPtr<double> const& pje ) : JetParticle( u, pje ) {}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetPositron::~JetPositron() {}


// **************************************************
//   class JetMuon
// **************************************************

JetMuon::JetMuon() : JetParticle( PH_NORM_mmu, PH_NORM_mmu ){
 q_ = - PH_MKS_e;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetMuon::JetMuon( double const& energy ) : JetParticle( PH_NORM_mmu, energy ){
 q_ = - PH_MKS_e;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


JetMuon::JetMuon( JetMuon const& u ) : JetParticle(u) {}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetMuon::JetMuon( Muon const & u,   EnvPtr<double> const& pje) : JetParticle(u, pje) {}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetMuon::~JetMuon() {
}


// **************************************************
//   class JetAntiMuon
// **************************************************

JetAntiMuon::JetAntiMuon() : JetParticle( PH_NORM_mmu, PH_NORM_mmu ){
 q_ = PH_MKS_e;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetAntiMuon::JetAntiMuon( double const& energy ) : JetParticle( PH_NORM_mmu, energy ){
 q_ = PH_MKS_e;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetAntiMuon::JetAntiMuon( JetAntiMuon const& u ): JetParticle(u) {}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetAntiMuon::JetAntiMuon( AntiMuon const& u,   EnvPtr<double> const& pje ): JetParticle(u, pje) {}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetAntiMuon::~JetAntiMuon() {
}

