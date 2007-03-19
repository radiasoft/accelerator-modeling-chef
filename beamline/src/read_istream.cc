/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      read_istream.cc
******  Version:   2.1
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
*****
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
******  Author: Leo Michelotti
******          Elliott McCrory (original author of this file)
******            
******                                                                
******          Fermilab                                           
******          P.O.Box 500                                        
******          Mail Stop 220                                      
******          Batavia, IL   60510                                
******                                                             
******          Phone: (630) 840 4956                              
******          Email: michelotti@fnal.gov                         
******                                                                
****** REVISION HISTORY
****** Mar 2007     ostiguy@fnal.gov
****** -eliminated unsafe casts
****** -support for reference counted elements
**************************************************************************
*************************************************************************/


/******  read_istream: global functions and class methods for doing   ****
******     read/write of ".bml" files via stream operators.           ****
******                                                                ****/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iomanip>
#include <beamline/beamline.h>
#include <beamline/beamline_elements.h>


using namespace std;

using FNAL::pcout;
using FNAL::pcerr;

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
  BBLens                * bblensPtr;
  beamline              * bl;
  combinedFunction      * cmbPtr;
  drift                 * driftPtr;
  hkick                 * hkickPtr;
  hmonitor              * hmonitorPtr;
  marker                * markerPtr;
  monitor               * monitorPtr;
  octupole              * octupolePtr;
  quadrupole            * quadrupolePtr;
  rbend                 * rbendPtr;
  sbend                 * sbendPtr;
  sector                * sectorPtr;
  sextupole             * sextupolePtr;
  srot                  * srotPtr;
  thin12pole            * pole12Ptr;
  thin14pole            * pole14Ptr;
  thin16pole            * pole16Ptr;
  thin18pole            * pole18Ptr;
  thinDecapole          * thinDecapolePtr;
  thinLamb              * thinLambPtr;
  thinMultipole         * thinMultipolePtr;
  thinOctupole          * thinOctupolePtr;
  thinQuad              * thinQuadPtr;
  thinSeptum            * thinSeptaPtr;
  thinSextupole         * thinSextupolePtr;
  thinrfcavity          * cavityPtr;
  rfcavity              * cavPtr;
  vkick                 * vkickPtr;
  vmonitor              * vmonitorPtr;
  Pinger                * PingerPtr;
  HPinger               * hPingerPtr;
  VPinger               * vPingerPtr;
  kick                  * kickPtr;
  Slot                  * slot;
  CF_rbend              * cfRbendPtr;
  CF_sbend              * cfSbendPtr;

  const double MIN_ANGLE = 2.0E-9;
  const int SIZE=80;
  char type[SIZE], *name;
  double length, strength, ref_ct, x, y, t;
  std::string previousLine;

  name = new char[SIZE];

  if ( is.eof() )
    return NULL;

  // read in the invariant stuff
  is >> type >> name >> length >> strength >> ref_ct >> x >> y >> t;
  
  // Figure out which type of element we have here
  if ( strcasecmp(type,                 "beamline") == 0 ) {
    bl = new beamline(name);
    element = bl;
  }
  else if ( strcasecmp(type,            "Slot") == 0 ) {
    slot = new Slot(name);
    element = slot;
  }
  else if ( strcasecmp(type,            "beamline_END") == 0 ) {
    element = NULL;
  }
  else if ( strcasecmp(type,            "combinedFunction_END") == 0 ) {
    element = NULL;
  }
  else if ( strcasecmp(type,            "slot_END") == 0 ) {
    element = NULL;
  }
  else if ( strcasecmp(type,            "combinedFunction") == 0 ) {
    cmbPtr = new combinedFunction(name);
    element = cmbPtr;
  }
  else if( strcasecmp(type,             "drift") == 0 ) {
    driftPtr = new drift(name, length);
    element = driftPtr;
  }
  else if( strcasecmp(type,             "hkick") == 0 ) {
    hkickPtr = new hkick(name, length, strength);
    element = hkickPtr;
  }
  else if( strcasecmp(type,             "vkick") == 0 ) {
    vkickPtr = new vkick(name, length, strength);
    element = vkickPtr;
  }
  else if( strcasecmp(type,             "kick") == 0 ) {
    kickPtr = new kick(name, length, strength, strength );
    element = kickPtr;
  }
  else if( strcasecmp(type,             "vpinger") == 0 ) {
    vPingerPtr = new VPinger(name, strength);
    element = vPingerPtr;
  }
  else if( strcasecmp(type,             "hpinger") == 0 ) {
    hPingerPtr = new HPinger(name, strength);
    element = hPingerPtr;
  }
  else if( strcasecmp(type,             "pinger") == 0 ) {
    PingerPtr = new Pinger(name, strength);
    element = PingerPtr;
  }
  else if( strcasecmp(type,             "hmonitor") == 0 ) {
    hmonitorPtr = new hmonitor(name, length);
    element = hmonitorPtr;
  }
  else if( strcasecmp(type,             "marker") == 0 ) {
    markerPtr = new marker(name);
    element = markerPtr;
  }
  else if( strcasecmp(type,             "monitor") == 0 ) {
    monitorPtr = new monitor(name, length);
    element = monitorPtr;
  }
  else if( strcasecmp(type,             "octupole") == 0 ) {
    octupolePtr = new octupole(name, length, strength);
    element = octupolePtr;
  }
  else if( strcasecmp(type,             "quadrupole") == 0 ) {
    quadrupolePtr = new quadrupole(name, length, strength);
    element = quadrupolePtr;
  }
  else if( strcasecmp(type,             "rbend") == 0 ) {
    if( 0 <= strength ) 
    { rbendPtr = new rbend(name, length, strength,  1.0 /* Dummy large angle */ ); }
    else 
    { rbendPtr = new rbend(name, length, strength, -1.0 /* Dummy large angle */ ); }
    element = rbendPtr;
  }
  else if( strcasecmp(type,             "sbend") == 0 ) {
    if( 0 <= strength ) 
    { sbendPtr = new sbend(name, length, strength,   1.0 /* Dummy large angle */ ); }
    else
    { sbendPtr = new sbend(name, length, strength, - 1.0 /* Dummy large angle */ ); }
    element = sbendPtr;
  }
  else if( strcasecmp(type,             "sextupole") == 0 ) {
    sextupolePtr = new sextupole(name, length, strength);
    element = sextupolePtr;
  }
  else if( strcasecmp(type,             "srot") == 0 ) {
    srotPtr = new srot(name, strength);
    element = srotPtr;
  }
  else if( strcasecmp(type,             "thinDecapole") == 0 ) {
    thinDecapolePtr = new thinDecapole(name, strength);
    element = thinDecapolePtr;
  }
  else if( strcasecmp(type,             "thinLamb") == 0 ) {
    thinLambPtr = new thinLamb(name);
    element = thinLambPtr;
  }
  else if( strcasecmp(type,             "thinMultipole") == 0 ) {
    thinMultipolePtr = new thinMultipole(name, strength);
    element = thinMultipolePtr;
  }
  else if( strcasecmp(type,             "thinOctupole") == 0 ) {
    thinOctupolePtr = new thinOctupole(name, strength);
    element = thinOctupolePtr;
  }
  else if( strcasecmp(type,             "thinQuad") == 0 ) {
    thinQuadPtr = new thinQuad(name, strength);
    element = thinQuadPtr;
  }
  else if( strcasecmp(type,             "thinSeptum") == 0 ) {
    thinSeptaPtr = new thinSeptum(name);
    element = thinSeptaPtr;
  }
  else if( strcasecmp(type,             "thinSextupole") == 0 ) {
    thinSextupolePtr = new thinSextupole(name, strength);
    element = thinSextupolePtr;
  }
  else if( strcasecmp(type,             "thinrfcavity") == 0 ) {
    cavityPtr = new thinrfcavity(name,0,(strength*1.0e9),0,0,0);
    element = cavityPtr;
  }
  else if( strcasecmp(type,             "rfcavity") == 0 ) {
    cavPtr = new rfcavity(name,length,0,(strength*1.0e9),0,0,0);
    element = cavPtr;
  }
  else if( strcasecmp(type,             "vmonitor" ) == 0 ) {
    vmonitorPtr = new vmonitor(name, length);
    element = vmonitorPtr;
  }
  else if( strcasecmp(type,             "BBLens") == 0 ) {
    bblensPtr = new BBLens(name, length);
    element = bblensPtr;
  }
  else if( strcasecmp(type,             "sector") == 0 ) {
    sectorPtr = new sector(name, length);
    element = sectorPtr;
  }
  else if( strcasecmp(type,             "thin12pole") == 0 ) {
    pole12Ptr = new thin12pole(name, length);
    element = pole12Ptr;
  }
  else if( strcasecmp(type,             "thin14pole") == 0 ) {
    pole14Ptr = new thin14pole(name, length);
    element = pole14Ptr;
  }
  else if( strcasecmp(type,             "thin16pole") == 0 ) {
    pole16Ptr = new thin16pole(name, length);
    element = pole16Ptr;
  }
  else if( strcasecmp(type,             "thin18pole") == 0 ) {
    pole18Ptr = new thin18pole(name, length);
    element = pole18Ptr;
  }
  else if( strcasecmp(type,             "CF_rbend") == 0 ) {
    if( 0 <= strength ) 
    { cfRbendPtr = new CF_rbend(name, length, strength,  1.0 /* Dummy large angle */ ); }
    else
    { cfRbendPtr = new CF_rbend(name, length, strength, -1.0 /* Dummy large angle */ ); }
    element = cfRbendPtr;
  }
  else if( strcasecmp(type,             "CF_sbend") == 0 ) {
    if( 0 <= strength ) 
    { cfSbendPtr = new CF_sbend(name, length, strength,  1.0 /* Dummy large angle */ ); }
    else
    { cfSbendPtr = new CF_sbend(name, length, strength, -1.0 /* Dummy large angle */ ); }
    element = cfSbendPtr;
  }
  else {
    (*pcerr) << "\n **** WARNING **** read_istream(istream&): Unknown element type \"" << type << "\" "
             << "\n **** WARNING **** Previous line read: " << previousLine
             << "\n **** WARNING **** This program may crash and burn soon."
             << "\n **** WARNING **** At the minimum, a memory leak has been created."
             << endl;
    ostringstream uic;
    uic      << "\n **** WARNING **** read_istream(istream&): Unknown element type \"" << type << "\" "
             << "\n **** WARNING **** Previous line read: " << previousLine
             << "\n **** WARNING **** This program may crash and burn soon."
             << endl;
    return NULL;
  }

  // Save away the current line in case we need to report something 
  // unreadable next time.
  previousLine.clear();
  previousLine = type;
  previousLine += "  ";
  previousLine += name;
  previousLine += "  ";
  { ostringstream uic;
    uic  << length << "  " << strength << "... and so forth.";
    previousLine += uic.str();
  }
  // REMOVE: sprintf(previousLine,"%s  %s  %lf  %lf  %lf  %lf  %lf",
  // REMOVE:         type,name,length,strength,x,y,t);

  // Get the rest of the description if we got a real element
  if ( element ) {
    element->setReferenceTime(ref_ct);
    if( 0 != strcmp( element->Type(), "beamline" ) ) {
      element->readFrom(is);
    }
    else {
      double energy;
      is >> energy;
      static_cast<beamline*>(element)->setEnergy( energy );
      bmlnElmnt *e = NULL;
      do {
        e = read_istream(is);
        if ( e ) {
          static_cast<beamline*>(element)->append(ElmPtr(e)); 
	}
      } while ( e );
    }

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
 

istream& operator>>(istream& is, beamline& bl)
{
  const int SIZE=80;
  char name[SIZE], type[SIZE];
  double length, strength, ref_ct, x, y, t;

  // Read in a beamline
  is >> type >> name >> length >> strength >> ref_ct >> x >> y >> t;
  // Some of these arguments are meaningless for a beamline but must
  // be included because beamline inherits from bmlnElmnt.

  if ( strcasecmp(type, "beamline") != 0 ) {
    (*pcerr) << "\n **** WARNING **** Expecting data file to begin with a \"beamline\" directive"
             << "\n **** WARNING **** First element, " 
             << type << ",  " << name << ", ignored"
             << "\n **** WARNING **** This program may crash and burn soon."
             << endl;
    ostringstream uic;
    uic      << "\n **** WARNING **** Expecting data file to begin with a \"beamline\" directive"
             << "\n **** WARNING **** First element, " 
             << type << ",  " << name << ", ignored"
             << "\n **** WARNING **** This program may crash and burn soon."
             << endl;
    throw(   bmlnElmnt::GenericException( __FILE__, __LINE__
           , "istream& operator>>(istream& is, beamline& bl)"
           , uic.str().c_str() ) );
  }
  else {
    bl.rename(name);
    bl.setReferenceTime(ref_ct);  // Meaningless???  No!
  }
  
  // ??? REMOVE: bl.readFrom(is); // Polymorphically call the right readFrom().
  bmlnElmnt *e = 0;
  is >> bl.nominalEnergy_;
  // Now, continue reading is until we see the end of this beamline
  length = 0;
  do {
    e = read_istream(is);     // (Recursion)
    // read_istream will return NULL when end of file or end of beamline is reached.
    if ( e ) 
      bl.append(ElmPtr(e));
  } while ( e );
  
  if ( x!=0 || y!=0 || t!=0 ) {
    alignmentData align;
    align.xOffset = x;
    align.yOffset = y;
    align.tilt    = t;
    bl.setAlignment(align);
  }
  return is;
}
