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
******  - use covariant return types for Clone()
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
#include <basic_toolkit/Barnacle.h>
#include <basic_toolkit/Matrix.h>
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


class DLLEXPORT Particle {

  friend class JetParticle;
  friend class particle_core_access;  

public:

  enum PhaseSpaceIndex { xIndex   = 0, yIndex   = 1, cdtIndex =  2, 
                         npxIndex = 3, npyIndex = 4, ndpIndex  = 5 };

protected:

  std::string tag_;        // tag for arbitrary identification
                           // of a particle.
  double      q_;          // electric charge [C]
  double      m_;          // mass in GeV / c^2
  double      p_;          // reference momentum in GeV / c
  double      gamma_;      // reference gamma
  double      beta_;       // normalized reference velocity = v/c
  double      bRho_;       // normalized reference momentum / charge
  Vector      state_;      


  Particle( double  const& massGeV_c2, double const& charge, double const& pc);  
  Particle( double  const& massGeV_c2, double const& charge, double const& pc, Vector const& state );

public:

  Particle( Particle    const& );

  explicit Particle( JetParticle const& );

  virtual ~Particle();
  void     dtor();

  virtual Particle* Clone( void* p=0) const;

  Particle&    operator=(Particle const&);


  // Dimension of phase space

  static const int PSD;
  int psd();

  void SetReferenceEnergy(   double const& energyGeV     );
  void SetReferenceMomentum( double const& momentumGeV_c );

  double setWeight( double const&);       // Returns previous value.

  void setStateToZero();

  double get_x()     const;
  double get_y()     const;
  double get_cdt()   const;
  double get_npx()   const;
  double get_npy()   const;
  double get_npz()   const;
  double get_ndp()   const;

  void set_x   ( double u );
  void set_y   ( double u );
  void set_cdt ( double u );
  void set_npx ( double u );
  void set_npy ( double u );
  void set_ndp ( double u );

  Vector&       State();
  Vector const& State()                    const;

  double        Energy()                   const;
  double        KineticEnergy()            const;
  double        Momentum()                 const;
  double        NormalizedMomentum()       const;
  double const& Mass()                     const;
  double        Gamma()                    const;
  double        Beta()                     const;
  double        BetaX()                    const;
  double        BetaY()                    const;
  double        BetaZ()                    const;
  double        pn()                       const;
  double const& ReferenceBRho()            const;
  double const& ReferenceBeta()            const;
  double const& ReferenceGamma()           const;
  double const& ReferenceMomentum()        const;
  double        ReferenceEnergy()          const;
  double const& Weight()                   const;
  double const& Charge()                   const;
  Vector        VectorBeta()               const;
  Vector        VectorMomentum()           const;
  Vector        NormalizedVectorMomentum() const;
  double        BRho()                     const;

  BarnacleList dataHook;   // Carries data as service to application program.

  // Tagging methods

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

  Proton*   Clone(void* p=0)  const;

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

  AntiProton*    Clone(void* p=0) const;
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


  Electron*    Clone(void* p=0) const;
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

  Positron*    Clone(void* p=0) const;
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

  Muon*    Clone(void* p=0) const;
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

  AntiMuon*    Clone(void* p=0) const;
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

  static Vector& State(Particle &p) { return p.State(); } 

};

// -------------------------------  Inline members --------------------------------------------------------------


  inline int  Particle::psd()          { return Particle::PSD; }

  inline double  Particle::get_x()     const { return state_[xIndex  ]; }
  inline double  Particle::get_y()     const { return state_[yIndex  ]; }
  inline double  Particle::get_cdt()   const { return state_[cdtIndex]; }
  inline double  Particle::get_npx()   const { return state_[npxIndex]; }
  inline double  Particle::get_npy()   const { return state_[npyIndex]; }
  inline double  Particle::get_ndp()   const { return state_[ndpIndex]; }

  inline double  Particle::get_npz()   const { return sqrt( ( 1.0 + state_[ndpIndex] )*( 1.0 + state_[ndpIndex] ) 
                                         - state_[npxIndex]*state_[npxIndex] - state_[npyIndex]*state_[npyIndex] );      }   


  inline void  Particle::set_x   ( double u )  { state_[xIndex  ] = u; }
  inline void  Particle::set_y   ( double u )  { state_[yIndex  ] = u; }
  inline void  Particle::set_cdt ( double u )  { state_[cdtIndex] = u; }
  inline void  Particle::set_npx ( double u )  { state_[npxIndex] = u; }
  inline void  Particle::set_npy ( double u )  { state_[npyIndex] = u; }
  inline void  Particle::set_ndp ( double u )  { state_[ndpIndex] = u; }

  inline Vector&       Particle::State()       { return state_; } 
  inline Vector const& Particle::State() const { return state_; } 

  inline double         Particle::Momentum()                 const   { return p_ * ( 1.0 + state_[ndpIndex] );  }
  inline double         Particle::Energy()                   const   { double p = Momentum(); 
                                                                       return sqrt( p*p + m_*m_ );       }
  inline double         Particle::KineticEnergy()            const   { return Energy() - m_;             }
  inline double         Particle::NormalizedMomentum()       const   { return ( 1.0 + state_[ndpIndex] );}
  inline double  const& Particle::Mass()                     const   { return m_;                        }
  inline double         Particle::Gamma()                    const   { return Energy() / m_;             }
  inline double         Particle::Beta()                     const   { return Momentum() / Energy();     }
  inline double         Particle::pn()                       const   { return beta_*gamma_;              } 
  inline double         Particle::BetaX()                    const   { return (get_npx()*ReferenceMomentum())/Energy(); }
  inline double         Particle::BetaY()                    const   { return (get_npy()*ReferenceMomentum())/Energy(); }
  inline double         Particle::BetaZ()                    const   { return (get_npz()*ReferenceMomentum())/Energy(); }
  inline double  const& Particle::ReferenceBRho()            const   { return bRho_;                     }
  inline double  const& Particle::ReferenceBeta()            const   { return beta_;                     }
  inline double  const& Particle::ReferenceGamma()           const   { return gamma_;                    }
  inline double  const& Particle::ReferenceMomentum()        const   { return p_;                        }
  inline double         Particle::ReferenceEnergy()          const   { return sqrt(p_*p_ + m_*m_);       }
  inline double  const& Particle::Charge()                   const   { return q_;                        }
  inline double         Particle::BRho()                     const   { return bRho_*( 1.0 + state_[ndpIndex] ); }

#endif // PARTICLE_H
