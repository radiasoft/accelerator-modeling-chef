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


#ifndef QTMONITOR_H
#define QTMONITOR_H

#ifndef MONITOR_H
#include "monitor.h"
#endif

class beamline;

#include <qobject.h>

class QtMonitor : public QObject, public monitor
{
Q_OBJECT

  public:
    QtMonitor( const char* = 0 );
    QtMonitor( const QtMonitor& );
    ~QtMonitor();

    static int setAzimuth( const beamline& );
    static int setAzimuth( const beamline* );
    // Returns number of QtMonitors in the beamline.

    void localPropagate( Particle& );
    bmlnElmnt* Clone() const { return new QtMonitor( *this ); }

  signals:
    void ping( double, const Vector& );

  private:
    double _azimuth;
};

#endif // QTMONITOR_H
