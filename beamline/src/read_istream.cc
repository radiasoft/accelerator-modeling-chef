/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                ****
******  read_istream: global functions and class methods for doing    ****
******                read/write of ".bml" files                      ****
******                                                                ****
******  Copyright (c) 1991  Universities Research Association, Inc.   ****
******                All Rights Reserved                             ****
******                                                                ****
******  Author:    Elliott McCrory                                    ****
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
**************************************************************************
******                                                                ****
******  February 3, 1998                                              ****
******  First implementation and test. The basic idea is to isolate   ****
******  the streaming operators here so that if you do not use them   ****
******  you do not have to link in all possible bmlnElmnt's.  So any  ****
******  method or function that has the possibility of polymorphically ***
******  calling any bmlnElmnt is put here.                            ****
******                                                                ****
**************************************************************************
*************************************************************************/

#include "beamline.h"
#ifdef __VISUAL_CPP__
#include <iomanip>
#else
#include <iomanip.h>
#endif

#include "combinedFunction.h"
#include "kick.h"
#include "Slot.h"
#if !defined(__VISUAL_CPP__) && !defined(__BORLAND_CPP__)
#include "mwiremonitor.h"
#endif
#include "pinger.h"
#include "CF_rbend.h"

bmlnElmnt* read_istream(istream& is)
{
  // This is, in essence, the global "factory" function needed for operator<<().
  //
  // Read in a bmlnElmnt.  Recurses when a beamline is found (withing
  // beamline::readFrom(). Returns the bmnlElmnt which has been read
  // in.  Note that knowledge of ALL possible types of bmlnElmnt's is
  // necessary, as is shown in the long string of "if/else if/else
  // if/..." below.

  // This will be the return value at the end
  bmlnElmnt* element = NULL;

  // Need one possible object of each type of bmlnElmnt
  BBLens		* bblensPtr;
  JetQuadrupole		* jQuadPtr; 
  JetthinQuad		* jtQuadPtr;
  JetthinSext		* jtSextPtr;
  beamline		* bl;
  combinedFunction	* cmbPtr;
  drift			* driftPtr;
  hkick			* hkickPtr;
  hmonitor		* hmonitorPtr;
  marker		* markerPtr;
  monitor		* monitorPtr;
  octupole		* octupolePtr;
  quadrupole		* quadrupolePtr;
  rbend			* rbendPtr;
  sbend			* sbendPtr;
  sector		* sectorPtr;
  sextupole		* sextupolePtr;
  srot			* srotPtr;
  thin12pole		* pole12Ptr;
  thin14pole		* pole14Ptr;
  thin16pole		* pole16Ptr;
  thin18pole		* pole18Ptr;
  thinDecapole		* thinDecapolePtr;
  thinLamb		* thinLambPtr;
  thinMultipole		* thinMultipolePtr;
  thinOctupole		* thinOctupolePtr;
  thinQuad		* thinQuadPtr;
  thinSeptum		* thinSeptaPtr;
  thinSextupole		* thinSextupolePtr;
  thinrfcavity		* cavityPtr;
  vkick			* vkickPtr;
  vmonitor		* vmonitorPtr;
  Pinger                * PingerPtr;
  HPinger               * hPingerPtr;
  VPinger               * vPingerPtr;
  kick                  * kickPtr;
  Slot			* slot;
  CF_rbend              * cfRbendPtr;

  const double MIN_ANGLE = 2.0E-9;
  const int SIZE=80;
  char type[SIZE], *name;
  double length, strength, x, y, t;
  static char previousLine[200];

  name = new char[SIZE];

  if ( is.eof() )
    return NULL;

  // read in the invariant stuff
  is >> type >> name >> length >> strength >> x >> y >> t;
  
  // Figure out which type of element we have here

  // cerr << "Have an element of type \"" << type << "\", name: " << name << "\n";

  if ( strcasecmp(type, 		"beamline") == 0 ) {
    bl = new beamline(name);
    element = bl;
  }
  else if ( strcasecmp(type,            "Slot") == 0 ) {
    slot = new Slot(name);
    element = slot;
  }
  else if ( strcasecmp(type, 		"beamline_END") == 0 ) {
    element = NULL;
  }
  else if ( strcasecmp(type, 		"combinedFunction_END") == 0 ) {
    element = NULL;
  }
  else if ( strcasecmp(type,            "slot_END") == 0 ) {
    element = NULL;
  }
  else if ( strcasecmp(type, 		"combinedFunction") == 0 ) {
    cmbPtr = new combinedFunction(name);
    element = cmbPtr;
  }
  else if( strcasecmp(type, 		"drift") == 0 ) {
    driftPtr = new drift(name, length);
    element = driftPtr;
  }
  else if( strcasecmp(type, 		"hkick") == 0 ) {
    hkickPtr = new hkick(name, strength);
    element = hkickPtr;
  }
  else if( strcasecmp(type, 		"vkick") == 0 ) {
    vkickPtr = new vkick(name, strength);
    element = vkickPtr;
  }
  else if( strcasecmp(type, 		"kick") == 0 ) {
    kickPtr = new kick(name);
    element = kickPtr;
  }
  else if( strcasecmp(type, 		"vpinger") == 0 ) {
    vPingerPtr = new VPinger(name, strength);
    element = vPingerPtr;
  }
  else if( strcasecmp(type, 		"hpinger") == 0 ) {
    hPingerPtr = new HPinger(name, strength);
    element = hPingerPtr;
  }
  else if( strcasecmp(type, 		"pinger") == 0 ) {
    PingerPtr = new Pinger(name, strength);
    element = PingerPtr;
  }
  else if( strcasecmp(type, 		"hmonitor") == 0 ) {
    hmonitorPtr = new hmonitor(name);
    element = hmonitorPtr;
  }
  else if( strcasecmp(type, 		"marker") == 0 ) {
    markerPtr = new marker(name);
    element = markerPtr;
  }
  else if( strcasecmp(type, 		"monitor") == 0 ) {
    monitorPtr = new monitor(name);
    element = monitorPtr;
  }
#if !defined(__VISUAL_CPP__) && !defined(__BORLAND_CPP__)
  else if( strcasecmp(type, 		"mwireMonitor") == 0 ) {
    monitorPtr = new mwireMonitor(name);
    element = monitorPtr;
  }
#endif
  else if( strcasecmp(type, 		"octupole") == 0 ) {
    octupolePtr = new octupole(name, length, strength);
    element = octupolePtr;
  }
  else if( strcasecmp(type, 		"quadrupole") == 0 ) {
    quadrupolePtr = new quadrupole(name, length, strength);
    element = quadrupolePtr;
  }
  else if( strcasecmp(type, 		"rbend") == 0 ) {
    rbendPtr = new rbend(name, length, strength, MIN_ANGLE);
    element = rbendPtr;
  }
  else if( strcasecmp(type, 		"sbend") == 0 ) {
    sbendPtr = new sbend(name, length, strength, MIN_ANGLE);
    element = sbendPtr;
  }
  else if( strcasecmp(type, 		"sextupole") == 0 ) {
    sextupolePtr = new sextupole(name, length, strength);
    element = sextupolePtr;
  }
  else if( strcasecmp(type, 		"srot") == 0 ) {
    srotPtr = new srot(name, strength);
    element = srotPtr;
  }
  else if( strcasecmp(type, 		"thinDecapole") == 0 ) {
    thinDecapolePtr = new thinDecapole(name, strength);
    element = thinDecapolePtr;
  }
  else if( strcasecmp(type, 		"thinLamb") == 0 ) {
    thinLambPtr = new thinLamb(name);
    element = thinLambPtr;
  }
  else if( strcasecmp(type, 		"thinMultipole") == 0 ) {
    thinMultipolePtr = new thinMultipole(name, strength);
    element = thinMultipolePtr;
  }
  else if( strcasecmp(type, 		"thinOctupole") == 0 ) {
    thinOctupolePtr = new thinOctupole(name, strength);
    element = thinOctupolePtr;
  }
  else if( strcasecmp(type, 		"thinQuad") == 0 ) {
    thinQuadPtr = new thinQuad(name, strength);
    element = thinQuadPtr;
  }
  else if( strcasecmp(type, 		"thinSeptum") == 0 ) {
    thinSeptaPtr = new thinSeptum(name);
    element = thinSeptaPtr;
  }
  else if( strcasecmp(type, 		"thinSextupole") == 0 ) {
    thinSextupolePtr = new thinSextupole(name, strength);
    element = thinSextupolePtr;
  }
  else if( strcasecmp(type, 		"thinrfcavity") == 0 ) {
    cavityPtr = new thinrfcavity(name);
    element = cavityPtr;
  }
  else if( strcasecmp(type, 		"vmonitor" ) == 0 ) {
    vmonitorPtr = new vmonitor(name);
    element = vmonitorPtr;
  }
  else if( strcasecmp(type, 		"BBLens") == 0 ) {
    bblensPtr = new BBLens(name, length);
    element = bblensPtr;
  }
  else if( strcasecmp(type, 		"JetQuadrupole") == 0 ) {
    jQuadPtr = new JetQuadrupole(name, strength, length, 1);
    element = jQuadPtr;
  }
  else if( strcasecmp(type, 		"JetthinQuad") == 0 ) {
    jtQuadPtr = new JetthinQuad(name, strength, 1);
    element = jtQuadPtr;
  }
  else if( strcasecmp(type, 		"sector") == 0 ) {
    sectorPtr = new sector(name, length);
    element = sectorPtr;
  }
  else if( strcasecmp(type, 		"thin12pole") == 0 ) {
    pole12Ptr = new thin12pole(name, length);
    element = pole12Ptr;
  }
  else if( strcasecmp(type, 		"thin14pole") == 0 ) {
    pole14Ptr = new thin14pole(name, length);
    element = pole14Ptr;
  }
  else if( strcasecmp(type, 		"thin16pole") == 0 ) {
    pole16Ptr = new thin16pole(name, length);
    element = pole16Ptr;
  }
  else if( strcasecmp(type, 		"thin18pole") == 0 ) {
    pole18Ptr = new thin18pole(name, length);
    element = pole18Ptr;
  }
  else if( strcasecmp(type, 		"JetthinSext") == 0 ) {
    jtSextPtr = new JetthinSext(name, length, 1);
    element = jtSextPtr;
  }
  else if( strcasecmp(type, 		"CF_rbend") == 0 ) {
    cfRbendPtr = new CF_rbend(name, length, strength, MIN_ANGLE);
    element = cfRbendPtr;
  }
  else {
    cerr << "read_istream(istream&): Unknown element type \"" << type << "\"\n";
    cerr << "Previous line read in was: \n" << previousLine << endl;
  }

  // Save away the current line in case we need to report something 
  // unreadible next time.
  sprintf(previousLine,"%s  %s  %lf  %lf  %lf  %lf  %lf",
	  type,name,length,strength,x,y,t);

  // Get the rest of the description if we got a real element
  if ( element ) {
    element->readFrom(is);

    // Check if this element is misaligned
    if ( x!=0 || y!=0 || t!= 0 ) {
      alignmentData align;
      align.xOffset = x;
      align.yOffset = y;
      align.tilt    = t;
      element->setAlignment(align);
    }
  }

  delete [] name;
  return element;
}
 

// ==== Relevant to class beamline. ==================================================
// This is put here so linking to beamline.o does not necessarily bring in
// all the modules from all the bmlnElmnt descendants.

istream& beamline::readFrom(istream& is)
{
  bmlnElmnt *e = NULL;
  is >> nominalEnergy;
  // cerr << "Beamline " << Name() << " has energy of " << nominalEnergy << "\n";
  // Now, continue reading is until we see the end of this beamline
  length = 0;
  do {
    e = read_istream(is);	// (Recursion)
    // read_istream will return NULL when end of file or end of beamline is reached.
    if ( e ) 
      append(e);
  } while ( e );

  return is;
}


istream& operator>>(istream& is, beamline& bl)
{
  const int SIZE=80;
  char name[SIZE], type[SIZE];
  double length, strength, x, y, t;

  // Read in a beamline
  is >> type >> name >> length >> strength >> x >> y >> t;

  if ( strcasecmp(type, "beamline") != 0 ) {
    cerr << " **** WARNING **** Expecting data file to begin with a \"beamline\" directive\n";
    cerr << " **** WARNING **** First element, " << name << ", ignored\n";
  }
  else
    bl.Rename(name);
  
  bl.readFrom(is);		// Polymorphically call the right readFrom().

  if ( x!=0 || y!=0 || t!=0 ) {
    alignmentData align;
    align.xOffset = x;
    align.yOffset = y;
    align.tilt    = t;
    bl.setAlignment(align);
  }
  return is;
}

// ==== Relevant to class bmlnElmnt ==================================================

ostream& operator<<(ostream& os, bmlnElmnt& b)
{
  if ( &b ) {
    os << OSTREAM_DOUBLE_PREC 
       << b.Type() 
       << " " 
       << b.Name() 
       << " " 
       << b.Length() 
       << " " 
       << b.Strength() 
       << " " ;
    os << (*b.align) << "\n";
    b.writeTo(os); // Polymorphically call the appropriate writeTo().
  }
  return os;
}

