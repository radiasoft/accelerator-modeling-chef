/*************************************************************************
**************************************************************************
**************************************************************************
******
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and
******             synchrotrons.
******
******  File:      CF_rbend.cc
******
******  Copyright Universities Research Association, Inc./ Fermilab
******            All Rights Reserved
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
******
****** Aug 2007           ostiguy@fnal.gov
****** - composite structure based on regular beamline
******
****** Dec                ostiguy@fnal.gov
****** - new typesafe propagator architecture
******
****** Apr 2008           michelotti@fnal.gov
****** - modified setStrength method
****** - added placeholder setLength method
****** - changed interpretation of "ang" argument to
******   two constructors from "entry angle" to "bend angle,"
******   in order to conform with usage in other bend constructors.
****** - corrected rbend::Split
******   : including adding methods to nullify edge effects
******
*************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iomanip>

#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/GenericException.h>
#include <basic_toolkit/MathConstants.h>
#include <basic_toolkit/PhysicsConstants.h>
#include <beamline/beamline.h>
#include <beamline/CF_rbend.h>
#include <beamline/CF_rbendPropagators.h>
#include <beamline/quadrupole.h>
#include <beamline/sextupole.h>
#include <beamline/rbend.h>
#include <beamline/octupole.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/Alignment.h>
#include <beamline/marker.h>
#include <beamline/Edge.h>


using namespace std;

using FNAL::pcout;
using FNAL::pcerr;


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_rbend::CF_rbend()
  : bmlnElmnt( "", 0.0, 0.0 ),
    usFaceAngle_(0.0),
    dsFaceAngle_(0.0),
    usAngle_(M_TWOPI),
    dsAngle_(-M_TWOPI),
    multipoles_()
{
  propagator_ = PropagatorPtr( new Propagator(40) );
  propagator_->setup(*this);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_rbend::CF_rbend( const char*   nm,   // name
                    double const& lng,  // length      [ meter    ]
                    double const& fld   // field       [ tesla    ]
                     )
  : bmlnElmnt( nm, lng, fld ),
    usFaceAngle_(0.0),
    dsFaceAngle_(0.0),
    usAngle_(M_TWOPI),
    dsAngle_(-M_TWOPI),
    multipoles_()
{
  propagator_ = PropagatorPtr( new Propagator(40) ); // number of blocks: 4n+1 bends + 2(4n) multipoles
  propagator_->setup(*this);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_rbend::CF_rbend( char const*   nm,   // name
                    double const& lng,  // length      [ meter    ]
                    double const& fld,  // field       [ tesla    ]
                    double const& ang   // bend angle  [ radians  ]
                    )
  : bmlnElmnt( nm, lng, fld ),
   usFaceAngle_(0.0),
   dsFaceAngle_(0.0),
       usAngle_(ang/2.0),
       dsAngle_(-ang/2.0),
    multipoles_()
{
  propagator_ = PropagatorPtr( new Propagator(40) ); // number of blocks: 4n+1 bends + 2(4n) multipoles
  propagator_->setup(*this);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


CF_rbend::CF_rbend( const char*   nm,   // name
                    double const& lng,  // length     [ meter    ]
                    double const& fld,  // field      [ tesla    ]
                    double const& us,   // upstream face angle [radians]
                    double const& ds )
  : bmlnElmnt( nm, lng, fld ),
   usFaceAngle_(us),
   dsFaceAngle_(ds),
       usAngle_(M_TWOPI),
       dsAngle_(-M_TWOPI),
    multipoles_()
{
  propagator_ = PropagatorPtr( new Propagator(40) ); // number of blocks: 4n+1 bends + 2(4n) multipoles
  propagator_->setup(*this);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_rbend::CF_rbend( const  char*   nm,   // name
                    double const&  lng,  // length      [meter]
                    double const&  fld,  // field       [tesla]
                    double const&  ang,  // bend angle  [radians]
                    double const&  us,   // upstream edge angle [radians]
                    double const&  ds )  // downstream edge angle [radians] )
  : bmlnElmnt( nm, lng, fld ),
    angle_(ang),
    usFaceAngle_(us),
    dsFaceAngle_(ds),
    usAngle_((ang/2.0) + us),
    dsAngle_(-((ang/2.0) + ds)),
    multipoles_()
{
  propagator_ = PropagatorPtr( new Propagator(1) ); // number of blocks: 4n+1 bends + 2(4n) multipoles
  propagator_->setup(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


CF_rbend::CF_rbend( CF_rbend const& x )
  : bmlnElmnt( x ),
     usFaceAngle_(x.usFaceAngle_),
     dsFaceAngle_(x.dsFaceAngle_),
         usAngle_(x.usAngle_),
         dsAngle_(x.dsAngle_),
      multipoles_(x.multipoles_),
      propagator_(PropagatorPtr(x.propagator_->Clone()))
{ }

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_rbend::~CF_rbend()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_rbend& CF_rbend::operator=( CF_rbend const& rhs)
{
  if ( &rhs == this ) return *this;

  bmlnElmnt::operator=( rhs );

  usFaceAngle_ =  rhs.usFaceAngle_;
  dsFaceAngle_ =  rhs.dsFaceAngle_;

  usAngle_     =  rhs.usAngle_;
  dsAngle_     =  rhs.dsAngle_;

  propagator_  =  rhs.propagator_;

  bml_       =  BmlPtr( rhs.bml_->Clone() );

  return *this;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_rbend::localPropagate( Particle& p )
{
  (*propagator_)(*this, p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_rbend::localPropagate( JetParticle& p )
{
  (*propagator_)(*this, p);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_rbend::localPropagate( ParticleBunch& b )
{
  (*propagator_)(*this, b);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_rbend::localPropagate( JetParticleBunch& b )
{
  (*propagator_)(*this, b);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_rbend::setEntryAngle( Particle const& p )
{
  return this->setEntryAngle( atan2( p.get_npx(), p.get_npz() ) );
  // i.e. tan(phi) = px/pz, where pz = longitudinal momentum
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_rbend::setExitAngle( Particle const& p )
{
  return this->setExitAngle( atan2( p.get_npx(), p.get_npz() ) );
  // i.e. tan(phi) = px/pz, where pz = longitudinal momentum
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_rbend::setEntryAngle( double const& phi )
{
  double ret = usAngle_;
  usAngle_ = phi;
  propagator_->setup(*this);
  return ret;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_rbend::setExitAngle( double const& phi /* radians */ )
{
  double ret = dsAngle_;
  dsAngle_ = phi;
  propagator_->setup(*this);
  return ret;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int CF_rbend::setOctupole( double const& arg_x )
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
  return 0;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int CF_rbend::setSextupole( double const& arg_x )
{
  int counter = 0;
  for ( beamline::const_iterator it  = bml_->begin();
                                 it != bml_->end(); ++it ) {
    if( ( boost::dynamic_pointer_cast<thinSextupole const>(*it) ) ) ++counter;
  }

  if (counter==0) return 1;

  for ( beamline::iterator it  = bml_->begin();
	                   it != bml_->end(); ++it ) {
    if ( boost::dynamic_pointer_cast<thinSextupole>(*it) ) {
      (*it)->setStrength( arg_x/counter );
    }
  }
  return 0;

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool CF_rbend::hasParallelFaces() const
{
  return (    (std::abs(usFaceAngle_) < 1.0e-9)
           && (std::abs(dsFaceAngle_) < 1.0e-9) );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool CF_rbend::hasStandardFaces() const
{
  return (    (std::abs(usFaceAngle_) < 1.0e-9)
           && (std::abs(dsFaceAngle_) < 1.0e-9) );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int CF_rbend::setQuadrupole( double const& arg_x )
{

  int counter = 0;
  for ( beamline::const_iterator it  = bml_->begin();
                                 it != bml_->end(); ++it ) {
    if( ( boost::dynamic_pointer_cast<thinQuad const>(*it) ) ) ++counter;
  }

  if (counter==0) return 1;

  for ( beamline::iterator it  = bml_->begin();
	                   it != bml_->end(); ++it ) {
    if ( boost::dynamic_pointer_cast<thinQuad>(*it) ) {
      (*it)->setStrength( arg_x/counter );
    }
  }
  return 0;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int CF_rbend::setDipoleField( double const& arg_x )
{

  strength_ = arg_x;

  for ( beamline::iterator it  = bml_->begin();
	                   it != bml_->end(); ++it ) {
    if( boost::dynamic_pointer_cast<rbend>(*it) ) {
     (*it)->setStrength( arg_x );
   }
  }

  return 0;
}


// REMOVE: //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// REMOVE: //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// REMOVE:
// REMOVE: void CF_rbend::setStrength( double const& s )
// REMOVE: {
// REMOVE:
// REMOVE:   double ratio = (strength_ != 0.0) ? s/strength_ : 0.0;
// REMOVE:
// REMOVE:   CFRbendPtr q;
// REMOVE:
// REMOVE:   for ( beamline::iterator it  = bml_->begin();
// REMOVE:                            it != bml_->end(); ++it ) {
// REMOVE:
// REMOVE:     if ( ratio != 0.0  ) {
// REMOVE:          (*it)->setStrength( ratio * (*it)->Strength() );
// REMOVE:     }
// REMOVE:     else {
// REMOVE:       if ( q = boost::dynamic_pointer_cast<CF_rbend>(*it) ) q->setStrength(s);
// REMOVE:     }
// REMOVE:
// REMOVE:   }
// REMOVE:
// REMOVE:   strength_ = s;
// REMOVE: }
// REMOVE:
// REMOVE:
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_rbend::setStrength( double const& s )
{
  if( strength_ == 0 ) {
    throw( GenericException( __FILE__, __LINE__,
           "void CF_rbend::setStrength( double const& s )",
           "Cannot set strength of CF_rbend when initial strength is zero."
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
      // CF_rbend comes from splitting another CF_rbend, so that
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

void CF_rbend::setLength( double const& )
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

double CF_rbend::getOctupole() const
{
 // Returns the **integrated** octupole

  double strength = 0.0;

  for ( beamline::const_iterator it  = bml_->begin();
                                 it != bml_->end(); ++it ) {
    if( boost::dynamic_pointer_cast<thinOctupole const>(*it) )  {
      strength += (*it)->Strength();
    }
  }
  return strength;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_rbend::getSextupole() const
{
  // Returns the **integrated** sextupole

  double strength = 0.0;

  for ( beamline::const_iterator it = bml_->begin();
                                 it != bml_->end(); ++it ) {
    if( boost::dynamic_pointer_cast<thinSextupole const>(*it) ) {
      strength += (*it)->Strength();
    }
  }
  return strength;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_rbend::getQuadrupole() const
{
  // Returns the **integrated** quadrupole

  double strength = 0.0;

  for ( beamline::const_iterator it  = bml_->begin();
                                 it != bml_->end(); ++it ) {
    if( boost::dynamic_pointer_cast<thinQuad const>(*it) ) {
       strength += (*it)->Strength();
     }
  }

  return strength;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_rbend::getDipoleField() const
{
  return strength_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_rbend::Split( double const& pc, ElmPtr& a, ElmPtr& b ) const
{
  // Preliminary tests ...
  // -----------------------------
  if( ( pc <= 0.0 ) || ( pc >= 1.0 ) ) {
    ostringstream uic;
    uic << "Requested percentage = " << pc << "; should be in [0,1].";
    throw( GenericException( __FILE__, __LINE__,
           "void CF_rbend::Split( double const& pc, bmlnElmnt** a, bmlnElmnt** b )",
           uic.str().c_str() ) );
  }

  alignmentData ald( Alignment() );
  if(    ( 0. != ald.xOffset || 0. != ald.yOffset )
      && ( !hasParallelFaces()                    ) ) {
    ostringstream uic;
    uic  <<   "Not allowed to displace an rbend with non-parallel faces";
            "\nwith an Alignment struct.  That rolls are allowed in such"
            "\ncases is only a matter of courtesy. This is NOT encouraged!";
    throw( GenericException( __FILE__, __LINE__,
           "void rbend::Split( double const& pc, ElmPtr& a, ElmPtr& b ) const",
           uic.str().c_str() ) );
  }

  static bool firstTime = true;
  if( firstTime ) {
    firstTime = false;
    (*pcerr) << "\n"
            "\n*** WARNING ***"
            "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** void CF_rbend::Split( double const& pc, ElmPtr& a, ElmPtr& b ) const"
            "\n*** WARNING *** The new, split elements must be commissioned with"
            "\n*** WARNING *** RefRegVisitor before being used."
            "\n*** WARNING *** "
         << endl;
  }


  // We assume "strength_" means field, not field*length_.
  // "length_," "strength_," and "_angle" are private data members.
  // -----------------------------
  CF_rbend* p_a = 0;
  CF_rbend* p_b = 0;

  a = CFRbendPtr( p_a = new CF_rbend(   ""
                                      , pc*length_
                                      , strength_
                                      , usFaceAngle_
                                      , 0.0           ));
  p_a->setEntryAngle( getEntryAngle() );
  p_a->nullExitEdge();

  b = CFRbendPtr ( p_b = new CF_rbend(   ""
                                       , (1.0 - pc)*length_
                                       , strength_
                                       , 0.0
                                       , dsFaceAngle_        ));
  p_b->nullEntryEdge();
  p_b->setExitAngle( getExitAngle() );


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

void CF_rbend::nullExitEdge()
{
  if( bml_ ) {
    ElmPtr& endpoint = bml_->lastElement();
    if( typeid(*endpoint) == typeid(marker) ) {
      // Nothing needs to be done.
      // This occurs if the current CF_rbend is a piece
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
               "void CF_rbend::nullExitEdge()",
               uic.str().c_str() ) );
    }
  }
  else {
    throw GenericException( __FILE__, __LINE__,
      "void CF_rbend::nullExitEdge()",
      "An impossibility: internal beamline is null.");
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_rbend::nullEntryEdge()
{
  if( bml_ ) {
    ElmPtr& startpoint = bml_->firstElement();
    if( typeid(*startpoint) == typeid(marker) ) {
      // Nothing needs to be done.
      // This occurs if the current CF_rbend is a piece
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
               "void CF_rbend::nullExitEdge()",
               uic.str().c_str() ) );
    }
  }
  else {
    throw GenericException( __FILE__, __LINE__,
      "void CF_rbend::nullExitEdge()",
      "An impossibility: internal beamline is null.");
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_rbend::peekAt( double& s, Particle const& prt ) const
{
 (*pcout) << setw(12) << s;
 s += OrbitLength( prt );
 (*pcout) << setw(12) << s
                  << " : "
      << setw(10) << this
      << setw(15) << ident_
      << setw(15) << Type()
      << setw(12) << length_
      << setw(12) << strength_
      << setw(12) << ((this->getQuadrupole())/length_)
      << setw(12) << (2.0*(this->getSextupole())/length_)
      << setw(12) << (6.0*(this->getOctupole())/length_)
      << setw(12) << shuntCurrent_
      << endl;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& CF_rbend::writeTo( ostream& os )
{
  int n = bml_->countHowMany();

  if( 0 != n%12 ) {
    throw( GenericException( __FILE__, __LINE__,
           "ostream& CF_rbend::writeTo( ostream& os )",
           "Unexpected number of blocks." ) );
  }

  os << (n/12) << "  ";
  os << OSTREAM_DOUBLE_PREC << usFaceAngle_ << " "
     << OSTREAM_DOUBLE_PREC << dsFaceAngle_ << endl;
  os << OSTREAM_DOUBLE_PREC << usAngle_ << " "
     << OSTREAM_DOUBLE_PREC << dsAngle_ << endl;
  os << OSTREAM_DOUBLE_PREC << getQuadrupole() << " ";
  os << OSTREAM_DOUBLE_PREC << getSextupole() << " ";
  os << OSTREAM_DOUBLE_PREC << getOctupole() << " ";
  os << "\n";
  return os;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

istream& CF_rbend::readFrom( istream& is )
{
  double quadStrength = 0.0;
  double sextStrength = 0.0;
  double octStrength  = 0.0;
  int    n            = 0;

  is >> n
     >> usFaceAngle_
     >> dsFaceAngle_
     >> usAngle_
     >> dsAngle_
     >> quadStrength
     >> sextStrength
     >> octStrength;

  // Set multipoles
  setQuadrupole( quadStrength );
  setSextupole( sextStrength );

  return is;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* CF_rbend::Type() const
{
  return "CF_rbend";
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool CF_rbend::isMagnet() const
{
  return true;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_rbend::OrbitLength( Particle const& p )
{
  double tworho = 2.0 * ( p.Momentum() / PH_CNV_brho_to_p ) / strength_;
  return tworho * asin( length_ / tworho );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


double CF_rbend::AdjustPosition( Particle const& p )
{
  // This probably won't work properly ...
  // because the current env may not be appropriate

  JetParticle jetparticle(p);
  return AdjustPosition( jetparticle );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_rbend::AdjustPosition( JetParticle const& arg_jp )
{
  if( ( 0.0 != usFaceAngle_ ) || ( 0.0 != dsFaceAngle_ ) ) {
    throw( GenericException( __FILE__, __LINE__,
           "double CF_rbend::AdjustPosition( const JetParticle& arg_jp )",
           "In this version: only implemented for parallel faces." ) );
  }

  enum { x = 0, y, cdt, xp, yp, dpop };

  JetParticle jetparticle(arg_jp);
  Particle       particle(arg_jp);

  Mapping& jetstate =  jetparticle.State();
  Vector&     state =     particle.State();

  Jet__environment_ptr env = jetstate.Env();

  Vector    instate =     state;

  const double x_i  = state[x];
  const double xp_i = state[xp];

  // Initialize the derivative...

  jetparticle.setState(instate);

  propagate(jetparticle);

  double m = jetstate.Jacobian()( xp, x );
  if( fabs(m) < 1.0e-12 ) {
     throw( GenericException( __FILE__, __LINE__,
           "double CF_rbend::AdjustPosition( const JetParticle& arg_jp )",
           "Horrible, inexplicable error: a multi-valued solution is suspected." ) );
  }
  m = 1.0 / m;

  // Initialize the difference ...

  double z = instate[x]  = x_i;
  state = instate;

  propagate( particle );

  double f = state[xp] + xp_i;

  int i = 0;
  while( ( i < 5 ) || ( ( i < 15 ) && (fabs(f) > 1.0e-9) ) )
  {
    i++;

    // One Newton's step ...
    z -= m*f;

    instate[x] = z;
    state      = instate;

    // Recalculate inverse derivative ...

    jetparticle.setState(instate);
    propagate( jetparticle );

    m =  (jetstate.Jacobian())( xp, x );
    if( fabs(m) < 1.0e-12 ) {
      throw( GenericException( __FILE__, __LINE__,
             "double CF_rbend::AdjustPosition( const JetParticle& arg_jp )",
             "Horrible, inexplicable error: a multi-valued solution is suspected." ) );
    }
    m = 1.0 / m;

    // Recalculate difference ...

    propagate( particle);

    f = state[xp] + xp_i;
  }

  //------------------------------------------------------------------------
  // Step procedure when Newton's method fails ...
  //------------------------------------------------------------------------'

  double delta = 1.0e-4;           // step 0.1 mm

  if( i >= 15 ) {
    (*pcerr) << "*** WARNING ***                                      \n"
            "*** WARNING *** CF_rbend::AdjustPosition             \n"
            "*** WARNING *** No convergence within 15 Newton      \n"
            "*** WARNING *** iterations for magnet "
         <<                               this->Name()
         <<                           ". Proceeding to find       \n"
            "*** WARNING *** best solution by stepping.           \n"
            "*** WARNING ***                                      \n"
         << endl;


    instate[x]  = 0.0;
    state       = instate;

    propagate( particle );

    double error = state[xp] + xp_i; // this should be 0 if the angles are equal in magnitude and opposite in sign.

    instate[x] = delta;
    state      = instate;

    propagate( particle );

    f = state[xp] + xp_i;

    if(      ( ( f >= 0.0 && error >= 0.0 ) || ( f <= 0.0 && error <= 0.0 ) )
          && ( fabs(error) < fabs(f) ) )
    {
      delta = - delta;
    }

    instate[x]  = 0.0;
    state       = instate;

    while( fabs(delta) > 0.9e-6 ) {

      instate[x] +=  delta;
      state       = instate;
      propagate( particle);

      f = state[xp] + xp_i;

      while( ( ( f <= 0.0 && error <= 0.0 ) || ( f >= 0.0 && error >= 0.0 ) ) &&
             ( fabs(f) < fabs(error) ) )
      {
        error = f;
        instate[x] +=  delta;
        state       = instate;
      	propagate( particle );

      	f = state[xp] + xp_i;
      }

      instate[x] +=  delta;
      state       = instate;
      delta      *= (-0.1);

    } // while

    (*pcerr) << "*** WARNING ***                                  \n"
            "*** WARNING *** CF_rbend::AdjustPosition             \n"
            "*** WARNING *** The step procedure suggests a best   \n"
            "*** WARNING *** solution with magnet displacement "
         <<                                  (-1000.0)*instate[x]
         <<                                           " mm with   \n"
            "*** WARNING *** bend angle error "
         <<                                   (2.0e6)*error
         <<                                       " microradians. \n"
            "*** WARNING ***                                      \n"
         << endl;

    z = state[x];

  } // if


  // Set the alignment of the internal beamline.
  // this->align_->getAlignment().xOffset -= z;

  alignmentData v;

  if ( align_ ) {
     v = align_->getAlignment();
  }

  // ??? Does not work: p_bml->setAlignment( v );
  // ??? The reason is that the alignment stategy is
  // ??? not correct for elements whose faces are not
  // ??? parallel.

  setAlignment( v );

  // ??? This will work only if the in and out faces
  // ??? of the CF_rbend element are parallel.

  return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const& CF_rbend::getBendAngle() const
{
  return angle_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_rbend::getExitFaceAngle() const
{
  return dsFaceAngle_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_rbend::getEntryFaceAngle() const
{
  return usFaceAngle_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_rbend::getEntryAngle() const
{
  return usAngle_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_rbend::getExitAngle() const
{
  return dsAngle_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_rbend::accept( BmlVisitor& v )
{
  v.visit(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_rbend::accept( ConstBmlVisitor& v ) const
{
  v.visit(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

