/*************************************************************************
**************************************************************************
**************************************************************************
******
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and
******             synchrotrons.
******
******  File:      Slot.cc
******
******             Implementation of class Slot
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
******  Copyright (c) 1998 Universities Research Association, Inc. / Fermilab
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
****** Jun 1998 (?)       michelotti@fnal.gov
****** - original (working) version
****** 
****** Mar 2007           ostiguy@fnal.gov
****** - support for reference counted elements
****** - reduced src file coupling due to visitor interface.
******   visit() takes advantage of (reference) dynamic type.
****** - use std::string for string operations.
******
****** Dec 2007           ostiguy@fnal.gov
****** - new typesafe propagator architecture
****** - eliminated (unused) code for slots with embedded elements
******
****** Apr 2008           michelotti@fnal.gov
****** - added placeholder Slot::setLength method
******
****** May 2008           ostiguy@fnal.gov
****** - setStrength() now dispatched to propagator by base class
******   (no longer virtual)
****** - added explicit implementation for assignment operator
******
****** Jan 2010           michelotti@fnal.gov
****** - modified void Slot::split(...) to forbid splitting "small" Slots,
******   smallness being specified by new public static variable.
****** - splitting previously worked correctly but led to confusion
******   in certain applications (viz. Synergia) which created Mappings
******   created between inserted markers which did not map 0->0.
****** - done in conjunction with introduction of class DriftConverter
******   in directory physics_toolkit.
******
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/GenericException.h>
#include <beamline/marker.h>
#include <beamline/Slot.h>
#include <beamline/SlotPropagators.h>
#include <beamline/beamline.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/ParticleBunch.h>
#include <beamline/Alignment.h>
#include <beamline/BmlVisitor.h>

using namespace std;

using FNAL::pcerr;
using FNAL::pcout;

namespace {

  typedef PhaseSpaceIndexing::index index;

  index const i_x   = Particle::i_x;
  index const i_y   = Particle::i_y;
  index const i_cdt = Particle::i_cdt;
  index const i_npx = Particle::i_npx;
  index const i_npy = Particle::i_npy;
  index const i_ndp = Particle::i_ndp;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Slot::Slot()
 : BmlnElmnt()
{
  propagator_ = PropagatorPtr( new Propagator(*this) );

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Slot::Slot( std::string const& nm )
: BmlnElmnt(nm)
{
  propagator_ = PropagatorPtr( new Propagator(*this) );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Slot::Slot( std::string const& nm, Frame const& y )
  : BmlnElmnt(nm), in_(), out_(y) 
{
  if( !out_.isOrthonormal() )
  {
    throw( GenericException( __FILE__, __LINE__,
           "Slot::Slot( char const * nm, Frame const& y )", 
           "Current implementation requires that frames be orthonormal." ) );
  }

  length_ = out_.getOrigin().Norm();

  propagator_ = PropagatorPtr( new Propagator(*this) );

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Slot::Slot( Slot const& x )
  : BmlnElmnt(x), in_( x.in_ ), out_( x.out_ ) 
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Slot::~Slot()
{
  if( align_) { delete align_; align_ = 0; }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Slot& Slot::operator=( Slot const& other) 
{
   if (this == &other) return *this;  

   BmlnElmnt::operator=(other);
   
   in_  = other.in_;
   out_ = other.out_;

   return *this;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Slot::makeUpstreamHorizontal   ( double const& lng, double const& ang )
{
  length_ = lng;

  in_.reset();
  out_.reset();

  Vector driftOffset(3); 
  driftOffset[2] = lng;
  
  out_.rotate( - ang, out_.getyAxis() );
  out_.translate( driftOffset );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Slot::makeDownstreamHorizontal ( double const& lng, double const& ang )
{
  length_ = lng;
  in_.reset();
  out_.reset();

  Vector driftOffset(3); 
  driftOffset[2] = lng;
  
  out_.translate( driftOffset );
  out_.rotate( - ang, out_.getyAxis() );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Slot::makeUpstreamVertical   ( double const& lng, double const& ang )
{
  length_ = lng;
  in_.reset();
  out_.reset();

  Vector driftOffset(3); 
  driftOffset[2] = lng;
  
  out_.rotate( - ang, out_.getxAxis() );
  out_.translate( driftOffset );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Slot::makeDownstreamVertical ( double const& lng, double const& ang )
{
  length_ = lng;
  in_.reset();
  out_.reset();

  Vector driftOffset(3); 
  driftOffset[2] = lng;
  
  out_.translate( driftOffset );
  out_.rotate( - ang, out_.getxAxis() );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char const*  Slot::Type()  const  
{ 
  return "Slot"; 
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool Slot::isMagnet()  const  
{ 
  return false; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool Slot::isThin()  const  
{ 
  return false; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool Slot::isPassive()  const  
{ 
  return false;  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool Slot::isDriftSpace()  const  
{ 
  return true;  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 int Slot::setInFrame( Frame const& frm )
{

  int ret = 0;
  if ( ret = checkFrame( frm ) ) { in_ = frm; }
  return ret;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int Slot::setOutFrame( Frame const& frm )
{


  int ret = 0;
  if( ret = frm.isOrthonormal() ) { out_ = frm; }   
  else {
    throw( GenericException( __FILE__, __LINE__,
         " int Slot::setOutFrame( const Frame& frm )", 
         "Current implementation requires that frames be orthonormal." ) );
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int Slot::checkFrame( Frame const& f ) const
{
  static const Frame zero;
  static const int y = 1;
  static const int z = 2;
 
  int ret = 0;

  if( !f.isOrthonormal() ) {
    throw( GenericException( __FILE__, __LINE__,
           " int Slot::checkFrame( const Frame& f ) const", 
           "Current implementation requires that frames be orthonormal." ) );
  }

  if( f.getOrigin() != zero.getOrigin() ) {
    throw( GenericException( __FILE__, __LINE__,
           " int Slot::checkFrame( const Frame& f ) const", 
           "Current implementation requires no displacement of origin." ) );
  }

  else if( (  f.getAxis(y) != zero.getAxis(y) ) && 
           (  f.getAxis(z) != zero.getAxis(z) )
         ) {
    throw( GenericException( __FILE__, __LINE__,
           " int Slot::checkFrame( const Frame& f ) const", 
           "Current implementation allows rotation about "
           "y or z axis, but not both." ) );
  }

  return ret;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Slot::OrbitLength( const Particle& x )
{
  static bool firstTime = true;

  if( firstTime ) {
      firstTime = false;
      (*pcerr) << "*** WARNING ***                                 \n"
              "*** WARNING *** Slot::OrbitLength               \n"
              "*** WARNING *** Returning the design length of  \n"
              "*** WARNING *** empty slots rather then the     \n"
              "*** WARNING *** orbit length.                   \n"
              "*** WARNING ***                                 \n"
           << endl;
  }    
  return length_;
}
 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& Slot::writeTo ( ostream& os )
{
#if 0
  // Write out private attributes, which are the "in" and "out" Frame's.
  os << in_ ;
  if ( bml_ != NULL ) {
    // print out the beamline contained in this slot
    os << "slot_BEGIN " << Name() << " 0 0 0 0 0\n";
    os << (*bml_);
    os << "slot_END " << Name() << " 0 0 0 0 0\n";
  } else if ( elm_ != NULL ) {
    os << "slot_BEGIN " << Name() << " 0 0 0 0 0\n";
    os << (*elm_);
    os << "slot_END " << Name() << " 0 0 0 0 0\n";
  } else {
    os << "no_slot_contents " << Name() << " 0 0 0 0 0\n";
  }
  os << out_ ;
#endif
  return os;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

istream& Slot::readFrom( istream& is )
{
  
#if 0
  // ********* BROKEN ********************

  // First, get the "in" frame"
  is >> in_;

  // Second, read in the stuff contained in the Slot.
  char type[30], name[60];
  double Length, Strength, x, y, t;

  // Now read in the BmlnElmnt, if there is one

  is >> type >> name >> Length >> Strength >> x >> y >> t;
  if ( strcasecmp(type, "slot_BEGIN") == 0 ) {
    elm_ = ElmPtr( read_istream(is) );  // Recursively read the BmlnElmnt.

    if ( elm_  && strcasecmp(elm_->Type(), "beamline") == 0 ) 

      bml_ = BmlPtr( (beamline*) elm_.get() );

    // The only element in this Slot is a single BmlnElmnt.  There is a 
    // "slot_END" line to read in.

    is >> type >> name >> Length >> Strength >> x >> y >> t;
    if ( strcasecmp(type, "slot_END") != 0 ) {
      ostringstream uic;
      uic << "Expecting \"slot_END\" but got " << type;
      throw( GenericException( __FILE__, __LINE__,
             "istream& Slot::readFrom( istream& is )", 
             uic.str().c_str() ) );
    }
    // Skip the next line--it is not "slot_BEGIN" (assume it is "slot_empty")
    ;
  }
    
  // Finally, read in the "out" frame information.
  is >> out_;
  length_ = ( out_.getOrigin() - in_.getOrigin()) .Norm();

#endif

  return is;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::pair<ElmPtr,ElmPtr> Slot::split( double const& pct ) const
{
  if( length_ <= Slot::minSplitLength ) {
    ostringstream uic;
    uic  << "\n*** ERROR *** "
         << "\n*** ERROR *** File: " << " " << __FILE__ << ", line " << __LINE__ << ": "
         << "\n*** ERROR *** Attempt to split a Slot with length <= "
         << Slot::minSplitLength << " meter."
         << "\n*** ERROR *** ";
    throw( GenericException( __FILE__, __LINE__, 
           "void Slot::split( double const& pct, ElmPtr& a, ElmPtr& b ) const",
           uic.str().c_str() ) );
  }


  ElmPtr a;
  ElmPtr b;

  if( pct < 0.0 || 1.0 < pct ) {
    (*pcerr) << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** void Slot::Split( double, ElmPtr&, ElmPtr& )"
         << "\n*** WARNING *** Function called with first argument = " 
         << pct <<             ", outside [0,1]."
         << "\n*** WARNING *** Null pointers are being returned." 
         << endl;
    return std::make_pair(a,b);
  }


  if( pct == 0.0 ) {
     a = MarkerPtr( new marker( "Null Slot" ) );
     b =   SlotPtr( new Slot( *this ) );
    return std::make_pair(a,b);
  }
  if( pct == 1.0 ) {
     a = SlotPtr( new Slot( *this ) );
     b = MarkerPtr( new marker( "Null Slot" ) );
    return std::make_pair(a,b); 
  }

  Vector d( out_.getOrigin() - in_.getOrigin() );

  Frame aOutFrame( in_ );
  aOutFrame.setOrigin( in_.getOrigin() + pct*d );
  Frame bOutFrame( out_.relativeTo( aOutFrame ) );

  a = SlotPtr( new Slot( "", aOutFrame ) );
  b = SlotPtr( new Slot( "", bOutFrame ) );

  // Rename

  a->rename( ident_ + string("_1") );
  b->rename( ident_ + string("_2") );

  return std::make_pair(a,b);

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Slot::accept( BmlVisitor& v ) 
{
 v.visit(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Slot::accept( ConstBmlVisitor& v ) const
{

  v.visit(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


