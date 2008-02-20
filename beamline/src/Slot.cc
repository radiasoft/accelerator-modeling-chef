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
******                                                                
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******
****** REVISION HISTORY
******
****** Mar 2007           ostiguy@fnal.gov
****** - support for reference counted elements
****** - reduced src file coupling due to visitor interface. 
******   visit() takes advantage of (reference) dynamic type.
****** - use std::string for string operations.
****** Dec 2007           ostiguy@fnal.gov
****** - new typesafe propagator architecture
****** - eliminated (unused) code for slots with embedded elements
******                                                                
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <basic_toolkit/iosetup.h>
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

  Particle::PhaseSpaceIndex const& i_x   = Particle::xIndex;
  Particle::PhaseSpaceIndex const& i_y   = Particle::yIndex;
  Particle::PhaseSpaceIndex const& i_cdt = Particle::cdtIndex;
  Particle::PhaseSpaceIndex const& i_npx = Particle::npxIndex;
  Particle::PhaseSpaceIndex const& i_npy = Particle::npyIndex;
  Particle::PhaseSpaceIndex const& i_ndp = Particle::ndpIndex;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Slot::Slot()
 : bmlnElmnt()
{
  align_ = new alignment;  // ??? why???

  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Slot::Slot( char const* nm )
: bmlnElmnt(nm)
{
  align_ = new alignment;

  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Slot::Slot( char const* nm, Frame const& y )
  : bmlnElmnt(nm), in_(), out_(y) 
{
  if( !out_.isOrthonormal() )
  {
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__,
           "Slot::Slot( char const * nm, Frame const& y )", 
           "Current implementation requires that frames be orthonormal." ) );
  }

  align_  = 0;
  length_ = out_.getOrigin().Norm();

  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Slot::Slot( Slot const& x )
  : bmlnElmnt(x), in_( x.in_ ), out_( x.out_ ), 
    propagator_(x.propagator_->Clone() )
{
  align_ =  x.align_ ? new alignment(*x.align_) : 0; 
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Slot::~Slot()
{
  if( align_) { delete align_; align_ = 0; }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Slot::makeUpstreamHorizontal   ( double const& lng, double const& ang )
{
  length_ = lng;

  in_.reset();
  out_.reset();

  Vector driftOffset(3); 
  driftOffset(2) = lng;
  
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
  driftOffset(2) = lng;
  
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
  driftOffset(2) = lng;
  
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
  driftOffset(2) = lng;
  
  out_.translate( driftOffset );
  out_.rotate( - ang, out_.getxAxis() );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char*  Slot::Type()  const  
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
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__,
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
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__,
           " int Slot::checkFrame( const Frame& f ) const", 
           "Current implementation requires that frames be orthonormal." ) );
  }

  if( f.getOrigin() != zero.getOrigin() ) {
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__,
           " int Slot::checkFrame( const Frame& f ) const", 
           "Current implementation requires no displacement of origin." ) );
  }

  else if( (  f.getAxis(y) != zero.getAxis(y) ) && 
           (  f.getAxis(z) != zero.getAxis(z) )
         ) {
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__,
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

void  Slot::localPropagate( Particle& p)
{
  (*propagator_)(*this, p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  Slot::localPropagate( JetParticle& p)
{
  (*propagator_)(*this, p);
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  Slot::localPropagate( ParticleBunch& b)
{
  (*propagator_)(*this, b);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  Slot::localPropagate( JetParticleBunch& b)
{
  (*propagator_)(*this, b);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& Slot::writeTo ( ostream& os )
{
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
  return os;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

istream& Slot::readFrom( istream& is )
{
  // Read in my private attributes

  // First, get the "in" frame"
  is >> in_;

  // Second, read in the stuff contained in the Slot.
  char type[30], name[60];
  double Length, Strength, x, y, t;

  // Now read in the bmlnElmnt, if there is one

  is >> type >> name >> Length >> Strength >> x >> y >> t;
  if ( strcasecmp(type, "slot_BEGIN") == 0 ) {
    elm_ = ElmPtr( read_istream(is) );  // Recursively read the bmlnElmnt.

    if ( elm_  && strcasecmp(elm_->Type(), "beamline") == 0 ) 

      bml_ = BmlPtr( (beamline*) elm_.get() );

    // The only element in this Slot is a single bmlnElmnt.  There is a 
    // "slot_END" line to read in.

    is >> type >> name >> Length >> Strength >> x >> y >> t;
    if ( strcasecmp(type, "slot_END") != 0 ) {
      ostringstream uic;
      uic << "Expecting \"slot_END\" but got " << type;
      throw( bmlnElmnt::GenericException( __FILE__, __LINE__,
             "istream& Slot::readFrom( istream& is )", 
             uic.str().c_str() ) );
    }
  } else {
    if( align_ != 0 ) {
      delete align_;
      align_ = 0;
    }
    // Skip the next line--it is not "slot_BEGIN" (assume it is "slot_empty")
    ;
  }
    
  // Finally, read in the "out" frame information.
  is >> out_;
  length_ = ( out_.getOrigin() - in_.getOrigin()) .Norm();

  return is;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Slot::Split( double const& pct, ElmPtr& a, ElmPtr& b ) const
{
  if( pct < 0.0 || 1.0 < pct ) {
    (*pcerr) << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** void Slot::Split( double, ElmPtr&, ElmPtr& )"
         << "\n*** WARNING *** Function called with first argument = " 
         << pct <<             ", outside [0,1]."
         << "\n*** WARNING *** Null pointers are being returned." 
         << endl;
    a = ElmPtr();
    b = ElmPtr();
    return;
  }

  
  if( pct == 0.0 ) {
     a = MarkerPtr( new marker( "Null Slot" ) );
     b =   SlotPtr( new Slot( *this ) );
    return;
  }
  if( pct == 1.0 ) {
     a = SlotPtr( new Slot( *this ) );
     b = MarkerPtr( new marker( "Null Slot" ) );
    return;
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


