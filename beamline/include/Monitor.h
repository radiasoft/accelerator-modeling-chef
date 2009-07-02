/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      Monitor.h
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
****** Sep 2007           ostiguy@fnal.gov
****** - added systematic and random errors 
****** - added bunch propagation
****** Dec 2007           ostiguy@fnal.gov
****** - new typesafe propagators
****** May 2008 ostiguy@fnal
****** - proper, explicit assignment operator
****** - propagator moved (back) to base class
****** - no assumption about internal structure
****** - monitor reading attributes declared mutable
******
**************************************************************************
*************************************************************************/
#ifndef MONITOR_H
#define MONITOR_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>
#include <boost/function.hpp>

class BmlVisitor;
class ConstBmlVisitor;

class Monitor;
class HMonitor;
class VMonitor;

typedef boost::shared_ptr<Monitor>        MonitorPtr;
typedef boost::shared_ptr<Monitor  const> ConstMonitorPtr;
typedef boost::shared_ptr<HMonitor>       HMonitorPtr;
typedef boost::shared_ptr<HMonitor const> ConstHMonitorPtr;
typedef boost::shared_ptr<VMonitor>       VMonitorPtr;
typedef boost::shared_ptr<VMonitor const> ConstVMonitorPtr;


class DLLEXPORT MonitorBase: public bmlnElmnt {

 protected :

  MonitorBase();
  MonitorBase( std::string const& name );
  MonitorBase( std::string const& name, double const& length );
  MonitorBase( MonitorBase const& );

 ~MonitorBase();

  MonitorBase& operator=(MonitorBase const& rhs);

 public:

  double        setDriftFraction( double f ); 
  double const& getDriftFraction()  const;

  bool isEnabled() const;
  bool enable( bool set);

  std::ostream& writeTo (std::ostream&); // FIXME !
  std::istream& readFrom(std::istream&); // FIXME !

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;
  
  bool        isMagnet()     const;
  bool        isThin()       const;
  bool        isPassive()    const;
  bool        isDriftSpace() const;

protected:

  bool                      enabled_;   // enabled or defective/disabled   

  mutable double            xpos_;   // recorded single particle or bunch averaged position
  mutable double            ypos_;   // recorded single particle or bunch averaged position
  mutable double            npx_;    // recorded single particle npx -- not really belongs in a monitor this is an extension
  mutable double            npy_;    // recorded single particle npy -- not really belongs in a monitor this is an extension

  double                    dx_;     // systematic offset
  double                    dy_;     // systematic offset

  double                    xrerr_;  // random relative error amplitude (gaussian distributed)
  double                    yrerr_;  // random relative error amplitude (gaussian distributed)

  double                    driftFraction_;
 
  static boost::function<double()> nrnd_;    // random number generator (default: normally distributed) 
};


class DLLEXPORT HMonitor : public MonitorBase {

  class Propagator; 

 public:

  HMonitor();
  HMonitor( std::string const& name );
  HMonitor( std::string const& name, double const& length); 
  HMonitor( HMonitor const& );

  HMonitor* Clone() const  { return new HMonitor( *this ); }

 ~HMonitor();

  HMonitor& operator=(HMonitor const& rhs);

  double     hposition()                    const; 
  void    setHposition( double const& pos ) const; 

  double    npx()                const;
  void   setNpx( double const& ) const;

  const char* Type()     const;

 };


class DLLEXPORT VMonitor : public MonitorBase{

  class Propagator; 

 public:

  VMonitor();
  VMonitor( std::string const& name );
  VMonitor( std::string const& name, double const& length); 
  VMonitor( VMonitor const& );

  VMonitor* Clone() const  { return new VMonitor( *this ); }

 ~VMonitor();

  VMonitor& operator=(VMonitor const& rhs);

  double     vposition()                    const; 
  void    setVposition( double const& pos ) const; 

  double    npy()                const;
  void   setNpy( double const& ) const;

  const char* Type()     const;

 };


class DLLEXPORT Monitor : public MonitorBase {

  class Propagator; 

 public:

  Monitor();
  Monitor( std::string const& name );
  Monitor( std::string const& name, double const& length);
  Monitor( Monitor const& );

  Monitor* Clone() const  { return new Monitor( *this ); }

 ~Monitor();

  Monitor& operator=(Monitor const& rhs);

  double     hposition()                    const; 
  void    setHposition( double const& pos ) const; 

  double    npx()                const;
  void   setNpx( double const& ) const;

  double     vposition()                    const; 
  void    setVposition( double const& pos ) const; 

  double    npy()                const;
  void   setNpy( double const& ) const;

  const char* Type()     const;
};



#endif // MONITOR_H
