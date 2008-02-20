/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      septum.h
******                                                                
******  Copyright (c) 1991 Universities Research Association, Inc.    
******                All Rights Reserved                             
******                                                                
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
******  REVISION HISTORY
******  Mar 2007           ostiguy@fnal.gov
****** - use covariant return types
****** - support for reference counted elements
******
******  Dec 2007           ostiguy@fnal.gov
****** - new typesafe propagator scheme
******
**************************************************************************
*************************************************************************/

#ifndef SEPTUM_H
#define SEPTUM_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>

class BmlVisitor;
class ConstBmlVisitor;

class thinSeptum;

typedef boost::shared_ptr<thinSeptum>             ThinSeptumPtr;
typedef boost::shared_ptr<thinSeptum const>  ConstThinSeptumPtr;


class DLLEXPORT thinSeptum : public bmlnElmnt{

  class Propagator; 

public:

  typedef boost::shared_ptr<BasePropagator<thinSeptum> > PropagatorPtr;   

  thinSeptum( char const*  name );
  
  thinSeptum( char const*  name,
	    double const& sPos,    // kick in strength in radians for x > xWire 
	    double const& sNeg,    // kick in strength in radians for x < xWire 
	    double const& x );     // position of wire septum in meters
   
  thinSeptum( double const& sPos,    
	    double const& sNeg,    
	    double const& x ); 
  
  thinSeptum( thinSeptum const& );

  thinSeptum* Clone() const { return new thinSeptum( *this ); }

  thinSeptum& operator=( thinSeptum const& rhs);

 ~thinSeptum();
  
  void setStrengths( double const& sPos, double const& sNeg); 
  void setWire( double const& x); 
  
  double const& getPosStrength() { return strengthPos_; }
  double const& getNegStrength() { return strengthNeg_; }
  double const& getWireX()       { return xWire_;       }
  
  void localPropagate(         Particle&  p );
  void localPropagate(      JetParticle&  p );
  void localPropagate(    ParticleBunch&  p );
  void localPropagate( JetParticleBunch&  p );

  void accept( BmlVisitor& v ); 
  void accept( ConstBmlVisitor& v ) const; 

  bool    isMagnet() const;
  const char* Type() const;

private:
 
  thinSeptum();           // default constructor forbidden

  double strengthPos_;    // kick in strength in radians for x > xWire
  double strengthNeg_;	  // kick in strength in radians for x < xWire
  double xWire_;	  // position of wire septum in meters

  PropagatorPtr  propagator_; 
  

  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);

};
 
#endif // SEPTUM_H
