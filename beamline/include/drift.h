/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
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
****** Dec 2007:          ostiguy@fnal.gov
****** - new typesafe propagators
****** May 2008 ostiguy@fnal
****** - proper, explicit assignment operator
****** - propagator moved (back) to base class
****** - no assumption about internal structure
******
**************************************************************************
*************************************************************************/
#ifndef DRIFT_H
#define DRIFT_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>

class   BmlVisitor;
class   ConstBmlVisitor;
class   Particle;
class   JetParticle;

template <typename Particle_t>
class TBunch;

typedef TBunch<Particle>    ParticleBunch;
typedef TBunch<JetParticle> JetParticleBunch;

class drift;

typedef boost::shared_ptr<drift>       DriftPtr;
typedef boost::shared_ptr<drift const> ConstDriftPtr;



class DLLEXPORT drift : public bmlnElmnt {

  class Propagator;  

public:

  drift();
  drift( std::string const& name, double length ); //  length of drift in meters

  drift( drift  const&);

  drift* Clone() const;

 ~drift();

  drift& operator=(drift const&); 

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  const char* Type() const;
  bool    isMagnet() const;

 private:

  friend std::ostream& operator<<(std::ostream&, bmlnElmnt&);
  friend bmlnElmnt*    read_istream(std::istream&);

};

#endif // DRIFT_H
