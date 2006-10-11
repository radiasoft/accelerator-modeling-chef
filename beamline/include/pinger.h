/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.1
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
**************************************************************************
*************************************************************************/


#ifndef PINGER_H
#define PINGER_H


#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>
#include <beamline/BmlVisitor.h>


class DLLEXPORT Pinger : public bmlnElmnt {
protected:
  double _kick_direction;	/* In which direction is the kick? */
  int    _counter;              /* Counts number of turns before firing. */
public:
  Pinger(       	/* Assumes a zero, horizontal kick */ );
  Pinger( double, 	/* kick size in radians,  */
	  double =0,    /* direction of kick, radians (H default) */ 
          int = -1      /* number of turns before firing */ );
  Pinger( char * 	/* name */
	  /* Assumes zero, horizontal kick */ );
  Pinger( char *, 	/* name */
	  double,	/* kick size in radians */
	  double =0,    /* direction of kick, radians (H default) */ 
          int = -1      /* number of turns before firing */ );
  Pinger( const Pinger& );
  ~Pinger(); 

  const char* Type() const;
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );
  void localPropagate( ParticleBunch& x );
  Pinger* Clone() const { return new Pinger( *this ); }
  void accept( BmlVisitor& v ) { v.visitPinger( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitPinger( this ); }
  void arm( int n ) { _counter = n; }
  void disarm()     { _counter = -1; }
  bool isArmed()    { return ( _counter >= 0 ); };
  
  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);
  
  double getKickDirection() { return _kick_direction; }
  void setKickDirection(double k) { _kick_direction = k; }
};

class DLLEXPORT HPinger : public Pinger {
 public:
  HPinger( double = 0.0 /* kick size in radians */,
           int    = -1  /* count */ );
  HPinger( char * /* name */  /* Assumes zero kick */ );
  HPinger( char * /* name */, 
           double = 0.0 /* kick size in radians */,
           int    = -1  /* count */ );
  HPinger( const HPinger& );
  ~HPinger();

  const char* Type() const;
  HPinger* Clone() const { return new HPinger( *this ); }
  void accept( BmlVisitor& v ) { v.visitHPinger( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitHPinger( this ); }
};

class DLLEXPORT VPinger : public Pinger {
 public:
  VPinger( double = 0.0 /* kick size in radians */,
           int    = -1  /* count */ );
  VPinger( char * /* name */  /* Assumes zero kick */ );
  VPinger( char * /* name */, 
           double = 0.0 /* kick size in radians */,
           int    = -1  /* count */ );
  VPinger( const VPinger& );
  ~VPinger();

  const char* Type() const;
  VPinger* Clone() const { return new VPinger( *this ); }
  void accept( BmlVisitor& v ) { v.visitVPinger( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitVPinger( this ); }
};

#endif /* PINGER_H */
