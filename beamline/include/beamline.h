/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                ****
******  BEAMLINE:  C++ objects for design and analysis  .             ****
******             of beamlines, storage rings, and                   ****
******             synchrotrons.                                      ****
******                                                                ****
******  Copyright (c) 1991  Universities Research Association, Inc.   ****
******                All Rights Reserved                             ****
******                                                                ****
******  Author:    Leo Michelotti                                     ****
******                                                                ****
******             Fermilab                                           ****
******             P.O.Box 500                                        ****
******             Mail Stop 345                                      ****
******             Batavia, IL   60510                                ****
******                                                                ****
******             Phone: (708) 840 4956                              ****
******             Email: michelot@hazel.fnal.gov                     ****
******                    michelotti@adcalc.fnal.gov                  ****
******                    almond::michelotti                          ****
******                                                                ****
******  Release    Version 1.0 : << not released >>                   ****
******  Dates:                                                        ****
******                                                                ****
**************************************************************************
**************************************************************************
******                                                                ****
******  This material resulted from work developed under a            ****
******  Government Contract and is subject to the following           ****
******  license: The Government retains a paid-up, nonexclusive,      ****
******  irrevocable worldwide license to reproduce, prepare           ****
******  derivative works, perform publicly and display publicly       ****
******  by or for the Government, including the right to distribute   ****
******  to other Government contractors.  Neither the United          ****
******  States nor the United States Department of Energy, nor        ****
******  any of their employees, makes any warranty, express or        ****
******  implied, or assumes any legal liability or responsibility     ****
******  for the accuracy, completeness or usefulness of any           ****
******  information, apparatus, product, or process disclosed, or     ****
******  represents that its use would not infringe privately owned    ****
******  rights.                                                       ****
******                                                                ****
******  These files are made avaliable for use by specific            ****
******  individuals or a specific group.  They are not to be          ****
******  reproduced or redistributed.                                  ****
******                                                                ****
******  These files are provided "as is" with no warranties of any    ****
******  kind including the warranties of design,                      ****
******  merchantibility and fitness for a particular purpose,         ****
******  or arising from a course of dealing, usage or trade           ****
******  practice.                                                     ****
******                                                                ****
******  These files are provided with no support or obligation on     ****
******  the part of U.R.A. or Fermilab to assist in their use,        ****
******  correction, modification or enhancement.                      ****
******                                                                ****
******  Neither U.R.A. nor Fermilab shall be liable with              ****
******  respect to the infringement of copyrights, trade              ****
******  secrets or any patents by these files or any part             ****
******  thereof.                                                      ****
******                                                                ****
******  In no event will U.R.A., Fermilab, or the author(s) be        ****
******  liable for any lost revenue or profits or other               ****
******  special, indirect and consequential damages, even if          ****
******  they have been advised of the possibility of such             ****
******  damages.                                                      ****
******                                                                ****
**************************************************************************
*************************************************************************/

#ifndef BEAMLINE_H
#define BEAMLINE_H

//#include <complex.h>   // ??? Why is this here?
#ifndef __VISUAL_CPP__
#include <stdio.h>
#include <string.h>
#endif

#ifdef __SUNPRO_CC
  #ifndef __STL_NEED_EXPLICIT
    #define __STL_NEED_EXPLICIT
  #endif
#endif

// ??? Are all these really needed???
#include "PhysicsConstants.h"
#include <math.h>
// #include <string>   (More trouble than it's worth.)
#include "MathConstants.h"
#include "mxyzptlk.h"
#include "slist.h"
#include "Barnacle.h"
#include "VectorD.h"
#include "JetVector.h"
#include "Particle.h"

#if defined (__VISUAL_CPP__) && !defined(__KCC)
#define strcasecmp stricmp
#endif

#ifdef __BORLAND_CPP__
#define strcasecmp strcmpi
#endif


// ................................ Beamline classes

#ifndef BMLNELMNT_H
#include "bmlnElmnt.h"
#endif

#ifndef KICK_H
#include "kick.h"
#endif

#ifndef RFCAVITY_H
#include "rfcavity.h"
#endif

#ifndef SROT_H
#include "srot.h"
#endif

#ifndef MONITOR_H
#include "monitor.h"
#endif

#ifndef MARKER_H
#include "marker.h"
#endif

#ifndef DRIFT_H
#include "drift.h"
#endif


#ifndef RBEND_H
#include "rbend.h"
#endif

#ifndef SBEND_H
#include "sbend.h"
#endif

#ifndef SECTOR_H
#include "sector.h"
#endif

#ifndef QUADRUPOLE_H
#include "quadrupole.h"
#endif

#ifndef JETQUADRUPOLE_H
#include "JetQuadrupole.h"
#endif

#ifndef SEXTUPOLE_H
#include "sextupole.h"
#endif

#ifndef OCTUPOLE_H
#include "octupole.h"
#endif

#ifndef DECAPOLE_H
#include "decapole.h"
#endif

#ifndef THINPOLES_H
#include "thinpoles.h"
#endif


#ifndef BBLENS_H
#include "BBLens.h"
#endif

#ifndef SEPTUM_H
#include "septum.h"
#endif

#ifndef LAMBERTSON_H
#include "lambertson.h"
#endif

#ifndef BEAMLINE_ITERATOR
#include "BeamlineIterator.h"
#endif


#ifndef CIRCUIT_H
#include "circuit.h"
#endif

#ifndef ICIRCUIT_H
#include "ICircuit.h"
#endif

#ifndef FCIRCUIT_H
#include "FCircuit.h"
#endif

#ifndef MOVER_H
#include "mover.h"
#endif

#ifndef BIPOLARCIRCUIT_H
#include "BipolarCircuit.h"
#endif

#endif // BEAMLINE_H
