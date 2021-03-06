/*************************************************************************
**************************************************************************
**************************************************************************
******
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and
******             synchrotrons.
******
******  File:      Slot.h
******
******             Description for class Slot
******             - representing a region of free space connecting
******               two completely general faces (Frames).
******             - generalization of "drift," which connects parallel
******               faces whose origins are incident on a normally
******               intersecting line.
******
******  Author:    Leo Michelotti
******             Phone: (630) 840 4956
******             Email: michelotti@fnal.gov
******
******  Copyright (c) 1998 Universities Research Association, Inc.
******                All Rights Reserved
******
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
******
****** ----------------
****** REVISION HISTORY
****** ----------------
****** Jun 1998 (?)        michelotti@fnal.gov
****** - original (working) version
******
****** Mar 2007            ostiguy@fnal.gov
****** - use covariant return types
****** - support for reference counted elements
******
****** Dec 2007            ostiguy@fnal.gov
****** - new typesafe propagators
******
****** Apr 2008            michelotti@fnal.gov
****** - added placeholder Slot::setLength method
******
****** May 2008 ostiguy@fnal
****** - proper, explicit assignment operator
****** - propagator moved (back) to base class
****** - no assumption about internal structure
******
****** Jan 2010            michelotti@fnal.gov
****** - added public static variables  Slot::minSplitLength
******   and  (const) Slot::minSplitLengthDefault  to specify
******   the size of a "small" Slot.
****** - done in conjunction with changes to Slot::Split(...).
******   See Slot.cc for additional information.
******
**************************************************************************
*************************************************************************/

#ifndef SLOT_H
#define SLOT_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/BmlnElmnt.h>
#include <basic_toolkit/Frame.h>

class BmlVisitor;
class ConstBmlVisitor;

class Slot;

typedef boost::shared_ptr<Slot> SlotPtr;
 
class DLLEXPORT Slot : public BmlnElmnt
{

  class Propagator;

 public:

   Slot();
   Slot( std::string const& name);
   Slot( std::string const& name, Frame const& out );
   Slot( Slot const& );

   ~Slot();
 
   Slot* clone() const
     { return new Slot( *this ); }

   Slot& operator=( Slot const& other); 

   void makeUpstreamHorizontal   ( double const& length, double const& angle );
   void makeDownstreamHorizontal ( double const& length, double const& angle );
   void makeUpstreamVertical     ( double const& length, double const& angle );
   void makeDownstreamVertical   ( double const& length, double const& angle );

   int  checkFrame( Frame const& ) const;

   void accept( BmlVisitor& v );
   void accept( ConstBmlVisitor& v ) const;

   std::pair<ElmPtr,ElmPtr> split( double const& pct) const;

   bool         isMagnet()     const;
   bool         isThin()       const;
   bool         isPassive()    const;
   bool         isDriftSpace() const;

   char const*  Type()      const;

   int  setInFrame( Frame const& );
   int setOutFrame( Frame const& );

   Frame const& getInFrame() const
     { return in_; }
   Frame const& getOutFrame() const
     { return out_; }

   double OrbitLength( Particle const& );


 public:
   static double       minSplitLength;
   static double const minSplitLengthDefault;


 private:

   Frame in_;
   Frame out_;

   std::ostream& writeTo ( std::ostream& );
   std::istream& readFrom( std::istream& );
};

#endif // SLOT_H
