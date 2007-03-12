/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      decapole.h
******  Version:   2.1
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

public:

  thinDecapole();
  thinDecapole(                    double const& strength);
  thinDecapole( char const*  name, double const& strength);
  thinDecapole( thinDecapole const& );

  thinDecapole* Clone() const { return new thinDecapole( *this ); }

  thinDecapole& operator=( thinDecapole const& rhs);  
  
 ~thinDecapole();


  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& p );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v );           
  void accept( ConstBmlVisitor& v ) const;  

  const char* Type() const;
  bool isMagnet()    const;

} ;


#endif // DECAPOLE_H
