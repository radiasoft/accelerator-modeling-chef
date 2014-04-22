/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      rbend.cc
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
******                                                                
****** Mar 2007           ostiguy@fnal.gov
****** - support for reference counted elements
****** - reduced src file coupling due to visitor interface. 
******   visit() takes advantage of (reference) dynamic type.
****** - use std::string for string operations. 
****** - eliminated unneeded dynamic casts in Split(...);
****** 
****** Oct 2007           michelotti@fnal.gov
****** - extended rbend::Split so that local alignment information 
******   (i.e. the alignment struct) is carried over to the new, 
******   split elements.  The results should be interpreted carefully.
******   This is a stopgap measure. In the longer term, I intend
******   to remove the (vestigial) alignment data from these classes.
****** 
****** Dec 2007           ostiguy@fnal.gov
****** - new typesafe propagators
****** - new implementation: rbend is now a composite element
******
****** Apr 2008            michelotti@fnal.gov
****** - added setStrength method
******   : not needed in earlier implementations because
******     rbend had no internal structure then.
****** - added placeholder setLength method
****** - corrected rbend::Split
******   : including adding methods to nullify edge effects
******
**************************************************************************
*************************************************************************/
//----------------------------------------------------------------------------------------------
//
// usFaceAngle_, dsFaceAngle_ 
//
// are the pole face angle w/r to nominal sbend pole face position
// for a pure, symmetric rbend,  usFaceAngle_ = dsFaceAngle_ = +/- (bend angle/2.0 ) 
//
// usAngle_ and dsAngle 
// are the angles of the fiducial trajectory w/r to the ***sbend*** pole faces.   
//
//---------------------------------------------------------------------------------------------



#include <iomanip>
#include <typeinfo>

#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/GenericException.h>
#include <basic_toolkit/MathConstants.h>
#include <basic_toolkit/PhysicsConstants.h>
#include <beamline/rbend.h>
#include <beamline/beamline.h>
#include <beamline/RBendPropagators.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/BmlVisitor.h>
#include <beamline/Alignment.h>
#include <beamline/marker.h>
#include <beamline/Edge.h>

using namespace std;
using FNAL::pcout;
using FNAL::pcerr;
;

// **************************************************
//   class rbend
// **************************************************

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

rbend::rbend()
  : bmlnElmnt( "", 1.0, 0.0 ),
    angle_(0.0),
    usFaceAngle_(0.0),
    dsFaceAngle_(0.0),
    usAngle_(M_TWOPI),  // uninitialized        
    dsAngle_(-M_TWOPI)  // uninitialized
{
  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

rbend::rbend( const char* n, double const& l, double const& s, double const& bendangle) 
  : bmlnElmnt( n, l, s ),
    angle_(bendangle), 
    usFaceAngle_(0.0),
    dsFaceAngle_(0.0),
    usAngle_(bendangle/2.0),
    dsAngle_(-bendangle/2.0)
{
  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

rbend::rbend( const char* n, double const& l, double const& s, double const& bendangle, double const& entryangle ) 
  : bmlnElmnt( n, l, s),
    angle_(bendangle), 
    usFaceAngle_(0.0),
    dsFaceAngle_(0.0),
    usAngle_(entryangle),
    dsAngle_(-entryangle)
{
 static bool firstTime = true;
 if( (0.0 < fabs(entryangle)) && (fabs( entryangle) < 1.0e-9) ) {
   usAngle_ = 0.0;
   dsAngle_ = 0.0;
   if( firstTime) {
     (*pcerr) <<   "*** WARNING *** "
             "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
          << "\n*** WARNING *** rbend::rbend( char* n, ... PropFunc* pf )"
             "\n*** WARNING *** | upstream entry angle | = " 
          << fabs(entryangle) 
          << " < 1 nanoradian."
             "\n*** WARNING *** It will be reset to zero."
             "\n*** WARNING *** This message is written once only."
          << endl;
     firstTime = false;
   }
 }

  propagator_ =  PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

rbend::rbend( const char* n, double const& l, double const& s, double const& bendangle, double const& us, double const& ds )
  : bmlnElmnt( n, l, s),
    angle_(bendangle), 
    usFaceAngle_(us),
    dsFaceAngle_(ds),
    usAngle_(  (bendangle/2.0) + us ),
    dsAngle_(-((bendangle/2.0) + ds))
{
 static bool firstTime = true;
 if ( (0.0 < fabs(us)) && (fabs( us ) < 1.0e-6) ) {
   usFaceAngle_ = 0.0;
   usAngle_     = 0.0;
   if( firstTime) {
     (*pcerr) <<   "*** WARNING *** "
             "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
          << "\n*** WARNING *** rbend::rbend( double const& l, ... PropFunc* pf )"
             "\n*** WARNING *** | upstream edge angle | = " 
          << fabs(us) 
          << " < 1 microradian."
             "\n*** WARNING *** It will be reset to zero."
             "\n*** WARNING *** This message is written once only."
          << endl;
     firstTime = false;
   }
 }
 if ( (0.0 < fabs(ds)) && (fabs( ds ) < 1.0e-6) ) {
   dsFaceAngle_ = 0.0;
   dsAngle_     = 0.0;
   if( firstTime) {
     (*pcerr) <<   "*** WARNING *** "
             "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
          << "\n*** WARNING *** rbend::rbend( double const& l, ... PropFunc* pf )"
             "\n*** WARNING *** | downstream edge angle | = " 
          << fabs(ds) 
          << " < 1 microradian."
             "\n*** WARNING *** It will be reset to zero."
             "\n*** WARNING *** This message is written once only."
          << endl;
     firstTime = false;
   }
 }

  propagator_ =  PropagatorPtr( new Propagator());
  propagator_->setup(*this);

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

rbend::rbend( char const* n, double const& l, double const& s,           double const& bendangle, 
                                              double const& entry_angle, 
                                              double const& us,          double const& ds )
  : bmlnElmnt( n, l, s),
    angle_(bendangle),
    usFaceAngle_(us), 
    dsFaceAngle_(ds),
    usAngle_(   entry_angle + us ),
    dsAngle_( -(entry_angle + ds))
{
 static bool firstTime = true;
 if( (0.0 < fabs(entry_angle)) && (fabs( entry_angle) < 1.0e-9) ) {
   usAngle_ = us;
   dsAngle_ = -ds;
   if( firstTime) {
     (*pcerr) <<   "*** WARNING *** "
             "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
          << "\n*** WARNING *** rbend::rbend( char* n, ... PropFunc* pf )"
             "\n*** WARNING *** | upstream entry angle | = " 
          << fabs(entry_angle) 
          << " < 1 nanoradian."
             "\n*** WARNING *** It will be reset to zero."
             "\n*** WARNING *** This message is written once only."
          << endl;
     firstTime = false;
   }
 }
 if ( (0.0 < fabs(us)) && (fabs( us ) < 1.0e-6) ) {
   usFaceAngle_ = 0.0;
   usAngle_     = entry_angle;
   if( firstTime) {
     (*pcerr) <<   "*** WARNING *** "
             "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
          << "\n*** WARNING *** rbend::rbend( double const& l, ... PropFunc* pf )"
             "\n*** WARNING *** | upstream edge angle | = " 
          << fabs(us) 
          << " < 1 microradian."
             "\n*** WARNING *** It will be reset to zero."
             "\n*** WARNING *** This message is written once only."
          << endl;
     firstTime = false;
   }
 }
 if ( (0.0 < fabs(ds)) && (fabs( ds ) < 1.0e-6) ) {
   dsFaceAngle_ = 0.0;
   dsAngle_     = -entry_angle;
   if( firstTime) {
     (*pcerr) <<   "*** WARNING *** "
             "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
          << "\n*** WARNING *** rbend::rbend( double const& l, ... PropFunc* pf )"
             "\n*** WARNING *** | downstream edge angle | = " 
          << fabs(ds) 
          << " < 1 microradian."
             "\n*** WARNING *** It will be reset to zero."
             "\n*** WARNING *** This message is written once only."
          << endl;
     firstTime = false;
   }
 }

  propagator_ =  PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

rbend::rbend( char const* n, double const& l, double const& s,           double const& bendangle, 
                                              double const& entry_angle, double const& exit_angle, 
                                              double const& us,          double const& ds )
try
  : bmlnElmnt( n, l, s),
    angle_(bendangle),
    usFaceAngle_(us), 
    dsFaceAngle_(ds),
    usAngle_(entry_angle + us),
    dsAngle_( exit_angle - ds)
{
  // ??? FIX ME ???
  throw( GenericException( __FILE__, __LINE__, 
         "rbend::rbend( char const* n, double const& l, double const& s, double const& bendangle, \n"
         "                                    double const& entry_angle, double const& exit_angle,\n"
         "                                    double const& us,          double const& ds          )",
         "THIS CONSTRUCTOR IS NOT WRITTEN." ) );
}
catch( GenericException const& ge )
{
  // This catch block is included only out of paranoia.
  // Nothing needs to be done here.
  throw ge;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

rbend::rbend( rbend const& x )
  : bmlnElmnt(x ),
    angle_(x.angle_),
    usFaceAngle_(x.usFaceAngle_), dsFaceAngle_(x.dsFaceAngle_),
    usAngle_(x.usAngle_),         dsAngle_(x.dsAngle_),
    propagator_(PropagatorPtr(x.propagator_->Clone()))
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

rbend::~rbend() 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rbend::setStrength( double const& s ) 
{
  if( strength_ == 0 ) {
    throw( GenericException( __FILE__, __LINE__, 
           "void rbend::setStrength( double const& s )", 
           "Cannot set strength of rbend when initial strength is zero."
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
      // rbend comes from splitting another rbend, so that
      // one or both edges have been replaced with markers --
      // setting its strength will do no harm.
    }
  }
  else {
    throw( GenericException( __FILE__, __LINE__, 
           "void rbend::setStrength( double const& s )", 
           "IMPOSSIBLE: Internal beamline not initialized!" ) );
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  

void rbend::setLength( double const& )
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

const char* rbend::Type() const 
{ 
  return "rbend"; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double rbend::OrbitLength( Particle const& x ) const
{
  // Computes arclength of orbit assuming a symmetric bend.
  // WARNING: This is not the true arclength under all conditions.
  double tworho = 2.0 * ( x.Momentum() / PH_CNV_brho_to_p ) / strength_;
  return tworho * asin( length_ / tworho );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double rbend::OrbitLength( Particle const& x )
{
    // Computes arclength of orbit assuming a symmetric bend.
    // WARNING: This is not the true arclength under all conditions.
    double tworho = 2.0 * ( x.Momentum() / PH_CNV_brho_to_p ) / strength_;
    return tworho * asin( length_ / tworho );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool rbend::isMagnet() const
{
  return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rbend::Split( double const& pc, ElmPtr& a, ElmPtr& b ) const
{
  // Preliminary tests ...
  // -----------------------------
  if( ( pc <= 0.0 ) || ( pc >= 1.0 ) ) {
    ostringstream uic;
    uic  << "pc = " << pc << ": this should be within [0,1].";
    throw( GenericException( __FILE__, __LINE__, 
           "void rbend::Split( double const& pc, ElmPtr& a, ElmPtr& b )", 
           uic.str().c_str() ) );
  }

  alignmentData ald( Alignment() );
  if(    ( 0. != ald.xOffset || 0. != ald.yOffset ) 
      && ( !hasParallelFaces()                    ) ) {
    ostringstream uic;
    uic  <<   "Not allowed to displace an rbend with non-parallel faces"
         <<   "\nwith an Alignment struct.  That rolls are allowed in such"
         <<   "\ncases is only a matter of courtesy. This is NOT encouraged!";
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
         << "\n*** WARNING *** void rbend::Split( double const& pc, ElmPtr& a, ElmPtr& b )"
            "\n*** WARNING *** The new, split elements must be commissioned with"
            "\n*** WARNING *** RefRegVisitor before being used."
            "\n*** WARNING *** "
         << endl;
  }

  // -------------------------------------------------------------------
  // WARNING: The following code assumes that an rbend element
  //          is modeled with a nested beamline, with edge effects, if any,
  //          incorporated in upstream and downstream edge elements. 
  //          It will *fail* when the propagator that assumes otherwise. 
  //--------------------------------------------------------------------
  // .. Check for the presence of a nested beamline with 3 elements ... 
  // ---------------------------------------------------------------
  bool valid_nested_beamline = bml_ ?  ( bml_->howMany() == 3 ) : false;
  if ( !valid_nested_beamline) { 
       throw GenericException( __FILE__, __LINE__, 
	  "void sbend::Split( double const& pc, ElmPtr& a, ElmPtr& b ) const",
          "Error: Cannot split: incompatible or missing nested beamline.");
  }

  RBendPtr rb_a( new rbend(   ""
                            , pc*length_
                            , strength_
                            , angle_*pc         // Wrong, but unimportant (I hope)!
                            , usFaceAngle_
                            , 0.0 ) ); 
  rb_a->setEntryAngle( this->getEntryAngle() ); // Reset from default
  rb_a->nullExitEdge();

  RBendPtr rb_b( new rbend(   ""
                            , (1.0 - pc)*length_
                            , strength_
                            , angle_*(1.0-pc)   // Wrong, but unimportant (I hope)!
                            , 0.0
                            , dsFaceAngle_ ) );
  rb_b->nullEntryEdge();
  rb_b->setExitAngle( this->getExitAngle() );   // Reset from default

  a = rb_a;
  b = rb_b;

  // Set the alignment struct
  // : this is a STOPGAP MEASURE!!!
  // -----------------------------------------------------------------
  a->setAlignment( ald );
  b->setAlignment( ald );

  // Rename
  // ------
  a->rename( ident_ + string("_1") );
  b->rename( ident_ + string("_2") );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rbend::nullExitEdge()
{
  if( bml_ ) {
    ElmPtr& endpoint = bml_->lastElement();
    if( typeid(*endpoint) == typeid(marker) ) {
      // Nothing needs to be done.
      // This occurs if the current rbend is a piece
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
               "void rbend::nullExitEdge()",
               uic.str().c_str() ) );
    }
  }
  else {
    throw GenericException( __FILE__, __LINE__, 
      "void rbend::nullExitEdge()",
      "An impossibility: internal beamline is null.");
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rbend::nullEntryEdge()
{
  if( bml_ ) {
    ElmPtr& startpoint = bml_->firstElement();
    if( typeid(*startpoint) == typeid(marker) ) {
      // Nothing needs to be done.
      // This occurs if the current rbend is a piece
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
               "void rbend::nullExitEdge()",
               uic.str().c_str() ) );
    }
  }
  else {
    throw GenericException( __FILE__, __LINE__, 
      "void rbend::nullExitEdge()",
      "An impossibility: internal beamline is null.");
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rbend::setPoleFaceAngle( Particle const& p )
{
  double psi = asin( (strength_*length_) / (2.0*p.ReferenceBRho()) );
  // i.e., sin( psi ) = (l/2) / rho
  //                  = Bl/(2*Brho)
  //                  = 1/2 symmetric bend angle
  setEntryAngle( psi );
  setExitAngle( -psi );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double rbend::setEntryAngle( Particle const& p )
{
  return setEntryAngle( atan2( p.get_npx(), p.get_npz() ) );
  // i.e. tan(phi) = px/pz, where pz = longitudinal momentum
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double rbend::setExitAngle( Particle const& p )
{
  return setExitAngle( atan2( p.get_npx(), p.get_npz() ) );
  // i.e. tan(phi) = px/pz, where pz = longitudinal momentum
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double rbend::setEntryAngle( double const& phi /* radians */ )
{
  double ret = usAngle_;
  usAngle_   = phi;
  propagator_->setup(*this);
  return ret;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double rbend::setExitAngle( double const& phi /* radians */ )
{
  double ret = dsAngle_;
  dsAngle_   = phi;  
  propagator_->setup(*this);
  return ret;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool rbend::hasParallelFaces() const
{
  return (    (std::abs(usFaceAngle_) < 1.0e-9) 
           && (std::abs(dsFaceAngle_) < 1.0e-9) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool rbend::hasStandardFaces() const
{
  return (    (std::abs(usFaceAngle_) < 1.0e-9) 
           && (std::abs(dsFaceAngle_) < 1.0e-9) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& rbend::writeTo(ostream& os)
{
  os << OSTREAM_DOUBLE_PREC << usFaceAngle_ << " "
     << OSTREAM_DOUBLE_PREC << dsFaceAngle_ << endl;
  os << OSTREAM_DOUBLE_PREC << usAngle_     << " "
     << OSTREAM_DOUBLE_PREC << dsAngle_     << endl;

  os << "\n";
  return os;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

istream& rbend::readFrom(istream& is)
{
  is >> usFaceAngle_ >> dsFaceAngle_;
  is >> usAngle_     >> dsAngle_;

  return is;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rbend::accept( BmlVisitor& v )            
{ 
  v.visit( *this ); 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rbend::accept( ConstBmlVisitor& v ) const 
{ 
  v.visit(*this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rbend::localPropagate( Particle& p ) 
{ 
  (*propagator_)(*this, p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rbend::localPropagate( JetParticle& p ) 
{ 
  (*propagator_)(*this, p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rbend::localPropagate( ParticleBunch& b ) 
{ 
  (*propagator_)(*this, b);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rbend::localPropagate( JetParticleBunch& b ) 
{ 
  (*propagator_)(*this, b);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double rbend::getEntryAngle() const
{
  return usAngle_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double rbend::getExitAngle() const
{
  return dsAngle_;
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double rbend::getEntryFaceAngle() const
{
  return usFaceAngle_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double  rbend::getExitFaceAngle() const
{
  return dsFaceAngle_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const& rbend::getBendAngle() const
{
  return angle_; // bend angle 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const& rbend::getPoleFaceAngle() const
{
  return usFaceAngle_; // FIXME !!!
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

