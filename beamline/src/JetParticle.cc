/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      JetParticle.cc
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
******  Authors:   Leo Michelotti (Original Version)
******             Jean-Francois Ostiguy
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
using namespace PhysicsConstants;

using FNAL::pcerr;
using FNAL::pcout;


// **************************************************
//   class JetParticle
// **************************************************

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetParticle::JetParticle( double const& mass, double const& charge, double const& momentum )
  : 
 lost_(false),       
  tag_(""),       
    q_(charge),
    m_(mass),         
    p_( momentum ),         
gamma_(refEnergy()/m_),     
 beta_(sqrt( 1.0 - 1.0 / ( gamma_*gamma_ ) ) ),      
 brho_(p_ / (q_/PH_MKS_e * PH_CNV_brho_to_p ))      
{                          

  state_ = Mapping( "id", Jet__environment::topEnv() );    

  if( momentum <= 0.0 ) {
  ostringstream uic;
  uic  << "Momentum " << momentum << " GeV is <= 0.0  ";
  throw( GenericException( __FILE__, __LINE__, 
         "JetParticle::JetParticle( double const&, double const&, double const& )",
         uic.str().c_str() ) );
  }
 

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetParticle::JetParticle( double const& mass, double const& charge, double const& momentum, Mapping const& state) 
 : 
 lost_(false),       
  tag_(""),       
    q_(charge),
    m_(mass),         
    p_( momentum ),         
gamma_(refEnergy()/m_),     
 beta_(sqrt( 1.0 - 1.0 / ( gamma_*gamma_ ) ) ),      
brho_(p_ /(q_/PH_MKS_e*PH_CNV_brho_to_p ) ),
 state_(state_)
{                          

 if( momentum <= 0.0 ) {
  ostringstream uic;
  uic  << "Momentum,  " << p_ << " GeV, <= 0.0";
  throw(  GenericException( __FILE__, __LINE__, 
         "JetParticle::JetParticle( double, double, double* )",
         uic.str().c_str() ) );
 }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetParticle::JetParticle( Particle const& u , EnvPtr<double> const& pje) 
 : 
 lost_(u.lost_),       
  tag_(u.tag_),       
    q_(u.q_),
    m_(u.m_),         
    p_(u.p_),         
gamma_(u.gamma_),     
 beta_(u.beta_),      
 brho_(u.brho_),
 state_(  u.state_.Dim() )
{
   int dim =  u.state_.Dim();       
 
   for ( int i=0; i<dim; ++i ) { 
     state_[i] = Jet::makeCoordinate( pje, i ); 
     state_[i] += u.state_[i];   
   }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetParticle::JetParticle(JetParticle const& u)
 : 
 lost_(u.lost_),       
  tag_(u.tag_),       
    q_(u.q_),
    m_(u.m_),         
    p_(u.p_),         
gamma_(u.gamma_),     
 beta_(u.beta_),      
 brho_(u.brho_),      
state_(u.state_) 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


JetParticle& JetParticle::operator=(JetParticle const& u) 
{
  if ( &u == this ) return *this;
 
  lost_  = u.lost_;       
  tag_   = u.tag_;
  q_     = u.q_;
  p_     = u.p_;
  m_     = u.m_;
  brho_  = u.brho_;
  beta_  = u.beta_;
  gamma_ = u.gamma_;
  state_ = u.state_;

  return *this;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetParticle::~JetParticle() 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JetParticle::dtor() 
{
  this->~JetParticle();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#if 0
void JetParticle::setState( Mapping const& u ) 
{
 state_ = u;
} 

#endif
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void   JetParticle::setState( Vector  const& u ) {    // sets the state to the identity Map. Vector elements define ref point. 

 if( state_.Dim() != u.Dim() ) {
  ostringstream uic;
  uic  << "Dimension of argument, " << u.Dim() << ", does not match "
          "phase space dimension, " << state_.Dim()<< "." << endl;
  throw( GenericException( __FILE__, __LINE__, 
         "void JetParticle::setState( const Vector& )", 
         uic.str().c_str() ) );
 }

 state_ = Mapping( "id", state_.Env() );

 for( int i = 0; i < state_.Dim(); ++i ) { 
   state_[i] = state_[i] + ( u[i] - state_.Env()->refPoint()[i] ); 
 }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Mapping& JetParticle::state() 
{
  return state_;
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Mapping const& JetParticle::state() const 
{
  return state_;
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JetParticle::setRefEnergy( double const& energy ) {

 if( energy < m_ ) {
  ostringstream uic;
  uic  << "Energy, " << refEnergy() << " GeV, is less than mass, " << m_ << " GeV.";
  throw(  GenericException( __FILE__, __LINE__, 
         "void JetParticle::SetrefEnergy( double )", 
         uic.str().c_str() ) );
 }

 gamma_ = energy / m_;
 beta_  = sqrt( 1.0 - 1.0 / ( gamma_*gamma_ ) );
 p_     = beta_*energy;

 brho_  = p_ / ( q_/PH_MKS_e* PH_CNV_brho_to_p );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JetParticle::setRefMomentum( double const& p ) 
{
 p_       = p;
 brho_    = p_ / ( q_/PH_MKS_e*PH_CNV_brho_to_p);
 gamma_   = refEnergy() / m_;
 beta_    = sqrt( 1.0 - 1.0 / ( gamma_*gamma_ ) );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetVector JetParticle::vectorBeta() const
{
  return vectorMomentum()/energy();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetVector JetParticle::vectorMomentum() const
{

 JetVector ret(3, state_.Env());

 ret[0] = p_ * state_[i_npx];  // px
 ret[1] = p_ * state_[i_npy];  
 ret[2] = p_ * npz();

 return ret;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetVector JetParticle::normalizedVectorMomentum() const
{
 JetVector ret(3, state_.Env());

 ret[0] = state_[i_npx];  // px
 ret[1] = state_[i_npy];  
 ret[2] = npz();
 
 return ret; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// **************************************************
//   class JetProton
// **************************************************

JetProton::JetProton() 
 : JetParticle( PH_NORM_mp, PH_MKS_e, PH_NORM_mp )
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetProton::JetProton( double const& energy ) 
: JetParticle( PH_NORM_mp, PH_MKS_e, energy )
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetProton::JetProton( JetProton const& u )
: JetParticle(u) 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetProton::JetProton( Proton const& u,   EnvPtr<double> const& pje )
: JetParticle(u, pje) 
{} 
 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetProton& JetProton::operator=(JetProton const& u) {
  
 if(&u == this)  return *this;

 JetParticle::operator=(u);

 return *this;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetProton::~JetProton() 
{}

// **************************************************
//   class JetAntiProton
// **************************************************

JetAntiProton::JetAntiProton() 
: JetParticle( PH_NORM_mp,  - PH_MKS_e, PH_NORM_mp)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetAntiProton::JetAntiProton( double const& energy ) 
: JetParticle( PH_NORM_mp, - PH_MKS_e, energy )
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetAntiProton::JetAntiProton( JetAntiProton const & u ) 
: JetParticle(u) 
{ }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


JetAntiProton::JetAntiProton( AntiProton const& u,  EnvPtr<double> const& pje ) 
: JetParticle(u, pje) 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetAntiProton& JetAntiProton::operator=(JetAntiProton const& u) 
{
  
 if(&u == this)  return *this;

 JetParticle::operator=(u);

 return *this;

}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetAntiProton::~JetAntiProton() 
{}

// **************************************************
//   class JetElectron
// **************************************************

JetElectron::JetElectron() 
: JetParticle( PH_NORM_me, - PH_MKS_e, PH_NORM_me ) 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetElectron::JetElectron( double const& momentum ) 
: JetParticle( PH_NORM_me, - PH_MKS_e, momentum ) 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetElectron::JetElectron( JetElectron const& u ) 
: JetParticle( u ) 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


JetElectron::JetElectron( Electron const& u,   EnvPtr<double> const& pje) 
: JetParticle( u, pje ) 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetElectron& JetElectron::operator=(JetElectron const& u) 
{
  
 if(&u == this)  return *this;

 JetParticle::operator=(u);

 return *this;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetElectron::~JetElectron() 
{ }

// **************************************************
//   class JetPositron
// **************************************************

JetPositron::JetPositron() 
: JetParticle( PH_NORM_me,  PH_MKS_e, PH_NORM_me ) 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetPositron::JetPositron( double const& momentum ) 
: JetParticle( PH_NORM_me,  PH_MKS_e, momentum ) 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetPositron::JetPositron( JetPositron const& u ) 
: JetParticle( u ) 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetPositron::JetPositron( Positron const& u,  EnvPtr<double> const& pje ) 
: JetParticle( u, pje ) 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetPositron& JetPositron::operator=(JetPositron const& u) 
{
  
 if(&u == this)  return *this;

 JetParticle::operator=(u);

 return *this;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetPositron::~JetPositron() {}


// **************************************************
//   class JetMuon
// **************************************************

JetMuon::JetMuon() 
: JetParticle( PH_NORM_mmu,  - PH_MKS_e, PH_NORM_mmu )
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetMuon::JetMuon( double const& momentum ) 
: JetParticle( PH_NORM_mmu, - PH_MKS_e,  momentum )
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetMuon::JetMuon( JetMuon const& u ) 
: JetParticle(u) 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetMuon::JetMuon( Muon const & u,   EnvPtr<double> const& pje) 
: JetParticle(u, pje) 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


JetMuon& JetMuon::operator=(JetMuon const& u) 
{
  
 if(&u == this)  return *this;

 JetParticle::operator=(u);

 return *this;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


JetMuon::~JetMuon() 
{}


// **************************************************
//   class JetAntiMuon
// **************************************************

JetAntiMuon::JetAntiMuon() 
: JetParticle( PH_NORM_mmu,  PH_MKS_e, PH_NORM_mmu )
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetAntiMuon::JetAntiMuon( double const& momentum ) 
: JetParticle( PH_NORM_mmu,PH_MKS_e,  momentum )
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetAntiMuon::JetAntiMuon( JetAntiMuon const& u )
: JetParticle(u) 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetAntiMuon::JetAntiMuon( AntiMuon const& u,   EnvPtr<double> const& pje )
 : JetParticle(u, pje) 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetAntiMuon& JetAntiMuon::operator=(JetAntiMuon const& u) 
{
  
 if(&u == this)  return *this;

 JetParticle::operator=(u);

 return *this;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetAntiMuon::~JetAntiMuon() 
{}

