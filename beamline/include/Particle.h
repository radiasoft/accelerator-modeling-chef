/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      Particle.h
******  Version:   2.4
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
******    with raw ptr as argument(s) when possible.
******  - use Vector and Mapping to store state info instead of raw
*****     arrays.    
******  - elements no longer declared friends. Doing so breaks encapsulation 
******    with no real benefit. Propagators should use access function to 
******    change state. There is no significant penalty, if any.       
******  - use empty core_access class as access control mechanism 
******                                                                
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

// Forward declaration


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


const int BMLN_dynDim  = 6;   // ??? Doesn't this imply that BMLN_dynDim
                              // ??? can be defined in several modules?

// .............................. Particle classes

class DLLEXPORT Particle : public gms::FastAllocator {

  friend class JetParticle;
  friend class particle_core_access;  

protected:

  std::string tag_;        // tag for arbitrary identification
                           // of a particle.
  double      q_;          // electric charge [C]
  double      E_;          // reference energy in GeV
  double      m_;          // mass in GeV / c^2
  double      p_;          // reference momentum in GeV / c
  double      gamma_;      // reference gamma
  double      beta_;       // normalized reference velocity = v/c
  double      pn_;         // normalized reference momentum = pc/mc^2 = p/mc
                           //                               = beta*gamma
  double      bRho_;       // normalized reference momentum / charge
  double      pni2_;       // ( 1/pn )^2

  double      wgt_;        // Statistical weight: i.e. macroparticle
                           // Default value: 1.0
  Vector      state_;      // (BMLN_dynDim);
                           // state_[0] = x
                           // state_[1] = y
                           // state_[2] = c dt
                           // state_[3] = px/p
                           // state_[4] = py/p
                           // state_[5] = dp/p


  Particle( double  const& massGeV_c2, double const& energyGeV );  
  Particle( double  const& massGeV_c2, double const& energyGeV, Vector const& state );

public:

  Particle( Particle    const& );

  explicit Particle( JetParticle const& );

  virtual ~Particle();

  virtual Particle* Clone( void* p=0) const;

  Particle&    operator=(Particle const&);

  // Phase space indices

  static int xIndex();     
  static int yIndex();     
  static int cdtIndex();   
  static int npxIndex();   
  static int npyIndex();   
  static int ndpIndex();   

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

  Vector  State()                   const;  // Returns the state as a Vector object.
  double  State( int i )            const;
  void    setState(Vector const& );

  Vector& getState();
  Vector  const& getState()         const;


  double Energy()                   const;
  double Momentum()                 const;
  double NormalizedMomentum()       const;
  double Mass()                     const;
  double Gamma()                    const;
  double Beta()                     const;
  double ReferenceBRho()            const;
  double ReferenceBeta()            const;
  double ReferenceGamma()           const;
  double ReferenceMomentum()        const;
  double PNI2()                     const;
  double ReferenceEnergy()          const;
  double Weight()                   const;
  inline double Charge()            const;
  Vector VectorBeta()               const;
  Vector VectorMomentum()           const;
  Vector NormalizedVectorMomentum() const;
  double BRho()                     const;

  BarnacleList dataHook;   // Carries data as service to application program.

  // Tagging methods

  std::string getTag();                         
  void        setTag(std::string const& tag);     


};

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

class DLLEXPORT Proton : public Particle {
public:

  Proton();
  Proton( double const& energyGeV );
  Proton( double const& energyGeV, Vector const& state );
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
  Electron( double   const& energyGeV );
  Electron( double   const& energyGeV , Vector const& state);
  Electron( Electron const& );

  explicit Electron( JetElectron const& );

 ~Electron();

  Electron&   operator=(Electron const&);


  Electron*    Clone(void* p=0) const;
};

class DLLEXPORT  Positron : public Particle {
public:
  Positron();
  Positron( double const& energyGeV );
  Positron( double const& energyGeV, Vector const& state );
  Positron( Positron const& );

  explicit Positron( JetPositron const& );

 ~Positron();

  Positron&   operator=(Positron const&);

  Positron*    Clone(void* p=0) const;
};

class DLLEXPORT Muon : public Particle {
public:
  Muon();
  Muon( double const& energyGeV );
  Muon( double const& energyGeV, Vector const& state);
  Muon( Muon const& );

  explicit Muon( JetMuon const& );

 ~Muon();

  Muon&   operator=(Muon const&);

  Muon*    Clone(void* p=0) const;
};

class DLLEXPORT  AntiMuon : public Particle {
public:
  AntiMuon();
  AntiMuon( double const& energyGeV );
  AntiMuon( double const& energyGeV, Vector const& state );
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

  static Vector& getState(Particle &p) { return p.getState(); } 

};

// -------------------------------  Inline members --------------------------------------------------------------

  inline int  Particle::xIndex()     { return 0; }
  inline int  Particle::yIndex()     { return 1; }
  inline int  Particle::cdtIndex()   { return 2; }
  inline int  Particle::npxIndex()   { return 3; }
  inline int  Particle::npyIndex()   { return 4; }
  inline int  Particle::ndpIndex()   { return 5; }

  inline int  Particle::psd()          { return Particle::PSD; }

  inline double  Particle::get_x()     const { return state_[0]; }
  inline double  Particle::get_y()     const { return state_[1]; }
  inline double  Particle::get_cdt()   const { return state_[2]; }
  inline double  Particle::get_npx()   const { return state_[3]; }
  inline double  Particle::get_npy()   const { return state_[4]; }
  inline double  Particle::get_ndp()   const { return state_[5]; }

  inline double  Particle::get_npz()   const { return sqrt( ( 1.0 + state_[5] )*( 1.0 + state_[5] ) 
                                               - state_[3]*state_[3] - state_[4]*state_[4] );      }   


  inline void  Particle::set_x   ( double u )  { state_[0] = u; }
  inline void  Particle::set_y   ( double u )  { state_[1] = u; }
  inline void  Particle::set_cdt ( double u )  { state_[2] = u; }
  inline void  Particle::set_npx ( double u )  { state_[3] = u; }
  inline void  Particle::set_npy ( double u )  { state_[4] = u; }
  inline void  Particle::set_ndp ( double u )  { state_[5] = u; }


  inline Vector   Particle::State()                   const   { return state_;                  }  // Returns the state as a Vector object.
  inline double   Particle::State( int i )            const   { return state_[i];               }


  inline double  Particle::Energy()                   const   { double p = Momentum(); 
                                                                return sqrt( p*p + m_*m_ );       }

  inline double  Particle::Momentum()                 const   { return p_ * ( 1.0 + state_[5] );  }
  inline double  Particle::NormalizedMomentum()       const   { return ( 1.0 + state_[5] );       }
  inline double  Particle::Mass()                     const   { return m_;                        }
  inline double  Particle::Gamma()                    const   { return Energy() / m_;             }
  inline double  Particle::Beta()                     const   { return Momentum() / Energy();     }
  inline double  Particle::ReferenceBRho()            const   { return bRho_;                     }
  inline double  Particle::ReferenceBeta()            const   { return beta_;                     }
  inline double  Particle::ReferenceGamma()           const   { return gamma_;                    }
  inline double  Particle::ReferenceMomentum()        const   { return p_;                        }
  inline double  Particle::PNI2()                     const   { return pni2_;                     }
  inline double  Particle::ReferenceEnergy()          const   { return E_;                        }
  inline double  Particle::Weight()                   const   { return wgt_;                      } 
  inline double  Particle::Charge()                   const   { return q_;                        }
  inline double  Particle::BRho()                     const   { return bRho_*( 1.0 + state_[5] ); }

#endif // PARTICLE_H
