/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      CF_sbend.cc
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
****** REVISION HISTORY
******
****** May 2008           ostiguy@fnal.gov
****** - setStrength() now dispatched to propagator by base class
******   (no longer virtual)
****** - added explicit implementation for assignment operator
****** Apr 2008            michelotti@fnal.gov
****** - modified setStrength method
****** - added placeholder setLength method
****** - modified sbend::Split
****** - added member functions to nullify edge effects
******   : used by modified sbend::Split
****** Aug 2007           ostiguy@fnal.gov
****** - composite structure based on regular beamline
****** Mar 2007           ostiguy@fnal.gov
****** - support for reference counted elements
****** - reduced src file coupling due to visitor interface. 
******   visit() takes advantage of (reference) dynamic type.
****** - use std::string for string operations. 
****** Dec 2007           ostiguy@fnal.gov
****** - new typesafe propagator architecture  
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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iomanip>

#include <basic_toolkit/GenericException.h>
#include <basic_toolkit/iosetup.h>
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

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_sbend::CF_sbend()
  : BmlnElmnt( "", 0.0, 0.0), 
          angle_(0.0),       
    usFaceAngle_(0.0),
    dsFaceAngle_(0.0),
        usAngle_(0.0),
        dsAngle_(0.0),
     multipoles_()
{
  propagator_ = PropagatorPtr( new Propagator(*this,1 ));
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_sbend::CF_sbend( std::string const& name,
                    double const&        lng,  // length     [ meter    ]
                    double const&        fld,  // field      [ tesla    ]
                    double const&        ang )
  : BmlnElmnt( name, lng, fld ),
        angle_(ang),
  usFaceAngle_(0.0),
  dsFaceAngle_(0.0),
      usAngle_(0.0),
      dsAngle_(0.0),
   multipoles_(4,std::complex<double>(0.0,0.0) )
{

  propagator_ = PropagatorPtr( new Propagator(*this, 1) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


CF_sbend::CF_sbend( std::string const& name,
                    double const&        lng,  // length     [ meter    ]
                    double const&        fld,  // field      [ tesla    ]
                    double const&        ang,  // bend angle [ radians ]
                    double const&        us,   // upstream edge angle [radians]
                    double const&        ds)   // downstream edge angle [radians]
: BmlnElmnt( name, lng, fld ),
       angle_(ang),
 usFaceAngle_(us),
 dsFaceAngle_(ds),
     usAngle_(us),
     dsAngle_(-ds),
  multipoles_(4, std::complex<double>(0.0,0.0) ) 
{
  propagator_ = PropagatorPtr( new Propagator(*this,1) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_sbend::CF_sbend( CF_sbend const& x )
  : BmlnElmnt( x ),
        angle_(x.angle_),
  usFaceAngle_(x.usFaceAngle_),
  dsFaceAngle_(x.dsFaceAngle_),
      usAngle_(x.usAngle_),
      dsAngle_(x.dsAngle_),
   multipoles_(x.multipoles_)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_sbend::~CF_sbend()
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_sbend& CF_sbend::operator=(CF_sbend const& rhs)
{
  if (this == &rhs) return *this;
  BmlnElmnt:operator=(rhs);
  angle_       = rhs.angle_;
  usFaceAngle_ = rhs.usFaceAngle_;
  dsFaceAngle_ = rhs.dsFaceAngle_;
  usAngle_     = rhs.usAngle_;
  dsAngle_     = rhs.dsAngle_;
  multipoles_  = rhs.multipoles_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_sbend::setEntryAngle( Particle const& p )
{
  return setEntryAngle( atan2( p.npx(), p.npz() ) );
  // i.e. tan(phi) = px/pz, where pz = longitudinal momentum
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_sbend::setExitAngle( Particle const& p )
{
  return setExitAngle( atan2( p.npx(), p.npz() ) );
  // i.e. tan(phi) = px/pz, where pz = longitudinal momentum
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_sbend::setEntryAngle( double const& phi /* radians */ )
{
  double ret = usAngle_;
  usAngle_   = phi;
  propagator_->ctor(*this);
  return ret;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_sbend::setExitAngle( double const& phi /* radians */ )
{
  double ret = dsAngle_;
  dsAngle_   = phi;  
  propagator_->ctor(*this);
  return ret;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_sbend::setOctupole( double const& pole )
{
  multipoles_[3] = pole;
  propagator_->ctor(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_sbend::setSextupole( double const& pole )
{
  multipoles_[2] = pole;
  propagator_->ctor(*this);
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

void CF_sbend::setQuadrupole( double const& pole)
{

  multipoles_[1] = pole;
  propagator_->ctor(*this);

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int CF_sbend::setDipoleField( double const& value)
{

#if 0
  for ( beamline::iterator it  = bml_->begin(); 
	                   it != bml_->end(); ++it ) {
    if( boost::dynamic_pointer_cast<sbend>(*it) ) { 
     (*it)->setStrength( value );
    }
  }

#endif
  setStrength( value ); 

  return 0;


}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_sbend::getOctupole() const
{
  return  multipoles_[3].real()*length_;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_sbend::getSextupole() const
{
  return multipoles_[2].real()*length_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_sbend::getQuadrupole() const
{
  return multipoles_[1].real()*length_;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_sbend::getDipoleField() const
{
  return Strength();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_sbend::propagateReference( Particle& p, double initialBRho, bool scaling )
{
  setEntryAngle(p);
  BmlnElmnt::propagateReference(p,initialBRho, scaling);
  setExitAngle(p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::pair<ElmPtr,ElmPtr> CF_sbend::split( double const& pc ) const
{

#if 0
  // Preliminary tests ...
  // -----------------------------
  if( ( pc <= 0.0 ) || ( pc >= 1.0 ) ) {
    ostringstream uic;
    uic << "Requested percentage = " << pc << "; should be in [0,1].";
    throw( GenericException( __FILE__, __LINE__, 
           " CF_sbend::split( double const& pc )", 
           uic.str().c_str() ) );
  }

  if( (alignment().xOffset() != 0.0 ) || 
      (alignment().yOffset() != 0.0 ) ) {
    if( !hasParallelFaces() ) {
      ostringstream uic;
      uic  <<   "Not allowed to displace an CF_sbend with non-parallel faces"
              "\nwith an Alignment struct.  That rolls are allowed in such"
              "\ncases is only a matter of courtesy. This is NOT encouraged!";
      throw( GenericException( __FILE__, __LINE__, 
             "CF_sbend::split( double const& pc ) const", 
             uic.str().c_str() ) );
    }
    if( 1.0e-10 < std::abs(pc - 0.5 ) ) {
      ostringstream uic;
      uic  <<   "Not allowed to split an CF_sbend displaced"
              "\nwith an Alignment struct other than in its middle."
              "\nThat rolls are allowed in such cases is only a matter"
              "\nof courtesy. This is NOT encouraged!";
      throw( GenericException( __FILE__, __LINE__, 
             " CF_sbend::split( double const& pc ) const", 
             uic.str().c_str() ) );
    }
  }

  static bool firstTime = true;
  if( firstTime ) {
    firstTime = false;
    (*pcerr) << "\n"
            "\n*** WARNING ***"
            "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** CF_sbend::split( double pc )"
            "\n*** WARNING *** A new reference trajectory must be registered."
            "\n*** WARNING *** "
         << endl;
  }

  // We assume "strength_" means field, not field*length_.
  // "length_," "strength_," and "angle_" are private data members.
  // -----------------------------

  CFSbendPtr p_a(   new CF_sbend(   ""
                                  , pc*length_
                                  , Strength()
                                  , pc*angle_
                                  , usFaceAngle_
                                  , 0.0            ));

  p_a->setEntryAngle( getEntryAngle() );
  p_a->bml_->back()->setStrength(0.0);
  
  CFSbendPtr p_b(    new CF_sbend(   ""
                                  , (1.0 - pc)*length_
                                  , Strength()
                                  , (1.0 - pc)*angle_
                                  , 0.0
                                  , dsFaceAngle_       ));

  p_b->bml_->front()->setStrength(0.0);
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
  p_a->setAlignment( alignment() );
  p_b->setAlignment( alignment() );

  // Rename
  // -----------------------------
  p_a->rename( ident_ + string("_1") );
  p_b->rename( ident_ + string("_2"));

  return std::make_pair(p_a,p_b);
#endif
  return std::make_pair(ElmPtr(), ElmPtr());
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& CF_sbend::writeTo( ostream& os )
{
#if 0
  os << OSTREAM_DOUBLE_PREC << angle_       << " ";
  os << OSTREAM_DOUBLE_PREC << usFaceAngle_ << " "
     << OSTREAM_DOUBLE_PREC << dsFaceAngle_ << endl;
  os << OSTREAM_DOUBLE_PREC << usAngle_     << " "
     << OSTREAM_DOUBLE_PREC << dsAngle_     << endl;
  os << OSTREAM_DOUBLE_PREC << getQuadrupole() << " ";
  os << OSTREAM_DOUBLE_PREC << getSextupole()  << " ";
  os << OSTREAM_DOUBLE_PREC << getOctupole()   << " ";
  os << "\n";
#endif
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

bool  CF_sbend::isThin()    const
{
  return false;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool  CF_sbend::isPassive() const
{
  return false;
}
  
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool  CF_sbend::isDriftSpace() const
{
  return false;
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
