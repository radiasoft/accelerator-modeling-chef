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

#ifndef MONITOR_H
#define MONITOR_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>

class BmlVisitor;
class ConstBmlVisitor;

class monitor;
class hmonitor;
class vmonitor;

typedef boost::shared_ptr<monitor>        MonitorPtr;
typedef boost::shared_ptr<monitor  const> ConstMonitorPtr;
typedef boost::shared_ptr<hmonitor>       HMonitorPtr;
typedef boost::shared_ptr<hmonitor const> ConstHMonitorPtr;
typedef boost::shared_ptr<vmonitor>       VMonitorPtr;
typedef boost::shared_ptr<vmonitor const> ConstVMonitorPtr;


class DLLEXPORT monitor : public bmlnElmnt {

public:

  // Constructors

  monitor();
  monitor( const char* name );
  monitor( const char* name, double const& length);
  monitor( const monitor& );

  monitor* Clone() const  { return new monitor( *this ); }

  virtual ~monitor();

  // ---------------------
  void setOutputStream( std::ostream& );
  void setOutputStream( std::ostream* );

  virtual bool on();  // returns previous state
  virtual bool off();

  virtual const char* version();

  double operator[]( int );  // Readout of data

  double setDriftFraction( double f ) 
  { double ret = _driftFraction; 
    if ( f <= 1 && f >= 0 ) _driftFraction = f; 
    return ret; 
  }

  double getDriftFraction() { return _driftFraction; }


  std::ostream& writeTo(std::ostream&); 
  std::istream& readFrom(std::istream&);

  void getState(double *s) const
  { for ( int i=0; i<6; i++ ) s[i] = _rgr[i]; } // DANGEROUS!!


  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }

  void localPropagate( Particle&   );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;
  
  inline bool State() const { return _onOffSwitch; }
  const char* Type() const;

private:

  double   _driftFraction;

protected:

  std::ostream* _outputStreamPtr;  // not owned
  bool          _onOffSwitch;
  double*       _rgr;
  


};


class DLLEXPORT hmonitor : public monitor {

 public:

  // Constructors

  hmonitor();
  hmonitor( const char*  name );
  hmonitor( const char*, double const& length); 
  hmonitor( hmonitor const& );

  hmonitor* Clone() const  { return new hmonitor( *this ); }

  virtual ~hmonitor();

  double operator[]( int );    // Readout of data

  void localPropagate( ParticleBunch& x )  { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle&   );
  void localPropagate( JetParticle& );

  const char* Type() const;

 } ;


class DLLEXPORT vmonitor : public monitor {

public:
  // Constructors

  vmonitor();  // Data to be written to standard output
  vmonitor( const char* name );
  vmonitor( const char* name, double const& length); 
  vmonitor( const vmonitor& );

  vmonitor* Clone() const  { return new vmonitor( *this ); }

  virtual ~vmonitor();

  double operator[]( int );    // Readout of data

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle&   );
  void localPropagate( JetParticle& );

  const char* Type() const;

};

#endif // MONITOR_H
