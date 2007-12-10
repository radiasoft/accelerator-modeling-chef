/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      monitor.h
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
****** Sep 2007           ostiguy@fnal.gov
******
****** - added systematic and random errors 
****** - added bunch propagation
******   
****** Dec 2007           ostiguy@fnal.gov
****** - new typesafe propagators
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

  class Propagator; 

public:

  typedef boost::shared_ptr<BasePropagator<monitor> > PropagatorPtr;   

  monitor();
  monitor( char const* name );
  monitor( char const* name, double const& length);
  monitor( monitor const& );

  monitor* Clone() const  { return new monitor( *this ); }

 ~monitor();

  double hposition() const; 
  double vposition() const; 

  void setHposition( double const& );
  void setVposition( double const& );

  double const& npx() const; 
  double const& npy() const; 

  void setNpx( double const& ); 
  void setNpy( double const& );

  double        setDriftFraction( double f ); 
  double const& getDriftFraction()  const;

  std::ostream& writeTo (std::ostream&); 
  std::istream& readFrom(std::istream&);

  void localPropagate( Particle&   );
  void localPropagate( JetParticle& );
  void localPropagate( ParticleBunch& x );
  void localPropagate( JetParticleBunch& x );

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;
  
  bool isEnabled() const;
  bool enable( bool set);

  const char* Type()     const;
  bool        isMagnet() const;

protected:

  bool          enabled_;          // enabled or defective/disabled   

  double        xpos_;   // recorded single particle or bunch averaged position
  double        ypos_;   // recorded single particle or bunch averaged position
  double        npx_;    // recorded single particle npx -- not really belongs in a monitor this is an extension
  double        npy_;    // recorded single particle npy -- not really belongs in a monitor this is an extension

  double        dx_;     // systematic offset
  double        dy_;     // systematic offset

  double        xrerr_;  // random relative error amplitude (gaussian distributed)
  double        yrerr_;  // random relative error amplitude (gaussian distributed)

 private:

  double        driftFraction_;
  PropagatorPtr propagator_;

};

//==================================================================================

class DLLEXPORT hmonitor : public monitor {

  class Propagator;

 public:

  typedef boost::shared_ptr<BasePropagator<hmonitor> > PropagatorPtr;   

  hmonitor();
  hmonitor( const char*  name );
  hmonitor( const char*, double const& length); 
  hmonitor( hmonitor const& );

  hmonitor* Clone() const  { return new hmonitor( *this ); }

 ~hmonitor();

  void localPropagate( Particle&   );
  void localPropagate( JetParticle& );
  void localPropagate( ParticleBunch& x );
  void localPropagate( JetParticleBunch& x );

  const char* Type()     const;
  bool        isMagnet() const;

 private:

  PropagatorPtr propagator_;

 };

//======================================================================================

class DLLEXPORT vmonitor : public monitor {

  class Propagator;

public:

  typedef boost::shared_ptr<BasePropagator<vmonitor> > PropagatorPtr;   

  vmonitor();  
  vmonitor( const char* name );
  vmonitor( const char* name, double const& length); 
  vmonitor( const vmonitor& );

  vmonitor* Clone() const  { return new vmonitor( *this ); }

 ~vmonitor();

  void localPropagate( Particle&   );
  void localPropagate( JetParticle& );
  void localPropagate( ParticleBunch& x );
  void localPropagate( JetParticleBunch& x );

  const char* Type()     const;
  bool        isMagnet() const;

 private:

  PropagatorPtr propagator_;

};

#endif // MONITOR_H
