/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                ****
******  BEAMLINE:  C++ objects for design and analysis  .             ****
******             of beamlines, storage rings, and                   ****
******             synchrotrons.                                      ****
******                                                                ****
******  Copyright (c) 1991  Universities Research Association, Inc.   ****
******                All Rights Reserved                             ****
******                                                                ****
******  Author:    Leo Michelotti                                     ****
******                                                                ****
******             Fermilab                                           ****
******             P.O.Box 500                                        ****
******             Mail Stop 345                                      ****
******             Batavia, IL   60510                                ****
******                                                                ****
******             Phone: (708) 840 4956                              ****
******             Email: michelot@hazel.fnal.gov                     ****
******                    michelotti@adcalc.fnal.gov                  ****
******                    almond::michelotti                          ****
******                                                                ****
******  Release    Version 1.0 : << not released >>                   ****
******  Dates:                                                        ****
******                                                                ****
**************************************************************************
**************************************************************************
******                                                                ****
******  This material resulted from work developed under a            ****
******  Government Contract and is subject to the following           ****
******  license: The Government retains a paid-up, nonexclusive,      ****
******  irrevocable worldwide license to reproduce, prepare           ****
******  derivative works, perform publicly and display publicly       ****
******  by or for the Government, including the right to distribute   ****
******  to other Government contractors.  Neither the United          ****
******  States nor the United States Department of Energy, nor        ****
******  any of their employees, makes any warranty, express or        ****
******  implied, or assumes any legal liability or responsibility     ****
******  for the accuracy, completeness or usefulness of any           ****
******  information, apparatus, product, or process disclosed, or     ****
******  represents that its use would not infringe privately owned    ****
******  rights.                                                       ****
******                                                                ****
******  These files are made avaliable for use by specific            ****
******  individuals or a specific group.  They are not to be          ****
******  reproduced or redistributed.                                  ****
******                                                                ****
******  These files are provided "as is" with no warranties of any    ****
******  kind including the warranties of design,                      ****
******  merchantibility and fitness for a particular purpose,         ****
******  or arising from a course of dealing, usage or trade           ****
******  practice.                                                     ****
******                                                                ****
******  These files are provided with no support or obligation on     ****
******  the part of U.R.A. or Fermilab to assist in their use,        ****
******  correction, modification or enhancement.                      ****
******                                                                ****
******  Neither U.R.A. nor Fermilab shall be liable with              ****
******  respect to the infringement of copyrights, trade              ****
******  secrets or any patents by these files or any part             ****
******  thereof.                                                      ****
******                                                                ****
******  In no event will U.R.A., Fermilab, or the author(s) be        ****
******  liable for any lost revenue or profits or other               ****
******  special, indirect and consequential damages, even if          ****
******  they have been advised of the possibility of such             ****
******  damages.                                                      ****
******                                                                ****
**************************************************************************
*************************************************************************/

#ifndef BEAMLINE_H
#define BEAMLINE_H

//#include <complex.h>   // ??? Why is this here?
#ifndef __VISUAL_CPP__
#include <stdio.h>
#include <string.h>
#endif

#ifdef __SUNPRO_CC
  #ifndef __STL_NEED_EXPLICIT
    #define __STL_NEED_EXPLICIT
  #endif
#endif

#include "PhysicsConstants.h"
#include <math.h>
// #include <string>   (More trouble than it's worth.)
#include "MathConstants.h"
#include "mxyzptlk.h"
#include "slist.h"
#include "Barnacle.h"
#include "VectorD.h"
#include "JetVector.h"
#include "Particle.h"

#if defined (__VISUAL_CPP__) && !defined(__KCC)
#define strcasecmp stricmp
#endif

#ifdef __BORLAND_CPP__
#define strcasecmp strcmpi
#endif


// Predefined class names ...
class Particle;             // ??? These lines should not be
class ParticleBunch;        // ??? necessary.
class JetParticle;          // =============================


// ??? REMOVE: typedef void (*PROPFUNC)    ( bmlnElmnt*, Particle& );
// ??? REMOVE: typedef void (*JETPROPFUNC) ( bmlnElmnt*, JetParticle& );


// ................................ Beamline classes

#ifndef BMLNELMNT_H
#include "bmlnElmnt.h"
#endif

#ifndef KICK_H
#include "kick.h"
#endif

class octupole : public bmlnElmnt
{
// ??? REMOVE private:
// ??? REMOVE   void image( int, slist*, BMLN_posInfo* );
public:
  octupole( double, /* length   */
            double  /* strength */ );
  octupole( char*,  /* name     */
            double, /* length   */
            double  /* strength */ );
  octupole( const octupole& );
  octupole( bmlnElmntData& );
  ~octupole();

  void accept( BmlVisitor& v ) { v.visitOctupole( this ); }

  void setStrength( double );
  void setStrength( double, int );

  void setCurrent( double );

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "octupole") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new octupole( *this ); }
  void Split( double, bmlnElmnt**, bmlnElmnt** );
};


//
// *** class thinRFCavity written by Jian Ping Shan
// *** April 4, 1994
//
struct thinrfcavityData ;
class thinrfcavity : public bmlnElmnt
{
private:
  double w_rf;                  // RF frequency [MHz]  // ??? What??
  double phi_s;                 // synchronous phase
  double sin_phi_s;             // sine of synchronous phase
  // The max energy gain per turn [GeV] is represented by bmlnELmnt::strength
  double Q;                     // quality factor
  double R;                     // shunt impedance
  void image( int, slist*, BMLN_posInfo* );
  ostream& writeTo(ostream&);
  istream& readFrom(istream&);

public:
  thinrfcavity( char * ); // Name
  thinrfcavity( double,   // RF frequency [MHz]
                double,   // max energy gain per turn [eV] (strength)
                double,   // synchronous phase [radians]
                double,   // Q
                double    // R shunt impedance
                );
  thinrfcavity( char *,   // Name
                double,   // RF frequency [MHz]
                double,   // max energy gain per turn [eV] (strength)
                double,   // synchronous phase [radians]
                double,   // Q
                double    // R shunt impedance
                );
  thinrfcavity( const thinrfcavity& );
  thinrfcavity( thinrfcavityData& );
  ~thinrfcavity();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitThinrfcavity( this ); }

  thinrfcavityData* image();
  void eliminate();

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "thinrfcavity") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new thinrfcavity( *this ); }

};

struct thinrfcavityData : public bmlnElmntData {
  double w_rf;                  // RF frequency [MHz]  // ??? What??
  double phi_s;                 // synchronous phase
  double sin_phi_s;             // sine of synchronous phase
  // The max energy gain per turn [GeV] is represented by bmlnELmnt::strength
  double Q;                     // quality factor
  double R;                     // shunt impedance

  thinrfcavityData();
  thinrfcavityData( thinrfcavityData& );
  ~thinrfcavityData();
  void eliminate();
  void* clone();


};


class srot : public bmlnElmnt
{
// ??? REMOVE private:
// ??? REMOVE   void image( int, slist*, BMLN_posInfo* );
public:
  srot();
  srot( double /* strength = rotation angle in radians */ );
  srot( char * /* name */ );
  srot( char * /* name */, double /* strength */ );
  srot( const srot& );
  srot( bmlnElmntData& );
  ~srot();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitSrot( this ); }

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "srot") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new srot( *this ); }
};


struct monitorData;

class monitor : public bmlnElmnt
{
 private:
  double driftFraction;
protected:
  FILE*    outputFile;
  void     image( int, slist*, BMLN_posInfo* );
  char     onOff;
  double*  rgr;

public:
  monitor();                   // Data to be written to standard output
  monitor( char* );            // Name identifier.
  monitor( char*, double );    // Name identifier, length.
  monitor( FILE* );            // Data written to file.
  monitor( char*, FILE* );     //                      ... and name.
  monitor( bmlnElmntData& );
  monitor( const monitor& );
  virtual ~monitor();
  // ---------------------
  void setOutputFile( FILE* );
  virtual void on();
  virtual void off();
  double operator[]( int );    // Readout of data
  double setDriftFraction(double f) { if ( f <= 1 && f >= 0 ) driftFraction = f; return driftFraction; }
  double getDriftFraction() { return driftFraction; }
  virtual ostream& writeTo(ostream&);	// Is this "virtual" tag really necessary?  Probably!
  virtual istream& readFrom(istream&);
  void getState(double *s) { for ( int i=0; i<6; i++ ) s[i] = rgr[i]; }
  // ---------------------
  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle&   );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitMonitor( this ); }
  
  inline int State() const { return onOff; }
  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "monitor") != 0 ) return bmlnElmnt::isType(c); else return 1; }

  bmlnElmnt* Clone() const { return new monitor( *this ); }
} ;

class hmonitor : public monitor
{
public:
  hmonitor();                   // Data to be written to standard output
  hmonitor( char* );            // Name identifier.
  hmonitor( char*, double );    // Name identifier, length.
  hmonitor( FILE* );            // Data written to file.
  hmonitor( char*, FILE* );     // Data written to file ... and name.
  hmonitor( bmlnElmntData& );
  hmonitor( const hmonitor& );
  ~hmonitor();
  double operator[]( int );    // Readout of data
  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle&   );
  void localPropagate( JetParticle& );

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "hmonitor") != 0 ) return monitor::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new hmonitor( *this ); }
} ;

struct monitorData : public bmlnElmntData {
  char    onOff;
  double  driftFraction;

  monitorData();

  monitorData( monitorData& );
  ~monitorData();
  void eliminate();
  void* clone();

};


class vmonitor : public monitor
{
public:
  vmonitor();                   // Data to be written to standard output
  vmonitor( char* );            // Name identifier.
  vmonitor( char*, double );    // Name identifier, length.
  vmonitor( FILE* );            // Data written to file.
  vmonitor( char*, FILE* );     // Data written to file ... and name.
  vmonitor( bmlnElmntData& );
  vmonitor( const vmonitor& );
  ~vmonitor();
  double operator[]( int );    // Readout of data
  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle&   );
  void localPropagate( JetParticle& );

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "vmonitor") != 0 ) return monitor::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new vmonitor( *this ); }
} ;

#ifndef MARKER_H
#include "marker.h"
#endif

#ifndef DRIFT_H
#include "drift.h"
#endif


struct rbendData;
class rbend : public bmlnElmnt
{
private:
  // ??? REMOVE: static void P_Face    ( const bmlnElmnt*, Particle& );
  // ??? REMOVE: static void J_Face    ( const bmlnElmnt*, JetParticle& );
  double poleFaceAngle;
  double tanPFAngle;

  // bmlnElmnt::strength -> magnetic field [T]
  void image( int, slist*, BMLN_posInfo* );
  ostream& writeTo(ostream&);
  istream& readFrom(istream&);

public:

  class MAD_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "rbend::MAD_Prop"; }
  };
  static MAD_Prop LikeMAD;

  class NoEdge_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "rbend::NoEdge_Prop"; }

    NoEdge_Prop();
    ~NoEdge_Prop();
    char isApproximate();
    void makeApproximate();
    void makeExact();
  private:
    double _fastArcsin( double x     ) const;
    Jet    _fastArcsin( const Jet& x ) const;
    char   _approx;
  };
  static NoEdge_Prop NoEdge;

  class Exact_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "rbend::Exact_Prop"; }

    Exact_Prop();
    ~Exact_Prop();
    char isApproximate();
    void makeApproximate();
    void makeExact();
    void setPropagator( NoEdge_Prop* );
  private:
    NoEdge_Prop* _myPropagator;
  };
  static Exact_Prop Exact;

  class InEdge_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "rbend::InEdge_Prop"; }

    InEdge_Prop();
    ~InEdge_Prop();
    char isApproximate();
    void makeApproximate();
    void makeExact();
    void setPropagator( NoEdge_Prop* );
  private:
    NoEdge_Prop* _myPropagator;
  };
  static InEdge_Prop InEdge;

  class OutEdge_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "rbend::OutEdge_Prop"; }

    OutEdge_Prop();
    ~OutEdge_Prop();
    char isApproximate();
    void makeApproximate();
    void makeExact();
    void setPropagator( NoEdge_Prop* );
  private:
    NoEdge_Prop* _myPropagator;
  };
  static OutEdge_Prop OutEdge;

  // ------------------------------------------------------

  rbend( double,     // length  [ meters ]
         double,     // field   [ tesla ]
         PropFunc*    = &rbend::Exact );

  rbend( double,     // length  [ meters ]
         double,     // field   [ tesla ]
         double,     // Pole face angle [ radians ]
         PropFunc*    = &rbend::Exact );

  rbend( char*,      // name
         double,     // length  [ meters ]
         double,     // field   [ tesla ]
         PropFunc*    = &rbend::Exact );

  rbend( char*,      // name
         double,     // length  [ meters ]
         double,     // field   [ tesla ]
         double,     // Pole face angle [ radians ]
         PropFunc*    = &rbend::Exact );

  rbend( rbendData& );
  rbend( const rbend& );
  ~rbend();

  void eliminate();

  void accept( BmlVisitor& v ) { v.visitRbend( this ); }

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle&    p ) { (*Propagator)( this, p ); }
  void localPropagate( JetParticle& p ) { (*Propagator)( this, p ); }

  rbendData* image();

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "rbend") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new rbend( *this ); }
  double OrbitLength( const Particle& );
  void Split( double, bmlnElmnt**, bmlnElmnt** );


  double PoleFaceAngle()       const {return poleFaceAngle;} 
  double getPoleFaceAngle()    const {return poleFaceAngle;} 
  double getTanPoleFaceAngle() const {return tanPFAngle;}    
  double getEntryAngle()       const {return poleFaceAngle;} 

  double setPoleFaceAngle( const Particle& );
  double setPoleFaceAngle( const JetParticle& );
  void   setEntryAngle( const Particle& x )
  {
    this->setPoleFaceAngle( x );
  }
  void setEntryAngle( const JetParticle& x )
  {
    this->setPoleFaceAngle( x );
  }
  void setEntryAngle( double a )
  { 
     poleFaceAngle = a;
     tanPFAngle    = tan( poleFaceAngle );
  }
  double setAngle( double a )
  { 
     this->setEntryAngle(a);
     return poleFaceAngle;
  }
};

struct rbendData : public bmlnElmntData {
  double angle;     // pole face angle angle [ radians ]

  rbendData();
  rbendData( rbendData& );
  ~rbendData();
  void eliminate();
  void* clone();

};


struct sbendData;
class sbend : public bmlnElmnt
{
private:
  // bmlnElmnt::strength -> magnetic field [T]
  double angle;      // bend angle  [ radians ]
  void image( int, slist*, BMLN_posInfo* );
  ostream& writeTo(ostream&);
  istream& readFrom(istream&);

public:

  class MAD_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "sbend::MAD_Prop"; }
  };
  static MAD_Prop LikeMAD;

  class Exact_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "sbend::Exact_Prop"; }
  };
  static Exact_Prop Exact;

  class Approx_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "sbend::Fast_Prop"; }
  private:
    double fastArcsin( double x     ) const;
    Jet    fastArcsin( const Jet& x ) const;
  };
  static Approx_Prop Approx;

  void P_Face ( Particle&,    const double& /* psi */  ) const;
  void J_Face ( JetParticle&, const double& /* psi */  ) const;
  
  sbend( double,     // length  [ meters ]
         double,     // field   [ tesla ]
         double,     // angle   [ radians ]
         PropFunc*    = &sbend::Exact );

  sbend( char*,      // name
         double,     // length  [ meters ]
         double,     // magnetic field [T]
         double,     // angle   [ radians ]
         PropFunc*    = &sbend::Exact );

  sbend( sbendData& );
  sbend( const sbend& );
  ~sbend();

  double setAngle(double a)	{ return (angle = a); }
  double getAngle() { return angle; }

  void geomToEnd   ( BMLN_posInfo& );
  void geomToStart ( BMLN_posInfo& );
  void eliminate();

  void enterLocalFrame( Particle&    ) const;
  void enterLocalFrame( JetParticle& ) const;
  void leaveLocalFrame( Particle&    ) const;
  void leaveLocalFrame( JetParticle& ) const;

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle&    p ) { (*Propagator)( this, p ); }
  void localPropagate( JetParticle& p ) { (*Propagator)( this, p ); }

  void accept( BmlVisitor& v ) { v.visitSbend( this ); }

  // ??? REMOVE void localPropagate( Particle&   p );
  // ??? REMOVE void localPropagate( JetParticle& p );
  sbendData* image();

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "sbend") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new sbend( *this ); }
  void Split( double, bmlnElmnt**, bmlnElmnt** );

} ;

struct sbendData : public bmlnElmntData {
  double angle;     // bend angle [ radians ]

  sbendData();
  sbendData( sbendData& );
  ~sbendData();
  void eliminate();
  void* clone();

};


struct sectorData;
class sector : public bmlnElmnt
{
private:
  char   mapType;        // ??? Unnecessary. Get rid of this!
  // ??? REMOVE Jet*   map;
  /* NEW */ Mapping    myMap;
  double betaH     [2];  // 0 = entry;  1 = exit
  double alphaH    [2];
  double deltaPsiH;
  double betaV     [2];
  double alphaV    [2];
  double deltaPsiV;
  double mapMatrix[BMLN_dynDim][BMLN_dynDim];
  void image( int, slist*, BMLN_posInfo* );
  double (*DeltaT) ( double );
  Jet    (*JetDeltaT) ( const Jet& );
public:
  sector( double* betaH,  double* alphaH,  double* psiH,
          double* betaV,  double* alphaV,  double* psiV, double length );
  sector( char*, double* betaH,  double* alphaH,  double* psiH,
                 double* betaV,  double* alphaV,  double* psiV, double length );

  sector(             Jet*, double /* length */ );
  sector( char* name, Jet*, double );
  sector(             const Mapping&,  double /* length */ );
  sector( char* name, const Mapping&,  double );
  sector( sectorData& );
  sector( const sector& );
  ~sector();

  void geomToEnd   ( BMLN_posInfo& );
  void geomToStart ( BMLN_posInfo& );
  void eliminate();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitSector( this ); }

  sectorData* image();
  void setFrequency( double (*)( double ) );
  void setFrequency( Jet (*)( const Jet& ) );
  void setLength( double );

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "sector") != 0 ) return bmlnElmnt::isType(c); else return 1; }

  bmlnElmnt* Clone() const { return new sector( *this ); }
} ;

struct sectorData : public bmlnElmntData {
  char   mapType;
  Jet*    map;
  double betaH     [2];
  double alphaH    [2];
  double deltaPsiH;
  double betaV     [2];
  double alphaV    [2];
  double deltaPsiV;
  double mapMatrix[BMLN_dynDim][BMLN_dynDim];

  sectorData();
  sectorData( sectorData& );
  ~sectorData();
  void eliminate();
  void* clone();

};


class quadrupole : public bmlnElmnt
{
private:
  // ??? REMOVE   void image( int, slist*, BMLN_posInfo* );
  ostream& writeTo(ostream&);
  istream& readFrom(istream&);

public:
 
  class MAD_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "quadrupole::MAD_Prop"; }
  };
  static MAD_Prop LikeMAD;

  class TPOT_Prop : public bmlnElmnt::PropFunc
  {
  private:
    int _n;  // number of thinquad kicks
  public:
    TPOT_Prop( int /* _n */ = 4 );
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "quadrupole::TPOT_Prop"; }
    int  get_n() const;
    void set_n( int );
    void setup( quadrupole* ) const;
  };
  friend class TPOT_Prop;
  static TPOT_Prop LikeTPOT;

  quadrupole();
  quadrupole( char*  n,         // name
              double l,         // length,        in meters
              double s,         // strength = B', in Tesla-meters^-1
              PropFunc*    = &quadrupole::LikeTPOT );

  quadrupole( double l,         // length,        in meters
              double s,         // strength = B', in Tesla-meters^-1
              PropFunc*    = &quadrupole::LikeTPOT );


  quadrupole( bmlnElmntData& );
  quadrupole( const quadrupole& );
  ~quadrupole();

  void setStrength( double );
  void setStrength( double, int );

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle&    p ) { (*Propagator)( this, p ); }
  void localPropagate( JetParticle& p ) { (*Propagator)( this, p ); }

  void accept( BmlVisitor& v ) { v.visitQuadrupole( this ); }

  // ??? REMOVE void peekAt( double& s, Particle* = 0 );
  void eliminate();

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "quadrupole") != 0 ) return bmlnElmnt::isType(c); else return 1; }

  bmlnElmnt* Clone() const { return new quadrupole( *this ); }
  void Split( double, bmlnElmnt**, bmlnElmnt** );

} ;

class JetQuadrupole : public bmlnElmnt
{
 private:
  Jet JetStrength;
  int strengthIndex;
  istream& readFrom(istream&);
  ostream& writeTo(ostream&);

public:

  class MAD_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "JetQuadrupole::MAD_Prop"; }
  };
  static MAD_Prop LikeMAD;

  class TPOT_Prop : public bmlnElmnt::PropFunc
  {
  public:
  private:
    int _n;  // number of thinquad kicks
  public:
    TPOT_Prop( int /* _n */ = 4 );
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "JetQuadrupole::TPOT_Prop"; }
    int  get_n() const;
    void set_n( int );
    void setup( JetQuadrupole* ) const;
  };
  friend class TPOT_Prop;
  static TPOT_Prop LikeTPOT;

  JetQuadrupole();
  JetQuadrupole( char*  n,         // name
              double l,         // length,        in meters
              double s,         // strength = B', in Tesla-meters^-1
              int index,        // What jet index > 6
              int    m = 4,     // number of thinquad kicks
              PropFunc* = 0 );
  JetQuadrupole( double l,         // length,        in meters
              double s,         // strength = B', in Tesla-meters^-1
              int index,        // What jet index > 6
              int    m = 4,     // number of thinquad kicks
              PropFunc* = 0 );
  JetQuadrupole( bmlnElmntData& );
  JetQuadrupole( const JetQuadrupole& );
  ~JetQuadrupole();

  void setStrength( double );
  void setStrength( double, int );
  Jet getJetStrength() { return JetStrength; }

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle&    p ) { (*Propagator)( this, p ); }
  void localPropagate( JetParticle& p ) { (*Propagator)( this, p ); }

  void accept( BmlVisitor& v ) { v.visitJetQuadrupole( this ); }

  // ??? REMOVE void peekAt( double& s, Particle* = 0 );
  void eliminate();

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "JetQuadrupole") != 0 ) return bmlnElmnt::isType(c); else return 1; }

  bmlnElmnt* Clone() const { return new JetQuadrupole( *this ); }
  void Split( double, bmlnElmnt**, bmlnElmnt** );

} ;


// NULLIFIED   JetQuadrupoleData();
// NULLIFIED   JetQuadrupoleData( JetQuadrupoleData& );
// NULLIFIED   ~JetQuadrupoleData();
// NULLIFIED   void eliminate();
// NULLIFIED   void* clone();
// NULLIFIED 
// NULLIFIED };
// NULLIFIED 
// NULLIFIED 

class thinQuad : public bmlnElmnt
{
// ??? REMOVE private:
// ??? REMOVE   void image( int, slist*, BMLN_posInfo* );
public:
  thinQuad( double );    // B'L in Tesla; + = horizontally focussing
  thinQuad( char*,       // name
            double );    // B'L in Tesla; + = horizontally focussing
  thinQuad( bmlnElmntData& );
  thinQuad( const thinQuad& );
  ~thinQuad();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& p );
  void localPropagate( JetParticle& );
  void eliminate();

  void accept( BmlVisitor& v ) { v.visitThinQuad( this ); }

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "thinQuad") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  
  bmlnElmnt* Clone() const { return new thinQuad( *this ); }
} ;


class JetthinQuad : public bmlnElmnt
 {
 private:
   Jet KL;
   int strengthIndex;
  ostream& writeTo(ostream&);
  istream& readFrom(istream&);

 public:
   JetthinQuad( double, int );
   JetthinQuad( char*,      // name
                double,     // B-prime L in Tesla; + = horizontally focussing
                int );      // index of focal length parameter (> 6)
   JetthinQuad( bmlnElmntData& );
   JetthinQuad( const JetthinQuad& );
   ~JetthinQuad();
 
   void setStrength( double );
   void setStrength( double, int );
 
   void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
   void localPropagate( Particle& p );
   void localPropagate( JetParticle& );

   void accept( BmlVisitor& v ) { v.visitJetthinQuad( this ); }

   void eliminate();
 
   const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "JetthinQuad") != 0 ) return bmlnElmnt::isType(c); else return 1; }

   bmlnElmnt* Clone() const { return new JetthinQuad( *this ); }

} ;
 

class thinSextupole : public bmlnElmnt
{
// ??? REMOVE private:
// ??? REMOVE   void image( int, slist*, BMLN_posInfo* );
public:
  thinSextupole( double /* strength */ );
  thinSextupole( char*  /* name */,
                 double /* strength */ );
  thinSextupole( bmlnElmntData& );
  thinSextupole( const thinSextupole& );
  ~thinSextupole();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& p );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitThinSextupole( this ); }

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "thinSextupole") != 0 ) return bmlnElmnt::isType(c); else return 1; }

  bmlnElmnt* Clone() const { return new thinSextupole( *this ); }
} ;

class JetthinSext : public bmlnElmnt
{
private:
  Jet K2L;
  int strengthIndex;
  ostream& writeTo(ostream&);
  istream& readFrom(istream&);

public:
  JetthinSext( double, int );
  JetthinSext( char*,       // name
	       double,      // B-prime L in Tesla; + = horizontally focussing
	       int );       // index of focal length parameter (> 6)
  JetthinSext( bmlnElmntData& );
  JetthinSext( const JetthinSext& );
  ~JetthinSext();
  
  void setStrength( double );
  void setStrength( double, int );
  
  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& p );
  void localPropagate( JetParticle& );
  
  void accept( BmlVisitor& v ) { v.visitJetthinSext( this ); }

  void eliminate();
  
  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "JetthinSext") != 0 ) return bmlnElmnt::isType(c); else return 1; }

  bmlnElmnt* Clone() const { return new JetthinSext( *this ); }
  
} ;
 
class thinOctupole : public bmlnElmnt
{
// ??? REMOVE private:
// ??? REMOVE   void image( int, slist*, BMLN_posInfo* );
public:
  thinOctupole( double /* strength */ );
  thinOctupole( char*  /* name */,
                double /* strength */ );
  thinOctupole( bmlnElmntData& );
  thinOctupole( const thinOctupole& );
  ~thinOctupole();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& p );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitThinOctupole( this ); }

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "thinOctupole") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  
  bmlnElmnt* Clone() const { return new thinOctupole( *this ); }
} ;

class thinDecapole : public bmlnElmnt
{
// ??? REMOVE private:
// ??? REMOVE   void image( int, slist*, BMLN_posInfo* );
public:
  thinDecapole( double /* strength */ );
  thinDecapole( char*  /* name */,
                double /* strength */ );
  thinDecapole( bmlnElmntData& );
  thinDecapole( const thinDecapole& );
  ~thinDecapole();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& p );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitThinDecapole( this ); }

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "thinDecapole") != 0 ) return bmlnElmnt::isType(c); else return 1; }

  bmlnElmnt* Clone() const { return new thinDecapole( *this ); }
} ;

class thin12pole : public bmlnElmnt
{
public:
  thin12pole( double /* strength */ );
  thin12pole( char*  /* name */,
              double /* strength */ );
  thin12pole( bmlnElmntData& );
  thin12pole( const thin12pole& );
  ~thin12pole();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& p );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitThin12pole( this ); }

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "thin12pole") != 0 ) return bmlnElmnt::isType(c); else return 1; }

  bmlnElmnt* Clone() const { return new thin12pole( *this ); }
} ;

class thin14pole : public bmlnElmnt
{
public:
  thin14pole( double /* strength */ );
  thin14pole( char*  /* name */,
              double /* strength */ );
  thin14pole( bmlnElmntData& );
  thin14pole( const thin14pole& );
  ~thin14pole();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& p );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitThin14pole( this ); }

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "thin14pole") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new thin14pole( *this ); }
} ;

class thin16pole : public bmlnElmnt
{
public:
  thin16pole( double /* strength */ );
  thin16pole( char*  /* name */,
              double /* strength */ );
  thin16pole( bmlnElmntData& );
  thin16pole( const thin16pole& );
  ~thin16pole();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& p );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitThin16pole( this ); }

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "thin16pole") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new thin16pole( *this ); }
} ;

class thin18pole : public bmlnElmnt
{
public:
  thin18pole( double /* strength */ );
  thin18pole( char*  /* name */,
              double /* strength */ );
  thin18pole( bmlnElmntData& );
  thin18pole( const thin18pole& );
  ~thin18pole();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& p );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitThin18pole( this ); }

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "thin18pole") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new thin18pole( *this ); }
} ;

class thinMultipole : public bmlnElmnt
{
// ??? REMOVE private:
// ??? REMOVE   void image( int, slist*, BMLN_posInfo* );
public:
  thinMultipole( double /* strength */ );
  thinMultipole( char*  /* name */,
                double /* strength */ );
  thinMultipole( bmlnElmntData& );
  thinMultipole( const thinMultipole& );
  ~thinMultipole();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& p );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitThinMultipole( this ); }

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "thinMultipole") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new thinMultipole( *this ); }
} ;


class sextupole : public bmlnElmnt
{
// ??? REMOVE private:
// ??? REMOVE   void image( int, slist*, BMLN_posInfo* );
public:
  sextupole( double, /* length   */
             double  /* strength */ );
  sextupole( char*,  /* name     */
             double, /* length   */
             double  /* strength */ );
  sextupole( bmlnElmntData& );
  sextupole( const sextupole& );
  ~sextupole();

  void setStrength( double );
  void setStrength( double, int );

  void setCurrent( double );

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& p );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitSextupole( this ); }

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "sextupole") != 0 ) return bmlnElmnt::isType(c); else return 1; }

  bmlnElmnt* Clone() const { return new sextupole( *this ); }
  void Split( double, bmlnElmnt**, bmlnElmnt** );
} ;


class BBLens : public bmlnElmnt
{
private:
  double emittance[3];   // One sigma (noninvariant) emittance / pi
  double gamma;          // Relativistic gamma
  double beta;           // Relativistic beta
  double num;            // Number of proton charges; if the bunch
                         // is negatively charged, then this number
                         // is negative.
  double sigma[3];       // This will depend on position in the 
                         // lattice.  The third component is zero
                         // for now.
public:
  BBLens( const char*   = 0
             /* name */,
          double        = 1.0
             /* length [m]: the length of the bunch
                in its rest frame */,
          double        = 3.3e11
             /* strength : total number of proton
                charges in the bunch */,
          double        = 1000.0
             /* gamma of the bunch, in the lab frame */,
          const double* = 0
             /* pointer to an array containing
                three values for 6 X invariant emittance [m] / pi  */
        );
  BBLens( const BBLens& );
  ~BBLens();

  char useRound;         // By default = 1
                         // If 1: then round beam approximation
                         // used when horizontal and vertical 
                         // sigmas approximately equal.

  void Kludge( double  /* num    */, 
               double  /* gamma  */, 
         const double* /* sigma  */ );
  void KludgeNum( double ); 
  void KludgeSigma( const double* );
  void AdjustSigma();

  Vector NormalizedEField( double x, double y );
             /* returns the "normalized" electric field
                in the rest frame of the bunch, in inverse
                meters.  To get the field [V/m], this must
                be multiplied by Q/(2 pi epsilon_o), where
                Q is the line density of charge [C/m] (in
                rest frame). */

  JetVector NormalizedEField( const Jet& x, const Jet& y );
             /* returns the "normalized" electric field
                in the rest frame of the bunch, in inverse
                meters.  To get the field [V/m], this must
                be multiplied by Q/(2 pi epsilon_o), where
                Q is the line density of charge [C/m] (in
                rest frame). */

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitBBLens( this ); }

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "BBLens") != 0 ) return bmlnElmnt::isType(c); else return 1; }

  bmlnElmnt* Clone() const { return new BBLens( *this ); }
  Vector Beta();
  void GetSigma( double* );
};

struct thinSeptumData;
class thinSeptum : public bmlnElmnt
{
private:
  double strengthPos;    // kick in strength in radians for x > xWire
  double strengthNeg;	 // kick in strength in radians for x < xWire
  double xWire;		 // position of wire septum in meters
  void image( int, slist*, BMLN_posInfo* );
  ostream& writeTo(ostream&);
  istream& readFrom(istream&);

public:
  
  thinSeptum();
  
  thinSeptum( char*  n );
  
  thinSeptum( char*  n,      // name
	    double sPos,    // kick in strength in radians for x > xWire 
	    double sNeg,    // kick in strength in radians for x < xWire 
	    double x );     // position of wire septum in meters
   
  thinSeptum( double sPos,    
	    double sNeg,    
	    double x ); 
  
  thinSeptum( thinSeptumData& );
  thinSeptum( const thinSeptum& );
  ~thinSeptum();
  
  void setStrengths( double sPos, double sNeg); 
  void setWire( double x); 
  
  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle&    p );
  void localPropagate( JetParticle& p );

  void accept( BmlVisitor& v ) { v.visitThinSeptum( this ); }

  thinSeptumData* image();
  void eliminate();
  
  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "thinSeptum") != 0 ) return bmlnElmnt::isType(c); else return 1; }

  bmlnElmnt* Clone() const { return new thinSeptum( *this ); }

};
 
struct thinSeptumData : public bmlnElmntData {
  double strengthPos;
  double strengthNeg;
  double xWire;
  
  thinSeptumData();
  thinSeptumData( thinSeptumData& );
  ~thinSeptumData();
  void eliminate();
  void* clone();

};

 
struct thinLambData;
class thinLamb : public bmlnElmnt
{
private:
  double    xSeptum;	   // position of Lambertson septum in meters.
  beamline* ExtBeamline;   // pointer to the beamline of extracted particles.
  double    RefState[6];   // A particle in the "field" region
                           // with phase space state of RefState()
                           // will be on the reference orbit of the
                           // extraction line.  The specified coordinates are 
                           // SUBTRACTED from the particle coordinates.
  void image( int, slist*, BMLN_posInfo* );
  ostream& writeTo(ostream&);
  istream& readFrom(istream&);

public:
  
  thinLamb();
  thinLamb( char* n);      // name
  
  thinLamb( char*  n,      // name
	   double x,       // x position of the septum.
	   beamline* b,    // extracted beamline pointer.
	   double* s );    // reference state for extraction beamline.
  
  thinLamb( double x,      // x position of the septum.
	   beamline* b,    // extracted beamline pointer.
	   double* s );    // reference state for extraction beamline.
  
  thinLamb( thinLambData& );
  thinLamb( const thinLamb& );
  ~thinLamb();
  
  void setSeptum( double x); 
  void setBeamline( beamline* b); 
  void setRefState( const double* s);

  void getRefState( double* );
  
  void localPropagate( ParticleBunch& x );
  void localPropagate( Particle&    p );
  void localPropagate( JetParticle& p );
  
  void accept( BmlVisitor& v ) { v.visitThinLamb( this ); }

  thinLambData* image();
  void eliminate();
  
  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "thinLamb") != 0 ) return bmlnElmnt::isType(c); else return 1; }

  bmlnElmnt* Clone() const { return new thinLamb( *this ); }
};
 

struct thinLambData : public bmlnElmntData {
  double xSeptum;
  beamline* ExtBeamline;
  double RefState[6];
  
  thinLambData();
  thinLambData( thinLambData& );
  ~thinLambData();
  void eliminate();
  void* clone();
};
 
 

#ifndef BEAMLINE_ITERATOR
#include "BeamlineIterator.h"
#endif


class circuit : public dlist
{
protected:
  char onOffSwitch;     // switch to turn current on and off
  char* ident;          // name of circuit
  int numElm;           // number of elements in the circuit
public:
  circuit();
  circuit( const char* );
  circuit( bmlnElmnt* );
  circuit( const char*, bmlnElmnt* );
  circuit(const circuit&);
  virtual ~circuit();
  virtual void switchOn();
  virtual void switchOff();
  virtual void set( void* );
  virtual void get( void* );
  char* getName();

  void append( bmlnElmnt* q );
} ;

class ICircuit : public circuit {
protected:
  double current;
public:
  ICircuit();
  ~ICircuit();
  ICircuit( const char* );
  ICircuit( bmlnElmnt* );
  ICircuit( const char*, bmlnElmnt* );
  void switchOn();
  void switchOff();
  void set( void* );
  void get( void* );
};

class FCircuit : public circuit {
protected:
  double field;
public:
  FCircuit();
  ~FCircuit();
  FCircuit( const char* );
  FCircuit( bmlnElmnt* );
  FCircuit( const char*, bmlnElmnt* );
  void switchOn();
  void switchOff();
  void set( void* );
  void get( void* );
  void setCurrent( void* );
  void getCurrent( void* );
  void append( bmlnElmnt* );
};

class mover : public circuit {
protected:
  alignmentData align;
public:
  mover();
  ~mover();
  mover( const char* );
  mover( bmlnElmnt* );
  mover( const char*, bmlnElmnt* );
  void append( bmlnElmnt* );
  void switchOn();
  void switchOff();
  void set( void* );
  void get( void* );

};

struct PolarityBarn : public BarnacleData {
  double polarity;
  PolarityBarn(double x) : polarity(x) {}
  PolarityBarn(const PolarityBarn& x){polarity = x.polarity;}
  ~PolarityBarn() {}
};

class BipolarCircuit : public circuit {
protected:
  double field;
public:
  BipolarCircuit();
  ~BipolarCircuit();
  BipolarCircuit( const char* );
  BipolarCircuit( bmlnElmnt* );
  BipolarCircuit( const char*, bmlnElmnt* );
  void switchOn();
  void switchOff();
  void set( void* );
  void get( void* );
  void append( bmlnElmnt* );

};

#endif // BEAMLINE_H
