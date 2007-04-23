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
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
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
****** REVISION HISTORY
****** Mar 2007            ostiguy@fnal.gov
******
****** - use covariant return types
****** - support for reference counted elements
******
**************************************************************************
*************************************************************************/

#ifndef RFCAVITY_H
#define RFCAVITY_H

#include <beamline/bmlnElmnt.h>
#include <basic_toolkit/MathConstants.h>

class BmlVisitor;
class ConstBmlVisitor;
class RefRegVisitor;

class rfcavity;
class thinrfcavity;

typedef boost::shared_ptr<rfcavity>            RFCavityPtr;
typedef boost::shared_ptr<rfcavity const> ConstRFCavityPtr;

typedef boost::shared_ptr<thinrfcavity>            ThinRFCavityPtr;
typedef boost::shared_ptr<thinrfcavity const> ConstThinRFCavityPtr;


class rfcavity : public bmlnElmnt {

  friend class LinacCavity;

public:
  rfcavity( const char* = "NONAME" ); // Name
  rfcavity( double const&,   // length [m]
            double const&,   // RF frequency [Hz]
            double const&,   // max energy gain per turn [eV] (strength*10**9)
            double const&,   // synchronous phase [radians]
            double const&,   // Q
            double const&    // R shunt impedance
          );
  rfcavity( const char*, // Name
            double const&,   // length [m]
            double const&,   // RF frequency [Hz]
            double const&,   // max energy gain per turn [eV] (strength*10**9)
            double const&,   // synchronous phase [radians]
            double const&,   // Q
            double const&    // R shunt impedance
          );
  rfcavity( const rfcavity& );

  rfcavity* Clone() const { return new rfcavity( *this ); }
  virtual ~rfcavity();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ); 
  void accept( ConstBmlVisitor& v ) const; 

  void acceptInner( BmlVisitor& v );
  void acceptInner( ConstBmlVisitor& v ) const;

  void acceptInner( RefRegVisitor& v );

  const char* Type() const;
  bool    isMagnet() const;

  inline double getPhi()            const { return phi_s_; }
  inline double getFrequency()      const { return w_rf_; }
  inline double getQ()              const { return Q_; }
  inline double getR()              const { return R_; }
  inline double getHarmonicNumber() const { return h_; }

  void setHarmonicNumber( int );
  void setHarmonicNumber( double const& );
  void setFrequency( double const& );
  void setFrequencyRelativeTo( double const& );
  void setRadialFrequency( double const& );
  void setRadialFrequencyRelativeTo( double const& );
  void setPhi( double const& );  // radians
  void setStrength( double const& );  // eV

private:
  double w_rf_;                  // RF frequency [Hz]
  double phi_s_;                 // synchronous phase
  double sin_phi_s_;             // sine of synchronous phase
  // The max energy gain per turn [GeV] is represented by bmlnELmnt::strength
  double Q_;                     // quality factor
  double R_;                     // shunt impedance
  double h_;                     // harmonic number 
                                //   = ratio cavity frequency to
                                //     revolution frequency of a ring
                                //   Note: this is *NOT* a cavity attribute,
                                //   but is included for convenience.
  bmlnElmnt** u_;               // Address of first internal bmlElmnt pointer
  bmlnElmnt** v_;               // Address of final internal bmlElmnt pointer

  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);

  void finishConstructor();
};


//
// *** class thinRFCavity written by Jian Ping Shan
// *** April 4, 1994
//

class thinrfcavity : public bmlnElmnt
{

public:

  thinrfcavity( const char* = "NONAME" ); // Name
  thinrfcavity( double const&,   // RF frequency [Hz]
                double const&,   // max energy gain per turn [eV] (strength*10**9)
                double const&,   // synchronous phase [radians]
                double const&,   // Q
                double const&    // R shunt impedance
                );
  thinrfcavity( const char *,   // Name
                double const&,   // RF frequency [Hz]
                double const&,   // max energy gain per turn [eV] (strength*10**9)
                double const&,   // synchronous phase [radians]
                double const&,   // Q
                double const&    // R shunt impedance
                );
  thinrfcavity( const thinrfcavity& );
  thinrfcavity* Clone() const { return new thinrfcavity( *this ); }

  virtual ~thinrfcavity();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  const char* Type() const;
  bool    isMagnet() const;

  inline double getPhi()            const { return phi_s_; }
  inline double getFrequency()      const { return w_rf_; }
  inline double getQ()              const { return Q_; }
  inline double getR()              const { return R_; }
  inline double getHarmonicNumber() const { return h_; }

  void            setHarmonicNumber( int );
  void            setHarmonicNumber( double const& );
  void                 setFrequency( double const& );
  void       setFrequencyRelativeTo( double const& );
  void           setRadialFrequency( double const& );
  void setRadialFrequencyRelativeTo( double const& );
  void                       setPhi( double const& );  // radians

private:
  double w_rf_;                  // RF frequency [Hz]
  double phi_s_;                 // synchronous phase
  double sin_phi_s_;             // sine of synchronous phase
  // The max energy gain per turn [GeV] is represented by bmlnELmnt::strength
  double Q_;                     // quality factor
  double R_;                     // shunt impedance
  double h_;                     // harmonic number 
                                 //   = ratio cavity frequency to
                                 //     revolution frequency of a ring
                                 //   Note: this is *NOT* a cavity attribute,
                                 //   but is included for convenience.
  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);
};


#endif // RFCAVITY_H
