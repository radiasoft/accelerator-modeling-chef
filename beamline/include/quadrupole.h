/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      quadrupole.h
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
****** Dec 2008           ostiguy@fnal
****** - restored basic support for MAD-style propagator
****** May 2008           ostiguy@fnal
****** - proper, explicit assignment operator
****** - propagator moved (back) to base class
****** - no implicit assumption about internal structure
****** Apr 2008           michelotti@fnal.gov
****** - added quadrupole::setLength(..) method to override
******   the base class implementation.
****** Mar 2007           ostiguy@fnal.gov
****** - covariant return types
****** - support for reference counted elements
****** December 2007      ostiguy@fnal.gov
****** - new typesafe propagator scheme
******
**************************************************************************
*************************************************************************/
#ifndef QUADRUPOLE_H
#define QUADRUPOLE_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/BmlnElmnt.h>


class BmlVisitor;
class ConstBmlVisitor;
class quadrupole;
class thinQuad;

typedef boost::shared_ptr<quadrupole> QuadrupolePtr; 
typedef boost::shared_ptr<thinQuad>   ThinQuadPtr; 

typedef boost::shared_ptr<quadrupole const> ConstQuadrupolePtr; 
typedef boost::shared_ptr<thinQuad const>   ConstThinQuadPtr; 


class DLLEXPORT quadrupole : public BmlnElmnt {

  class    Propagator;  
  class MADPropagator;  

public:


  // length    in meters^-1
  // strength (B') in Tesla-meters^-1

  quadrupole();
  quadrupole( std::string const& name, double const& length, double const& strength );
  quadrupole( quadrupole const& );

  quadrupole* clone() const;

 ~quadrupole();

 
  quadrupole& operator=( quadrupole const& o);

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  char const* Type()     const;

  bool        isMagnet()     const;
  bool        isThin()       const;
  bool        isPassive()    const;
  bool        isDriftSpace() const;

 private:

  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);

} ;


//-------------------------------------------------------------------------------

class DLLEXPORT thinQuad : public BmlnElmnt {

  class    Propagator;  
  class MADPropagator;  

public:

  // integrated_strength (B'L) in Tesla; + = horizontally focussing

  thinQuad();
  thinQuad( std::string const& name, double const& integrated_strength );    // B'L in Tesla; + = horizontally focussing
  thinQuad( thinQuad const& );

  thinQuad* clone() const;

 ~thinQuad();

  thinQuad& operator=( thinQuad const& o);

  void accept( BmlVisitor& v );           
  void accept( ConstBmlVisitor& v ) const;

  char const* Type()       const;

  bool        isMagnet()     const;
  bool        isThin()       const;
  bool        isPassive()    const;
  bool        isDriftSpace() const;


};

#endif // QUADRUPOLE_H
