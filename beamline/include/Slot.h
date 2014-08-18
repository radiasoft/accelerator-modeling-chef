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
#include <beamline/bmlnElmnt.h>
#include <basic_toolkit/Frame.h>

class BmlVisitor;
class ConstBmlVisitor;

class Slot;

typedef boost::shared_ptr<Slot> SlotPtr;

class DLLEXPORT Slot : public bmlnElmnt
{
 public:
   class Propagator;
   typedef boost::shared_ptr<BasePropagator<Slot> > PropagatorPtr;   

   Slot();
   Slot( char const*  name);
   Slot( char const*  name, Frame const& out );
   Slot( const Slot& );
   ~Slot();
 
   void makeUpstreamHorizontal   ( double const& length, double const& angle );
   void makeDownstreamHorizontal ( double const& length, double const& angle );
   void makeUpstreamVertical     ( double const& length, double const& angle );
   void makeDownstreamVertical   ( double const& length, double const& angle );

   int  checkFrame( const Frame& ) const;

   void accept( BmlVisitor& v );
   void accept( ConstBmlVisitor& v ) const;

   void localPropagate(         Particle& );
   void localPropagate(      JetParticle& );
   void localPropagate(    ParticleBunch& );
   void localPropagate( JetParticleBunch& );
 
   Slot* Clone() const
     { return new Slot( *this ); }

   void Split( double const& s, ElmPtr&, ElmPtr& ) const;

   bool         isMagnet()  const;
   const char*  Type()      const;

   int  setInFrame( Frame const& );
   int setOutFrame( Frame const& );

   void setLength( double const& );

   Frame const& getInFrame() const
     { return in_; }
   Frame const& getOutFrame() const
     { return out_; }

   double OrbitLength( Particle const& ) const;
   double OrbitLength( Particle const& );


 public:
   static double       minSplitLength;
   static double const minSplitLengthDefault;


  void usePropagator( PropagatorPtr& );

 private:
   Frame in_;
   Frame out_;

   PropagatorPtr propagator_;

   std::ostream& writeTo ( std::ostream& );
   std::istream& readFrom( std::istream& );
};

#endif // SLOT_H
