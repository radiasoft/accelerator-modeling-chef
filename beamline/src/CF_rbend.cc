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
****** Aug 2007           ostiguy@fnal.gov
****** - composite structure based on regular beamline
****** Dec                ostiguy@fnal.gov
****** - new typesafe propagator architecture
****** Apr 2008           michelotti@fnal.gov
****** - modified setStrength method
****** - added placeholder setLength method
****** - changed interpretation of "ang" argument to
******   two constructors from "entry angle" to "bend angle,"
******   in order to conform with usage in other bend constructors.
****** - corrected rbend::Split
******   : including adding methods to nullify edge effects
****** May 2008           ostiguy@fnal.gov
****** - setStrength() now dispatched to propagator by base class
******   (no longer virtual)
****** - added explicit implementation for assignment operator
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
    multipoles_(4, std::complex<double>(0.0,0.0) )
{
  propagator_ = PropagatorPtr( new Propagator(1) ); 
  propagator_->setup(*this);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_rbend::CF_rbend( std::string const&  name,
                    double const& length, double const& field )  
  : bmlnElmnt( name, length, field ),
    usFaceAngle_(0.0),
    dsFaceAngle_(0.0),
    usAngle_(M_TWOPI),
    dsAngle_(-M_TWOPI),
    multipoles_(4, std::complex<double>(0.0,0.0) )
{
  propagator_ = PropagatorPtr( new Propagator(1) ); // number of blocks: 4n+1 bends + 2(4n) multipoles  
  propagator_->setup(*this);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_rbend::CF_rbend( std::string const& name,
                    double const& length,
                    double const& field,
                    double const& angle )
  : bmlnElmnt( name, length, field ),
   usFaceAngle_(0.0),
   dsFaceAngle_(0.0),
       usAngle_(angle/2.0),
       dsAngle_(-angle/2.0),
    multipoles_(4, std::complex<double>(0.0,0.0) )
{
  propagator_ = PropagatorPtr( new Propagator(1) ); // number of blocks: 4n+1 bends + 2(4n) multipoles  
  propagator_->setup(*this);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


CF_rbend::CF_rbend( std::string const& name,
                    double const& length,
                    double const& field,
                    double const& us,   // upstream face angle [radians]
                    double const& ds )
  : bmlnElmnt( name, length, field ),
   usFaceAngle_(us),
   dsFaceAngle_(ds),
       usAngle_(M_TWOPI),
       dsAngle_(-M_TWOPI),
    multipoles_(4, std::complex<double>(0.0,0.0) )
{
  propagator_ = PropagatorPtr( new Propagator(1) ); // number of blocks: 4n+1 bends + 2(4n) multipoles  
  propagator_->setup(*this);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_rbend::CF_rbend( std::string const&   name,
                    double const&  length,  //   [meter]
                    double const&  field,   //   [tesla]
                    double const&  angle,   //   bend angle  [radians]
                    double const&  us,   // upstream edge angle [radians]
                    double const&  ds )  // downstream edge angle [radians] )
  : bmlnElmnt( name, length, field ),
    angle_(angle),
    usFaceAngle_(us),
    dsFaceAngle_(ds),
    usAngle_((angle/2.0) + us),
    dsAngle_(-((angle/2.0) + ds)),
    multipoles_(4, std::complex<double>(0.0,0.0) )
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
      multipoles_(x.multipoles_)
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
 
  bml_       =  BmlPtr( rhs.bml_->Clone() );  

  return *this;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_rbend::propagateReference(Particle& p, double initialBRho, bool scaling)
{
  setEntryAngle(p);
  bmlnElmnt::propagateReference(p,initialBRho, scaling );
  setExitAngle(p);
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

int CF_rbend::setDipoleField( double const& value )
{
 
  setStrength( value );
 
  for ( beamline::iterator it  = bml_->begin(); 
	                   it != bml_->end(); ++it ) {
    if( boost::dynamic_pointer_cast<rbend>(*it) ) { 
     (*it)->setStrength( value );
   }
  }

  return 0;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_rbend::setQuadrupole( double const& value)
{
  multipoles_[1] = value;
  propagator_->setup(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_rbend::setSextupole( double const& value)
{
  multipoles_[2] = value;
  propagator_->setup(*this);
 
}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_rbend::setOctupole( double const& value )
{
  multipoles_[3] = value;
  propagator_->setup(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


double CF_rbend::getDipoleField() const
{
  return Strength();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_rbend::getQuadrupole() const
{
  return multipoles_[1].real();
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_rbend::getSextupole() const
{
  return multipoles_[2].real();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


double CF_rbend::getOctupole() const
{
  return multipoles_[3].real();
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

std::pair<ElmPtr,ElmPtr> CF_rbend::split( double const& pc ) const
{
  // Preliminary tests ...
  // -----------------------------
  if( ( pc <= 0.0 ) || ( pc >= 1.0 ) ) {
    ostringstream uic;
    uic << "Requested percentage = " << pc << "; should be in [0,1].";
    throw( GenericException( __FILE__, __LINE__, 
           "CF_rbend::split( double const& pc )", 
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
           "CF_rbend::split( double const& pc ) const", 
           uic.str().c_str() ) );
  }

  static bool firstTime = true;
  if( firstTime ) {
    firstTime = false;
    (*pcerr) << "\n"
            "\n*** WARNING ***"
            "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** CF_rbend::Split( double const& ) const"
            "\n*** WARNING *** A new reference trajectory must be registered."
            "\n*** WARNING *** "
         << endl;
  }


  // We assume "strength_" means field, not field*length_.
  // "length_," "strength_," and "_angle" are private data members.
  // -----------------------------

  CFRbendPtr p_a(    new CF_rbend(   ""
                                   , pc*length_
			           , Strength()
                                   , usFaceAngle_
                                   , 0.0           ));
  p_a->setEntryAngle( getEntryAngle() );
  p_a->bml_->back()->setStrength(0.0);

  CFRbendPtr p_b(   new CF_rbend(   ""
                                   , (1.0 - pc)*length_
				   , Strength()
                                   , 0.0
                                   , dsFaceAngle_        ));
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
  p_a->setAlignment( ald );
  p_b->setAlignment( ald );

  // Rename
  // -----------------------------
  p_a->rename( ident_ + string("_1") );
  p_b->rename( ident_ + string("_2"));

  return make_pair(p_a,p_b);

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& CF_rbend::writeTo( ostream& os )
{
#if 0
  int n = bml_->countHowMany();

  if( n%12 ) {
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
#endif
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

bool CF_rbend::isThin() const
{
  return false;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool CF_rbend::isPassive() const
{
  return false;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool CF_rbend::isDriftSpace() const
{
  return false;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_rbend::OrbitLength( Particle const& p )
{
  double tworho = 2.0 * ( p.Momentum() / PH_CNV_brho_to_p ) / Strength();
  return tworho * asin( length_ / tworho );
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

  double const x_i  = state[x];
  double const xp_i = state[xp]; 

  // Initialize the derivative...

  jetparticle.setState( instate ); 

  propagate(jetparticle);

  double m = jetstate.jacobian()[xp][x];
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
 
    m =  (jetstate.jacobian())[xp][x];
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

