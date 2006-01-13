/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      rfcavity.h
******  Version:   2.1
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


#ifndef RFCAVITY_H
#define RFCAVITY_H

#include "bmlnElmnt.h"
#include "MathConstants.h"
#include "RefRegVisitor.h"

class rfcavity : public bmlnElmnt
{
private:
  double w_rf;                  // RF frequency [Hz]
  double phi_s;                 // synchronous phase
  double sin_phi_s;             // sine of synchronous phase
  // The max energy gain per turn [GeV] is represented by bmlnELmnt::strength
  double Q;                     // quality factor
  double R;                     // shunt impedance
  double h;                     // harmonic number 
                                //   = ratio cavity frequency to
                                //     revolution frequency of a ring
                                //   Note: this is *NOT* a cavity attribute,
                                //   but is included for convenience.
  bmlnElmnt** _u;               // Address of first internal bmlElmnt pointer
  bmlnElmnt** _v;               // Address of final internal bmlElmnt pointer

  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);

  void _finishConstructor();

public:
  rfcavity( const char* = "NONAME" ); // Name
  rfcavity( double,   // length [m]
            double,   // RF frequency [Hz]
            double,   // max energy gain per turn [eV] (strength*10**9)
            double,   // synchronous phase [radians]
            double,   // Q
            double    // R shunt impedance
          );
  rfcavity( const char*, // Name
            double,   // length [m]
            double,   // RF frequency [Hz]
            double,   // max energy gain per turn [eV] (strength*10**9)
            double,   // synchronous phase [radians]
            double,   // Q
            double    // R shunt impedance
          );
  rfcavity( const rfcavity& );
  ~rfcavity();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitRfcavity( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitRfcavity( this ); }
  void acceptInner( RefRegVisitor& v );
  void acceptInner( BmlVisitor& v );
  void acceptInner( ConstBmlVisitor& v );

  void eliminate();

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "rfcavity") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new rfcavity( *this ); }
  inline double getPhi() const { return phi_s; }
  inline double getFrequency() const { return w_rf; }
  inline double getQ() const { return Q; }
  inline double getR() const { return R; }
  inline double getHarmonicNumber() const { return h; }

  void setHarmonicNumber( int );
  void setHarmonicNumber( double );
  void setFrequency( double );
  void setFrequencyRelativeTo( double );
  void setRadialFrequency( double );
  void setRadialFrequencyRelativeTo( double );
  void setPhi( double );  // radians
  void setStrength( double );  // eV
};


//
// *** class thinRFCavity written by Jian Ping Shan
// *** April 4, 1994
//

class thinrfcavity : public bmlnElmnt
{
private:
  double w_rf;                  // RF frequency [Hz]
  double phi_s;                 // synchronous phase
  double sin_phi_s;             // sine of synchronous phase
  // The max energy gain per turn [GeV] is represented by bmlnELmnt::strength
  double Q;                     // quality factor
  double R;                     // shunt impedance
  double h;                     // harmonic number 
                                //   = ratio cavity frequency to
                                //     revolution frequency of a ring
                                //   Note: this is *NOT* a cavity attribute,
                                //   but is included for convenience.
  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);

public:
  thinrfcavity( const char * ); // Name
  thinrfcavity( double,   // RF frequency [Hz]
                double,   // max energy gain per turn [eV] (strength*10**9)
                double,   // synchronous phase [radians]
                double,   // Q
                double    // R shunt impedance
                );
  thinrfcavity( const char *,   // Name
                double,   // RF frequency [Hz]
                double,   // max energy gain per turn [eV] (strength*10**9)
                double,   // synchronous phase [radians]
                double,   // Q
                double    // R shunt impedance
                );
  thinrfcavity( const thinrfcavity& );
  ~thinrfcavity();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitThinrfcavity( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitThinrfcavity( this ); }

  void eliminate();

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "thinrfcavity") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new thinrfcavity( *this ); }
  inline double getPhi() const { return phi_s; }
  inline double getFrequency() const { return w_rf; }
  inline double getQ() const { return Q; }
  inline double getR() const { return R; }
  inline double getHarmonicNumber() const { return h; }

  void setHarmonicNumber( int );
  void setHarmonicNumber( double );
  void setFrequency( double );
  void setFrequencyRelativeTo( double );
  void setRadialFrequency( double );
  void setRadialFrequencyRelativeTo( double );
  void setPhi( double );  // radians
};


#endif // RFCAVITY_H
