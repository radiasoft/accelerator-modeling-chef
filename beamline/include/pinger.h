/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      pinger.h
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
******                                                                
****** REVISION HISTORY
******
****** Mar 2007           ostiguy@fnal.gov
****** - use covariant return types
****** - support for reference counted elements
******
****** Dec 2007           ostiguy@fnal.gov
****** - new typesafe propagator scheme
******
**************************************************************************
*************************************************************************/

#ifndef PINGER_H
#define PINGER_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>

class BmlVisitor;
class ConstBmlVisitor;

class DLLEXPORT Pinger : public bmlnElmnt {

protected:

  class Propagator;

public:
   
  typedef boost::shared_ptr<BasePropagator<Pinger> > PropagatorPtr;   

  Pinger();       	                                              // Assumes a zero, horizontal kick
  Pinger( double const& kick_rad, double const& direction_rad =0, int ntrns = -1);
            // direction_rad==0 (Hor) (def) ntrs =number of turns before firing
  Pinger( const char*  name);                                       // Assumes zero, horizontal kick 
  Pinger( const char*  name, double const& kick_rad, double const& direction_rad=0, int ntrns= -1);
  Pinger( Pinger const& );
  Pinger* Clone() const { return new Pinger( *this ); }

 ~Pinger(); 

  const char* Type()     const;
  bool        isMagnet() const;

  void localPropagate( Particle& );
  void localPropagate( JetParticle& );
  void localPropagate( ParticleBunch& x );
  void localPropagate( JetParticleBunch& x );

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  void arm( int n )  { counter_ = n; }
  bool countdown()   { counter_ = counter_ > -1 ? --counter_ : -1 ; return (counter_ == -1);}
  void disarm()      { counter_ = -1; }
  bool isArmed()     { return ( counter_ >= 0 ); };
  
  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);
  
  double const& getKickDirection()                { return kick_direction_; }
  void          setKickDirection(double const& k) { kick_direction_ = k; }

  void usePropagator( PropagatorPtr& );

protected:

  double        kick_direction_;	/* In which direction is the kick? */
  int           counter_;               /* Counts number of turns before firing. */
  PropagatorPtr propagator_; 

};


class DLLEXPORT HPinger : public Pinger {

 public:

  HPinger();       	                        // Assumes a zero, horizontal kick
  HPinger( const char* name);                  // Assumes zero kick 
  HPinger( const char* name, double const& kick_rad = 0.0, int count = -1);
  HPinger( HPinger const& );

  HPinger* Clone() const { return new HPinger( *this ); }

 ~HPinger();

  const char* Type() const;
  bool        isMagnet() const;

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

};

class DLLEXPORT VPinger : public Pinger {

 public:

  VPinger();       	                                             
  VPinger( char const* name ); //Assumes zero kick 
  VPinger( char const* name,  double const& kick_rad = 0.0, int count   = -1);

  VPinger( VPinger const& );

  VPinger* Clone() const { return new VPinger( *this ); }

 ~VPinger();

  const char* Type() const;
  bool        isMagnet() const;

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

};

#endif /* PINGER_H */
