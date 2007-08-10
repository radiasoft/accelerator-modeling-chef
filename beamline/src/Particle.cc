/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      Particle.cc
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
******  Authors:   Leo Michelotti (Original version)                                     
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
******
******  REVISION HISTORY:
******
******  Dec 2006 ostiguy@fnal.gov                                                                
******
******  Major revision 
****** 
******  - use covariant return types for Clone()
******  - eliminated ConvertToXXX() type conversion functions; 
******    use explicit mixed type constructors instead.
******  - take max advantage of constructor member initialization (useful 
******    for bunches, since a lot of particles may be instantiated)
******  - streamlined public interface. Eliminated members
******    with raw ptr as argument(s) when possible.
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
//   class Particle
// **************************************************

const int Particle::PSD = 6;


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Particle::Particle( double const& mass, double const& energy ) 
:   tag_(""),   
      q_(0.0),   
      E_(energy),  
      m_(mass), 
      p_(sqrt(E_*E_-m_*m_)), 
  gamma_(E_/m_), 
   beta_( sqrt( 1.0 - 1.0 / (gamma_*gamma_)) ), 
     pn_(beta_*gamma_), 
   bRho_( p_/ PH_CNV_brho_to_p), 
   pni2_( (pn_ != 0.0 ) ? 1.0 / ( pn_*pn_ ): 1.0e33 ), 
    wgt_(1.0), 
  state_(PSD)


{
 for( int i = 0; i < BMLN_dynDim; i++ ) state_[i] = 0.0;

 if( E_ < m_ ) {
  ostringstream uic;
  uic  << "Energy, " << E_ << " GeV, is less than mass, " << m_ << " GeV.";
  throw(  GenericException( __FILE__, __LINE__, 
         "Particle::Particle( double, double )", 
          uic.str().c_str() ) );
 }

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Particle::Particle( double const& mass, double const& energy, Vector const& s ) 
:     tag_(""),   
      q_(0.0),   
      E_(energy),  
      m_(mass), 
      p_( sqrt(E_*E_ - m_*m_) ), 
  gamma_( E_/m_ ), 
   beta_( sqrt( 1.0 - 1.0 / ( gamma_*gamma_ ) )), 
     pn_( beta_*gamma_), 
   bRho_( p_ / PH_CNV_brho_to_p ), 
   pni2_( pn_ != 0.0 ?  1.0 / ( pn_*pn_ ): 1.0e33 ), 
    wgt_(1.0), 
  state_(PSD)

{

 for( int i = 0; i < BMLN_dynDim; i++ ) state_[i] = s[i];

 if( E_ < m_ ) {
  ostringstream uic;
  uic  << "Energy, " << E_ << " GeV, is less than mass, " << m_ << " GeV.";
  throw(  GenericException( __FILE__, __LINE__, 
         "Particle::Particle( double, double, double* )", 
         uic.str().c_str() ) );
 }

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Particle::Particle( Particle const& u ) 
:   tag_(u.tag_),   
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
  state_(u.state_)
{ }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Particle::Particle( JetParticle const& u ) 
:  tag_(u.tag_), 
     q_(u.q_),
     E_(u.E_), 
     m_(u.m_), 
     p_(u.p_), 
 gamma_(u.gamma_), 
  beta_(u.beta_), 
    pn_(u.pn_), 
  bRho_(u.bRho_), 
  pni2_(u.pni2_), 
   wgt_(u.wgt_)
{
  state_ = u.state_.standardPart();

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Particle* Particle::Clone( void* p) const  
{ 
   return p ? new (p) Particle(*this) : new Particle(*this); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Particle::~Particle() 
{ }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#if 0
void Particle::setState(  Vector const & u ) 
{

 if( u.Dim() != BMLN_dynDim ) {
  ostringstream uic;
  uic  << "Dimension of argument, " << u.Dim() << ", does not match "
          "phase space dimension, " << BMLN_dynDim << "." << endl;
  throw( GenericException( __FILE__, __LINE__, 
         "void Particle::setState( const Vector& )", 
         uic.str().c_str() ) );
 }

 state_ = u;
} 
#endif

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Particle::setStateToZero() {
 for( int i = 0; i < BMLN_dynDim; i++ )  state_[i] = 0.0;
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Particle::SetReferenceEnergy( double const& energy ) 
{

  if( energy < m_ ) {
    ostringstream uic;
    uic  << "Energy, " << energy << " GeV, is less than mass, " << m_ << " GeV.";
    throw( GenericException( __FILE__, __LINE__, 
         "void Particle::SetReferenceEnergy( double )", 
         uic.str().c_str() ) );
  }

 E_     = energy;
 p_     = sqrt( E_*E_ - m_*m_ );
 bRho_  = p_ / PH_CNV_brho_to_p;
 gamma_ = E_ / m_;
 beta_  = sqrt( 1.0 - 1.0 / ( gamma_*gamma_ ) );
 pn_    = beta_*gamma_;
 pni2_  = pn_ != 0.0 ? 1.0 / ( pn_*pn_ ) : 1.0e33;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Particle::SetReferenceMomentum( double const& new_p ) 
{

 p_     = new_p;
 E_     = sqrt( new_p*new_p + m_*m_ );
 bRho_  = p_ / PH_CNV_brho_to_p;
 gamma_ = E_ / m_;
 beta_  = sqrt( 1.0 - 1.0 / ( gamma_*gamma_ ) );
 pn_    = beta_*gamma_;
 pni2_  = pn_ != 0.0 ? 1.0 / ( pn_*pn_ ) : 1.0e33; 

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Particle::setWeight( double const& w )
{
  double ret   = wgt_;
  wgt_         = (w >= 0.0 ) ?  w : 1.0; 
  return ret;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Vector Particle::VectorBeta() const
{
 return VectorMomentum()/Energy();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Vector Particle::VectorMomentum() const
{
 Vector ret(3);

 ret(0) = p_ * state_[3];
 ret(1) = p_ * state_[4];
 ret(2) = p_ * get_npz();

 return ret;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Vector Particle::NormalizedVectorMomentum() const
{

 Vector ret(3);

 ret(0) = state_[3];
 ret(1) = state_[4];
 ret(2) = get_npz();

 return ret;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Particle& Particle::operator=( Particle const& p ) 
{

  if( &p == this ) return *this;

  tag_   = p.tag_;
  q_     = p.q_;
  E_     = p.E_;
  p_     = p.p_;
  m_     = p.m_;
  pn_    = p.pn_;
  pni2_  = p.pni2_;
  bRho_  = p.bRho_;
  beta_  = p.beta_;
  gamma_ = p.gamma_;
  state_ = p.state_;  

 return *this;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::string Particle::getTag()
{
  return tag_;
}                         

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void   Particle::setTag(std::string const& tag)
{
  tag_ = tag; 
}     

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// **************************************************
//   class Proton
// **************************************************

Proton::Proton() 
 : Particle( PH_NORM_mp ,PH_NORM_mp) 
{
 q_ = PH_MKS_e;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Proton::Proton( double const& energy ) 
 : Particle( PH_NORM_mp, energy )
{
 q_ = PH_MKS_e;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Proton::Proton( double const& energy, Vector const& s ) 
 : Particle( PH_NORM_mp, energy, s )
{
 q_ = PH_MKS_e;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Proton::Proton( Proton const& u ) 
 : Particle( u ) 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Proton::Proton( JetProton const &p )
 : Particle(p) 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Proton*           Proton::Clone(void* p)  const 
{ 
  return p ? new(p) Proton( *this )     : new Proton(*this );     
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


Proton::~Proton() 
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Proton& Proton::operator=(Proton const& p) 
{

  if ( &p == this ) return *this;

  Particle::operator=(p);
 
  return *this;
}

// **************************************************
//   class AntiProton
// **************************************************


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

AntiProton::AntiProton() 
 : Particle( PH_NORM_mp, PH_NORM_mp ) 
{
 q_ = - PH_MKS_e;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

AntiProton::AntiProton( double const& energy ) 
 : Particle( PH_NORM_mp, energy )
{
 q_ = - PH_MKS_e;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

AntiProton::AntiProton( double const& energy, Vector const& s ) 
 : Particle( PH_NORM_mp, energy, s )
{
 q_ = - PH_MKS_e;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

AntiProton::AntiProton( AntiProton const& u ) 
 : Particle( u ) 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


AntiProton::AntiProton( JetAntiProton const& p)
 : Particle(p) 
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

AntiProton*   AntiProton::Clone(void* p)    const 
{ 
  return p ? new(p) AntiProton( *this ) : new AntiProton(*this);  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

AntiProton::~AntiProton() {
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

AntiProton& AntiProton::operator=(AntiProton const& p) 
{

  if ( &p == this ) return *this;

  Particle::operator=(p);
 
  return *this;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// **************************************************
//   class Electron
// **************************************************

Electron::Electron() 
 : Particle ( PH_NORM_me,  PH_NORM_me )
{
 q_ = - PH_MKS_e;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Electron::Electron( double const& W ) 
 : Particle( PH_NORM_me, W )
{
 q_ = - PH_MKS_e;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Electron::Electron( double const& W, Vector const& s ) 
: Particle( PH_NORM_me, W, s )
{
 q_ = - PH_MKS_e;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Electron::Electron( Electron const& u ) 
 : Particle( u ) 
{ }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


Electron::Electron( JetElectron const& p)
 : Particle(p) 
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Electron*       Electron::Clone(void* p)    const 
{ 
  return p ? new(p) Electron( *this )   : new Electron( *this );  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Electron::~Electron() {}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Electron& Electron::operator=(Electron const& p) {

  if ( &p == this ) return *this;

  Particle::operator=(p);
 
  return *this;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


// **************************************************
//   class Positron
// **************************************************

Positron::Positron() : Particle ( PH_NORM_me, PH_NORM_me )
{
 q_ =  PH_MKS_e;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Positron::Positron( double const& W ) : Particle( PH_NORM_me, W )
{
 q_ =  PH_MKS_e;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Positron::Positron( double const& W, Vector const& s ) : Particle( PH_NORM_me, W, s )
{
 q_ =  PH_MKS_e;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Positron::Positron( Positron const& u ) : Particle( u ) 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Positron::Positron( JetPositron const& p): Particle(p) {}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Positron*       Positron::Clone(void* p)    const 
{ 
   return p ? new(p) Positron( *this ) : new Positron(*this) ;   
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Positron::~Positron() 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


Positron& Positron::operator=(Positron const& p) 
{

  if ( &p == this ) return *this;

  Particle::operator=(p);
 
  return *this;
}

// **************************************************
//   class Muon
// **************************************************


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
Muon::Muon() : Particle( PH_NORM_mmu,  PH_NORM_mmu ) 
{
 q_ = -PH_MKS_e;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Muon::Muon( double const& energy ) : Particle( PH_NORM_mmu, energy )
{
 q_ = -PH_MKS_e;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Muon::Muon( double const& energy, Vector const& s ) : Particle( PH_NORM_mmu, energy, s )
{
 q_ = -PH_MKS_e;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Muon::Muon( Muon const& u ) : Particle( u ) {
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Muon::Muon(JetMuon const& p): Particle(p) {} 


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Muon*  Muon::Clone(void* p)  const 
{ 
  return p ? new(p) Muon( *this ) : new Muon( *this );      
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Muon::~Muon() 
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Muon& Muon::operator=(Muon const& p) {

  if ( &p == this ) return *this;

  Particle::operator=(p);
 
  return *this;

}

// **************************************************
//   class AntiMuon
// **************************************************


AntiMuon::AntiMuon() : Particle( PH_NORM_mmu,  PH_NORM_mmu ) 
{
 q_ = PH_MKS_e;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

AntiMuon::AntiMuon( double const& energy ) : Particle( PH_NORM_mmu, energy ){
 q_ = PH_MKS_e;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

AntiMuon::AntiMuon( double const& energy, Vector const& s ) : Particle( PH_NORM_mmu, energy, s ){
 q_ = PH_MKS_e;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

AntiMuon::AntiMuon( AntiMuon const& u ) : Particle( u ) {
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

AntiMuon::AntiMuon( JetAntiMuon const & p): Particle(p) {}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

AntiMuon* AntiMuon::Clone(void* p) const 
{ 
  return p ? new(p) AntiMuon( *this )   : new AntiMuon( *this );  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

AntiMuon::~AntiMuon() 
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


AntiMuon& AntiMuon::operator=(AntiMuon const& p) {

  if ( &p == this ) return *this;

  Particle::operator=(p);
 
  return *this;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

