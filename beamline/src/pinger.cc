#include <pinger.h>

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
  disarm();
}


Pinger::Pinger(double k, double r) : bmlnElmnt() {
  strength = k;
  _kick_direction = r;
  disarm();
}

Pinger::Pinger(char *n, double k, double r) : bmlnElmnt(n) {
  strength = k;
  _kick_direction = r;
  disarm();
}

Pinger::Pinger(char *n) : bmlnElmnt(n) {
  strength = 0;
  _kick_direction = 0;
  disarm();
}

Pinger::Pinger(const Pinger& p) : bmlnElmnt((bmlnElmnt&) p) {
  disarm();
}

Pinger::~Pinger() {
  ;
}

const char* Pinger::Type() const 
{ 
  return "Pinger"; 
}

istream& Pinger::readFrom(istream& is) {
  is >> _kick_direction;
  return is;
}

ostream& Pinger::writeTo(ostream& os) {
  os << _kick_direction << "\n";
  return os;
}

/* HPinger Constructors */

HPinger::HPinger() : Pinger((double) 0, 0) {
  disarm();
}


HPinger::HPinger(double kick) : Pinger(kick, 0) {
  disarm();
}

HPinger::HPinger(char *name, double kick) : Pinger(name, kick, 0) {
  disarm();
}

HPinger::HPinger(char *name) : Pinger(name, 0, 0) {
  disarm();
}

HPinger::HPinger(const HPinger& p) : Pinger(p) {
  disarm();
}

HPinger::~HPinger() {
  ;
}

const char* HPinger::Type() const 
{ 
  return "HPinger"; 
}


/* VPinger Constructors */

VPinger::VPinger() : Pinger((double) 0, M_PI_2) {
  disarm();
}


VPinger::VPinger(double kick) : Pinger(kick, M_PI_2) {
  disarm();
}

VPinger::VPinger(char *name, double kick) : Pinger(name, kick, M_PI_2) {
  disarm();
}

VPinger::VPinger(char *name) : Pinger(name, 0, M_PI_2) {
  disarm();
}

VPinger::VPinger(const VPinger& p) : Pinger(p) {
  disarm();
}

VPinger::~VPinger() {
  ;
}

const char* VPinger::Type() const 
{ 
  return "VPinger"; 
}
