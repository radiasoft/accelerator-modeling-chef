/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      monitor.h
******  Version:   3.0
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
******  Usage, modification, and redistribution are subject to terms
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


#ifndef MONITOR_H
#define MONITOR_H

// #include <iostream>
#include "bmlnElmnt.h"

class monitor : public bmlnElmnt
{
private:
  double   _driftFraction;

protected:
  std::ostream* _outputStreamPtr;  // not owned
  bool          _onOffSwitch;
  double*       _rgr;
  
public:
  // Constructors
  monitor();
  monitor( const char* /*name*/ );
  monitor( const char* /*name*/, double /*length*/ );
  monitor( const monitor& );
  virtual ~monitor();

  // ---------------------
  void setOutputStream( ostream& );
  void setOutputStream( ostream* );

  virtual bool on();  // returns previous state
  virtual bool off();

  virtual const char* version();

  double operator[]( int );  // Readout of data

  double setDriftFraction( double f ) 
  { double ret = _driftFraction; 
    if ( f <= 1 && f >= 0 ) _driftFraction = f; 
    return ret; 
  }
  double getDriftFraction() 
  { return _driftFraction; }

  // Is this "virtual" tag really necessary?  Probably!
  virtual ostream& writeTo(ostream&); 
  virtual istream& readFrom(istream&);

  void getState(double *s) 
  { for ( int i=0; i<6; i++ ) s[i] = _rgr[i]; } // DANGEROUS!!

  // ---------------------
  void localPropagate( ParticleBunch& x ) 
  { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle&   );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitMonitor( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitMonitor( this ); }
  
  inline bool State() const { return _onOffSwitch; }
  const char* Type() const;
  virtual int isType(char* c) 
  { if ( strcmp(c, "monitor") != 0 ) return bmlnElmnt::isType(c); 
    else return 1;
  }

  bmlnElmnt* Clone() const 
  { return new monitor( *this ); }
};



class hmonitor : public monitor
{
public:
  // Constructors
  hmonitor();
  hmonitor( const char* /*name*/ );
  hmonitor( const char*, double ); 
  hmonitor( const hmonitor& );
  virtual ~hmonitor();

  double operator[]( int );    // Readout of data
  void localPropagate( ParticleBunch& x ) 
  { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle&   );
  void localPropagate( JetParticle& );

  const char* Type() const;
  virtual int isType(char* c) 
  { if ( strcmp(c, "hmonitor") != 0 ) return monitor::isType(c); 
    else return 1; 
  }
  bmlnElmnt* Clone() const 
  { return new hmonitor( *this ); }
} ;


class vmonitor : public monitor
{
public:
  // Constructors
  vmonitor();  // Data to be written to standard output
  vmonitor( const char* /*name*/ );
  vmonitor( const char*, double ); 
  vmonitor( const vmonitor& );
  virtual ~vmonitor();

  double operator[]( int );    // Readout of data
  void localPropagate( ParticleBunch& x ) 
  { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle&   );
  void localPropagate( JetParticle& );

  const char* Type() const;
  virtual int isType(char* c) 
  { if ( strcmp(c, "vmonitor") != 0 ) return monitor::isType(c); 
    else return 1; 
  }
  bmlnElmnt* Clone() const 
  { return new vmonitor( *this ); }
} ;


#endif // MONITOR_H
