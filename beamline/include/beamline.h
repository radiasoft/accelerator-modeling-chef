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

#include "PhysicsConstants.h"
#include <math.h>
#include "MathConstants.h"
#include "apstring.h"
#include "mxyzptlk.h"
#include "slist.h"
#include "Barnacle.h"
#include "VectorD.h"
#include "JetVector.h"
#include "Particle.h"
#include "Aperture.h"   // O.K.
#include "BmlVisitor.h"

#if defined (__VISUAL_CPP__) && !defined(__KCC)
#define strcasecmp stricmp
#endif

#ifdef __BORLAND_CPP__
#define strcasecmp strcmpi
#endif

#define OSTREAM_DOUBLE_PREC setprecision(20)


// Predefined class names ...
class bmlnElmnt;
class Particle;             // ??? These lines should not be
class ParticleBunch;        // ??? necessary.
class JetParticle;          // =============================


// ??? REMOVE: typedef void (*PROPFUNC)    ( bmlnElmnt*, Particle& );
// ??? REMOVE: typedef void (*JETPROPFUNC) ( bmlnElmnt*, JetParticle& );


struct BMLN_posInfo {
    Vector inPoint;      // This struct defines the upstream and
    Vector inAxes  [3];  // downstream faces of the magnets slot.
    Vector outPoint;
    Vector outAxes [3];
    BMLN_posInfo( );
    BMLN_posInfo( const BMLN_posInfo& );
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
  double       cosTilt;         // cos(tilt)
  double       sinTilt;         // sin(tilt)
public:
  alignment();
  alignment( double, // xOffset
             double, // yOffset
             double  // roll (=tilt)
           );
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
  void misalign( double* );
  void align( double* );
  void misalign( JetVector& );
  void align( JetVector& );

  void setAlignment(const alignmentData&);
  alignmentData getAlignment();

  double x_offset() { return xOffset; }
  double y_offset() { return yOffset; }
  double roll()     { return tilt;    }
  double cos_roll() { return cosTilt; }
  double sin_roll() { return sinTilt; }

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
public:
  // --------------------------
  class PropFunc
  {
    public:
      virtual int operator()( bmlnElmnt*, Particle&    ) = 0;
      virtual int operator()( bmlnElmnt*, JetParticle& ) = 0;
      virtual const char* Type() const                   = 0;
  };
  // --------------------------

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

  PropFunc*    Propagator;

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
  bmlnElmnt( const char*   n = "NONAME" /* name     */,
             PropFunc*       = 0 
           );		   
  bmlnElmnt( double  l       /* length   */,
             PropFunc*       = 0
           );		   
  bmlnElmnt( double  l       /* length   */,
             double  s       /* strength */,
             PropFunc*       = 0
           );		   
  bmlnElmnt( const char*   n /* name     */,
             double        l /* length   */,
             PropFunc*       = 0
           );		   
  bmlnElmnt( const char*   n /* name     */,
             double        l /* length   */,
             double        s /* strength */,
             PropFunc*       = 0
           );
  bmlnElmnt( const bmlnElmnt&  );
  bmlnElmnt( bmlnElmntData& );

  virtual ~bmlnElmnt();

  // ----------------------------------------------
  Aperture*    pAperture;  // O.K.
  
  BarnacleList dataHook;   // Carries data as service to application program.
  // ??? REMOVE:  lattFunc     lattInfo;   // Information on maps and lattice functions.
  bmlnElmnt& operator=( const bmlnElmnt& );

  virtual void accept( BmlVisitor& ) = 0;

  PropFunc* setPropFunction ( const PropFunc* a );  // return previous
  PropFunc* setPropFunction ( const PropFunc& a );  // Propagator
  PropFunc* getPropFunction() { return Propagator; }

  void propagate( Particle& x ) 
  {
    if( align == 0 ) {
      localPropagate  ( x );
    }
    else {
      enterLocalFrame ( x );
      localPropagate  ( x );
      leaveLocalFrame ( x );
    }
  }

  void propagate( JetParticle& x )
  {
    if( align == 0 ) {
      localPropagate  ( x );
    }
    else {
      enterLocalFrame ( x );
      localPropagate  ( x );
      leaveLocalFrame ( x );
    }
  }

  void propagate( ParticleBunch& x )
  {
    static Particle* p;
    if( align == 0 ) {
      localPropagate  ( x );
    }
    else {
      slist_iterator getNext( (slist&) x );
      while((  p = (Particle*) getNext()  )) enterLocalFrame( *p );
      localPropagate  ( x );
      getNext.Reset( (const slist&) x );
      while((  p = (Particle*) getNext()  )) leaveLocalFrame( *p );
    }
  }

  virtual void localPropagate( Particle& );
  virtual void localPropagate( JetParticle& );
  virtual void localPropagate( ParticleBunch& );

  virtual void enterLocalFrame( Particle&                ) const;
  virtual void enterLocalFrame( JetParticle&             ) const;
  // ??? To do (?): virtual void enterLocalFrame( double*                  ) const;
  // ??? To do (?): virtual void enterLocalFrame( JetVector&               ) const;
  // ??? To do (?): virtual void enterLocalFrame( const Particle&, double* ) const;
  // ??? To do (?): virtual void enterLocalFrame( const JetParticle&, Jet* ) const;

  virtual void leaveLocalFrame( Particle&                ) const;
  virtual void leaveLocalFrame( JetParticle&             ) const;
  // ??? To do (?): virtual void leaveLocalFrame( double*                  ) const;
  // ??? To do (?): virtual void leaveLocalFrame( JetVector&               ) const;
  // ??? To do (?): virtual void leaveLocalFrame( const Particle&, double* ) const;
  // ??? To do (?): virtual void leaveLocalFrame( const JetParticle&, Jet* ) const;

  bmlnElmntData* image();
  void image( bmlnElmntData* );

  // Editing functions
  virtual bmlnElmnt* Clone() const = 0;  
                                   // Each type of bmlnElmnt will
                                   // create a copy of itself and
                                   // return a pointer to the new
                                   // object.
  bmlnElmnt* Clone(char* name);    // Clone it and change the name.

  virtual void Split( double s, bmlnElmnt**, bmlnElmnt** );
                                   // Splits the element at percent orbitlength
                                   // s from the in-face and returns
                                   // addresses to the two pieces
                                   // in the second and third arguments.
                                   // The user has responsibility for
                                   // eventually deleting these.
  virtual void peekAt( double& s, Particle* = 0 );

  virtual void setLength     ( double );
  virtual void setStrength   ( double );
  virtual void setStrength   ( double, int );  // for JetQuad
  virtual void setCurrent    ( double );
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
  char hasName( const char* x ) const
    { return ( 0 == strcmp( ident, x ) ); }
  virtual char*  Name() 
    { return ident; }
  virtual char*  Type() const = 0;
  virtual int    isType(char* s) { return strcmp(s, "bmlnElmnt") == 0; }


  void setFlavor( const apstring& x )
    { flavor = x; }
  void setFlavor( const char* x )
    { flavor = x; }
  apstring Flavor()
    { return flavor; }
  apstring getFlavor()
    { return flavor; }
  apstring FlavorOrType()
    { if( flavor.length() == 0 ) return apstring(this->Type());
      else                       return flavor;
    }
  apstring getFlavorOrType()
    { if( flavor.length() == 0 ) return apstring(this->Type());
      else                       return flavor;
    }


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

private:
  /* All the work is done in friend ostream& operator<<(),
     placeholder for if descendants want to do somthing. */
  virtual ostream& writeTo(ostream& os) 	{ return os; }
  virtual istream& readFrom(istream& is)	{ return is; };
  friend ostream& operator<<(ostream&, bmlnElmnt&);
  friend bmlnElmnt* read_istream(istream&);

  apstring     flavor;     // Allows for "flavors" of types of elements.
};

class hkick : public bmlnElmnt
{
public:
  hkick();
  hkick( double );   // kick size in radians
  hkick( char* );    // name; assumes zero kick
  hkick( char*,      // name
         double );   // kick size in radians
  hkick( char*,      // name
         double,     // length
         double );   // kick size in radians
  hkick( const hkick& );
  hkick( bmlnElmntData& );

  ~hkick();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "hkick") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new hkick( *this ); }

  void accept( BmlVisitor& v ) { v.visitHkick( this ); }
};


class vkick : public bmlnElmnt
{
public:
  vkick();            // Assumes zero kick
  vkick( double );    // kick size in radians
  vkick( char* );     // name; assumes zero kick
  vkick( char*,       // name
         double  );   // kick size in radians
  vkick( char*,       // name
         double,      // length
         double  );   // kick size in radians
  vkick( const vkick& );
  vkick( bmlnElmntData& );
  ~vkick();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitVkick( this ); }

  char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "vkick") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new vkick( *this ); }
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

  void accept( BmlVisitor& v ) { v.visitOctupole( this ); }

  void setStrength( double );
  void setStrength( double, int );

  void setCurrent( double );

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  char* Type() const;
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

  char* Type() const;
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

  char* Type() const;
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
  char* Type() const;
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

  char* Type() const;
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

  char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "vmonitor") != 0 ) return monitor::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new vmonitor( *this ); }
} ;

class marker : public bmlnElmnt
{
// ??? REMOVE private:
// ??? REMOVE   void image( int, slist*, BMLN_posInfo* );
public:
  marker();                   // Data to be written to standard output
  marker( char* );            // Name identifier.
  marker( const marker& );
  marker( bmlnElmntData& );
  ~marker();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle&   );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitMarker( this ); }

  char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "marker") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new marker( *this ); }
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

  void accept( BmlVisitor& v ) { v.visitDrift( this ); }

  char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "drift") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new drift( *this ); }
} ;


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

  // ??? REMOVE: char* Type() const { return "rbend"; }
  char* Type() const;
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

  char* Type() const;
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

  char* Type() const;
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

  char* Type() const;
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

  char* Type() const;
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

  // ??? REMOVE: char* Type() const { return "thinQuad"; }
  char* Type() const;
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
 
   char* Type() const;
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

  // ??? REMOVE: char* Type() const { return "thinSextupole"; }
  char* Type() const;
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
  
  char* Type() const;
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

  char* Type() const;
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

  char* Type() const;
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

  char* Type() const;
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

  char* Type() const;
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

  char* Type() const;
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

  char* Type() const;
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

  char* Type() const;
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

  char* Type() const;
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

  char* Type() const;
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
  
  char* Type() const;
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
  
  char* Type() const;
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
  beamline( const beamline& );
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


  int startAt( const bmlnElmnt*,     // Resets the "beginning" of the
               int = 1 );            // beamline to the element given
                                     // by the argument. Should be used
                                     // only for rings. Returns non-zero
                                     // if error occurs (esp., no such
                                     // element. The optional integer
                                     // argument allows one to reset to
                                     // nth occurrence of the element.
  int startAt( const char*,          // Resets the "beginning" of the
               int = 1 );            // beamline to the element whose
                                     // name is given by the argument.


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
  char putAbove( bmlnElmnt& x, bmlnElmnt& y ); // Insert y above (before;  upstream of) x
  char putBelow( bmlnElmnt& x, bmlnElmnt& y ); // Insert y below (after, downstream of) x

  beamline remove( bmlnElmnt&, bmlnElmnt& );
  char     remove( void * );
  char     remove( const bmlnElmnt& );
  char     remove( const bmlnElmnt* );

  // GEOMETRY

  void geomToEnd   ( BMLN_posInfo& );
  void geomToStart ( BMLN_posInfo& );
  void resetGeometry();
  void setAlignment ( const alignmentData& );


  // PROPAGATE PARTICLES

  void localPropagate( ParticleBunch& );
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void enterLocalFrame( Particle&    ) const;
  void enterLocalFrame( JetParticle& ) const;
  void leaveLocalFrame( Particle&    ) const;
  void leaveLocalFrame( JetParticle& ) const;

  void accept( BmlVisitor& v ) { v.visitBeamline( this ); }

  void propLattFunc( );
  void propLattFunc( FILE* );
  void propLattFunc( char* );


  // EDIT PROPERTIES

  void setEnergy( double /* Nominal energy in GeV */ );
  void unTwiss();

  void eraseBarnacles( const char* = 0 );

  // ANALYSIS

  // ??? These three twiss functions should be eliminated.
  // ??? They are hopelessly obsolete.

  int twiss( JetParticle&, 
             double = 0.00001 /* dpp */,
             char   = 1       /* attachFlag */ );
                           // Computes lattice functions all the
                           // way around the ring.
                           // Attaches a lattRing Barnacle labelled 
                           // "Ring" to the beamline, and a lattFunc
                           // Barnacle labelled "Twiss" to every element
                           // in the line.

  int twiss( char, 
             JetParticle& );
                           // Only computes lattice functions at
                           // the beginning of the ring.
                           // Uses the same method as MAD.
  int twiss( lattFunc&,
             JetParticle&,
             char  = 1        /* attachFlag */ );
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
  int    countHowManyDeeply();
  int    howDeep();
  inline bmlnElmnt* firstElement()
    {
      return (bmlnElmnt*) ( ((dlist*) this)->firstInfoPtr() );
    }
  inline bmlnElmnt* lastElement()
    {
      return (bmlnElmnt*) ( ((dlist*) this)->lastInfoPtr() );
    }
  char twissIsDone()
    {
      return twissDone;
    }
  void setTwissIsDone()
    {
      twissDone = 1;
    }
  void unsetTwissIsDone()
    {
      twissDone = 0;
    }
  double Energy() const { return nominalEnergy; }
  char*  Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "beamline") != 0 ) return bmlnElmnt::isType(c); else return 1; }

  bmlnElmnt* Clone() const;
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
  BipolarCircuit( char * );
  BipolarCircuit( bmlnElmnt* );
  BipolarCircuit( char*, bmlnElmnt* );
  void switchOn();
  void switchOff();
  void set( void* );
  void get( void* );
  void append( bmlnElmnt* );

};

class beamlineOverseer
{
private:
  dlist setOfBeamlines;
  dlist setOfProbes;
} ;


#endif // BEAMLINE_H
