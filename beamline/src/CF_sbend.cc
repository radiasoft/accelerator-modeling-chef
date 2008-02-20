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
****** Aug 2007           ostiguy@fnal.gov
****** - composite structure based on regular beamline
****** Dec 2007           ostiguy@fnal.gov
****** - new typesafe propagator architecture  
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

using namespace std;

using FNAL::pcout;
using FNAL::pcerr;

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_sbend::CF_sbend()
  : bmlnElmnt( "", 0.0, 0.0), 
          angle_(0.0),       
    usFaceAngle_(0.0),
    dsFaceAngle_(0.0),
        usAngle_(0.0),
        dsAngle_(0.0),
     multipoles_()
{
  propagator_ = PropagatorPtr( new Propagator(1) ); 
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
   multipoles_()
{
  propagator_ = PropagatorPtr( new Propagator(1) ); 
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
  multipoles_()
{
  propagator_ = PropagatorPtr( new Propagator(1) ); 
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
   multipoles_(x.multipoles_),
   propagator_(PropagatorPtr(x.propagator_->Clone()))
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_sbend::~CF_sbend()
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_sbend::peekAt( double& s, Particle const& prt ) 
{
 (*pcout) << setw(12) << s;
 s += OrbitLength( prt );
 (*pcout) << setw(12) << s           
                  << " : " 
      << setw(10) << (int) this  
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
    if( ( boost::dynamic_pointer_cast<thinOctupole>(*it) ) ) ++counter;
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

int CF_sbend::setSextupole( double const& arg_x )
{

  int counter = 0;
  for ( beamline::const_iterator it  = bml_->begin(); 
                                 it != bml_->end(); ++it ) {
    if( ( boost::dynamic_pointer_cast<thinSextupole>(*it) ) ) ++counter;
  }

  if (counter==0) return 1;
 
  for ( beamline::iterator it  = bml_->begin(); 
                           it != bml_->end(); ++it ) {
   if ( boost::dynamic_pointer_cast<thinSextupole>(*it) )  {
     (*it)->setStrength( arg_x/counter );
   }
  }
  
  return 0;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool CF_sbend::hasParallelFaces() const
{
  return ( std::abs( usFaceAngle_ - dsFaceAngle_ ) <  1.0e-9 );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


bool CF_sbend::hasStandardFaces() const
{
  return ( (std::abs(usFaceAngle_) < 1.0e-9) && (std::abs(dsFaceAngle_) < 0.5e-9) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int CF_sbend::setQuadrupole( double const& arg_x )
{

  int counter = 0;
  for ( beamline::const_iterator it  = bml_->begin(); 
                                 it != bml_->end(); ++it ) {

    if( boost::dynamic_pointer_cast<thinQuad>(*it) ) ++counter;
  }
  
  if (counter==0) return 1;
 
  for ( beamline::iterator it  = bml_->begin(); 
	                   it != bml_->end(); ++it ) {
   if( boost::dynamic_pointer_cast<thinQuad>(*it) ) {
    (*it)->setStrength( arg_x/counter );
   }
  }
  
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

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_sbend::setStrength( double const& s )
{

  double ratio = (strength_ != 0.0) ? s/strength_ : 0.0;
 
  CFSbendPtr q;

  for ( beamline::iterator it  = bml_->begin(); 
                           it != bml_->end(); ++it ) {
    
    if ( ratio != 0.0  ) {
         (*it)->setStrength( ratio * (*it)->Strength() );
    } 
    else {
      if ( q = boost::dynamic_pointer_cast<CF_sbend>(*it) ) q->setStrength(s);
    }
   
  }


  strength_ = s; 

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_sbend::getOctupole() const
{

  // Returns the **integrated** octupole

  double strength = 0.0;

  for ( beamline::const_iterator it  = bml_->begin(); 
                                 it != bml_->end(); ++it ) {
    if( boost::dynamic_pointer_cast<thinOctupole>(*it) )  {
      strength += (*it)->Strength();
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

  for ( beamline::const_iterator it = bml_->begin(); 
                                 it != bml_->end(); ++it ) {
    if( boost::dynamic_pointer_cast<thinSextupole>(*it) ) {
      strength += (*it)->Strength();
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

  for ( beamline::const_iterator it  = bml_->begin(); 
                                 it != bml_->end(); ++it ) {
    if( boost::dynamic_pointer_cast<thinQuad>(*it) ) {
       strength += (*it)->Strength();
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
    (*pcerr) << "\n*** WARNING ***                             "
            "\n*** WARNING *** void CF_sbend::Split        "
            "\n*** WARNING *** Combined split elements     "
            "\n*** WARNING *** are not identical to the    "
            "\n*** WARNING *** original.                   "
            "\n*** WARNING ***                           \n"
         << endl;
  }

  if( ( pc <= 0.0 ) || ( pc >= 1.0 ) ) {
    ostringstream uic;
    uic << "Requested percentage = " << pc << "; should be in [0,1].";
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "void CF_sbend::Split( double const& pc, bmlnElmnt** a, bmlnElmnt** b )", 
           uic.str().c_str() ) );
  }


  // We assume "strength_" means field, not field*length_.
  // "length_," "strength_," and "angle_" are private data members.

  CF_sbend* p_a = 0;
  CF_sbend* p_b = 0;

  a = CFSbendPtr( p_a = new CF_sbend("",         pc*length_, strength_,         pc*angle_,  usFaceAngle_, 0.0 ) );

  p_a->setEntryAngle( getEntryAngle() );
  p_a->setExitAngle( 0.0 );    // Will matter

  b =  CFSbendPtr( p_b = new CF_sbend("", (1.0 - pc)*length_, strength_, (1.0 - pc)*angle_, 0.0, dsFaceAngle_ ) );

  p_b->setEntryAngle( 0.0 );   // Will matter
  p_b->setExitAngle( getExitAngle() );


  // Assign quadrupole strength
  double quadStrength = getQuadrupole();  // quadStrength = B'l

  p_a->setQuadrupole( pc*quadStrength );
  p_b->setQuadrupole( (1.0 - pc)*quadStrength );


  // Rename

  p_a->rename( ident_ + string("_1") );
  p_b->rename( ident_ + string("_2") );

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
