#if HAVE_CONFIG_H
#include <config.h>
#endif
/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.1
******                                    
******  File:      pinger.cc
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


#include <pinger.h>

using namespace std;

/** Implements the classes Pinger, HPinger and VPinger.  This bmlnElmt kicks
    the beam the same as an hkick and vkick, but only the first time a
    particle passes through the device after it is armed.

    The user must explicitly arm the pinger for it to work.  Note also that
    having the pinger armed before a closed-orbit calculation is performed
    won't work!

    E. McCrory, November, 1997
    **/

/* baseclass Pinger Constructors */

Pinger::Pinger() : bmlnElmnt() {
  strength = 0;
  _kick_direction = 0;
  _counter = -1;
}


Pinger::Pinger( double k, double r, int c ) : bmlnElmnt() {
  strength = k;
  _kick_direction = r;
  _counter = c;
}

Pinger::Pinger(char *n, double k, double r, int c) : bmlnElmnt(n) {
  strength = k;
  _kick_direction = r;
  _counter = c;
}

Pinger::Pinger(char *n) : bmlnElmnt(n) {
  strength = 0;
  _kick_direction = 0;
  _counter = -1;
}

Pinger::Pinger(const Pinger& p) : bmlnElmnt((bmlnElmnt&) p) {
}

Pinger::~Pinger() {
}

const char* Pinger::Type() const 
{ 
  return "Pinger"; 
}

istream& Pinger::readFrom(istream& is) {
  cout << "DGN: istream& Pinger::readFrom(istream& is) {" << endl;
  is >> _kick_direction >> _counter;
  return is;
}

ostream& Pinger::writeTo(ostream& os) {
  cout << "DGN: ostream& Pinger::writeTo(ostream& os) {" << endl;
  os << _kick_direction << "  " << _counter << "\n";
  return os;
}

/* HPinger Constructors */

HPinger::HPinger() : Pinger() {
}


HPinger::HPinger(double kick, int c) : Pinger(kick, 0.0, c) {
}

HPinger::HPinger(char *name, double kick, int c) : Pinger(name, kick, 0.0, c) {
}

HPinger::HPinger(char *name) : Pinger(name, 0.0, 0.0, -1) {
}

HPinger::HPinger(const HPinger& p) : Pinger(p) {
}

HPinger::~HPinger() {
}

const char* HPinger::Type() const 
{ 
  return "HPinger"; 
}


/* VPinger Constructors */

VPinger::VPinger() : Pinger(0.0, M_PI_2, -1) {
}


VPinger::VPinger(double kick, int c) : Pinger(kick, M_PI_2, c) {
}

VPinger::VPinger(char *name, double kick, int c) : Pinger(name, kick, M_PI_2, c) {
}

VPinger::VPinger(char *name) : Pinger(name, 0.0, M_PI_2, -1) {
}

VPinger::VPinger(const VPinger& p) : Pinger(p) {
}

VPinger::~VPinger() {
}

const char* VPinger::Type() const 
{ 
  return "VPinger"; 
}
