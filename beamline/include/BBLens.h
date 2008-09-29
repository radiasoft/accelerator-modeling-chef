/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      BBLens.h
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
****** REVISION HISTORY:
******
****** Mar 2007         ostiguy@fnal.gov
****** - use covariant return type
****** - added support for reference counted smart pointers
****** - added proper  assigment operator  
****** Dec 2007         ostiguy@fnal.gov
****** - new typesafe propagators
****** May 2008 ostiguy@fnal.gov
****** - proper, explicit assignment operator
****** - propagator moved (back) to base class
****** - no assumption about internal structure
****** - function to compute E field now templated 
**************************************************************************
*************************************************************************/
#ifndef BBLENS_H
#define BBLENS_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>
#include <vector>

class BmlVisitor;
class ConstBmlVisitor;

template <typename T>
class TJetVector;

typedef TJetVector<double> JetVector;


class BBLens;

typedef boost::shared_ptr<BBLens>            BBLensPtr;
typedef boost::shared_ptr<BBLens const> ConstBBLensPtr;


template <typename T>
struct EVector_t;

template <> 
struct EVector_t<Jet> {
  typedef JetVector Type;
  typedef JetC      ComplexComponentType;
}; 

template <> 
struct EVector_t<double> {
  typedef               Vector   Type;
  typedef std::complex<double>   ComplexComponentType;
}; 

class DLLEXPORT BBLens : public bmlnElmnt {

  class Propagator;

public:

  BBLens(  std::string const& name="", double const&  length = 1.0
             /* length [m]: the length of the bunch
                in its rest frame */,
          double const&        = 3.3e11
             /* strength : total number of proton
                charges in the bunch */,
          double const&        = 1000.0
             /* gamma of the bunch, in the lab frame */,
          const double* = 0
             /* pointer to an array containing
                three values for 6 X invariant emittance [m] / pi  */
        );
  BBLens( BBLens const& );

  BBLens* Clone() const { return new BBLens( *this ); }

 ~BBLens();

  BBLens& operator=( BBLens const&);

  char useRound;         // By default = 1
                         // If 1: then round beam approximation
                         // used when horizontal and vertical 
                         // sigmas approximately equal.

  void setDistParameters( double const& num, double const& gamma, double const* sigma);

  void          setDistCharge( double const& nparticles); 
  double const& getDistCharge() const; 

  void                       setSigmas(std::vector<double> const& );
  std::vector<double> const& getSigmas() const;

  template <typename T> 
  typename EVector_t<T>::Type         NormalizedEField( T const& x, T const& y ) const; 
                                      /* returns the "normalized" electric field
                                         in the rest frame of the bunch, in inverse
                                         meters.  To get the field [V/m], this must
                                         be multiplied by Q/(2 pi epsilon_o), where
                                         Q is the line density of charge [C/m] (in
                                         rest frame). */


  void accept( BmlVisitor& v );            
  void accept( ConstBmlVisitor& v ) const; 

  const char* Type()          const;
  bool        isMagnet()      const;
  bool        isDriftSpace()  const;
  bool        isPassive()     const;                   
  bool        isThin()        const;                   


  Vector Beta() const;
  

  template <typename T >
  static double toDouble( T const& value);


private:

  double emittance_[3];                // One sigma (noninvariant) emittance / pi
  double gamma_;                       // Relativistic gamma
  double beta_;                        // Relativistic beta
  double num_;                         // Number of proton charges; if the bunch
                                       // is negatively charged, then this number
                                       // is negative.
  std::vector<double> sigmas_;         // This will depend on position in the 
                                       // lattice.  The third component is zero
                                       // for now.

};

// specializations

template <>  
double BBLens::toDouble( double const& value);

template <>
double BBLens::toDouble ( Jet const& value); 

#ifndef  BEAMLINE_EXPLICIT_TEMPLATES
#include <beamline/BBLens.tcc>
#endif

#endif // BBLENS_H
