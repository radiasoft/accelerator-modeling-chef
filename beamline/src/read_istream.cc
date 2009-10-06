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
******
****** NOTE: THIS CODE IS OLD AND BADLY BROKEN. 
******       PERSISTENCE SHOULD BE RE-IMPLEMNTED PROPERLY
******
**************************************************************************
*************************************************************************/


/******  read_istream: global functions and class methods for doing   ****
******     read/write of ".bml" files via stream operators.           ****
******                                                                ****/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iomanip>
#include <basic_toolkit/GenericException.h>
#include <beamline/beamline.h>
#include <beamline/beamline_elements.h>


using namespace std;

using FNAL::pcout;
using FNAL::pcerr;

BmlnElmnt* read_istream(istream& is)
{
  // This is, in essence, the global "factory" function needed for operator<<().
  //
  // Read in a BmlnElmnt.  Recurses when a beamline is found (withing
  // beamline::readFrom(). Returns the bmnlElmnt which has been read
  // in.  Note that knowledge of ALL possible types of BmlnElmnt's is
  // necessary, as is shown in the long string of "if/else if/else
  // if/..." below.

  // This will be the return value at the end

  BmlnElmnt* element = 0;

  std::string name;
  std::string type;

  double const MIN_ANGLE = 2.0E-9;
  int    const SIZE      = 80;

  double length, strength, ref_ct, x, y, t;
  std::string previousLine;

  if ( is.eof() )
    return NULL;

  // read in the invariant stuff
  is >> type >> name >> length >> strength >> ref_ct >> x >> y >> t;
  
  // Figure out which type of element we have here
  if ( type == "beamline" ) { 
    element = new beamline(name);
  }
  else if ( type == "Slot") {
    element = new Slot(name);
  }
  else if ( type == "beamline_END") {
    element = 0;
  }
  else if ( type == "combinedFunction_END" ) {
    element = 0;
  }
  else if ( type == "slot_END" ) {
    element = 0;
  }
  else if ( type == "combinedFunction") {
    element = new combinedFunction(name);
  }
  else if(  type == "Drift" ) {
    element = new Drift(name, length);
   }
  else if(type == "hkick")  {
    element = new hkick(name, length, strength);
  }
  else if (type ==             "vkick")  {
    element = new vkick(name, length, strength);
  }
  else if( type ==             "kick")  {
    element = new kick(name, length, strength, strength );
  }
  else if( type ==             "vpinger")  {
    element = new VPinger(name, strength);
  }
  else if( type ==             "hpinger")  {
    element = new HPinger(name, strength);
  }
  else if( type ==             "pinger")  {
   element = new Pinger(name, strength);
  }
  else if( type ==             "HMonitor")  {
    element = new HMonitor(name, length);
  }
  else if(type ==             "marker")  {
    element = new marker(name);
  }
  else if(type ==             "Monitor")  {
    element = new Monitor(name, length);
  }
  else if(type ==             "octupole")  {
    element = new octupole(name, length, strength);
  }
  else if(type ==             "quadrupole")  {
    element = new quadrupole(name, length, strength);
  }
  else if(type ==             "rbend")  {
    if( 0 <= strength ) 
    { element = new rbend(name, length, strength,  1.0 /* Dummy large angle */ ); }
    else 
    { element = new rbend(name, length, strength, -1.0 /* Dummy large angle */ ); }
  }
  else if(type ==             "sbend")  {
    if( 0 <= strength ) 
    { element = new sbend(name, length, strength,   1.0 /* Dummy large angle */ ); }
    else
    { element = new sbend(name, length, strength, - 1.0 /* Dummy large angle */ ); }
  }
  else if(type ==             "sextupole")  {
    element = new sextupole(name, length, strength);
  }
  else if(type ==             "srot")  {
    element = new srot(name, strength);
  }
  else if(type ==             "thinDecapole")  {
    element = new thinDecapole(name, strength);
  }
  else if(type ==             "thinLamb")  {
    element = new thinLamb(name);
  }
  else if(type ==             "thinMultipole")  {
    element = new thinMultipole(name);
  }
  else if(type ==             "thinOctupole")  {
    element = new thinOctupole(name, strength);
  }
  else if(type ==             "thinQuad")  {
    element = new thinQuad(name, strength);
  }
  else if(type ==             "thinSeptum")  {
   element = new thinSeptum(name);
  }
  else if(type ==             "thinSextupole")  {
    element = new thinSextupole(name, strength);
  }
  else if(type ==             "thinrfcavity")  {
    element = new thinrfcavity(name,0,(strength*1.0e9),0,0,0);
  }
  else if(type ==             "rfcavity")  {
    element = new rfcavity(name,length,0,(strength*1.0e9),0,0,0);
  }
  else if(type ==             "VMonitor" )  {
    element = new VMonitor(name, length);
  }
  else if(type ==             "BBLens")  {
    element = new BBLens(name, length);
  }
  else if(type ==             "sector")  {
    element = new sector(name, length);
  }
  else if(type ==             "thin12pole")  {
    element = new thin12pole(name, length);
  }
  else if(type ==             "thin14pole")  {
    element = new thin14pole(name, length);
  }
  else if(type ==             "thin16pole")  {
    element = new thin16pole(name, length);
  }
  else if(type ==             "thin18pole")  {
    element = new thin18pole(name, length);
  }
  else if(type ==             "CF_rbend")  {
    if( 0 <= strength ) 
    { element = new CF_rbend(name, length, strength,  1.0 /* Dummy large angle */ ); }
    else
    { element = new CF_rbend(name, length, strength, -1.0 /* Dummy large angle */ ); }
  }
  else if(type ==             "CF_sbend")  {
    if( 0 <= strength ) 
    { element = new CF_sbend(name, length, strength,  1.0 /* Dummy large angle */ ); }
    else
    { element = new CF_sbend(name, length, strength, -1.0 /* Dummy large angle */ ); }
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

  // Get the rest of the description if we got a real element

  BmlnElmnt* e = 0;

  if ( element ) {
    element->setReferenceTime(ref_ct);
    if( std::string(element->Type()) == "beamline" )  {
      element->readFrom(is);
    }
    else {
      double momentum;
      is >> momentum;
      static_cast<beamline*>(element)->setMomentum( momentum );

      do {
        e = read_istream(is);
        if ( e ) {
          static_cast<beamline*>(element)->append(ElmPtr(e)); 
	}
      } while ( e );
    }

    // Check if this element is misaligned

    if ( !(e->alignment().isNull()) ) {
      element->setAlignment(e->alignment());
    }
  }

  return element;
}
 

istream& operator>>(istream& is, beamline& bl)
{
#if 0 
  const int SIZE=80;
  char name[SIZE], type[SIZE];
  double length, strength, ref_ct, x, y, t;

  // Read in a beamline
  is >> type >> name >> length >> strength >> ref_ct >> x >> y >> t;
  // Some of these arguments are meaningless for a beamline but must
  // be included because beamline inherits from BmlnElmnt.

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
    throw(   GenericException( __FILE__, __LINE__
           , "istream& operator>>(istream& is, beamline& bl)"
           , uic.str().c_str() ) );
  }
  else {
    bl.rename(name);
    bl.setReferenceTime(ref_ct);  // Meaningless???  No!
  }
  
  // ??? REMOVE: bl.readFrom(is); // Polymorphically call the right readFrom().
  BmlnElmnt *e = 0;
  is >> bl.nominalMomentum_;
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
    align.roll    = t;
    bl.setAlignment(align);
  }
#endif
  return is;
}
