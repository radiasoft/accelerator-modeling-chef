/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      monitor.h
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

#include "bmlnElmnt.h"

class monitor : public bmlnElmnt
{
 private:
  double driftFraction;
protected:
  FILE*    outputFile;
  char     onOff;
  double*  rgr;

public:
  monitor();                   // Data to be written to standard output
  monitor( char* );            // Name identifier.
  monitor( char*, double );    // Name identifier, length.
  monitor( FILE* );            // Data written to file.
  monitor( char*, FILE* );     //                      ... and name.
  monitor( bmlnElmntData& );
  monitor( const monitor& );
  virtual ~monitor();
  // ---------------------
  void setOutputFile( FILE* );
  virtual void on();
  virtual void off();
  double operator[]( int );    // Readout of data
  double setDriftFraction(double f) { if ( f <= 1 && f >= 0 ) driftFraction = f; return driftFraction; }
  double getDriftFraction() { return driftFraction; }
  virtual ostream& writeTo(ostream&);	// Is this "virtual" tag really necessary?  Probably!
  virtual istream& readFrom(istream&);
  void getState(double *s) { for ( int i=0; i<6; i++ ) s[i] = rgr[i]; }
  // ---------------------
  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle&   );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitMonitor( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitMonitor( this ); }
  
  inline int State() const { return onOff; }
  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "monitor") != 0 ) return bmlnElmnt::isType(c); else return 1; }

  bmlnElmnt* Clone() const { return new monitor( *this ); }
} ;

class hmonitor : public monitor
{
public:
  hmonitor();                   // Data to be written to standard output
  hmonitor( char* );            // Name identifier.
  hmonitor( char*, double );    // Name identifier, length.
  hmonitor( FILE* );            // Data written to file.
  hmonitor( char*, FILE* );     // Data written to file ... and name.
  hmonitor( bmlnElmntData& );
  hmonitor( const hmonitor& );
  ~hmonitor();
  double operator[]( int );    // Readout of data
  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle&   );
  void localPropagate( JetParticle& );

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "hmonitor") != 0 ) return monitor::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new hmonitor( *this ); }
} ;

struct monitorData : public bmlnElmntData {
  char    onOff;
  double  driftFraction;

  monitorData();

  monitorData( monitorData& );
  ~monitorData();
  void eliminate();
  void* clone();

};


class vmonitor : public monitor
{
public:
  vmonitor();                   // Data to be written to standard output
  vmonitor( char* );            // Name identifier.
  vmonitor( char*, double );    // Name identifier, length.
  vmonitor( FILE* );            // Data written to file.
  vmonitor( char*, FILE* );     // Data written to file ... and name.
  vmonitor( bmlnElmntData& );
  vmonitor( const vmonitor& );
  ~vmonitor();
  double operator[]( int );    // Readout of data
  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle&   );
  void localPropagate( JetParticle& );

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "vmonitor") != 0 ) return monitor::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new vmonitor( *this ); }
} ;

#endif // MONITOR_H
