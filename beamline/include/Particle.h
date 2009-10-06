/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      Particle.h
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Copyright (c) 1991 Universities Research Association, Inc.    
******                All Rights Reserved                             
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
******  - use covariant return types for clone()
******  - eliminated ConvertToXXX() type conversion functions; 
******    use explicit mixed type constructors instead.
******  - take max advantage of constructor member initialization (useful 
******    for bunches, since a lot of particles may be instantiated)
******  - streamlined public interface. Eliminated members
******    with raw ptr as argument(s) whenever possible.
******  - use Vector and Mapping rather than raw arrays to store state info 
******  - use empty core_access class as access control mechanism 
******  - elements no longer declared friends. Doing so breaks encapsulation 
******    with no real benefit. 
******                                                                
******  Oct 2007 ostiguy@fnal.gov
******  - private allocator for state vector  
****** May 2008  ostiguy@fnal.gov
****** - eliminated a few data members. They are now computed on the fly, as needed.
****** - brho is now a signed quantity
****** - particle types are intialized on the basis of momentum
**************************************************************************
*************************************************************************/
#ifndef PARTICLE_H
#define PARTICLE_H

#include <exception>

#include <basic_toolkit/globaldefs.h>
#include <basic_toolkit/VectorD.h>
#include <basic_toolkit/Matrix.h>
#include <beamline/PhaseSpaceIndexing.h>
#include <boost/shared_ptr.hpp>
#include <gms/FastAllocator.h>

class JetParticle;

class JetProton;
class JetElectron;
class JetMuon;

class JetAntiProton;
class JetPositron;
class JetAntiMuon;

class Particle;

class Proton;
class Electron;
class Muon;

class AntiProton;
class Positron;
class AntiMuon;

typedef boost::shared_ptr<Particle>           ParticlePtr;
typedef boost::shared_ptr<Particle const>     ConstParticlePtr;

typedef boost::shared_ptr<Proton>             ProtonPtr;
typedef boost::shared_ptr<Proton const>       ConstProtonPtr;

typedef boost::shared_ptr<AntiProton>         AntiProtonPtr;
typedef boost::shared_ptr<AntiProton const>   ConstAntiProtonPtr;

typedef boost::shared_ptr<Positron>           PositronPtr;
typedef boost::shared_ptr<Positron const>     ConstPositronPtr;

typedef boost::shared_ptr<Electron>           ElectronPtr;
typedef boost::shared_ptr<Electron const>     ConstElectronPtr;

typedef boost::shared_ptr<Muon>               MuonPtr;
typedef boost::shared_ptr<Muon const>         ConstMuonPtr;

typedef boost::shared_ptr<AntiMuon>           AntiMuonPtr;
typedef boost::shared_ptr<AntiMuon const>     ConstAntiMuonPtr;


class particle_core_access; 

class DLLEXPORT Particle : public PhaseSpaceIndexing  {


  friend class JetParticle;
  friend class particle_core_access;  

protected:

  bool        lost_;  
  std::string tag_;        // tag for arbitrary identification
                           // of a particle.
  double      q_;          // electric charge [C]
  double      m_;          // mass in GeV / c^2
  double      p_;          // reference momentum in GeV / c
  double      gamma_;      // reference gamma
  double      beta_;       // normalized reference velocity = v/c
  double      brho_;       // normalized reference momentum / charge
  Vector      state_;      


  Particle( double  const& massGeV_c2, double const& charge, double const& pc);  
  Particle( double  const& massGeV_c2, double const& charge, double const& pc, Vector const& state );

public:

  Particle( Particle    const& );

  explicit Particle( JetParticle const& );

  virtual ~Particle();
  void     dtor();

  virtual Particle* clone( void* p=0) const;

  Particle&    operator=(Particle const&);


  // Dimension of phase space

  static const int PSD;
  int psd();

  bool isLost() const;
  void setLost( bool );

  void setRefEnergy(   double const& energyGeV     );
  void setRefMomentum( double const& momentumGeV_c );

  double setWeight( double const&);       // Returns previous value.

  void setStateToZero();

  double x()     const;
  double y()     const;
  double cdt()   const;
  double npx()   const;
  double npy()   const;
  double npz()   const;
  double ndp()   const;

  void   x   ( double u );
  void   y   ( double u );
  void   cdt ( double u );
  void   npx ( double u );
  void   npy ( double u );
  void   ndp ( double u );

  Vector&       state();
  Vector const& state()                    const;

  double        energy()                   const;
  double        kineticEnergy()            const;
  double        momentum()                 const;
  double        normalizedMomentum()       const;
  double const& mass()                     const;
  double        gamma()                    const;
  double        beta()                     const;
  double        betaX()                    const;
  double        betaY()                    const;
  double        betaZ()                    const;
  double        pn()                       const;
  double const& refBrho()            const;
  double const& refBeta()            const;
  double const& refGamma()           const;
  double const& refMomentum()        const;
  double        refEnergy()          const;
  double const& weight()                   const;
  double const& charge()                   const;
  Vector        vectorBeta()               const;
  Vector        vectorMomentum()           const;
  Vector        normalizedVectorMomentum() const;
  double        brho()                     const;


  std::string const&  getTag() const;                         
  void                setTag(std::string const& tag);     


};

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

class DLLEXPORT Proton : public Particle {
public:

  Proton();
  Proton( double const& momentum);
  Proton( double const& momentum, Vector const& state );
  Proton( Proton const& );

  explicit Proton( JetProton const& );

 ~Proton();

  Proton&   operator=(Proton const&);

  Proton*   clone(void* p=0)  const;

};

class DLLEXPORT  AntiProton : public Particle {
public:
  AntiProton();
  AntiProton( double const& energyGeV );
  AntiProton( double const& energyGeV, Vector const& state );
  AntiProton( AntiProton const& );

  explicit AntiProton( JetAntiProton const& );

 ~AntiProton();

  AntiProton&   operator=(AntiProton const&);

  AntiProton*    clone(void* p=0) const;
};


class DLLEXPORT  Electron : public Particle {
public:
  Electron();
  Electron( double   const& pc );
  Electron( double   const& pc , Vector const& state);
  Electron( Electron const& );

  explicit Electron( JetElectron const& );

 ~Electron();

  Electron&   operator=(Electron const&);


  Electron*    clone(void* p=0) const;
};

class DLLEXPORT  Positron : public Particle {
public:
  Positron();
  Positron( double const& pc );
  Positron( double const& pc, Vector const& state );
  Positron( Positron const& );

  explicit Positron( JetPositron const& );

 ~Positron();

  Positron&   operator=(Positron const&);

  Positron*    clone(void* p=0) const;
};

class DLLEXPORT Muon : public Particle {
public:
  Muon();
  Muon( double const& pc );
  Muon( double const& pc, Vector const& state);
  Muon( Muon const& );

  explicit Muon( JetMuon const& );

 ~Muon();

  Muon&   operator=(Muon const&);

  Muon*    clone(void* p=0) const;
};

class DLLEXPORT  AntiMuon : public Particle {
public:
  AntiMuon();
  AntiMuon( double const& pc );
  AntiMuon( double const& pc, Vector const& state );
  AntiMuon( AntiMuon const& );

  explicit AntiMuon( JetAntiMuon const& );

 ~AntiMuon();

  AntiMuon&   operator=(AntiMuon const&);

  AntiMuon*    clone(void* p=0) const;
};


//-------------------------------------------------------------------------------------------
// particle_core_access 
// 
// An empty class used to grant access to private members of the Particle class in a controlled. 
// manner. Typically, it is inherited by functors that need access to Particle's private data.  
//---------------------------------------------------------------------------------------------

class particle_core_access 
{

 protected:

  static Vector& state(Particle &p) { return p.state(); } 

};

// -------------------------------  Inline members --------------------------------------------------------------


  inline int  Particle::psd()          { return Particle::PSD; }

  inline double  Particle::x()     const { return state_[i_x  ]; }
  inline double  Particle::y()     const { return state_[i_y  ]; }
  inline double  Particle::cdt()   const { return state_[i_cdt]; }
  inline double  Particle::npx()   const { return state_[i_npx]; }
  inline double  Particle::npy()   const { return state_[i_npy]; }
  inline double  Particle::ndp()   const { return state_[i_ndp]; }

  inline double  Particle::npz()   const { return sqrt( ( 1.0 + state_[i_ndp] )*( 1.0 + state_[i_ndp] ) 
                                            - state_[i_npx]*state_[i_npx] - state_[i_npy]*state_[i_npy] );      }   


  inline void  Particle::x   ( double u )  { state_[i_x  ] = u; }
  inline void  Particle::y   ( double u )  { state_[i_y  ] = u; }
  inline void  Particle::cdt ( double u )  { state_[i_cdt] = u; }
  inline void  Particle::npx ( double u )  { state_[i_npx] = u; }
  inline void  Particle::npy ( double u )  { state_[i_npy] = u; }
  inline void  Particle::ndp ( double u )  { state_[i_ndp] = u; }

  inline Vector&       Particle::state()       { return state_; } 
  inline Vector const& Particle::state() const { return state_; } 

  inline double         Particle::momentum()                 const   { return p_ * ( 1.0 + state_[i_ndp] );  }
inline double           Particle::energy()                   const   { double p = momentum(); 
                                                                       return sqrt( p*p + m_*m_ );       }
  inline double         Particle::kineticEnergy()            const   { return energy() - m_;             }
  inline double         Particle::normalizedMomentum()       const   { return ( 1.0 + state_[i_ndp] );   }
  inline double  const& Particle::mass()                     const   { return m_;                        }
  inline double         Particle::gamma()                    const   { return energy() / m_;             }
  inline double         Particle::beta()                     const   { return momentum() / energy();     }
  inline double         Particle::pn()                       const   { return beta_*gamma_;              } 
  inline double         Particle::betaX()                    const   { return (npx()*refMomentum())/energy(); }
  inline double         Particle::betaY()                    const   { return (npy()*refMomentum())/energy(); }
  inline double         Particle::betaZ()                    const   { return (npz()*refMomentum())/energy(); }
  inline double  const& Particle::refBrho()                  const   { return brho_;                     }
  inline double  const& Particle::refBeta()                  const   { return beta_;                     }
  inline double  const& Particle::refGamma()                 const   { return gamma_;                    }
  inline double  const& Particle::refMomentum()              const   { return p_;                        }
  inline double         Particle::refEnergy()                const   { return sqrt(p_*p_ + m_*m_);       }
  inline double  const& Particle::charge()                   const   { return q_;                        }
  inline double         Particle::brho()                     const   { return brho_*( 1.0 + state_[i_ndp] ); }

  inline bool           Particle:: isLost()                  const   { return lost_; } 
  inline void           Particle:: setLost( bool set)                { lost_ = set;  } 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||




#endif // PARTICLE_H
