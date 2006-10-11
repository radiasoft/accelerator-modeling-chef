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
******  File:      QtMonitor.cc
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


// ************************************
// 
// File: QtMonitor.cc
// 
// Author: Leo Michelotti
// Date: September 17, 2001
// 
// ************************************

#include <typeinfo>

#include "QtMonitor.h"
#include "bmlnElmnt.h"
#include "Particle.h"
#include "BeamlineIterator.h"

// -------------------------------
// Implementation: class QtMonitor
// -------------------------------

QtMonitor::QtMonitor( const char* identifier )
: monitor( (char*) identifier ), _azimuth(0.0)
{
}

QtMonitor::QtMonitor( const QtMonitor& x )
: QObject(this), monitor( x ), _azimuth( x._azimuth )
{
}

QtMonitor::~QtMonitor()
{
}

int QtMonitor::setAzimuth( const beamline* x )
{
  DeepBeamlineIterator dbi( *x );
  bmlnElmnt* q;
  int n = 0;
  double s = 0.0;

  while((  q = dbi++  )) {
    s += q->Length();
    if(typeid(*q) == typeid(QtMonitor)) {
      n++;
      dynamic_cast<QtMonitor*>(q)->_azimuth = s;
    }
  }

  return n;
}

int QtMonitor::setAzimuth( const beamline& x )
{
  return QtMonitor::setAzimuth( &x );
}


void QtMonitor::localPropagate( Particle& p )
{
  this->monitor::localPropagate( p );
  emit ping( _azimuth, p.State() );
}
