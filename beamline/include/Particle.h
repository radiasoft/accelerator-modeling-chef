/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.1
******                                    
******  File:      Particle.h
******                                                                
******  Copyright (c) 1991 Universities Research Association, Inc.    
******                All Rights Reserved                             
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
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


#ifndef PARTICLE_H
#define PARTICLE_H

#include <exception>

#include "VectorD.h"
#include "Mapping.h"
#include "Barnacle.h"

// Forward declaration
class JetParticle;

const int BMLN_dynDim  = 6;   // ??? Doesn't this imply that BMLN_dynDim
                              // ??? can be defined in several modules?

// .............................. Particle classes
class Particle {
protected:

  friend class alignment;
  friend class bmlnElmnt;
  friend class combinedFunction;
  friend class hkick;
  friend class octupole;
  friend class thinrfcavity;
  friend class rfcavity;
  friend class srot;
  friend class vkick;
  friend class monitor;
  friend class hmonitor;
  friend class vmonitor;
  friend class marker;
  friend class drift;
  friend class rbend;
  friend class sbend;
  friend class sector;
  friend class quadrupole;
  friend class JetQuadrupole;
  friend class thinQuad;
  friend class JetthinQuad;
  friend class thinSextupole;
  friend class JetthinSext;
  friend class thinOctupole;
  friend class thinDecapole;
  friend class thin12pole;
  friend class thin14pole;
  friend class thin16pole;
  friend class thin18pole;
  friend class thinMultipole;
  friend class sextupole;
  friend class BBLens;
  friend class thinSeptum;
  friend class thinLamb;
  friend class beamline;
  friend class Pinger;
  friend class HPinger;
  friend class VPinger;
  friend class kick;
  friend class Slot;
  friend class jetprop;  // ??? Why is this here ???

  double   state [ BMLN_dynDim ];
                      // state[0] = x
                      // state[1] = y
                      // state[2] = c dt
                      // state[3] = px/p
                      // state[4] = py/p
                      // state[5] = dp/p
  double   q;         // electric charge [C]
  double   E;         // reference energy in GeV
  double   p;         // reference momentum in GeV / c
  double   m;         // mass in GeV / c^2
  double   pn;        // normalized reference momentum = pc/mc^2 = p/mc
                      //                               = beta*gamma
  double   pni2;      // ( 1/pn )^2
  double   bRho;      // normalized reference momentm / charge
  double   beta;      // normalized reference velocity = v/c
  double   gamma;     // reference gamma

  Particle( double  /* mass [GeV/c^2] */ );
  Particle( double  /* mass [GeV/c^2] */,
            double  /* energy [GeV]   */ );  // ??? momentum here???
  Particle( double  /* mass [GeV/c^2] */,
            double  /* energy [GeV]   */,
            double* /* state          */ );
  Particle( const Particle& );

public:
  virtual JetParticle* ConvertToJetParticle() const = 0;
  
  virtual ~Particle();
  virtual Particle* Clone() const = 0;
  
  // Phase space indices
  short int xIndex()   const  { return 0; }
  short int yIndex()   const  { return 1; }
  short int cdtIndex() const  { return 2; }
  short int npxIndex() const  { return 3; }
  short int npyIndex() const  { return 4; }
  short int ndpIndex() const  { return 5; }

  // Dimension of phase space
  static const short int PSD;
  short int psd()        { return Particle::PSD; }

  void SetReferenceEnergy( double /* energy [GeV] */ );
  void SetReferenceMomentum( double /* momentum [GeV/c] */ );
  void setState      ( double* );
  void setState      ( const Vector& );
  void getState      ( double* );
  void getState      ( Vector& );
  void getState      ( Vector* );
  void setStateToZero();

  inline double get_x()     const { return state[0]; }
  inline double get_y()     const { return state[1]; }
  inline double get_cdt()   const { return state[2]; }
  inline double get_npx()   const { return state[3]; }
  inline double get_npy()   const { return state[4]; }
  inline double get_npz()   const { return sqrt( ( 1.0 + state[5] )*( 1.0 + state[5] ) 
                                                 - state[3]*state[3]  
                                                 - state[4]*state[4] ); }
  inline double get_ndp()   const { return state[5]; }

  void set_x   ( double u )  { state[0] = u; }
  void set_y   ( double u )  { state[1] = u; }
  void set_cdt ( double u )  { state[2] = u; }
  void set_npx ( double u )  { state[3] = u; }
  void set_npy ( double u )  { state[4] = u; }
  void set_ndp ( double u )  { state[5] = u; }

  inline Vector State() const
         {  return Vector( BMLN_dynDim, state ); } 
         // Returns the state as a Vector object.

  inline double State( int i ) const { 
   if( (0 <= i) && (i <= 5) ) return state[i];
   else                       return 123.456789; 
  }
  inline double Energy() const {
   double u;
   u = p*( 1.0+state[5] );
   return sqrt( u*u + m*m );
  }
  inline double Momentum() const {
   return p*(1.0+state[5]);
  }
  inline double NormalizedMomentum() const {
   return (1.0+state[5]);
  }
  inline double Mass() const {
   return m;
  }
  inline double Gamma() const {
   return Energy() / m;}
  inline double Beta() const {
   return Momentum() / Energy();
  }
  inline double ReferenceBRho() const {
   return bRho;
  }
  inline double ReferenceBeta() const {
   return beta;
  }
  inline double ReferenceGamma() const {
   return gamma;
  }
  inline double ReferenceMomentum() const {
   return p; 
  }
  inline double PNI2() const {
   return pni2;
  }
  inline double ReferenceEnergy() const {
   return E;
  }
  inline double Charge() const {
   return q;
  }
  Vector VectorBeta() const;
  Vector VectorMomentum() const;
  Vector NormalizedVectorMomentum() const;
  inline double BRho() const {
   return bRho*( 1.0 + state[5] );
  }
  Particle& operator=(const Particle&);
  BarnacleList dataHook;   // Carries data as service to application program.

  // State indices ...
  static int _x();
  static int _y();
  static int _cdt();
  static int _xp();    // Actually, p_x / p_ref
  static int _yp();    //           p_y / p_ref
  static int _dpop();

  // Exceptions
  struct GenericException : public std::exception
  {
    GenericException( std::string, int, const char* = "", const char* = "" );
    // Miscellaneous errors
    // 1st argument: name of file in which exception is thrown
    // 2nd         : line from which exception is thrown
    // 3rd         : identifies function containing throw
    // 4th         : identifies type of error
    ~GenericException() throw() {}
    const char* what() const throw();
    std::string errorString;
  };
};


  inline int Particle::_x()    { return 0; }
  inline int Particle::_y()    { return 1; }
  inline int Particle::_cdt()  { return 2; }
  inline int Particle::_xp()   { return 3; }
  inline int Particle::_yp()   { return 4; }
  inline int Particle::_dpop() { return 5; }



//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

class Proton : public Particle {
public:
  Proton();
  Proton( double /* energy [GeV] */ );
  Proton( double /* energy [GeV] */, double* /* state */ );
  Proton( const Proton& );
  ~Proton();
  JetParticle* ConvertToJetParticle() const;
  Particle* Clone() const { return new Proton( *this ); }
};

class AntiProton : public Particle {
public:
  AntiProton();
  AntiProton( double /* energy [GeV] */ );
  AntiProton( double /* energy [GeV] */, double* /* state */ );
  AntiProton( const AntiProton& );
  ~AntiProton();
  JetParticle* ConvertToJetParticle() const;
  Particle* Clone() const { return new AntiProton( *this ); }
};

class Electron : public Particle {
public:
  Electron();
  Electron( double /* energy [GeV] */ );
  Electron( double /* energy [GeV] */, double* /* state */ );
  Electron( const Electron& );
  ~Electron();
  JetParticle* ConvertToJetParticle() const;
  Particle* Clone() const { return new Electron( *this ); }
};

class Positron : public Particle {
public:
  Positron();
  Positron( double /* energy [GeV] */ );
  Positron( double /* energy [GeV] */, double* /* state */ );
  Positron( const Positron& );
  ~Positron();
  JetParticle* ConvertToJetParticle() const;
  Particle* Clone() const { return new Positron( *this ); }
};

class Muon : public Particle {
public:
  Muon();
  Muon( double /* energy [GeV] */ );
  Muon( double /* energy [GeV] */, double* /* state */ );
  Muon( const Muon& );
  ~Muon();
  JetParticle* ConvertToJetParticle() const;
  Particle* Clone() const { return new Muon( *this ); }
};

class AntiMuon : public Particle {
public:
  AntiMuon();
  AntiMuon( double /* energy [GeV] */ );
  AntiMuon( double /* energy [GeV] */, double* /* state */ );
  AntiMuon( const AntiMuon& );
  ~AntiMuon();
  JetParticle* ConvertToJetParticle() const;
  Particle* Clone() const { return new AntiMuon( *this ); }
};

class JetParticle {
protected:

  friend class alignment;
  friend class bmlnElmnt;
  friend class combinedFunction;
  friend class hkick;
  friend class octupole;
  friend class thinrfcavity;
  friend class rfcavity;
  friend class srot;
  friend class vkick;
  friend class monitor;
  friend class hmonitor;
  friend class vmonitor;
  friend class marker;
  friend class drift;
  friend class rbend;
  friend class sbend;
  friend class sector;
  friend class quadrupole;
  friend class JetQuadrupole;
  friend class thinQuad;
  friend class JetthinQuad;
  friend class thinSextupole;
  friend class JetthinSext;
  friend class thinOctupole;
  friend class thinDecapole;
  friend class thin12pole;
  friend class thin14pole;
  friend class thin16pole;
  friend class thin18pole;
  friend class thinMultipole;
  friend class sextupole;
  friend class BBLens;
  friend class thinSeptum;
  friend class thinLamb;
  friend class beamline;
  friend class Pinger;
  friend class HPinger;
  friend class VPinger;
  friend class kick;
  friend class Slot;

  Mapping state;
                      // state[0] = x
                      // state[1] = y
                      // state[2] = c dt
                      // state[3] = px/p
                      // state[4] = py/p
                      // state[5] = dp/p
  double   q;         // electric charge [C]
  double   E;         // reference energy in GeV
  double   p;         // reference momentum in GeV / c
  double   m;         // mass in GeV / c^2
  double   pn;        // normalized reference momentum = pc/mc^2 = p/mc
                      //                               = beta*gamma
  double   pni2;      // ( 1/pn )^2
  double   bRho;      // normalized reference momentum / charge
  double   beta;      // normalized reference velocity = v/c
  double   gamma;     // reference gamma

  JetParticle( double  /* mass [GeV/c^2] */ );
  JetParticle( double  /* mass [GeV/c^2] */,
               double  /* energy [GeV]   */ );   // ??? momentum here???
  JetParticle( double  /* mass [GeV/c^2] */,
               double  /* energy [GeV]   */,
               double* /* state          */ );
  JetParticle( const Particle& );
  JetParticle( const JetParticle& );

public:
  virtual Particle* ConvertToParticle() const = 0;
  virtual ~JetParticle();
  virtual JetParticle* Clone() const = 0;

  // Phase space indices
  short int xIndex()   const   { return 0; }
  short int yIndex()   const   { return 1; }
  short int cdtIndex() const   { return 2; }
  short int npxIndex() const   { return 3; }
  short int npyIndex() const   { return 4; }
  short int ndpIndex() const   { return 5; }

  // Dimension of phase space
  short int psd()        { return Particle::PSD; }

  void SetReferenceEnergy( double /* Energy */ );

  void setState( double* );
  void setState( const Vector& );
  void setState( const Mapping& );
  void getState( Mapping& );
  void getState( Jet* );

  inline Jet get_x()     const { return state(0); }
  inline Jet get_y()     const { return state(1); }
  inline Jet get_cdt()   const { return state(2); }
  inline Jet get_npx()   const { return state(3); }
  inline Jet get_npy()   const { return state(4); }
  inline Jet get_npz()   const { return sqrt( ( 1.0 + state(5) )*( 1.0 + state(5) ) 
                                              - state(3)*state(3)  
                                              - state(4)*state(4) ); }
  inline Jet get_ndp()   const { return state(5); }

  void set_x   ( const Jet& u )  { state.SetComponent(0,u); }
  void set_y   ( const Jet& u )  { state.SetComponent(1,u); }
  void set_cdt ( const Jet& u )  { state.SetComponent(2,u); }
  void set_npx ( const Jet& u )  { state.SetComponent(3,u); }
  void set_npy ( const Jet& u )  { state.SetComponent(4,u); }
  void set_ndp ( const Jet& u )  { state.SetComponent(5,u); }

  inline Mapping State() const { return state; }
  // inline Mapping& State() { return state; }
  Jet& State( int );

  MatrixD SymplecticTest();  // Tests the state for the
                             // symplectic condition, 1 = - MJM^t; 
                             // returns unit matrix if all is well.
                             // Note: this assumes a 6x6 state: 
                             // ( x, y, cdt; px/p, py/p, dp/p )

  inline Jet Energy() const {
   Jet dum;
   dum = p*(state(5) + 1.0);
   return sqrt( dum*dum + m*m );
  }
  inline Jet Momentum() const {
   return p*(state(5) + 1.0);
  }
  inline Jet NormalizedMomentum() const {
   return (state(5) + 1.0);
  }
  inline double Mass() const {
   return m;
  }
  inline double ReferenceBRho() const {
   return bRho;
  }
  inline double ReferenceBeta() const {
   return beta;
  }
  inline double ReferenceGamma() const {
   return gamma;
  }
  inline Jet Gamma() const {
   return Energy() / m;
  }
  inline double ReferenceMomentum() const {
   return p; 
  }
  inline double PNI2() const {
   return pni2;
  }
  inline double ReferenceEnergy() const {
   return E;
  }
  inline double Charge() const {
   return q;
  }
  JetVector VectorBeta() const;
  JetVector VectorMomentum() const;
  JetVector NormalizedVectorMomentum() const;
  inline Jet Beta() const {
   return Momentum() / Energy();
  }
  // ??? REMOVE inline Jet Beta() const {
  // ??? REMOVE  Jet ret( Gamma() );
  // ??? REMOVE  ret = sqrt( 1.0 - ( 1.0 / ( ret*ret ) ) );
  // ??? REMOVE  return ret;
  // ??? REMOVE }
  inline Jet BRho() const {
   return bRho*( 1.0 + state(5) );
  }
  JetParticle& operator=(const JetParticle&);
  BarnacleList dataHook;   // Carries data as service to application program.
};

struct JetProton : public JetParticle {
  JetProton();
  JetProton( double /* Energy [GeV] */ );
  JetProton( const Proton& );
  JetProton( const JetProton& );
  ~JetProton();
  JetParticle* Clone() const { return new JetProton( *this ); }
  Particle* ConvertToParticle() const;
};

struct JetAntiProton : public JetParticle {
  JetAntiProton();
  JetAntiProton( double /* Energy [GeV] */ );
  JetAntiProton( const AntiProton& );
  JetAntiProton( const JetAntiProton& );
  ~JetAntiProton();
  JetParticle* Clone() const { return new JetAntiProton( *this ); }
  Particle* ConvertToParticle() const;
};

struct JetElectron : public JetParticle {
  JetElectron();
  JetElectron( double /* Energy */ );
  JetElectron( const Electron& );
  JetElectron( const JetElectron& );
  ~JetElectron();
  JetParticle* Clone() const { return new JetElectron( *this ); }
  Particle* ConvertToParticle() const;
};

struct JetPositron : public JetParticle {
  JetPositron();
  JetPositron( double /* Energy */ );
  JetPositron( const Positron& );
  JetPositron( const JetPositron& );
  ~JetPositron();
  JetParticle* Clone() const { return new JetPositron( *this ); }
  Particle* ConvertToParticle() const;
};

struct JetMuon : public JetParticle {
  JetMuon();
  JetMuon( double /* Energy [GeV] */ );
  JetMuon( const Muon& );
  JetMuon( const JetMuon& );
  ~JetMuon();
  JetParticle* Clone() const { return new JetMuon( *this ); }
  Particle* ConvertToParticle() const;
};

struct JetAntiMuon : public JetParticle {
  JetAntiMuon();
  JetAntiMuon( double /* Energy [GeV] */ );
  JetAntiMuon( const AntiMuon& );
  JetAntiMuon( const JetAntiMuon& );
  ~JetAntiMuon();
  JetParticle* Clone() const { return new JetAntiMuon( *this ); }
  Particle* ConvertToParticle() const;
};

#endif // PARTICLE_H
