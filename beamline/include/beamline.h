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

#include <complex.h>   // ??? Why is this here?
#include <stdio.h>
#include <string.h>
#include "PhysicsConstants.h"
#include "MathConstants.h"
#include "mxyzptlk.h"
#include "slist.h"
#include "Barnacle.h"
#include "VectorD.h"
#include "JetVector.h"
#include "Particle.h"
#include "Aperture.h" // O.K.

#ifdef __BORLAND_CPP__
#define strcasecmp strcmpi
#endif

#define OSTREAM_DOUBLE_PREC setprecision(20)

class bmlnElmnt;
class Particle;             // ??? These lines should not be
class ParticleBunch;        // ??? necessary.
class JetParticle;          // =============================

typedef void (*PROPFUNC)    ( bmlnElmnt*, Particle& );
typedef void (*JETPROPFUNC) ( bmlnElmnt*, JetParticle& );


struct BMLN_posInfo {
    Vector inPoint;      // This struct defines the upstream and
    Vector inAxes  [3];  // downstream faces of the magnets slot.
    Vector outPoint;
    Vector outAxes [3];
    BMLN_posInfo( );
    BMLN_posInfo( BMLN_posInfo& );
    BMLN_posInfo& operator=( const BMLN_posInfo& );   // ??? Does this struct
                                                      // ??? need a destructor?
    friend ostream& operator<<(ostream&, BMLN_posInfo&);

    } ;


// ................................ Beamline classes

struct lattRing : public BarnacleData {
  struct {
    double hor;
    double ver;
  } tune;
  struct {
    double hor;
    double ver;
  } chromaticity;

  lattRing();
  ~lattRing() {}
  lattRing& operator=( const lattRing& );
} ;
 ostream& operator<<(ostream&, const lattRing&);

struct lattFunc : public BarnacleData {
  double arcLength;
  struct {
    double hor;
    double ver;
  } dispersion;
  struct {
    double hor;
    double ver;
  } dPrime;
  struct {
    double hor;
    double ver;
  } beta;
  struct {
    double hor;
    double ver;
  } alpha;
  struct {
    double hor;
    double ver;
  } psi;

  lattFunc();
  ~lattFunc() {}
  lattFunc& operator=( const lattFunc& );
} ;
 ostream& operator<<(ostream&, const lattFunc&);

struct alignmentData {
  double       xOffset;
  double       yOffset;
  double       tilt;
  alignmentData();
  alignmentData(const alignmentData&);
  ~alignmentData();
  alignmentData& operator=(const alignmentData&);
};


class alignment {
private:
  double       xOffset;         // offset in meters
  double       yOffset;         // offset in meters
  double       tilt;            // roll in radians
                                // we could put in pitch someday
public:
  alignment();
  alignment(const double,const double,const double);
  alignment(const alignment&);
  alignment(const alignmentData&);
  ~alignment();
  alignment& operator=(const alignment&);
                                // these methods will overwrite an array
                                // BMLM_dynDim in length
                                // I don\'t think that the Particle or
                                // JetParticle should be modified directly
  void align(const Particle&, BMLN_posInfo, double*);
  void align(double*, BMLN_posInfo, double*);
  void misalign(const Particle&, BMLN_posInfo, double*);
  void align(JetParticle&, BMLN_posInfo, Jet*);
  void align(Jet*, BMLN_posInfo, Jet*);
  void misalign(JetParticle&, BMLN_posInfo, Jet*);
  void setAlignment(const alignmentData&);
  alignmentData getAlignment();
  friend ostream& operator<<(ostream&, alignment&);
};


struct bmlnElmntData {
  char            type[80];
  bmlnElmnt*      address;
  char*           name;
  int             depth;
  double          length;
  double          strength;

    //
    // O.K.
    //
    //  double          aperture;

  Aperture*       pAperture;
  
  alignmentData   align;
  BMLN_posInfo    geometry;
  double          iToField;
  double	  shuntCurrent;
  char            more;

  bmlnElmntData();
  bmlnElmntData( bmlnElmntData& );
  virtual ~bmlnElmntData();
  virtual void eliminate();
  virtual void* clone();
};

class beamline;
class bmlnElmnt
{
private:
  /* All the work is done in friend ostream& operator<<(),
     placeholder for if descendants want to do somthing. */
  virtual ostream& writeTo(ostream& os) 	{ return os; }
  virtual istream& readFrom(istream& is)	{ return is; };
  friend ostream& operator<<(ostream&, bmlnElmnt&);
  friend bmlnElmnt* read_istream(istream&);

protected:
  char*        ident;      // Name identifier of the element.
  double       length;     // Length of object [ meters ]
  double       strength;   // Interpretation depends on object.
  int          depth;      // Depth of objects below this element.
  
  alignment*   align;
  double       iToField;   // Conversion factor for current through
                           // magnet in amperes to field or gradient etc.
  double       shuntCurrent; // Does this element have a shunt?
  BMLN_posInfo geometry;

  beamline*    p_bml;      // The element may be composite.
  bmlnElmnt*   p_bml_e;    // with one active part.

  PROPFUNC     Propagator;
  JETPROPFUNC  JetPropagator;

  virtual void image( int,           // depth of image
                      slist*,        // list to append responses to
                      BMLN_posInfo*  // current geometry, updated
                    );
  virtual void geomToEnd   ( BMLN_posInfo& );
  virtual void geomToStart ( BMLN_posInfo& );
  virtual void eliminate();

  friend class beamline;
  // friend class circuit;
  friend beamline& operator-( beamline& );

public:
  Aperture*    pAperture;  // O.K.
  
  bmlnElmnt( const char*   n = "NONAME" /* name     */,
             PROPFUNC        = 0,
             JETPROPFUNC     = 0
           );		   
  bmlnElmnt( const double  l /* length   */,
             PROPFUNC        = 0,
             JETPROPFUNC     = 0
           );		   
  bmlnElmnt( const double  l /* length   */,
             const double  s /* strength */,
             PROPFUNC        = 0,
             JETPROPFUNC     = 0
           );		   
  bmlnElmnt( const char*   n /* name     */,
             const double  l /* length   */,
             PROPFUNC        = 0,
             JETPROPFUNC     = 0
           );		   
  bmlnElmnt( const char*   n /* name     */,
             const double  l /* length   */,
             const double  s /* strength */,
             PROPFUNC        = 0,
             JETPROPFUNC     = 0
           );
  bmlnElmnt( bmlnElmnt&  );
  bmlnElmnt( bmlnElmntData& );

  virtual ~bmlnElmnt();

  BarnacleList dataHook;   // Carries data as service to application program.
//  lattFunc     lattInfo;   // Information on maps and lattice functions.
  bmlnElmnt& operator=( const bmlnElmnt& );

  void setPropFunction(PROPFUNC a)	{ Propagator = a; }
  void setJPropFunction(JETPROPFUNC a)	{ JetPropagator = a; }

  virtual void propagate( ParticleBunch& );
  virtual void propagate( Particle& );
  virtual void propagate( JetParticle& );

  bmlnElmntData* image();
  void image( bmlnElmntData* );

  // Editing functions
  virtual bmlnElmnt* Clone() = 0;  // Each type of bmlnElmnt will
                                   // create a copy of itself and
                                   // return a pointer to the new
                                   // object.

  virtual void Split( double s, bmlnElmnt**, bmlnElmnt** );
                                   // Splits the element at percent orbitlength
                                   // s from the in-face and returns
                                   // addresses to the two pieces
                                   // in the second and third arguments.
                                   // The user has responsibility for
                                   // eventually deleting these.
  virtual void peekAt( double& s, Particle* = 0 );

  virtual void setStrength   ( const double );
  virtual void setStrength   ( const double, const int );  // for JetQuad
  virtual void setCurrent    ( const double );
  virtual void setAlignment  ( const alignmentData& );
          void set           ( const bmlnElmntData& );
          void setAperture   ( Aperture* );
          void Rename        ( const char* );
  void setShunt(double a);

  // Query functions ...
  alignmentData  Alignment( void );
  Aperture*      getAperture( void );	// returns a clone of the aperture class
  inline int     hasAperture( void ) const { return  ( pAperture ? 1 : 0 ); }
  inline double  Strength() 	const { return strength; }
  inline double  Length() 	const { return length; }
  virtual double Current() 	const { return strength/iToField; }
  inline char    IsYourName( const char* x ) { return strcmp( x, ident ); }
  inline char*   Name() { return ident; }
  virtual char*  Type() = 0;
  virtual double OrbitLength( const Particle& ) { return length; }
                                   // Returns length of design orbit
                                   // segment through the element.
                                   // Will be different from "length"
                                   // for rbends.
  double& IToField() 		{ return iToField; }
  double  getShunt() const	{ return shuntCurrent; }

#ifdef OBJECT_DEBUG
  static int objectCount;
#endif

};

class hkick : public bmlnElmnt
{
// ??? REMOVE private:
// ??? REMOVE   void image( int, slist*, BMLN_posInfo* );
public:
  hkick(        /* Assumes zero kick */ );
  hkick( double /* kick size in radians */ );
  hkick( char * /* name */  /* Assumes zero kick */ );
  hkick( char * /* name */, double /* kick size in radians */ );
  hkick( const hkick& );
  hkick( bmlnElmntData& );
  ~hkick();
  void propagate( ParticleBunch& x ) { bmlnElmnt::propagate( x ); }
  void propagate( Particle& );
  void propagate( JetParticle& );
  char* Type() { return "hkick"; }
  bmlnElmnt* Clone() { return new hkick( *this ); }
};

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

  void setStrength( const double );
  void setStrength( const double, const int );

  void setCurrent( const double );

  void propagate( ParticleBunch& x ) { bmlnElmnt::propagate( x ); }
  void propagate( Particle& );
  void propagate( JetParticle& );

  char* Type() { return "octupole"; }
  bmlnElmnt* Clone() { return new octupole( *this ); }
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
  double eV;                    // max energy gain per turn [eV]
  double Q;                     // quality factor
  double R;                     // shunt impedance
  void image( int, slist*, BMLN_posInfo* );
  ostream& writeTo(ostream&);
  istream& readFrom(istream&);

public:
  thinrfcavity( char * ); // Name
  thinrfcavity( double,   // RF frequency [MHz]
                double,   // max energy gain per turn [eV]
                double,   // synchronous phase [radians]
                double,   // Q
                double    // R shunt impedance
                );
  thinrfcavity( char *,   // Name
                double,   // RF frequency [MHz]
                double,   // max energy gain per turn [eV]
                double,   // synchronous phase [radians]
                double,   // Q
                double    // R shunt impedance
                );
  thinrfcavity( thinrfcavity& );
  thinrfcavity( thinrfcavityData& );
  ~thinrfcavity();

  void propagate( ParticleBunch& x ) { bmlnElmnt::propagate( x ); }
  void propagate( Particle& );
  void propagate( JetParticle& );
  thinrfcavityData* image();
  void eliminate();

  char* Type() { return "thinrfcavity"; }
  bmlnElmnt* Clone() { return new thinrfcavity( *this ); }

};

struct thinrfcavityData : public bmlnElmntData {
  double w_rf;                  // RF frequency [MHz]  // ??? What??
  double phi_s;                 // synchronous phase
  double sin_phi_s;             // sine of synchronous phase
  double eV;                    // max energy gain per turn [eV]
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

  void propagate( ParticleBunch& x ) { bmlnElmnt::propagate( x ); }
  void propagate( Particle& );
  void propagate( JetParticle& );

  char* Type() { return "srot"; }
  bmlnElmnt* Clone() { return new srot( *this ); }
};


class vkick : public bmlnElmnt
{
// ??? REMOVE private:
// ??? REMOVE   void image( int, slist*, BMLN_posInfo* );
public:
  vkick(        /* Assumes zero kick */ );
  vkick( double /* kick size in radians */ );
  vkick( char * /* name */ /* Assumes zero kick */ );
  vkick( char * /* name */, double /* kick size in radians */ );
  vkick( const vkick& );
  vkick( bmlnElmntData& );
  ~vkick();

  void propagate( ParticleBunch& x ) { bmlnElmnt::propagate( x ); }
  void propagate( Particle& );
  void propagate( JetParticle& );

  char* Type() { return "vkick"; }
  bmlnElmnt* Clone() { return new vkick( *this ); }
};


struct monitorData;

class monitor : public bmlnElmnt
{
protected:
  FILE*    outputFile;
  void     image( int, slist*, BMLN_posInfo* );
  char     onOff;
  double*  rgr;

public:
  monitor();                   // Data to be written to standard output
  monitor( char* );            // Name identifier.
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
  // ---------------------
  void propagate( ParticleBunch& x ) { bmlnElmnt::propagate( x ); }
  void propagate( Particle&   );
  void propagate( JetParticle& );
  inline int State() const {return onOff;}
  char* Type() { return "monitor"; }
  bmlnElmnt* Clone() { return new monitor( *this ); }
} ;

class hmonitor : public monitor
{
public:
  hmonitor();                   // Data to be written to standard output
  hmonitor( char* );            // Name identifier.
  hmonitor( FILE* );            // Data written to file.
  hmonitor( char*, FILE* );     // Data written to file ... and name.
  hmonitor( bmlnElmntData& );
  hmonitor( const hmonitor& );
  ~hmonitor();
  double operator[]( int );    // Readout of data
  void propagate( ParticleBunch& x ) { bmlnElmnt::propagate( x ); }
  void propagate( Particle&   );
  void propagate( JetParticle& );

  char* Type() { return "hmonitor"; }
  bmlnElmnt* Clone() { return new hmonitor( *this ); }
} ;

struct monitorData : public bmlnElmntData {
  char    onOff;

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
  vmonitor( FILE* );            // Data written to file.
  vmonitor( char*, FILE* );     // Data written to file ... and name.
  vmonitor( bmlnElmntData& );
  vmonitor( const vmonitor& );
  ~vmonitor();
  double operator[]( int );    // Readout of data
  void propagate( ParticleBunch& x ) { bmlnElmnt::propagate( x ); }
  void propagate( Particle&   );
  void propagate( JetParticle& );

  char* Type() { return "vmonitor"; }
  bmlnElmnt* Clone() { return new vmonitor( *this ); }
} ;

class marker : public bmlnElmnt
{
// ??? REMOVE private:
// ??? REMOVE   void image( int, slist*, BMLN_posInfo* );
public:
  marker();                   // Data to be written to standard output
  marker( char* );            // Name identifier.
  marker( marker& );
  marker( bmlnElmntData& );
  ~marker();

  void propagate( ParticleBunch& x ) { bmlnElmnt::propagate( x ); }
  void propagate( Particle&   );
  void propagate( JetParticle& );

  char* Type() { return "marker"; }
  bmlnElmnt* Clone() { return new marker( *this ); }
} ;

class drift : public bmlnElmnt
{
// ??? REMOVE private:
// ??? REMOVE   void image( int, slist*, BMLN_posInfo* );
public:
  drift();
  drift( double );             // length of drift in meters
  drift( char* );              // name
  drift( char*,                // name
         double );             // length of drift in meters
  drift( bmlnElmntData& );
  drift( const drift& );
  ~drift();

  char* Type() { return "drift"; }
  bmlnElmnt* Clone() { return new drift( *this ); }
} ;


struct rbendData;
class rbend : public bmlnElmnt
{
private:
  static void P_OutFace    ( bmlnElmnt*, Particle& );
  static void J_OutFace    ( bmlnElmnt*, JetParticle& );
  static void P_InFace    ( bmlnElmnt*, Particle& );
  static void J_InFace    ( bmlnElmnt*, JetParticle& );
  double poleFaceAngle;

  // bmlnElmnt::strength -> magnetic field [T]
  void image( int, slist*, BMLN_posInfo* );
  ostream& writeTo(ostream&);
  istream& readFrom(istream&);

public:
  static void P_LikeMAD    ( bmlnElmnt*, Particle& );
  static void J_LikeMAD    ( bmlnElmnt*, JetParticle& );
  static void P_Exact      ( bmlnElmnt*, Particle& );
  static void J_Exact      ( bmlnElmnt*, JetParticle& );
  static void P_NoEdge     ( bmlnElmnt*, Particle& );
  static void J_NoEdge     ( bmlnElmnt*, JetParticle& );
  static void P_InEdge     ( bmlnElmnt*, Particle& );
  static void J_InEdge     ( bmlnElmnt*, JetParticle& );
  static void P_OutEdge    ( bmlnElmnt*, Particle& );
  static void J_OutEdge    ( bmlnElmnt*, JetParticle& );

  rbend( double,     // length  [ meters ]
         double,     // field   [ tesla ]
#ifdef __GNUG__
         PROPFUNC    = 0,
         JETPROPFUNC = 0 );
#else
         PROPFUNC    = rbend::P_Exact,
         JETPROPFUNC = rbend::J_Exact );
#endif
  rbend( double,     // length  [ meters ]
         double,     // field   [ tesla ]
         double,     // Pole face angle [ radians ]
#ifdef __GNUG__
         PROPFUNC    = 0,
         JETPROPFUNC = 0 );
#else
         PROPFUNC    = rbend::P_Exact,
         JETPROPFUNC = rbend::J_Exact );
#endif


  rbend( char*,      // name
         double,     // length  [ meters ]
         double,     // field   [ tesla ]
#ifdef __GNUG__
         PROPFUNC    = 0,
         JETPROPFUNC = 0 );
#else
         PROPFUNC    = rbend::P_Exact,
         JETPROPFUNC = rbend::J_Exact );
#endif

  rbend( char*,      // name
         double,     // length  [ meters ]
         double,     // field   [ tesla ]
         double,     // Pole face angle [ radians ]
#ifdef __GNUG__
         PROPFUNC    = 0,
         JETPROPFUNC = 0 );
#else
         PROPFUNC    = rbend::P_Exact,
         JETPROPFUNC = rbend::J_Exact );
#endif

  rbend( rbendData& );
  rbend( const rbend& );
  ~rbend();

  double setAngle(double a) 	{ return (poleFaceAngle = a); }
  void eliminate();

  void propagate( ParticleBunch& x ) { bmlnElmnt::propagate( x ); }
  void propagate( Particle&    p ) {    Propagator( this, p ); }
  void propagate( JetParticle& p ) { JetPropagator( this, p ); }
  rbendData* image();

  char* Type() { return "rbend"; }
  bmlnElmnt* Clone() { return new rbend( *this ); }
  double OrbitLength( const Particle& );
  void Split( double, bmlnElmnt**, bmlnElmnt** );
  double& PoleFaceAngle() {return poleFaceAngle;}

} ;

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
  static void P_LikeMAD    ( bmlnElmnt*, Particle& );
  static void J_LikeMAD    ( bmlnElmnt*, JetParticle& );
  static void P_Exact      ( bmlnElmnt*, Particle& );
  static void J_Exact      ( bmlnElmnt*, JetParticle& );
  
  sbend( double,     // length  [ meters ]
         double,     // field   [ tesla ]
         double,     // angle   [ radians ]
#ifdef __GNUG__
         PROPFUNC    = 0,
         JETPROPFUNC = 0 );
#else
         PROPFUNC    = sbend::P_Exact,
         JETPROPFUNC = sbend::J_Exact );
#endif

  sbend( char*,      // name
         double,     // length  [ meters ]
         double,     // magnetic field [T]
         double,     // angle   [ radians ]
#ifdef __GNUG__
         PROPFUNC    = 0,
         JETPROPFUNC = 0 );
#else
         PROPFUNC    = sbend::P_Exact,
         JETPROPFUNC = sbend::J_Exact );
#endif

  sbend( sbendData& );
  sbend( const sbend& );
  ~sbend();

  double setAngle(double a)	{ return (angle = a); }
  void geomToEnd   ( BMLN_posInfo& );
  void geomToStart ( BMLN_posInfo& );
  void eliminate();

  void propagate( ParticleBunch& x ) { bmlnElmnt::propagate( x ); }
  void propagate( Particle&    p ) {    Propagator( this, p ); }
  void propagate( JetParticle& p ) { JetPropagator( this, p ); }

  // ??? REMOVE void propagate( Particle&   p );
  // ??? REMOVE void propagate( JetParticle& p );
  sbendData* image();

  char* Type() { return "sbend"; }
  bmlnElmnt* Clone() { return new sbend( *this ); }
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
  double (*DeltaT) ( const double );
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

  void propagate( ParticleBunch& x ) { bmlnElmnt::propagate( x ); }
  void propagate( Particle& );
  void propagate( JetParticle& );
  sectorData* image();
  void setFrequency( double (*)( const double ) );
  void setFrequency( Jet (*)( const Jet& ) );

  char* Type() { return "sector"; }
  bmlnElmnt* Clone() { return new sector( *this ); }
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
  static void P_LikeMAD    ( bmlnElmnt*, Particle& );
  static void J_LikeMAD    ( bmlnElmnt*, JetParticle& );
  static void P_LikeTPOT   ( bmlnElmnt*, Particle& );
  static void J_LikeTPOT   ( bmlnElmnt*, JetParticle& );

  quadrupole();
  quadrupole( char*  n,         // name
              double l,         // length,        in meters
              double s,         // strength = B', in Tesla-meters^-1
              int    m = 4,     // number of thinquad kicks
#ifdef __GNUG__
         PROPFUNC    = 0,
         JETPROPFUNC = 0 );
#else
              PROPFUNC    = quadrupole::P_LikeTPOT,
              JETPROPFUNC = quadrupole::J_LikeTPOT );
#endif
  quadrupole( double l,         // length,        in meters
              double s,         // strength = B', in Tesla-meters^-1
              int    m = 4,     // number of thinquad kicks
#ifdef __GNUG__
         PROPFUNC    = 0,
         JETPROPFUNC = 0 );
#else
              PROPFUNC    = quadrupole::P_LikeTPOT,
              JETPROPFUNC = quadrupole::J_LikeTPOT );
#endif
  quadrupole( bmlnElmntData& );
  quadrupole( const quadrupole& );
  ~quadrupole();

  void setStrength( const double );
  void setStrength( const double, const int );

  void propagate( ParticleBunch& x ) { bmlnElmnt::propagate( x ); }
  void propagate( Particle&    p ) {    Propagator( this, p ); }
  void propagate( JetParticle& p ) { JetPropagator( this, p ); }

  // ??? REMOVE void peekAt( double& s, Particle* = 0 );
  void eliminate();

  char* Type() { return "quadrupole"; }
  bmlnElmnt* Clone() { return new quadrupole( *this ); }
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
  static void P_LikeMAD    ( bmlnElmnt*, Particle& );
  static void J_LikeMAD    ( bmlnElmnt*, JetParticle& );
  static void P_LikeTPOT   ( bmlnElmnt*, Particle& );
  static void J_LikeTPOT   ( bmlnElmnt*, JetParticle& );

  JetQuadrupole();
  JetQuadrupole( char*  n,         // name
              double l,         // length,        in meters
              double s,         // strength = B', in Tesla-meters^-1
              int index,        // What jet index > 6
              int    m = 4,     // number of thinquad kicks
              PROPFUNC = 0,
              JETPROPFUNC = 0);
  JetQuadrupole( double l,         // length,        in meters
              double s,         // strength = B', in Tesla-meters^-1
              int index,        // What jet index > 6
              int    m = 4,     // number of thinquad kicks
              PROPFUNC = 0,
              JETPROPFUNC = 0);
  JetQuadrupole( bmlnElmntData& );
  JetQuadrupole( const JetQuadrupole& );
  ~JetQuadrupole();

  void setStrength( const double );
  void setStrength( const double, const int );

  void propagate( ParticleBunch& x ) { bmlnElmnt::propagate( x ); }
  void propagate( Particle&    p ) {    Propagator( this, p ); }
  void propagate( JetParticle& p ) { JetPropagator( this, p ); }

  // ??? REMOVE void peekAt( double& s, Particle* = 0 );
  void eliminate();

  char* Type() { return "JetQuadrupole"; }
  bmlnElmnt* Clone() { return new JetQuadrupole( *this ); }
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

  void propagate( ParticleBunch& x ) { bmlnElmnt::propagate( x ); }
  void propagate( Particle& p );
  void propagate( JetParticle& );
  void eliminate();

  char* Type() { return "thinQuad"; }
  bmlnElmnt* Clone() { return new thinQuad( *this ); }
} ;


 class JetthinQuad : public bmlnElmnt
 {
 private:
   Jet KL;
   int strengthIndex;
  ostream& writeTo(ostream&);
  istream& readFrom(istream&);

 public:
   JetthinQuad( const double, const int );
   JetthinQuad( char*,             // name
                const double,      // B-prime L in Tesla; + = horizontally focussing
                const int );       // index of focal length parameter (> 6)
   JetthinQuad( bmlnElmntData& );
   JetthinQuad( const JetthinQuad& );
   ~JetthinQuad();
 
   void setStrength( const double );
   void setStrength( const double, const int );
 
   void propagate( ParticleBunch& x ) { bmlnElmnt::propagate( x ); }
   void propagate( Particle& p );
   void propagate( JetParticle& );

   void eliminate();
 
   char* Type() { return "JetthinQuad"; }
   bmlnElmnt* Clone() { return new JetthinQuad( *this ); }

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

  void propagate( ParticleBunch& x ) { bmlnElmnt::propagate( x ); }
  void propagate( Particle& p );
  void propagate( JetParticle& );

  char* Type() { return "thinSextupole"; }
  bmlnElmnt* Clone() { return new thinSextupole( *this ); }
} ;

class JetthinSext : public bmlnElmnt
{
private:
  Jet K2L;
  int strengthIndex;
  ostream& writeTo(ostream&);
  istream& readFrom(istream&);

public:
  JetthinSext( const double, const int );
  JetthinSext( char*,             // name
	       const double,      // B-prime L in Tesla; + = horizontally focussing
	       const int );       // index of focal length parameter (> 6)
  JetthinSext( bmlnElmntData& );
  JetthinSext( const JetthinSext& );
  ~JetthinSext();
  
  void setStrength( const double );
  void setStrength( const double, const int );
  
  void propagate( ParticleBunch& x ) { bmlnElmnt::propagate( x ); }
  void propagate( Particle& p );
  void propagate( JetParticle& );
  
  void eliminate();
  
  char* Type() { return "JetthinSext"; }
  bmlnElmnt* Clone() { return new JetthinSext( *this ); }
  
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

  void propagate( ParticleBunch& x ) { bmlnElmnt::propagate( x ); }
  void propagate( Particle& p );
  void propagate( JetParticle& );

  char* Type() { return "thinOctupole"; }
  bmlnElmnt* Clone() { return new thinOctupole( *this ); }
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

  void propagate( ParticleBunch& x ) { bmlnElmnt::propagate( x ); }
  void propagate( Particle& p );
  void propagate( JetParticle& );

  char* Type() { return "thinDecapole"; }
  bmlnElmnt* Clone() { return new thinDecapole( *this ); }
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

  void propagate( ParticleBunch& x ) { bmlnElmnt::propagate( x ); }
  void propagate( Particle& p );
  void propagate( JetParticle& );

  char* Type() { return "thin12pole"; }
  bmlnElmnt* Clone() { return new thin12pole( *this ); }
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

  void propagate( ParticleBunch& x ) { bmlnElmnt::propagate( x ); }
  void propagate( Particle& p );
  void propagate( JetParticle& );

  char* Type() { return "thin14pole"; }
  bmlnElmnt* Clone() { return new thin14pole( *this ); }
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

  void propagate( ParticleBunch& x ) { bmlnElmnt::propagate( x ); }
  void propagate( Particle& p );
  void propagate( JetParticle& );

  char* Type() { return "thin16pole"; }
  bmlnElmnt* Clone() { return new thin16pole( *this ); }
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

  void propagate( ParticleBunch& x ) { bmlnElmnt::propagate( x ); }
  void propagate( Particle& p );
  void propagate( JetParticle& );

  char* Type() { return "thin18pole"; }
  bmlnElmnt* Clone() { return new thin18pole( *this ); }
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

  void propagate( ParticleBunch& x ) { bmlnElmnt::propagate( x ); }
  void propagate( Particle& p );
  void propagate( JetParticle& );

  char* Type() { return "thinMultipole"; }
  bmlnElmnt* Clone() { return new thinMultipole( *this ); }
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

  void setStrength( const double );
  void setStrength( const double, const int );

  void setCurrent( const double );

  void propagate( ParticleBunch& x ) { bmlnElmnt::propagate( x ); }
  void propagate( Particle& p );
  void propagate( JetParticle& );

  char* Type() { return "sextupole"; }
  bmlnElmnt* Clone() { return new sextupole( *this ); }
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
          const double  = 1.0
             /* length [m]: the length of the bunch
                in its rest frame */,
          const double  = 3.3e11
             /* strength : total number of proton
                charges in the bunch */,
          const double  = 1000.0
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

  void propagate( ParticleBunch& x ) { bmlnElmnt::propagate( x ); }
  void propagate( Particle& );
  void propagate( JetParticle& );

  char* Type() { return "BBLens"; }
  bmlnElmnt* Clone() { return new BBLens( *this ); }
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
  
  void setStrengths( const double sPos, const double sNeg); 
  void setWire( const double x); 
  
  void propagate( ParticleBunch& x ) { bmlnElmnt::propagate( x ); }
  void propagate( Particle&    p );
  void propagate( JetParticle& p );
  thinSeptumData* image();
  void eliminate();
  
  char* Type() { return "thinSeptum"; }
  bmlnElmnt* Clone() { return new thinSeptum( *this ); }

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
  double    RefState[6];   // A particle in the "field-free" region 
                           // with phase space state of RefState() 
                           // will be on the reference orbit of the 
                           // extraction line.
  void image( int, slist*, BMLN_posInfo* );
  ostream& writeTo(ostream&);
  istream& readFrom(istream&);

public:
  
  thinLamb();
  thinLamb( char* n);
  
  thinLamb( char*  n,      // name
	   double x,
	   beamline* b,
	   double* s );     
  
  thinLamb( double x,
	   beamline* b,    
	   double* s );      
  
  thinLamb( thinLambData& );
  thinLamb( const thinLamb& );
  ~thinLamb();
  
  void setSeptum( const double x); 
  void setBeamline( beamline* b); 
  void setRefState( const double* s);
  
  void propagate( ParticleBunch& x );
  void propagate( Particle&    p );
  void propagate( JetParticle& p );
  
  thinLambData* image();
  void eliminate();
  
  char* Type() { return "thinLamb"; }
  bmlnElmnt* Clone() { return new thinLamb( *this ); }

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
 
 

struct InsertionListElement {
  double     s;  // Position (design orbit length)
  bmlnElmnt* q;  // Element to be inserted.
  InsertionListElement( double     x = 0.0, /* s */
                        bmlnElmnt* p = 0    /* q */ 
                      ) { s = x; q = p; }
};


class InsertionList : private dlist
{
private:
  double smax;
  Proton prtn;
public:
  InsertionList( double /* momentum [GeV/c] */ = 1000.0 );
  InsertionList( const InsertionList& );
  ~InsertionList();

  void Append( const InsertionListElement& );
  void Append( const InsertionListElement* );
  void Insert( const InsertionListElement& );
  void Insert( const InsertionListElement* );
  void MergeUnique( InsertionList& );
  void MergeAll   ( InsertionList& );
  InsertionListElement* Get();   // Iterator; removes elements from list
  InsertionListElement* operator()( const int& ) const;
  InsertionList& operator=( const InsertionList& );
  int Size();
  void Clear();   // Preserves the InsertionListElements
  Proton GetParticle() { return prtn; }
  friend ostream& operator<<( ostream&, const InsertionList& );
};

inline 
InsertionListElement* 
InsertionList::Get()
{
  return (InsertionListElement*) dlist::get();
}

inline 
InsertionListElement* 
InsertionList::operator()( const int& n ) const
{
  return (InsertionListElement*) dlist::operator[]( n );
}


class beamlineImage;
class beamline : public bmlnElmnt, public dlist
{
private:
  double            nominalEnergy;    // In GeV
  int               numElem;          // Number of elements in the beamline
  char              twissDone;
  void image( int, slist*, BMLN_posInfo* );
  ostream& writeTo(ostream&);
  istream& readFrom(istream&);
  friend istream& operator>>( istream&, beamline& );

public:


  // CONSTRUCTORS AND DESTRUCTOR

  beamline( const char* nm = "NONAME" );
  beamline( bmlnElmnt* );
  beamline( beamline& );
  beamline( beamlineImage* );         // Creates a beamline from the
                                      //  list representation written
                                      //  by beamline::image.  In the
                                      //  process it empties the list.
  beamline( char*, bmlnElmnt* );
  beamline( char*, beamline* );
  beamline( char*, slist* );          // Creates a beamline from the
                                      //  list representation written
                                      //  by beamline::image.  ??? UNWRITTEN
  beamline( FILE* );                  // Reading persistent object stored
                                      //  in a binary file.

  ~beamline(); 	       	       	       // Notice that this destructor does not destroy
                                       // the beamline elements.  To do that, use
                                       // beamline::zap().
  void zap();                          // Destroys the beamline elements
                                       // without destroying the beamline
  void eliminate();                    // Combines zap and ~beamline



  // EDITING LATTICE

  void insert( bmlnElmnt& );
  void insert( bmlnElmnt* );
  void append( bmlnElmnt& );
  void append( bmlnElmnt* );

  void Split( double, bmlnElmnt**, bmlnElmnt** );

  void InsertElementAt( double s_0, double s, const bmlnElmnt& q );
  void InsertElementAt( double s_0, double s, const bmlnElmnt* q );

                                      // Will insert q into the beamline at
                                      // OrbitLength s, assuming the beamline
                                      // begins at OrbitLength s_0.  Normally
                                      // will be invoked by a user program using
                                      // s_0 = 0, but s_0 is included so the
                                      // module can be used recursively.

  void InsertElementsFromList( double& s_0, InsertionList&, slist& );

				      // Will insert elements from the list into
                                      // the beamline at locations specified in
                                      // the list.  Removed elements are stored
                                      // in the final argument, in case they
                                      // need to be eliminated by the user module.

  beamline& operator^( bmlnElmnt& );  // Alters the beamline
  beamline& operator^( beamline& );
  beamline& operator+( bmlnElmnt& );  // Does not alter the beamline
  beamline& operator+( beamline& );
  beamline& operator-( beamline& );
  friend beamline& operator-( beamline& );


  sector* makeSector ( int, JetParticle& );
                                     // Returns a pointer to a new
                                     // sector equivalent to the entire
                                     // beamline.  The argument is the
                                     // degree of the map.
  sector* MakeSector ( const bmlnElmnt&,   // Returns a pointer to a new sector
		      const bmlnElmnt&,   // equivalent to everything between
		      int,                // the first two arguments( exclusive).
		      JetParticle& );
  sector* MakeSectorFromStart 
    ( const bmlnElmnt&,   
     int,                
     JetParticle& );
  sector* MakeSectorToEnd 
    ( const bmlnElmnt&,   
     int,                
     JetParticle& );
  void     sectorize ( int, JetParticle& );
                                     // Alters the object itself.
  beamline sectorize ( bmlnElmnt&,   // Alters the object: everything between
                       bmlnElmnt&,   // the bmlnElmnt arguments replaced by a map.
                       int,          // <-- This argument is the degree of the map.
                       JetParticle& );
  void putAbove( bmlnElmnt& x, bmlnElmnt& y ); // Insert y above (before;  upstream of) x
  void putBelow( bmlnElmnt& x, bmlnElmnt& y ); // Insert y below (after, downstream of) x

  beamline remove( bmlnElmnt&, bmlnElmnt& );
  char     remove( void * );

  // GEOMETRY

  void geomToEnd   ( BMLN_posInfo& );
  void geomToStart ( BMLN_posInfo& );
  void resetGeometry();
  void setAlignment ( const alignmentData& );


  // PROPAGATE PARTICLES

  void propagate( ParticleBunch& );
  void propagate( Particle& );
  void propagate( JetParticle& );

  void propLattFunc( );
  void propLattFunc( FILE* );
  void propLattFunc( char* );


  // EDIT PROPERTIES

  void setEnergy( double /* Nominal energy in GeV */ );
  void unTwiss();


  // ANALYSIS

  int twiss( JetParticle& );
                           // Computes lattice functions all the
                           // way around the ring.
                           // Uses the same method as MAD.
  int twiss( char, JetParticle& );
                           // Only computes lattice functions at
                           // the beginning of the ring.
                           // Uses the same method as MAD.
  int twiss(lattFunc&,JetParticle&);
                           // Computes lattice functions for a
                           // non-periodic beamline.
                           // Uses the same method as MAD.


  // QUERIES

  void   peekAt( double& s, Particle* = 0 );
  beamlineImage  image( int );
  lattFunc whatIsLattice( int );     // After element n, 0 <= n.
  lattFunc whatIsLattice( char* n ); // n is name of element 
  int    howMany() const { return numElem; }
  int    countHowMany();
  int    howDeep();
  double Energy() const { return nominalEnergy; }
  char*  Type() { return "beamline"; }
  bmlnElmnt* Clone();
  double OrbitLength( const Particle& );
  lattRing whatIsRing();


  // CLONING AND STORING

  beamline& operator=( const beamline& );
  beamline* flatten();               // Produces a flattened version of itself.

  void   writeLattFunc( );
  void   writeLattFunc( FILE* );
  void   writeLattFunc( char* );

} ;

extern beamline& operator*( int, beamline& );
extern beamline& operator*( int, bmlnElmnt& );
extern beamline& operator*( beamline&, int );
extern beamline& operator*( bmlnElmnt&, int );
extern beamline& operator^( bmlnElmnt&, bmlnElmnt& );
extern beamline& operator-( beamline& );


struct beamlineData : public bmlnElmntData
{
  int             numElem;          // Number of elements in the beamline

  beamlineData();
  beamlineData( beamlineData& );
  ~beamlineData();
  void eliminate();
  void* clone();

};


class beamlineImage {
private:
  slist bak;

  beamlineImage();
  friend class beamline;

public:
  slist img;

  // ??? REMOVE beamlineImage( FILE* );
  beamlineImage( beamlineImage& );
  ~beamlineImage();
  void reset();    // If the user has manipulated img, the reset()
                  //  method restores it to its original state.
  void zap();      // Destroys all XXXXData objects.
  beamlineImage& operator=( const beamlineImage& );
                  // WARNING: there may be stray XXXXData objects
                  //  hanging around after this if ::zap() is not
                  //  used first.
};

class circuit : public dlist
{
protected:
  char onOffSwitch;     // switch to turn current on and off
  char* ident;          // name of circuit
  int numElm;           // number of elements in the circuit
public:
  circuit();
  circuit( char * );
  circuit( bmlnElmnt* );
  circuit( char*, bmlnElmnt* );
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
  ICircuit( char * );
  ICircuit( bmlnElmnt* );
  ICircuit( char*, bmlnElmnt* );
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
  FCircuit( char * );
  FCircuit( bmlnElmnt* );
  FCircuit( char*, bmlnElmnt* );
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
  mover( char* );
  mover( bmlnElmnt* );
  mover( char*, bmlnElmnt* );
  void switchOn();
  void switchOff();
  void set( void* );
  void get( void* );

};

class beamlineOverseer
{
private:
  dlist setOfBeamlines;
  dlist setOfProbes;
} ;

struct Slot
{
 char*       name;
 alignment*  pre;
 bmlnElmnt*  device;
 alignment*  post;

 Slot( bmlnElmnt& x, char* nm = "NONAME" ) {
   pre = post = 0;
   name = nm;
   device = &x;
 }
 Slot( bmlnElmnt& x, double length, char* nm = "NONAME" );
 Slot( double length, char* nm = "NONAME" ) {
   pre = post = 0;
   name = nm;
   device = new drift( length );
 }
 ~Slot();

 // void propagate( ParticleBunch& x ) { bmlnElmnt::propagate( x ); }
 void propagate( Particle& );
 void propagate( JetParticle& );

 void setAlignment(const alignmentData&);  // ??? <-- or whatever
};


#endif // BEAMLINE_H
