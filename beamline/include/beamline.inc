#ifndef BEAMLINE_INC
#define BEAMLINE_INC

#ifndef __VISUAL_CPP__
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#endif
#include <math.h>
#include "beamline.h"

static const double  pi           = 3.14159265358979323846;
static const double  twoPi        = 6.28318530717958647693;

#ifndef BMLN_SMALL
#define BMLN_SMALL    1.0e-14 // Used by DA::addTerm to decide
                              //   removal of a DAterm.
#endif  // BMLN_SMALL

// **************************************************
//  External declarations
// **************************************************

extern lattFunc BMLN_lattBuff;    // Introduced for use by resonance
extern double   BMLN_energyBuff;  // calculating functions.
extern double*  BMLN_dblBuff;

#endif // BEAMLINE_INC
