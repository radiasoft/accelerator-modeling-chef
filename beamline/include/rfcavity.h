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
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
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
******
****** Mar 2007            ostiguy@fnal.gov
******
****** - use covariant return types
****** - support for reference counted elements
****** Dec 2007            ostiguy@fnal.gov
****** - support for new style propagators
****** - inner structure now based on hidden beamline object
****** Apr 2008            michelotti@fnal.gov
****** - added placeholder rfcavity::setLength method
****** May 2008 ostiguy@fnal
****** - proper, explicit assignment operator
****** - propagator moved (back) to base class
****** - no implicit assumption about internal structure
**************************************************************************
*************************************************************************/

#ifndef RFCAVITY_H
#define RFCAVITY_H

#include <beamline/BmlnElmnt.h>
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


class rfcavity : public BmlnElmnt {

 class Propagator;
     
public:
 
  rfcavity( std::string const&  = "" ); 
  rfcavity( std::string const& name,  
            double const&,   // length [m]
            double const&,   // RF frequency [Hz]
            double const&,   // max energy gain per turn [eV] (strength*10**9)
            double const&,   // synchronous phase [radians]
            double const&,   // Q
            double const&    // R shunt impedance
          );
  rfcavity( rfcavity const& );

  rfcavity* clone() const { return new rfcavity( *this ); }
 ~rfcavity();

  rfcavity& operator=( rfcavity const& rhs);

  std::pair<ElmPtr,ElmPtr> split( double const& pct) const;

  void accept( BmlVisitor& v ); 
  void accept( ConstBmlVisitor& v ) const; 

  const char* Type()     const;
  bool    isMagnet()     const;
  bool    isThin()       const;
  bool    isPassive()    const;
  bool    isDriftSpace() const;
 
  double const&  phi()                const;
  double const&  frequency()          const;
  double const&  Q()                  const;
  double const&  R()                  const;
  double const&  harmon()             const;

  void              setHarmon( double const& );
  void           setFrequency( double const& );
  void setFrequencyRelativeTo( double const& );
  void                 setPhi( double const& radians);  
  void                   setQ( double const& Q);
  void                   setR( double const& R);

 protected:

  void   propagateReference( Particle& particle, double initialBRho, bool scaling );

private:

  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);


  // The max energy gain per turn [GeV] is represented by bmlnELmnt::strength

  double f_;                    // RF frequency [Hz]
  double phi_s_;                // synchronous phase
  double sin_phi_s_;            // sine of synchronous phase

  double Q_;                    // quality factor
  double R_;                    // shunt impedance

  double h_;                    // harmonic number 
                                //   = ratio cavity frequency to
                                //     revolution frequency of a ring
                                //   Note: this is *NOT* a cavity attribute,
                                //   but is included for convenience.
};


//---------------------------------------------------------------------------------------


class thinrfcavity : public BmlnElmnt {

  class Propagator;

public:

  thinrfcavity( std::string const&  name ="" ); 
  thinrfcavity( std::string const&  name,
                double const&,   // RF frequency [Hz]
                double const&,   // max energy gain per turn [eV] (strength*10**9)
                double const&,   // synchronous phase [radians]
                double const&,   // Q
                double const&    // R shunt impedance
                );
  thinrfcavity( thinrfcavity const& );
  thinrfcavity* clone() const { return new thinrfcavity( *this ); }

 ~thinrfcavity();

  thinrfcavity& operator=( thinrfcavity const& rhs);

  double const& frequency()       const; 
  double const& harmon()          const; 
  double const& phi()             const; 
  double const& Q()               const; 
  double const& R()               const; 


  void                 setFrequency( double const& );
  void       setFrequencyRelativeTo( double const& );
  void                    setHarmon( double const& );
  void                       setPhi( double const& );  // radians
  void                         setQ( double const& );
  void                         setR( double const&);

  char const*     Type()  const;

  bool        isMagnet()  const;
  bool          isThin()  const;
  bool       isPassive()  const;
  bool    isDriftSpace()  const;

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

private:

  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);

  double f_;                     // RF frequency [Hz]
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
};


#endif // RFCAVITY_H
