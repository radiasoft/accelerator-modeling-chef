/*
 *  File: ClosedOrbitSage.h
 *  The "Sage" for using FPSolver.
 *  
 *  Leo Michelotti
 *  November 23, 1998
 */

#ifndef CLORB_SAGE_H
#define CLORB_SAGE_H

#ifndef SAGE_H
#include "Sage.h"
#endif
#ifndef BEAMLINE_H
#include "beamline.h"
#endif

class ClosedOrbitSage : public Sage
{
 public:
  ClosedOrbitSage( const beamline*, bool = false );
  // Second argument is used by class Sage
  // to control cloning. (See Sage.h)
  ~ClosedOrbitSage();
  
  int findClosedOrbit( JetParticle* );

  void eraseAll();
  void setForcedCalc();
  void unsetForcedCalc();

 private:
  char _forced;
};


inline void ClosedOrbitSage::setForcedCalc()
{
  _forced = 1;
}

inline void ClosedOrbitSage::unsetForcedCalc()
{
  _forced = 0;
}



#endif // CLORB_SAGE_H
