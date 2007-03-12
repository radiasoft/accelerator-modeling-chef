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
****** REVISION HISTORY
******
****** Mar 2007           ostiguy@fnal.gov
****** - use covariant return types
****** - support for reference counted elements
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

public:

  Pinger( );       	                                              // Assumes a zero, horizontal kick
  Pinger( double const& kick_rad, double const& direction_rad =0, int ntrns = -1);
            // directon_rad==0 (Hor) (def) ntrs =number of turns before firing
  Pinger( char const*  name);                                       // Assumes zero, horizontal kick 
  Pinger( char const*  name, double const& kick_rad, double const& direction_rad=0, int ntrns= -1);
  Pinger( Pinger const& );
  Pinger* Clone() const { return new Pinger( *this ); }

  virtual ~Pinger(); 

  const char* Type() const;

  void localPropagate( Particle& );
  void localPropagate( JetParticle& );
  void localPropagate( ParticleBunch& x );

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  void arm( int n ) { counter_ = n; }
  void disarm()     { counter_ = -1; }
  bool isArmed()    { return ( counter_ >= 0 ); };
  
  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);
  
  double getKickDirection()        { return kick_direction_; }
  void  setKickDirection(double const& k) { kick_direction_ = k; }

protected:

  double kick_direction_;	/* In which direction is the kick? */
  int    counter_;              /* Counts number of turns before firing. */};

class DLLEXPORT HPinger : public Pinger {
 public:
  HPinger( double const& kick_rad,  int count= -1);
  HPinger( char const* name);                  // Assumes zero kick 
  HPinger( char const* name, double const& kick_rad = 0.0, int cont = -1);
  HPinger( HPinger const& );
  virtual ~HPinger();

  const char* Type() const;
  HPinger* Clone() const { return new HPinger( *this ); }

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;
};

class DLLEXPORT VPinger : public Pinger {
 public:
  VPinger( double const& kick_rad , int count = -1 );
  VPinger( char const* name ); //Assumes zero kick 
  VPinger( char const* name,  double const& kick_rad = 0.0, int count   = -1);
  VPinger( VPinger const& );

  VPinger* Clone() const { return new VPinger( *this ); }

  virtual ~VPinger();

  const char* Type() const;

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;
};

#endif /* PINGER_H */
