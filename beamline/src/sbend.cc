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
****** Dec 2007           ostiguy@fnal.gov
****** - new typesafe propagators
****** - new implementation: sbend is now a composite element
******
****** Oct 2007           michelotti@fnal.gov
****** - extended sbend::Split so that local alignment information 
******   (i.e. the alignment struct) is carried over to the new, 
******   split elements.  The results should be interpreted carefully.
******   This is a stopgap measure. In the longer term, I intend
******   to remove the (vestigial) alignment data from these classes.
******
****** Mar 2007           ostiguy@fnal.gov
****** - support for reference counted elements
****** - reduced src file coupling due to visitor interface. 
******   visit() takes advantage of (reference) dynamic type.
****** - use std::string for string operations. 
****** - eliminated unneeded dynamic casts in Split(...);
****** 
****** 
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iomanip>

#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/GenericException.h>
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


sbend::sbend( std::string const& n, double const& l, double const& s, double const& alpha ) 
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
   throw( GenericException( __FILE__, __LINE__, 
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


sbend::sbend( std::string const& n, double const& l, double const& s, double const& alpha, 
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
   throw( GenericException( __FILE__, __LINE__, 
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
      dsAngle_(x.dsAngle_)
{
  propagator_->setup(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

sbend::~sbend() 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

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
  double ret  = usAngle_;
  usAngle_    = phi;
  propagator_->setup(*this);
  return ret;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double sbend::setExitAngle( double const& phi /* radians */ )
{
  double ret = dsAngle_;
  dsAngle_   = phi;  
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

bool sbend::isThin() const
{
  return false;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool sbend::isPassive() const
{
  return false;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool sbend::isDriftSpace() const
{
  return false;
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::pair<ElmPtr,ElmPtr> sbend::split( double const& pc ) const
{
  // Preliminary tests ...
  // -----------------------------
  if( ( pc <= 0.0 ) || ( pc >= 1.0 ) ) {
    ostringstream uic;
    uic << "Requested percentage = " << pc << "; should be in [0,1].";
    throw( GenericException( __FILE__, __LINE__, 
           "sbend::split( double const& pc ) const",
           uic.str().c_str() ) );
  }

  if(  alignment().xOffset() == 0.0 || alignment().yOffset() == 0.0  ) {
    if( !hasParallelFaces() ) {
      ostringstream uic;
      uic  <<   "Not allowed to displace an sbend with non-parallel faces"
              "\nwith an Alignment struct.  That rolls are allowed in such"
              "\ncases is only a matter of courtesy. This is NOT encouraged!";
      throw( GenericException( __FILE__, __LINE__, 
             "sbend::split( double const& pc) const", 
             uic.str().c_str() ) );
    }
    if( 1.0e-10 < std::abs(pc - 0.5 ) ) {
      ostringstream uic;
      uic  <<   "Not allowed to split an sbend displaced"
              "\nwith an Alignment struct other than in its middle."
              "\nThat rolls are allowed in such cases is only a matter"
              "\nof courtesy. This is NOT encouraged!";
      throw( GenericException( __FILE__, __LINE__, 
             "sbend::split( double const& pc) const", 
             uic.str().c_str() ) );
    }
  }


  SBendPtr sb_a = SBendPtr( new sbend(   ""
                                       , length_*pc
                                       , Strength()
                                       , angle_*pc
                                       , usFaceAngle_
                                       , 0.0           ));
  sb_a->setEntryAngle( getEntryAngle() );  // Reset from default
  sb_a->enableEdges(true,false);

  SBendPtr sb_b = SBendPtr( new sbend(   ""
                                       , length_*(1.0-pc)
                                       , Strength()
                                       , angle_*(1.0-pc)
                                       , 0.0
                                       , dsFaceAngle_  ));
  sb_b->setExitAngle( getExitAngle() );    // Reset from default
  sb_b->enableEdges(false,true);

  // Set the alignment struct
  // : this is a STOPGAP MEASURE!!!
  // -----------------------------------------------------------------
  sb_a->setAlignment( alignment() );
  sb_b->setAlignment( alignment() );

  // Rename
  // ------
  sb_a->rename( ident_ + string("_1") );
  sb_b->rename( ident_ + string("_2") );

  return std::make_pair(sb_a,sb_b);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& sbend::writeTo(ostream& os)
{
#if 0
  os << OSTREAM_DOUBLE_PREC << angle_ << " "
     << OSTREAM_DOUBLE_PREC << usFaceAngle_ << " "
     << OSTREAM_DOUBLE_PREC << dsFaceAngle_ << endl;
  os << OSTREAM_DOUBLE_PREC << usAngle_ << " "
     << OSTREAM_DOUBLE_PREC << dsAngle_ << endl;

  os << "\n";
#endif
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

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sbend::propagateReference( Particle& p, double initialBRho, bool scaling) 
{
   setEntryAngle( p );
   bmlnElmnt::propagateReference( p, initialBRho, scaling);
   setExitAngle( p );
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
