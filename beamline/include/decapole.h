/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      decapole.h
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
******                                                                
****** REVISION HISTORY
******
******  Mar 2007:         ostiguy@fnal.gov
******  - use covariant return types
******  - support for reference counted elements
******  Dec 2007:         ostiguy@fnal.gov
******  - new typesafe propagators
******  May 2008 ostiguy@fnal
******  - proper, explicit assignment operator
******  - propagator moved (back) to base class
******  - no assumption about internal structure
******
**************************************************************************
*************************************************************************/

#ifndef DECAPOLE_H
#define DECAPOLE_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>

class BmlVisitor;
class ConstBmlVisitor;

class thinDecapole;

typedef boost::shared_ptr<thinDecapole>            ThinDecapolePtr;
typedef boost::shared_ptr<thinDecapole const> ConstThinDecapolePtr;


class DLLEXPORT thinDecapole : public bmlnElmnt {

  class Propagator;

public:

  thinDecapole();
  thinDecapole( std::string const& name, double const& strength);
  thinDecapole( thinDecapole const& );

  thinDecapole* Clone() const;

 ~thinDecapole();

  thinDecapole& operator=( thinDecapole const& rhs);  
  
  void accept( BmlVisitor& v );           
  void accept( ConstBmlVisitor& v ) const;  

  const char* Type()    const;

  bool isMagnet()       const;
  bool isThin()         const;
  bool isPassive()      const;
  bool isDriftSpace()   const;

} ;


#endif // DECAPOLE_H
