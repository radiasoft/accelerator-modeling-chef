/*************************************************************************
**************************************************************************
**************************************************************************
******
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and
******             synchrotrons.
******
******  File:      CF_sbend.cc
******
******  Copyright (c) Fermi Research Alliance, LLC
******                Universities Research Association, Inc.
******                Fermilab
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms
******  of the License supplied with this software.
******
******  Software and documentation created under
******  U.S. Department of Energy Contracts No. DE-AC02-76CH03000
******  and No. DE-AC02-07CH11359.
******
******  The U.S. Government retains a world-wide non-exclusive,
******  royalty-free license to publish or reproduce documentation
******  and software for U.S. Government purposes. This software
******  is protected under the U.S. and Foreign Copyright Laws.
******
******
******  Authors:   Leo Michelotti         michelotti@fnal.gov
******             Jean-Francois Ostiguy  ostiguy@fnal.gov
******
******
******  ----------------
******  REVISION HISTORY
******  ----------------
******
******  Mar 2007           ostiguy@fnal.gov
******  - support for reference counted elements
******  - reduced src file coupling due to visitor interface. 
******    visit() takes advantage of (reference) dynamic type.
******  - use std::string for string operations. 
******
******  Aug 2007           ostiguy@fnal.gov
******  - composite structure based on regular beamline
******
******  Dec 2007           ostiguy@fnal.gov
******  - new typesafe propagator architecture  
******
******  Apr 2008            michelotti@fnal.gov
******  - modified setStrength method
******  - added placeholder setLength method
******  - modified sbend::Split
******  - added member functions to nullify edge effects
******    : used by modified sbend::Split
******
******  Jan 2014
******  - added method CF_sbend::usePropagator, in conjunction
******    with creation of new class CF_sbend_MADPropagator. Putting
******    this here should be temporary. It logically belongs
******    in class bmlnElmnt.
******
******  Dec 2014           michelotti@fnal.gov
******  - added attribute integrated_strengths_ to capture
******    integrated values of multipole components
******
******  Jan 2015           michelotti@fnal.gov
******  - bug fix: added code to CF_sbend::Split(...) for handling
******    CF_sbends with fewer than two edge elements.
******
**************************************************************************
*************************************************************************/

//---------------------------------------------------------------------------------------------
// length    magnet length (along the fiducial trajectory) [ meter    ]
// fld       field (strength)                              [ tesla    ]
// ang       total bend angle [ radians ]
// us        upstream edge angle [radians]
// ds        downstream edge angle [radians]
// NOTE: the signs of us and ds are as defined according to the MAD convention
//----------------------------------------------------------------------------------------------



#include <iomanip>
#include <cfloat>

#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/GenericException.h>
#include <basic_toolkit/PhysicsConstants.h>
#include <beamline/beamline.h>
#include <beamline/CF_sbend.h>
#include <beamline/CF_sbendPropagators.h>
#include <beamline/quadrupole.h>
#include <beamline/sextupole.h>
#include <beamline/sbend.h>
#include <beamline/octupole.h>
#include <beamline/Particle.h>
#include <beamline/BmlVisitor.h>
#include <beamline/Alignment.h>
#include <beamline/marker.h>
#include <beamline/Edge.h>

using namespace std;

using FNAL::pcout;
using FNAL::pcerr;

#define UPPER_POLE_NUMBER 4   // = 1 + (8/2 - 1)  Octupole is largest multipole
                              // This needs to be redefined if more multipoles
                              // are added in the future.

#define SMALL_NUMBER (50.0*DBL_EPSILON)

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_sbend::CF_sbend()
  : bmlnElmnt( "", 0.0, 0.0),
          angle_(0.0),
    usFaceAngle_(0.0),
    dsFaceAngle_(0.0),
        usAngle_(0.0),
        dsAngle_(0.0),
    integrated_strengths_(UPPER_POLE_NUMBER,0.0)
{
  propagator_ = PropagatorPtr( new Propagator(40) );
  propagator_->setup(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_sbend::CF_sbend( const char*          nm,   // name
                    double const&        lng,  // length     [ meter    ]
                    double const&        fld,  // field      [ tesla    ]
                    double const&        ang )
  : bmlnElmnt( nm, lng, fld ),
        angle_(ang),
  usFaceAngle_(0.0),
  dsFaceAngle_(0.0),
      usAngle_(0.0),
      dsAngle_(0.0),
    integrated_strengths_(UPPER_POLE_NUMBER,0.0)
{
  propagator_ = PropagatorPtr( new Propagator(40) );
  propagator_->setup(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


CF_sbend::CF_sbend( const char*   nm,   // name
                    double const&        lng,  // length     [ meter    ]
                    double const&        fld,  // field      [ tesla    ]
                    double const&        ang,  // bend angle [ radians ]
                    double const&        us,   // upstream edge angle [radians]
                    double const&        ds)   // downstream edge angle [radians]
: bmlnElmnt( nm, lng, fld ),
       angle_(ang),
 usFaceAngle_(us),
 dsFaceAngle_(ds),
     usAngle_(us),
     dsAngle_(-ds),
    integrated_strengths_(UPPER_POLE_NUMBER,0.0)
{
  propagator_ = PropagatorPtr( new Propagator(40) );
  propagator_->setup(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_sbend::CF_sbend( CF_sbend const& x )
  : bmlnElmnt( x ),
        angle_(x.angle_),
  usFaceAngle_(x.usFaceAngle_),
  dsFaceAngle_(x.dsFaceAngle_),
      usAngle_(x.usAngle_),
      dsAngle_(x.dsAngle_),
    integrated_strengths_(x.integrated_strengths_),
   propagator_(PropagatorPtr(x.propagator_->Clone()))
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_sbend::~CF_sbend()
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_sbend::peekAt( double& s, Particle const& prt ) const
{
 (*pcout) << setw(12) << s;
 s += const_cast<CF_sbend*>(this)->OrbitLength( prt );  // Kludge!!
 (*pcout) << setw(12) << s           
                  << " : " 
      << setw(10) << this  
      << setw(15) << ident_       
      << setw(15) << Type()      
      << setw(12) << length_      
      << setw(12) << strength_    
      << setw(12) << getQuadrupole()/length_
      << setw(12) << 2.0*getSextupole()/length_
      << setw(12) << 6.0*getOctupole()/length_
      << setw(12) << shuntCurrent_
      << endl;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_sbend::localPropagate( Particle& p )
{
  (*propagator_)(*this, p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_sbend::localPropagate( JetParticle& p )
{
  (*propagator_)(*this, p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_sbend::localPropagate( ParticleBunch& b )
{
  (*propagator_)(*this, b);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_sbend::localPropagate( JetParticleBunch& b )
{
  (*propagator_)(*this, b);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_sbend::usePropagator( PropagatorPtr& x )
{
  propagator_ = x;
  propagator_->setup( *this );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_sbend::setEntryAngle( Particle const& p )
{
  return setEntryAngle( atan2( p.get_npx(), p.get_npz() ) );
  // i.e. tan(phi) = px/pz, where pz = longitudinal momentum
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_sbend::setExitAngle( Particle const& p )
{
  return setExitAngle( atan2( p.get_npx(), p.get_npz() ) );
  // i.e. tan(phi) = px/pz, where pz = longitudinal momentum
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_sbend::setEntryAngle( double const& phi /* radians */ )
{
  double ret = usAngle_;
  usAngle_   = phi;
  propagator_->setup(*this);
  return ret;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_sbend::setExitAngle( double const& phi /* radians */ )
{
  double ret = dsAngle_;
  dsAngle_   = phi;
  propagator_->setup(*this);
  return ret;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int CF_sbend::setOctupole( double const& arg_x )
{
  int counter = 0;
  for ( beamline::const_iterator it  = bml_->begin();
                                 it != bml_->end(); ++it ) {
    if( ( boost::dynamic_pointer_cast<thinOctupole const>(*it) ) ) ++counter;
  }

  if (counter==0) return 1;
 
  for ( beamline::iterator it  = bml_->begin(); 
                           it != bml_->end(); ++it ) {
    if ( boost::dynamic_pointer_cast<thinOctupole>(*it) ) { 
      (*it)->setStrength( arg_x/counter );
    }
  }

  integrated_strengths_[3] = arg_x;

  return 0;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int CF_sbend::setSextupole( double const& arg_x )
{
  int counter = 0;
  for ( beamline::const_iterator it  = bml_->begin();
                                 it != bml_->end(); ++it ) {
    if( ( boost::dynamic_pointer_cast<thinSextupole const>(*it) ) ) ++counter;
  }

  if (counter==0) return 1;

  for ( beamline::iterator it  = bml_->begin();
                           it != bml_->end(); ++it ) {
   if ( boost::dynamic_pointer_cast<thinSextupole>(*it) )  {
     (*it)->setStrength( arg_x/counter );
   }
  }

  integrated_strengths_[2] = arg_x;

  return 0;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool CF_sbend::hasParallelFaces() const
{
  return (    ( std::abs( 2.0*usFaceAngle_ - angle_ ) < 1.0e-9 )
           && ( std::abs( 2.0*dsFaceAngle_ - angle_ ) < 1.0e-9 ) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


bool CF_sbend::hasStandardFaces() const
{
  return (    (std::abs(usFaceAngle_) < 1.0e-9)
           && (std::abs(dsFaceAngle_) < 1.0e-9) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int CF_sbend::setQuadrupole( double const& arg_x )
{
  int counter = 0;
  for ( beamline::const_iterator it  = bml_->begin();
                                 it != bml_->end(); ++it ) {
    if( boost::dynamic_pointer_cast<thinQuad const>(*it) ) ++counter;
  }

  if (counter==0) return 1;
 
  for ( beamline::iterator it  = bml_->begin(); 
                           it != bml_->end(); ++it ) {
   if( boost::dynamic_pointer_cast<thinQuad>(*it) ) {
    (*it)->setStrength( arg_x/counter );
   }
  }

  integrated_strengths_[1] = arg_x;

  return 0;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int CF_sbend::setDipoleField( double const& arg_x )
{

  for ( beamline::iterator it  = bml_->begin(); 
                           it != bml_->end(); ++it ) {
    if( boost::dynamic_pointer_cast<sbend>(*it) ) { 
     (*it)->setStrength( arg_x );
    }
  }

  strength_ =  arg_x;

  return 0;


}


// REMOVE: //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// REMOVE: //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// REMOVE:
// REMOVE: void CF_sbend::setStrength( double const& s )
// REMOVE: {
// REMOVE:
// REMOVE:   double ratio = (strength_ != 0.0) ? s/strength_ : 0.0;
// REMOVE:
// REMOVE:   CFSbendPtr q;
// REMOVE:
// REMOVE:   for ( beamline::iterator it  = bml_->begin();
// REMOVE:                            it != bml_->end(); ++it ) {
// REMOVE:
// REMOVE:     if ( ratio != 0.0  ) {
// REMOVE:          (*it)->setStrength( ratio * (*it)->Strength() );
// REMOVE:     }
// REMOVE:     else {
// REMOVE:       if ( q = boost::dynamic_pointer_cast<CF_sbend>(*it) ) q->setStrength(s);
// REMOVE:     }
// REMOVE:
// REMOVE:   }
// REMOVE:
// REMOVE:
// REMOVE:   strength_ = s;
// REMOVE:
// REMOVE: }
// REMOVE:
// REMOVE:
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_sbend::setStrength( double const& s )
{
  if( strength_ == 0 ) {
    throw( GenericException( __FILE__, __LINE__,
           "void CF_sbend::setStrength( double const& s )",
           "Cannot set strength of CF_sbend when initial strength is zero."
           "\nCurrent version has no way of accessing attributes of edges." ) );
  }

  double oldStrength = strength_;
  bmlnElmnt::setStrength(s);
  double ratio = strength_ / oldStrength;

  if( bml_)
  {
    for ( beamline::iterator it  = bml_->begin();
                             it != bml_->end(); ++it ) {
      (*it)->setStrength( ratio*((*it)->Strength()) );
      // NOTE: if *it points to a marker -- i.e. if the
      // CF_sbend comes from splitting another CF_sbend, so that
      // one or both edges have been replaced with markers --
      // setting its strength will do no harm.
    }
  }
  else {
    throw( GenericException( __FILE__, __LINE__,
           "void sbend::setStrength( double const& s )",
           "IMPOSSIBLE: Internal beamline not initialized!" ) );
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  CF_sbend::setLength( double const& )
{
  ostringstream methodIdent;
  methodIdent << "void " << Type() << "::setLength( double const& )";

  (*pcerr) <<   "*** ERROR ****: "
              "\n*** ERROR ****: "  << __FILE__ << "," << __LINE__
           << "\n*** ERROR ****: void " << Type() << "::setLength( double const& )"
              "\n*** ERROR ****: Resetting the length of "
           << Type() << " is not allowed in this version."
              "\n*** ERROR ****: "
           << std::endl;

  ostringstream uic;
  uic << "Resetting the length of " << Type() << " is not allowed in this version.";
  throw( GenericException( __FILE__, __LINE__,
           methodIdent.str().c_str(),
           uic.str().c_str() ) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_sbend::nullExitEdge()
{
  if( bml_ ) {
    ElmPtr& endpoint = bml_->lastElement();
    if( typeid(*endpoint) == typeid(marker) ) {
      // Nothing needs to be done.
      // This occurs if the current CF_sbend is a piece
      // resulting from splitting another.
    }
    else if( typeid(*endpoint) == typeid(Edge) ) {
      endpoint = ElmPtr( new marker( "EdgeMarker" ) );
    }
    else {
      ostringstream uic;
      uic  <<   "Internal beamline ends in unrecognized element "
           << endpoint->Type() << " " << endpoint->Name();
      throw( GenericException( __FILE__, __LINE__,
               "void CF_sbend::nullExitEdge()",
               uic.str().c_str() ) );
    }
  }
  else {
    throw GenericException( __FILE__, __LINE__,
      "void CF_sbend::nullExitEdge()",
      "An impossibility: internal beamline is null.");
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_sbend::nullEntryEdge()
{
  if( bml_ ) {
    ElmPtr& startpoint = bml_->firstElement();
    if( typeid(*startpoint) == typeid(marker) ) {
      // Nothing needs to be done.
      // This occurs if the current CF_sbend is a piece
      // resulting from splitting another.
    }
    else if( typeid(*startpoint) == typeid(Edge) ) {
      startpoint = ElmPtr( new marker( "EdgeMarker" ) );
    }
    else {
      ostringstream uic;
      uic  <<   "Internal beamline ends in unrecognized element "
           << startpoint->Type() << " " << startpoint->Name();
      throw( GenericException( __FILE__, __LINE__,
               "void CF_sbend::nullExitEdge()",
               uic.str().c_str() ) );
    }
  }
  else {
    throw GenericException( __FILE__, __LINE__,
      "void CF_sbend::nullEntryEdge()",
      "An impossibility: internal beamline is null.");
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_sbend::getOctupole() const
{
  // Returns the **integrated** octupole

  double strength = 0.0;

  if( bml_ ) {
    for ( beamline::const_iterator it  = bml_->begin();
                                   it != bml_->end(); ++it ) {
      if( boost::dynamic_pointer_cast<thinOctupole const>(*it) )  {
        strength += (*it)->Strength();
      }
    }

    double test_value = ( 0. == strength) ? std::abs( integrated_strengths_[3] ) : std::abs( 1.0 - ( integrated_strengths_[3] / strength ) ) ;

    if( SMALL_NUMBER < test_value ) {
      ostringstream uic;
      uic << "Discrepancy in octupole strengths: " 
          << strength << " != " << integrated_strengths_[3];
      throw( GenericException( __FILE__, __LINE__,
             "double CF_sbend::getOctupole() const",
             uic.str().c_str() ) );
    }
  }
  else {
    if( 0. != integrated_strengths_[3] ) {    // This should be exact.
      ostringstream uic;
      uic << "No internal beamline and integrated_strengths_[3] = "
          << integrated_strengths_[3]
          << "!= 0";
      throw( GenericException( __FILE__, __LINE__,
             "double CF_sbend::getOctupole() const",
             uic.str().c_str() ) );
    }
  }

  return strength;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_sbend::getSextupole() const
{
  // Returns the **integrated** sextupole

  double strength = 0.0;

  if( bml_ ) {
    for ( beamline::const_iterator it = bml_->begin();
                                   it != bml_->end(); ++it ) {
      if( boost::dynamic_pointer_cast<thinSextupole const>(*it) ) {
        strength += (*it)->Strength();
      }
    }

    double test_value = ( 0. == strength) ? std::abs( integrated_strengths_[2] ) : std::abs( 1.0 - ( integrated_strengths_[2] / strength ) ) ;

    if( SMALL_NUMBER < test_value ) {
      ostringstream uic;
      uic << "Discrepancy in sextupole strengths: " 
          << strength << " != " << integrated_strengths_[2];
      throw( GenericException( __FILE__, __LINE__,
             "double CF_sbend::getSextupole() const",
             uic.str().c_str() ) );
    }
  }
  else {
    if( 0. != integrated_strengths_[2] ) {    // This should be exact.
      ostringstream uic;
      uic << "No internal beamline and integrated_strengths_[2] = "
          << integrated_strengths_[2]
          << "!= 0";
      throw( GenericException( __FILE__, __LINE__,
             "double CF_sbend::getSextupole() const",
             uic.str().c_str() ) );
    }
  }

  return strength;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_sbend::getQuadrupole() const
{
  // Returns the **integrated** quadrupole

  double strength = 0.0;

  if( bml_ ) {
    for ( beamline::const_iterator it  = bml_->begin();
                                   it != bml_->end(); ++it ) {
      if( boost::dynamic_pointer_cast<thinQuad const>(*it) ) {
         strength += (*it)->Strength();
       }
    }

    double test_value = ( 0. == strength) ? std::abs( integrated_strengths_[1] ) : std::abs( 1.0 - ( integrated_strengths_[1] / strength ) ) ;

    if( SMALL_NUMBER < test_value ) {
      ostringstream uic;
      uic << "Discrepancy in quadrupole strengths: " 
          << strength << " != " << integrated_strengths_[1];
      throw( GenericException( __FILE__, __LINE__,
             "double CF_sbend::getQuadrupole() const",
             uic.str().c_str() ) );
    }
  }
  else {
    if( 0. != integrated_strengths_[1] ) {    // This should be exact.
      ostringstream uic;
      uic << "No internal beamline and integrated_strengths_[1] = "
          << integrated_strengths_[1]
          << "!= 0";
      throw( GenericException( __FILE__, __LINE__,
             "double CF_sbend::getQuadrupole() const",
             uic.str().c_str() ) );
    }
  }

  return strength;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const& CF_sbend::getDipoleField() const
{
  return strength_;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_sbend::Split( double const& pc, ElmPtr& a, ElmPtr& b ) const
{
  // Preliminary tests ...
  // -----------------------------
  if( ( pc <= 0.0 ) || ( pc >= 1.0 ) ) {
    ostringstream uic;
    uic << "Requested percentage = " << pc << "; should be in [0,1].";
    throw( GenericException( __FILE__, __LINE__,
           "void CF_sbend::Split( double const& pc, bmlnElmnt** a, bmlnElmnt** b )",
           uic.str().c_str() ) );
  }

  alignmentData ald( Alignment() );
  if( 0. != ald.xOffset || 0. != ald.yOffset ) {
    if( !hasParallelFaces() ) {
      ostringstream uic;
      uic  <<   "Not allowed to displace an CF_sbend with non-parallel faces"
              "\nwith an Alignment struct.  That rolls are allowed in such"
              "\ncases is only a matter of courtesy. This is NOT encouraged!";
      throw( GenericException( __FILE__, __LINE__,
             "void CF_sbend::Split( double const& pc, ElmPtr& a, ElmPtr& b ) const",
             uic.str().c_str() ) );
    }
    if( 1.0e-10 < std::abs(pc - 0.5 ) ) {
      ostringstream uic;
      uic  <<   "Not allowed to split an CF_sbend displaced"
              "\nwith an Alignment struct other than in its middle."
              "\nThat rolls are allowed in such cases is only a matter"
              "\nof courtesy. This is NOT encouraged!";
      throw( GenericException( __FILE__, __LINE__,
             "void CF_sbend::Split( double const& pc, ElmPtr& a, ElmPtr& b ) const",
             uic.str().c_str() ) );
    }
  }

#ifdef ENABLE_PEDANTIC_WARNINGS
  static bool firstTime = true;
  if( firstTime ) {
    firstTime = false;
    (*pcerr) << "\n"
            "\n*** WARNING ***"
            "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** void sbend::Split( double pc, bmlnElmnt** a, bmlnElmnt** b )"
            "\n*** WARNING *** The new, split elements must be commissioned with"
            "\n*** WARNING *** RefRegVisitor before being used."
            "\n*** WARNING *** "
         << endl;
  }
#endif // ENABLE_PEDANTIC_WARNINGS

  // We assume "strength_" means field, not field*length_.
  // "length_," "strength_," and "angle_" are private data members.
  // -----------------------------

  bool hasEntryEdge = ( typeid(*(bml_->firstElement())) == typeid(Edge) );
  bool hasExitEdge  = ( typeid(*(bml_->lastElement()))  == typeid(Edge) );

  CF_sbend* p_a = 0;
  CF_sbend* p_b = 0;

  a = CFSbendPtr( p_a = new CF_sbend(   ""
                                      , pc*length_
                                      , strength_
                                      , pc*angle_
                                      , usFaceAngle_
                                      , 0.0            ));
  p_a->setEntryAngle( getEntryAngle() );

  if( !hasEntryEdge ) { p_a->nullEntryEdge(); }
  p_a->nullExitEdge();

  b = CFSbendPtr( p_b = new CF_sbend(   ""
                                      , (1.0 - pc)*length_
                                      , strength_
                                      , (1.0 - pc)*angle_
                                      , 0.0
                                      , dsFaceAngle_       ));
  p_b->setExitAngle( getExitAngle() );

  p_b->nullEntryEdge();
  if( !hasExitEdge  ) { p_b->nullExitEdge();  }

  // Assign pole strengths
  // Note: pole strengths scale with length.
  // -----------------------------
  double poleStrength = getQuadrupole();
  p_a->setQuadrupole( pc*poleStrength );
  p_b->setQuadrupole( (1.0 - pc)*poleStrength );

  poleStrength        = getSextupole();
  p_a->setSextupole( pc*poleStrength );
  p_b->setSextupole( (1.0 - pc)*poleStrength );

  poleStrength        = getOctupole();
  p_a->setOctupole( pc*poleStrength );
  p_b->setOctupole( (1.0 - pc)*poleStrength );


  // Set the alignment struct
  // : this is a STOPGAP MEASURE!!!
  // -----------------------------
  a->setAlignment( ald );
  b->setAlignment( ald );

  // Rename
  // -----------------------------
  a->rename( ident_ + string("_1") );
  b->rename( ident_ + string("_2"));
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& CF_sbend::writeTo( ostream& os )
{
  os << OSTREAM_DOUBLE_PREC << angle_       << " ";
  os << OSTREAM_DOUBLE_PREC << usFaceAngle_ << " "
     << OSTREAM_DOUBLE_PREC << dsFaceAngle_ << endl;
  os << OSTREAM_DOUBLE_PREC << usAngle_     << " "
     << OSTREAM_DOUBLE_PREC << dsAngle_     << endl;
  os << OSTREAM_DOUBLE_PREC << getQuadrupole() << " ";
  os << OSTREAM_DOUBLE_PREC << getSextupole()  << " ";
  os << OSTREAM_DOUBLE_PREC << getOctupole()   << " ";
  os << "\n";
  return os;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

istream& CF_sbend::readFrom( istream& is )
{
  double quadStrength = 0.0;  // Assignment mitigates
  double sextStrength = 0.0;  // obnoxious warning message
  double octStrength  = 0.0;  // from compiler.


  is >> angle_
     >> usFaceAngle_
     >> dsFaceAngle_
     >> usAngle_
     >> dsAngle_
     >> quadStrength
     >> sextStrength
     >> octStrength;

  // Rebuild basic element ...

  // Set multipoles
  setQuadrupole( quadStrength );
   setSextupole( sextStrength );
    setOctupole( octStrength );

  return is;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* CF_sbend::Type() const
{
  return "CF_sbend";
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool CF_sbend::isMagnet() const
{
  return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_sbend::OrbitLength( Particle const& x ) const
{
  return length_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_sbend::OrbitLength( Particle const& x )
{
    return length_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_sbend::accept( BmlVisitor& v )
{
  v.visit(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void CF_sbend::accept( ConstBmlVisitor& v ) const
{
  v.visit(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const& CF_sbend::getEntryAngle()   const
{
  return usAngle_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const&  CF_sbend::getExitAngle()    const
{
  return dsAngle_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void CF_sbend::setNumberOfKicks( const int n )
{
  // This piece of code must mimic the body of the CF_sbend constructor.
  propagator_ = PropagatorPtr( new Propagator(n) );
  propagator_->setup(*this);
}



///|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
///|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


int CF_sbend::numberOfKicks() const
{
  CF_sbend::Propagator const* the_pointer = dynamic_cast<CF_sbend::Propagator*>(propagator_.get());
  return the_pointer->numberOfKicks();
}


///|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
///|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
