/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      sextupole.h
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
****** Apr 2008           michelotti@fnal.gov
****** - added sextupole::setLength(..) method to override
******   the base class implementation.
****** Mar 2007        ostiguy@fnal.gov
****** - use covariant return types
****** - support for reference counted elements
****** Dec 2007        ostiguy@fnal.gov
****** - new typesafe propagator scheme
****** Apr 2008        michelotti@fnal.gov
****** - changed signature of setStrength(..) to
******   match that of the virtual bmlnElmnt method
****** May 2008 ostiguy@fnal
****** - proper, explicit assignment operator
****** - propagator moved (back) to base class
**************************************************************************
*************************************************************************/
#ifndef SEXTUPOLE_H
#define SEXTUPOLE_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>


class BmlVisitor;
class ConstBmlVisitor;

class sextupole;
class thinSextupole;

typedef boost::shared_ptr<sextupole>     SextupolePtr;
typedef boost::shared_ptr<thinSextupole> ThinSextupolePtr;

typedef boost::shared_ptr<sextupole const>     ConstSextupolePtr;
typedef boost::shared_ptr<thinSextupole const> ConstThinSextupolePtr;


class DLLEXPORT sextupole : public bmlnElmnt {

  friend class elm_core_access;
  class Propagator;

public:

  sextupole();
  sextupole( std::string const& name, double const& length, 
                                      double const& strength );
  sextupole( sextupole const& );
  sextupole* Clone() const;

 ~sextupole();

  sextupole& operator=( sextupole const& );

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  const char* Type() const;

  bool isMagnet()      const;
  bool isThin()        const;
  bool isPassive()     const;
  bool isDriftSpace()  const;

} ;



class DLLEXPORT thinSextupole : public bmlnElmnt {
  
  friend class elem_core_access;

  class Propagator;

public:

  thinSextupole();
  thinSextupole( std::string const& name,  double const& strength );

  thinSextupole( thinSextupole const& );

  thinSextupole* Clone() const;
 ~thinSextupole();

  thinSextupole& operator=( thinSextupole const& );

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  const char* Type() const;

  bool isMagnet()      const;
  bool isThin()        const;
  bool isPassive()     const;
  bool isDriftSpace()  const;

} ;

#endif // SEXTUPOLE_H
