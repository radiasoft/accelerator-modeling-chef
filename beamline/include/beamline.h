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


#ifndef MONITOR_H
#include "monitor.h"
#endif

#ifndef MARKER_H
#include "marker.h"
#endif

#ifndef DRIFT_H
#include "drift.h"
#endif


#ifndef RBEND_H
#include "rbend.h"
#endif

#ifndef SBEND_H
#include "sbend.h"
#endif

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


#ifndef QUADRUPOLE_H
#include "quadrupole.h"
#endif

#ifndef JETQUADRUPOLE_H
#include "JetQuadrupole.h"
#endif

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
