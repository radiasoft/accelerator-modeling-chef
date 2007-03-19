/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                                                
******  Version:   3.0                    
******                                    
******  File:      QtMonitor.h
******                                                                
******  Copyright (c) 2004  Universities Research Association, Inc.   
******                All Rights Reserved                             
******
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S.and Foreign Copyright Laws. 
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
****** REVISION HISTORY
****** Mar 2007 ostiguy@fnal.gov
****** - removed references to slist/dlist
****** - support for reference counted elements and beamlines
******                                                                
**************************************************************************
*************************************************************************/


#ifndef QTMONITOR_H
#define QTMONITOR_H


#include <beamline/monitor.h>
#include <beamline/BmlPtr.h>
#include <qobject.h>

class QtMonitor;

typedef boost::shared_ptr<QtMonitor>            QtMonitorPtr;
typedef boost::shared_ptr<QtMonitor const> ConstQtMonitorPtr;


class QtMonitor : public QObject, public monitor {

Q_OBJECT

  public:

    QtMonitor( const char* = 0 );
    QtMonitor( QtMonitor const& );
   ~QtMonitor();

    static int setAzimuth( beamline const& );

    // Returns number of QtMonitors in the beamline.

    void localPropagate( Particle& );
    QtMonitor* Clone() const { return new QtMonitor( *this ); }

  signals:
    void ping( double, const Vector& );

  private:
    double  azimuth_;
};

#endif // QTMONITOR_H
