/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Beamline:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      sbend.cc
******                                                                
******  Author:    Leo Michelotti                                     
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
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
****** Mar 2007           ostiguy@fnal.gov
****** - support for reference counted elements
****** - reduced src file coupling due to visitor interface. 
******   visit() takes advantage of (reference) dynamic type.
****** - use std::string for string operations. 
****** - eliminated unneeded dynamic casts in Split(...);
****** 
****** Oct 2007           michelotti@fnal.gov
****** - extended sbend::Split so that local alignment information 
******   (i.e. the alignment struct) is carried over to the new, 
******   split elements.  The results should be interpreted carefully.
******   This is a stopgap measure. In the longer term, I intend
******   to remove the (vestigial) alignment data from these classes.
****** 
****** Dec 2007           ostiguy@fnal.gov
****** - new typesafe propagators
****** - new implementation: sbend is now a composite element
******                                                                  
****** Apr 2008           michelotti@fnal.gov
****** - added placeholder setLength method
****** - added setStrength method
******   : not needed in earlier implementations because
******     sbend had no internal structure then.
****** - modified sbend::Split
****** - added member functions to nullify edge effects
******   : used by modified sbend::Split
****** 
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iomanip>

#include <basic_toolkit/iosetup.h>
#include <beamline/beamline.h>
#include <beamline/sbend.h>
#include <beamline/SBendPropagators.h>
#include <beamline/Particle.h>
#include <beamline/BmlVisitor.h>
#include <beamline/Alignment.h>
#include <beamline/marker.h>
#include <beamline/Edge.h>

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;


// **************************************************
//   class sbend
// **************************************************

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

sbend::sbend() 
  : bmlnElmnt( "", 0.0, 0.0),
        angle_(0.0),
  usFaceAngle_(0.0),
  dsFaceAngle_(0.0),
      usAngle_(0.0),
      dsAngle_(0.0)
{
  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


sbend::sbend( const char* n, double const& l, double const& s, double const& alpha ) 
  : bmlnElmnt( n, l, s ),
          angle_(alpha),
    usFaceAngle_(0.0),
    dsFaceAngle_(0.0),
        usAngle_(0.0),
        dsAngle_(0.0)

{
 if ( std::abs( alpha ) < 1.0e-9 ) {
   ostringstream uic;
   uic  << "| bend angle | = " << std::abs(alpha) << " < 1 nanoradian.";
   throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
          "sbend::sbend( char* n, double const& l, double const& s, double const& alpha, PropFunc* pf )",
          uic.str().c_str() ) );
 }
 if( (0.0 > s ) != (0.0 > alpha ) ) {
   (*pcerr) << "\n*** WARNING *** "
           "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
        << "\n*** WARNING *** sbend::sbend( ... )"
           "\n*** WARNING *** Magnetic field and bend angle arguments"
           "\n*** WARNING *** sent to constructor have opposite signs."
           "\n*** WARNING *** Field = " << s << " Tesla"
           "\n*** WARNING *** Bend angle = " << alpha << " radians"
           "\n*** WARNING *** Sign of the bend angle will be changed."
           "\n*** WARNING *** "
        << endl;
   angle_ = - angle_;
 }

  propagator_ =  PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


sbend::sbend( const char* n, double const& l, double const& s, double const& alpha, 
              double const& usfaceangle, double const& dsfaceangle )
  :    bmlnElmnt( n, l, s),
             angle_(alpha),
       usFaceAngle_( usfaceangle ),
       dsFaceAngle_( dsfaceangle ),
           usAngle_( usfaceangle ),
           dsAngle_(-dsfaceangle )
{

 
 static bool firstTime = true;

 if ( std::abs( alpha ) < 1.0e-9 ) {
   ostringstream uic;
   uic  << "| bend angle | = " << std::abs(alpha) << " < 1 nanoradian.";
   throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
          "sbend::sbend( double const& l, double const& s, ... )",
          uic.str().c_str() ) );
 }
 if( (0.0 > s ) != (0.0 > alpha ) ) {
   (*pcerr) << "\n*** WARNING *** "
           "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
        << "\n*** WARNING *** sbend::sbend( ... )"
           "\n*** WARNING *** Magnetic field and bend angle arguments"
           "\n*** WARNING *** sent to constructor have opposite signs."
           "\n*** WARNING *** Field = " << s << " Tesla"
           "\n*** WARNING *** Bend angle = " << alpha << " radians"
           "\n*** WARNING *** Sign of the bend angle will be changed."
           "\n*** WARNING *** "
        << endl;
   angle_ = - angle_;
 }

 if (  ( 0.0 < std::abs(usfaceangle)) 
    && ( std::abs( usfaceangle ) < 1.0e-6) ) {

   usFaceAngle_ = usAngle_ = 0.0;

   if( firstTime) {
     (*pcerr) <<   "*** WARNING *** "
             "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
          << "\n*** WARNING *** sbend::sbend( double const& l, ... PropFunc* pf )"
             "\n*** WARNING *** | upstream edge angle | = " 
          << std::abs(usfaceangle) 
          << " < 1 microradian."
             "\n*** WARNING *** It will be reset to zero."
             "\n*** WARNING *** This message is written once only."
          << endl;

     firstTime = false;
   }
 }
 if ( (std::abs(dsfaceangle) > 0.0) && (std::abs( dsfaceangle ) < 1.0e-6) ) {

    dsFaceAngle_ = dsAngle_ = 0.0;

   if( firstTime) {
     (*pcerr) <<   "*** WARNING *** "
             "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
          << "\n*** WARNING *** sbend::sbend( double const& l, ... PropFunc* pf )"
             "\n*** WARNING *** | downstream edge angle | = " 
          << std::abs(dsfaceangle) 
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

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

sbend::sbend( sbend const& x )
  :  bmlnElmnt( x ),
        angle_(x.angle_),
  usFaceAngle_(x.usFaceAngle_),
  dsFaceAngle_(x.dsFaceAngle_),
      usAngle_(x.usAngle_),
      dsAngle_(x.dsAngle_),
   propagator_(PropagatorPtr(x.propagator_->Clone()))
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

sbend::~sbend() 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sbend::setStrength( double const& s ) 
{
  if( strength_ == 0 ) {
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "void sbend::setStrength( double const& s )", 
           "Cannot set strength of sbend when initial strength is zero."
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
      // sbend comes from splitting another sbend, so that
      // one or both edges have been replaced with markers --
      // setting its strength will do no harm.
    }
  }
  else {
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "void sbend::setStrength( double const& s )", 
           "IMPOSSIBLE: Internal beamline not initialized!" ) );
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  

void sbend::setLength( double const& )
{
  (*pcerr) <<   "*** WARNING ****: "
              "\n*** WARNING ****: "  << __FILE__ << "," << __LINE__
           << "\n*** WARNING ****: void sbend::setLength( double const& )" 
              "\n*** WARNING ****: Attempt to explicitly set the length of a sbend."
              "\n*** WARNING ****: This is most likely an error.  Will not comply."
              "\n*** WARNING ****: Continuing, nonetheless... "
              "\n*** WARNING ****: " 
           << std::endl;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sbend::nullExitEdge()
{
  if( bml_ ) {
    ElmPtr& endpoint = bml_->lastElement();
    if( typeid(*endpoint) == typeid(marker) ) {
      // Nothing needs to be done.
      // This occurs if the current sbend is a piece
      // resulting from splitting another.
    }
    else if( typeid(*endpoint) == typeid(Edge) ) {
      endpoint = ElmPtr( new marker( "EdgeMarker" ) );
    }
    else {
      ostringstream uic;
      uic  <<   "Internal beamline ends in unrecognized element "
           << endpoint->Type() << " " << endpoint->Name();
      throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
               "void sbend::nullExitEdge()",
               uic.str().c_str() ) );
    }
  }
  else {
    throw GenericException( __FILE__, __LINE__, 
      "void sbend::nullExitEdge()",
      "An impossibility: internal beamline is null.");
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sbend::nullEntryEdge()
{
  if( bml_ ) {
    ElmPtr& startpoint = bml_->firstElement();
    if( typeid(*startpoint) == typeid(marker) ) {
      // Nothing needs to be done.
      // This occurs if the current sbend is a piece
      // resulting from splitting another.
    }
    else if( typeid(*startpoint) == typeid(Edge) ) {
      startpoint = ElmPtr( new marker( "EdgeMarker" ) );
    }
    else {
      ostringstream uic;
      uic  <<   "Internal beamline ends in unrecognized element "
           << startpoint->Type() << " " << startpoint->Name();
      throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
               "void sbend::nullExitEdge()",
               uic.str().c_str() ) );
    }
  }
  else {
    throw GenericException( __FILE__, __LINE__, 
      "void sbend::nullExitEdge()",
      "An impossibility: internal beamline is null.");
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double sbend::setEntryAngle( Particle const& p )
{
  return setEntryAngle( atan2( p.get_npx(), p.get_npz() ) );
  // i.e. tan(phi) = px/pz, where pz = longitudinal momentum
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double sbend::setExitAngle( Particle const& p )
{
  return setExitAngle( atan2( p.get_npx(), p.get_npz() ) );
  // i.e. tan(phi) = px/pz, where pz = longitudinal momentum
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double sbend::setEntryAngle( double const& phi /* radians */ )
{
  double ret = usAngle_;
  usAngle_  = phi;
  propagator_->setup(*this);
  return ret;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double sbend::setExitAngle( double const& phi /* radians */ )
{
  double ret = dsAngle_;
  dsAngle_ = phi;  
  propagator_->setup(*this);
  return ret;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool sbend::hasParallelFaces() const
{
  return (    ( std::abs( 2.0*usFaceAngle_ - angle_ ) < 1.0e-9 )
           && ( std::abs( 2.0*dsFaceAngle_ - angle_ ) < 1.0e-9 ) );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool sbend::hasStandardFaces() const
{
  return (    (std::abs(usFaceAngle_) < 1.0e-9) 
           && (std::abs(dsFaceAngle_) < 1.0e-9) );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool sbend::isMagnet() const
{
  return true;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sbend::Split( double const& pc, ElmPtr& a, ElmPtr& b ) const
{
  alignmentData ald( Alignment() );

  if( 0. != ald.xOffset || 0. != ald.yOffset ) {
    if( !hasParallelFaces() ) {
      ostringstream uic;
      uic  <<   "Not allowed to displace an sbend with non-parallel faces"
              "\nwith an Alignment struct.  That rolls are allowed in such"
              "\ncases is only a matter of courtesy. This is NOT encouraged!";
      throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
             "void sbend::Split( double const& pc, ElmPtr& a, ElmPtr& b ) const", 
             uic.str().c_str() ) );
    }
    if( 1.0e-10 < std::abs(pc - 0.5 ) ) {
      ostringstream uic;
      uic  <<   "Not allowed to split an sbend displaced"
              "\nwith an Alignment struct other than in its middle."
              "\nThat rolls are allowed in such cases is only a matter"
              "\nof courtesy. This is NOT encouraged!";
      throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
             "void sbend::Split( double const& pc, ElmPtr& a, ElmPtr& b ) const", 
             uic.str().c_str() ) );
    }
  }

  // -------------------------------------------------------------------
  // WARNING: The following code assumes that an sbend element
  //          is modeled with a nested beamline, whith edge effects 
  //          incorporated in upstream and downstream edge elements. 
  //          Il will *fail* if propagator assumes otherwise. 
  //--------------------------------------------------------------------

  // .. Check for the presence of a nested beamline with 3 elements ... 

  bool valid_nested_beamline = bml_ ? ( bml_->howMany() == 3 ) : false;
  
  if ( !valid_nested_beamline) { 
       throw GenericException( __FILE__, __LINE__, 
          "void sbend::Split( double const& pc, ElmPtr& a, ElmPtr& b ) const",
          "Error: Cannot split: incompatible or missing nested beamline.");
  }

  SBendPtr sb_a = SBendPtr( new sbend(   ""
                                       , length_*pc
                                       , strength_
                                       , angle_*pc
                                       , usFaceAngle_
                                       , 0.0           ));
  sb_a->setEntryAngle( this->getEntryAngle() );  // Reset from default
  sb_a->nullExitEdge();

  SBendPtr sb_b = SBendPtr( new sbend(   ""
                                       , length_*(1.0-pc)
                                       , strength_
                                       , angle_*(1.0-pc)
                                       , 0.0
                                       , dsFaceAngle_  ));
  sb_b->nullEntryEdge();
  sb_b->setExitAngle( this->getExitAngle() );    // Reset from default

  a = sb_a;
  b = sb_b;

  // Set the alignment struct
  // : this is a STOPGAP MEASURE!!!
  //   : the entire XXX::Split strategy should be/is being overhauled.
  // -----------------------------------------------------------------
  a->setAlignment( ald );
  b->setAlignment( ald );

  // Rename
  // ------
  a->rename( ident_ + string("_1") );
  b->rename( ident_ + string("_2") );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& sbend::writeTo(ostream& os)
{
  os << OSTREAM_DOUBLE_PREC << angle_ << " "
     << OSTREAM_DOUBLE_PREC << usFaceAngle_ << " "
     << OSTREAM_DOUBLE_PREC << dsFaceAngle_ << endl;
  os << OSTREAM_DOUBLE_PREC << usAngle_ << " "
     << OSTREAM_DOUBLE_PREC << dsAngle_ << endl;

  os << "\n";
  return os;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

istream& sbend::readFrom(istream& is)
{
  is >> angle_ >> usFaceAngle_ >> dsFaceAngle_;
  is >> usAngle_ >> dsAngle_;
  return is;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// **************************************************
//   Frame functions
// **************************************************

void sbend::enterLocalFrame( Particle& p ) const
{
  bmlnElmnt::enterLocalFrame(p);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sbend::enterLocalFrame( JetParticle& p ) const
{
 bmlnElmnt::enterLocalFrame(p);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sbend::leaveLocalFrame( Particle& p ) const
{
  bmlnElmnt::leaveLocalFrame(p);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sbend::leaveLocalFrame( JetParticle& p ) const
{
  bmlnElmnt::leaveLocalFrame(p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* sbend::Type() const 
{ 
  return "sbend"; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sbend::accept( BmlVisitor& v ) 
{ 
 v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sbend::accept( ConstBmlVisitor& v ) const 
{ 
  v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sbend::localPropagate( Particle& p ) 
{ 
  (*propagator_)(*this, p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sbend::localPropagate( JetParticle& p ) 
{ 
  (*propagator_)(*this, p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sbend::localPropagate( ParticleBunch& b ) 
{ 
  (*propagator_)(*this, b);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sbend::localPropagate( JetParticleBunch& b ) 
{ 
  (*propagator_)(*this, b);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double sbend::getEntryAngle() const
{
  return usAngle_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double sbend::getExitAngle() const
{
  return dsAngle_;
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double sbend::getEntryFaceAngle() const
{
  return usFaceAngle_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double sbend::getExitFaceAngle() const
{
  return dsFaceAngle_;
}
