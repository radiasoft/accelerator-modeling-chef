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

  monitor();
  monitor( std::string const& name );
  monitor( std::string const& name, double const& length);
  monitor( monitor const& );

  monitor* Clone() const  { return new monitor( *this ); }

 ~monitor();

  monitor& operator=(monitor const& rhs);

  double hposition() const; 
  double vposition() const; 

  void setHposition( double const& ) const;
  void setVposition( double const& ) const;

  double const& npx() const; 
  double const& npy() const; 

  void setNpx( double const& ) const; 
  void setNpy( double const& ) const;

  double        setDriftFraction( double f ); 
  double const& getDriftFraction()  const;

  std::ostream& writeTo (std::ostream&); 
  std::istream& readFrom(std::istream&);

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;
  
  bool isEnabled() const;
  bool enable( bool set);

  const char* Type()      const;
  bool        isMagnet()  const;
  bool        isThin()    const;
  bool        isPassive() const;

protected:

  bool          enabled_;          // enabled or defective/disabled   

  mutable double        xpos_;   // recorded single particle or bunch averaged position
  mutable double        ypos_;   // recorded single particle or bunch averaged position
  mutable double        npx_;    // recorded single particle npx -- not really belongs in a monitor this is an extension
  mutable double        npy_;    // recorded single particle npy -- not really belongs in a monitor this is an extension

  double        dx_;     // systematic offset
  double        dy_;     // systematic offset

  double        xrerr_;  // random relative error amplitude (gaussian distributed)
  double        yrerr_;  // random relative error amplitude (gaussian distributed)

 private:

  double        driftFraction_;
};

//==================================================================================

class DLLEXPORT hmonitor : public monitor {

  class Propagator;

 public:

  hmonitor();
  hmonitor( std::string const& name );
  hmonitor( std::string const& name, double const& length); 
  hmonitor( hmonitor const& );

  hmonitor* Clone() const  { return new hmonitor( *this ); }

 ~hmonitor();

  hmonitor& operator=(hmonitor const& rhs);

  const char* Type()     const;
  bool        isMagnet() const;
  bool        isThin()    const;
  bool        isPassive() const;

 };

//======================================================================================

class DLLEXPORT vmonitor : public monitor {

  class Propagator;

public:

  vmonitor();  
  vmonitor( std::string const& name );
  vmonitor( std::string const& name, double const& length); 
  vmonitor( vmonitor const& );

  vmonitor* Clone() const  { return new vmonitor( *this ); }

 ~vmonitor();

  vmonitor& operator=(vmonitor const& rhs);

  const char* Type()      const;
  bool        isMagnet()  const;
  bool        isThin()    const;
  bool        isPassive() const;


};

#endif // MONITOR_H
