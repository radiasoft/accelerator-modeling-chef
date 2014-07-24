/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      rfcavity.h
******                                                                
******  Copyright Fermi Research Alliance LLC / Fermilab
******            All Rights Reserved                             
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-07CH11359.
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******                                                                
******  
******  Author:    Leo Michelotti
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov
******                                                                
******             
******  ----------------
******  REVISION HISTORY
******  ----------------
******  Mar 2007            Jean-Francois Ostiguy
******                      ostiguy@fnal.gov
******  - use covariant return types
******  - support for reference counted elements
******
******  Dec 2007            ostiguy@fnal.gov
******  - support for new style propagators
******  - inner structure now based on hidden beamline object
******
******  Apr 2008            michelotti@fnal.gov
******  - added placeholder rfcavity::setLength method
******
******  Dec 2012            michelotti@fnal.gov
******  - added functionality for multiple harmonics
******    : simple-minded implementation; should be improved some day.
******
******  Jul 2014            michelotti@fnal.gov
******  - added functionality for cavity with frequency not
******    multiple of the principal harmonic
******    : to simulate slip stacking.
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

 class Propagator;
     
public:
 
  typedef boost::shared_ptr<BasePropagator<rfcavity> > PropagatorPtr;   

  rfcavity( const char* = "" ); 
  rfcavity( const char*,     // Name
            double const&,   // length [m]
            double const&,   // RF frequency [Hz]
            double const&,   // max energy gain per turn [eV] (strength*10**9)
            double const&,   // synchronous phase [radians]
            double const&,   // Q
            double const&    // R shunt impedance
          );
  rfcavity( rfcavity const& );

  rfcavity* Clone() const { return new rfcavity( *this ); }
  ~rfcavity();

  void Split( double const&, ElmPtr&, ElmPtr& ) const;

  void localPropagate(         Particle& );
  void localPropagate(      JetParticle& );
  void localPropagate(    ParticleBunch& );
  void localPropagate( JetParticleBunch& );

  void accept( BmlVisitor& v ); 
  void accept( ConstBmlVisitor& v ) const; 

  const char* Type() const;
  bool    isMagnet() const;
 
  double getReferenceTime()              const;

  double const&  getPhi()                const;
  double const&  getRadialFrequency()    const;
  double         getDesignEnergyGain()   const;
  double const&  getQ()                  const;
  double const&  getR()                  const;
  double const&  getHarmonicNumber()     const;

  void      setHarmonicNumber( int );
  void      setHarmonicNumber( double const& );
  void           setFrequency( double const& );
  void setFrequencyRelativeTo( double const& );
  void     setRadialFrequency( double const& );
  void  setRadialFrequencyRelativeTo( double const& );
  void  setDisplacedFrequency( double const& freq /* Hz */, double const& ref_rev_period /* sec */ );
  void                 setPhi( double const& radians);  
  void                   setQ( double const& Q);
  void                   setR( double const& R);
  void            setStrength( double const& eV);  
  void              setLength( double const& );

  void            addHarmonic( int const& harmonic, double const& relativeStrength, double const& phase );

  void             turnUpdate();

  struct multiple_harmonic_parameters
  {
    double harmonic_multiple;
    double relative_strength;
    double phase_shift;

    multiple_harmonic_parameters()
    : harmonic_multiple(1), relative_strength(0.0), phase_shift(0.0) {}

    multiple_harmonic_parameters( int const& n, double const& s, double const& phi )
    : harmonic_multiple(n), relative_strength(s), phase_shift(phi) {}
  };

  class rfcavity_core_access;

private:

  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);

  double w_rf_;                 // RF frequency [Hz]
  double initial_w_rf_;         // for use by cavities with displaced frequency
  double phi_s_;                // synchronous phase
  double sin_phi_s_;            // sine of synchronous phase

  // NOTE: The max energy gain per turn [GeV] is represented by bmlnELmnt::strength

  double Q_;                    // quality factor
  double R_;                    // shunt impedance
  double h_;                    // harmonic number 
                                //   = ratio cavity frequency to
                                //     revolution frequency of a ring
                                //   Note: this is *NOT* a cavity attribute,
                                //   but is included for convenience.

  std::vector<rfcavity::multiple_harmonic_parameters> my_harmonic_parameters_;

  double displaced_phase_slip_;
  double cumulative_displaced_phase_slip_;

  PropagatorPtr  propagator_;
};



class rfcavity::rfcavity_core_access
{
 public:
  static std::vector<rfcavity::multiple_harmonic_parameters>& get_harmonics_data( rfcavity& o ) 
  {
    return o.my_harmonic_parameters_;
  }
};



class thinrfcavity : public bmlnElmnt {

  class Propagator;

public:

  typedef boost::shared_ptr<BasePropagator<thinrfcavity> > PropagatorPtr;   

  thinrfcavity( const char* = "NONAME" ); // Name
  thinrfcavity( const char *,   // Name
                double const&,   // RF frequency [Hz]
                double const&,   // max energy gain per turn [eV] (strength*10**9)
                double const&,   // synchronous phase [radians]
                double const&,   // Q
                double const&    // R shunt impedance
                );
  thinrfcavity( thinrfcavity const& );
  thinrfcavity* Clone() const { return new thinrfcavity( *this ); }

  ~thinrfcavity();

  double const& getPhi()              const; 
  double const& getRadialFrequency()  const; 
  double const& getQ()                const; 
  double const& getR()                const; 
  double const& getHarmonicNumber()   const; 

  bool          isSingleMultiple()    const;

  void            setHarmonicNumber( int );
  void            setHarmonicNumber( double const& );
  void                 setFrequency( double const& );
  void       setFrequencyRelativeTo( double const& );
  void           setRadialFrequency( double const& );
  void setRadialFrequencyRelativeTo( double const& );
  void        setDisplacedFrequency( double const& freq /* Hz */, double const& ref_rev_period /* sec */ );
  void                       setPhi( double const& );  // radians

  char const* Type() const;
  bool    isMagnet() const;

  void localPropagate(         Particle& );
  void localPropagate(      JetParticle& );
  void localPropagate(    ParticleBunch& ); 
  void localPropagate( JetParticleBunch& ); 

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  void    addHarmonic( int const& harmonic, double const& relativeStrength, double const& phase );

  void             turnUpdate();

  class thinrfcavity_core_access;

private:

  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);

  double w_rf_;                 // RF frequency [Hz]
  double initial_w_rf_;         // for use by cavities with displaced frequency
  double phi_s_;                // synchronous phase
  double sin_phi_s_;            // sine of synchronous phase

  // NOTE: The max energy gain per turn [GeV] is represented by bmlnELmnt::strength

  double Q_;                    // quality factor
  double R_;                    // shunt impedance
  double h_;                    // harmonic number 
                                //   = ratio cavity frequency to
                                //     revolution frequency of a ring
                                //   Note: this is *NOT* a cavity attribute,
                                //   but is included for convenience.

  std::vector<rfcavity::multiple_harmonic_parameters> my_harmonic_parameters_;

  double displaced_phase_slip_;
  double cumulative_displaced_phase_slip_;

  PropagatorPtr  propagator_;
};


class thinrfcavity::thinrfcavity_core_access
{
 public:
  static std::vector<rfcavity::multiple_harmonic_parameters>& get_harmonics_data( thinrfcavity& o ) 
  {
    return o.my_harmonic_parameters_;
  }
  static double get_cumulative_displaced_phase_slip( thinrfcavity& o )
  {
    return o.cumulative_displaced_phase_slip_;
  }
};


inline bool thinrfcavity::isSingleMultiple() const
{
  return ( 1 == my_harmonic_parameters_.size() );
}


#endif // RFCAVITY_H
