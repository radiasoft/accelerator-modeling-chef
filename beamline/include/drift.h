/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      drift.h
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
****** Mar 2007:          ostiguy@fnal.gov
****** - use covariant return types
****** - support for reference counted elements
**************************************************************************
*************************************************************************/


#ifndef DRIFT_H
#define DRIFT_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>

class BmlVisitor;
class ConstBmlVisitor;
class Particle;
class JetParticle;

class drift;

typedef boost::shared_ptr<drift>       DriftPtr;
typedef boost::shared_ptr<drift const> ConstDriftPtr;

class DLLEXPORT drift : public bmlnElmnt
{
public:

  drift();
  drift( double        length);             // length of drift in meters
  drift( char   const* name);     
  drift( char   const* name, double length); 

  drift( drift  const&);

  drift* Clone() const { return new drift( *this ); }

 ~drift();

  void localPropagate(Particle&    p) { bmlnElmnt::localPropagate(p); }
  void localPropagate(JetParticle& p) { bmlnElmnt::localPropagate(p); }

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;


  const char* Type() const;
  bool    isMagnet() const;

 private:

 
  friend std::ostream& operator<<(std::ostream&, bmlnElmnt&);
  friend bmlnElmnt*    read_istream(std::istream&);

} ;



#endif // DRIFT_H
