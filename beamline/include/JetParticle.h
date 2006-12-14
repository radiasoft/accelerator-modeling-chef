/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      JetParticle.h
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
******
*******  REVISION HISTORY:
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
**************************************************************************                                                               
**************************************************************************
*************************************************************************/
#ifndef JETPARTICLE_H
#define JETPARTICLE_H

#include <exception>

#include <basic_toolkit/globaldefs.h>
#include <basic_toolkit/VectorD.h>
#include <basic_toolkit/Barnacle.h>
#include <basic_toolkit/Matrix.h>
#include <mxyzptlk/Mapping.h>
#include <mxyzptlk/Jet.h>
#include <mxyzptlk/JetVector.h>


class Particle;
class Proton;
class Electron;
class Muon;

class AntiProton;
class Positron;
class AntiMuon;


class JetParticle {

  friend class Particle;
  friend class jetparticle_core_access;

protected:

  std::string tag_;   // Utility tag for arbitrary identification
                      // of a particle.
  double   q_;        // electric charge [C]
  double   E_;        // reference energy in GeV
  double   m_;        // mass in GeV / c^2
  double   p_;        // reference momentum in GeV / c
  double   gamma_;    // reference gamma
  double   pn_;       // normalized reference momentum = pc/mc^2 = p/mc
  double   beta_;     // normalized reference velocity = v/c
  double   bRho_;     // normalized reference momentum / charge
                      //                               = beta*gamma
  double   pni2_;     // ( 1/pn )^2
  double   wgt_;      // Statistical weight: i.e. macrojetparticle
                      // Default value: 1.0

  Mapping state_;
                      // state_[0] = x
                      // state_[1] = y
                      // state_[2] = c dt
                      // state_[3] = px/p
                      // state_[4] = py/p
                      // state_[5] = dp/p
  // ctors

  JetParticle( double const&  massGeV_c2,  double  const& energyGeV );                
  JetParticle( double const&  massGeV_c2,  double  const& energyGeV,  Mapping const&  state );

public:

  JetParticle( Particle const& u , EnvPtr<double> const& pje = TJetEnvironment<double>::getLastEnv()); 
  JetParticle( JetParticle const& );

  virtual ~JetParticle();

  virtual JetParticle* Clone() const { return new JetParticle(*this); }

  JetParticle& operator=(JetParticle const&);

  // Phase space indices

  static const int PSD;

  static int xIndex();  
  static int yIndex();  
  static int cdtIndex();
  static int npxIndex();
  static int npyIndex();
  static int ndpIndex();

  // Dimension of phase space
  int psd()      const;

  void SetReferenceEnergy(   double const& EnergyGeV );
  void SetReferenceMomentum( double const& momentumGeV_c);

  double setWeight( double const& );       // Returns previous value.

  void     setState( Mapping const& );
  void     setState( Vector  const& );    // sets the state to the identity Map. Vector elements define ref point. 

  Mapping& getState();

  Mapping  State()  const;
  Jet&     State( int );

  Jet get_x()     const;
  Jet get_y()     const;
  Jet get_cdt()   const;
  Jet get_npx()   const;
  Jet get_npy()   const;
  Jet get_npz()   const;
                        
                        
  Jet get_ndp()   const;

  void set_x   ( Jet const& u );
  void set_y   ( Jet const& u );
  void set_cdt ( Jet const& u );
  void set_npx ( Jet const& u );
  void set_npy ( Jet const& u );
  void set_ndp ( Jet const& u );


  std::string getTag();                         
  void        setTag(std::string const& tag);     



  static void createStandardEnvironments( int = 1 );
  // Input: argument = degree (or "order")

  MatrixD SymplecticTest();  // Tests the state for the
                             // symplectic condition, 1 = - MJM^t; 
                             // returns unit matrix if all is well.
                             // Note: this assumes a 6x6 state: 
                             // ( x, y, cdt; px/p, py/p, dp/p )

  Jet Energy()               const;
  Jet Momentum()             const;
  Jet NormalizedMomentum()   const;
  double Mass()              const;
  double ReferenceBRho()     const;
  double ReferenceBeta()     const;
  double ReferenceGamma()    const;
  Jet    Gamma()             const;
  double ReferenceMomentum() const;
  double PNI2()              const;
  double ReferenceEnergy()   const; 
  double Weight()            const;
  double Charge()            const;

  JetVector VectorBeta()     const;
  JetVector VectorMomentum() const;
  JetVector NormalizedVectorMomentum() const;
  Jet Beta()                 const;
  Jet BRho()                 const; 

  BarnacleList dataHook;   // Carries data as service to application program.

 };

struct DLLEXPORT  JetProton : public JetParticle {
  JetProton();
  JetProton( double    const&    EnergyGeV );
  JetProton( JetProton const& );

  explicit JetProton( Proton const& p, EnvPtr<double> const& pje = TJetEnvironment<double>::getLastEnv()); 

 ~JetProton();

  JetProton* Clone()             const;
};

struct DLLEXPORT  JetAntiProton : public JetParticle {
  JetAntiProton();
  JetAntiProton( double        const& EnergyGeV  );
  JetAntiProton( JetAntiProton const& );

  explicit JetAntiProton( AntiProton const& p, EnvPtr<double> const& pje = TJetEnvironment<double>::getLastEnv()); 

 ~JetAntiProton();

  JetAntiProton* Clone()             const;
};

struct DLLEXPORT  JetElectron : public JetParticle {
  JetElectron();
  JetElectron( double      const& EnergyGeV );
  JetElectron( JetElectron const& );

  explicit JetElectron( Electron const& p, EnvPtr<double> const& pje = TJetEnvironment<double>::getLastEnv());

  ~JetElectron();

  JetElectron* Clone()             const;
};

struct DLLEXPORT  JetPositron : public JetParticle {
  JetPositron();
  JetPositron( double      const& EnergyGeV );
  JetPositron( JetPositron const& );

  explicit JetPositron( Positron const& p, EnvPtr<double> const& pje = TJetEnvironment<double>::getLastEnv()); 

 ~JetPositron();

  JetPositron* Clone()             const;
};

struct DLLEXPORT  JetMuon : public JetParticle {
  JetMuon();
  JetMuon( double  const& EnergyGeV);
  JetMuon( JetMuon const& );

  explicit JetMuon( Muon const& p, EnvPtr<double> const& pje = TJetEnvironment<double>::getLastEnv()); 

  ~JetMuon();

  JetMuon* Clone()             const;
};

struct DLLEXPORT  JetAntiMuon : public JetParticle {

  JetAntiMuon();
  JetAntiMuon( double      const& EnergyGeV  );
  JetAntiMuon( JetAntiMuon const& );


  explicit JetAntiMuon( AntiMuon const& p, EnvPtr<double> const& pje = TJetEnvironment<double>::getLastEnv()); 


 ~JetAntiMuon();

  JetAntiMuon* Clone()             const;
};

//-------------------------------------------------------------------------------------------
// jetparticle_core_access 
// 
// An empty class used to grant access to private members of the Particle class in a controlled. 
// manner. Typically, it is inherited by functors that need access to Particle's private data.  
//---------------------------------------------------------------------------------------------

class jetparticle_core_access 
{

 protected:

  static Mapping& getState(JetParticle &p) { return p.getState(); } 

};


// --------------------------------------------Jet Particle inline members -------------------------------------------------------------


  inline int JetParticle::xIndex()     { return 0; }
  inline int JetParticle::yIndex()     { return 1; }
  inline int JetParticle::cdtIndex()   { return 2; }
  inline int JetParticle::npxIndex()   { return 3; }
  inline int JetParticle::npyIndex()   { return 4; }
  inline int JetParticle::ndpIndex()   { return 5; }

  inline int JetParticle::psd()      const   { return JetParticle::PSD; }


  inline Mapping  JetParticle::State()  const { return state_; }


  inline Jet JetParticle::get_x()     const { return state_[0]; }
  inline Jet JetParticle::get_y()     const { return state_[1]; }
  inline Jet JetParticle::get_cdt()   const { return state_[2]; }
  inline Jet JetParticle::get_npx()   const { return state_[3]; }
  inline Jet JetParticle::get_npy()   const { return state_[4]; }
  inline Jet JetParticle::get_npz()   const { return sqrt( ( 1.0 + state_[5] )*( 1.0 + state_[5] ) 
                                              - state_[3]*state_[3]  
                                              - state_[4]*state_[4] ); }
  inline Jet JetParticle::get_ndp()   const { return state_[5]; }

  inline void JetParticle::set_x   ( Jet const& u )  { state_[0] = u; }
  inline void JetParticle::set_y   ( Jet const& u )  { state_[1] = u; }
  inline void JetParticle::set_cdt ( Jet const& u )  { state_[2] = u; }
  inline void JetParticle::set_npx ( Jet const& u )  { state_[3] = u; }
  inline void JetParticle::set_npy ( Jet const& u )  { state_[4] = u; }
  inline void JetParticle::set_ndp ( Jet const& u )  { state_[5] = u; }


  inline Jet    JetParticle::Energy()             const { Jet dum(p_*(state_(5) + 1.0)); return sqrt( dum*dum + m_*m_ );}
  inline Jet    JetParticle::Momentum()           const { return p_*(state_(5) + 1.0);                                   }
  inline Jet    JetParticle::NormalizedMomentum() const { return (state_(5) + 1.0);                                      }
  inline double JetParticle::Mass()               const { return m_;                                                     }
  inline double JetParticle::ReferenceBRho()      const { return bRho_;                                                  }
  inline double JetParticle::ReferenceBeta()      const { return beta_;                                                  }
  inline double JetParticle::ReferenceGamma()     const { return gamma_;                                                 }
  inline Jet    JetParticle::Gamma()              const { return Energy() / m_;                                          }
  inline double JetParticle::ReferenceMomentum()  const { return p_;                                                     }
  inline double JetParticle::PNI2()               const { return pni2_;                                                  }
  inline double JetParticle::ReferenceEnergy()    const { return E_;                                                     } 
  inline double JetParticle::Weight()             const { return wgt_;                                                   }
  inline double JetParticle::Charge()             const { return q_;                                                     }

  inline Jet       JetParticle::Beta()            const { return Momentum() / Energy();                                  }
  inline Jet       JetParticle::BRho()            const { return bRho_*( 1.0 + state_(5) );                              }


  inline JetProton*         JetProton::Clone()             const { return new     JetProton( *this ); }
  inline JetAntiProton* JetAntiProton::Clone()             const { return new JetAntiProton( *this ); }
  inline JetElectron*     JetElectron::Clone()             const { return new   JetElectron( *this ); }
  inline JetPositron*     JetPositron::Clone()             const { return new   JetPositron( *this ); }
  inline JetMuon*             JetMuon::Clone()             const { return new       JetMuon( *this ); }
  inline JetAntiMuon*     JetAntiMuon::Clone()             const { return new   JetAntiMuon( *this ); }

#endif // JETPARTICLE_H
