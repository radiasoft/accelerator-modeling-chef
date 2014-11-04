/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      combinedFunction.cc
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
****** - Use stack-based local Particle/jetparticle
****** 
****** Dec 2007           ostiguy@fnal.gov
******  - new typesafe propagator architecture
****** 
****** Apr 2008           michelotti@fnal.gov
****** - modified signature of setLength method to conform
******   to the virtual method in base class bmlnElmnt.
****** - wrote placeholder implementation for combinedFunction::setLength
******   : evidently, one had never been written
****** - added placeholder setStrength method.
****** 
**************************************************************************
*************************************************************************/



#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/GenericException.h>
#include <beamline/beamline.h>
#include <beamline/combinedFunction.h>
#include <beamline/JetParticle.h>
#include <beamline/Particle.h>
#include <beamline/sbend.h>
#include <beamline/rbend.h>
#include <beamline/quadrupole.h>
#include <beamline/sextupole.h>
#include <beamline/octupole.h>
#include <beamline/decapole.h>
#include <beamline/thinpoles.h>
#include <beamline/BmlVisitor.h>
#include <beamline/marker.h>

using namespace std;

using FNAL::pcout;
using FNAL::pcerr;

// **************************************************
//   class combinedFunction
// **************************************************

combinedFunction::combinedFunction()
: bmlnElmnt()
{
  bml_ = BmlPtr( new beamline );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

combinedFunction::combinedFunction( const char* n ) : bmlnElmnt(n) 
{
  bml_ =  BmlPtr( new beamline );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

combinedFunction::combinedFunction( const char* n, beamline const& b ) 
: bmlnElmnt(n) 
{
  bml_ = BmlPtr( b.Clone() );
  length_ = b.Length();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

combinedFunction::combinedFunction( beamline const& b ) 
: bmlnElmnt(b.Name().c_str())
{
  bml_ =  BmlPtr( b.Clone() );
  length_ = b.Length();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

combinedFunction::combinedFunction( combinedFunction const& x ) 
: bmlnElmnt( x )
{
  bml_ =  BmlPtr(  x.bml_->Clone() );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

combinedFunction::~combinedFunction() 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void combinedFunction::Split( double const&, ElmPtr& a, ElmPtr& b ) const
{
  (*pcerr) <<   "*** WARNING ****: "
              "\n*** WARNING ****: "  << __FILE__ << "," << __LINE__
           << "\n*** WARNING ****: void " << Type() << "::Split( double const&, ElmPtr&, ElmPtr& ) const"
              "\n*** WARNING ****: Splitting a " << Type() << " is forbidden in this version."
              "\n*** WARNING ****: " 
           << std::endl;
  ostringstream uic;
  uic  <<   "Splitting a " << Type() << " is forbidden in this version.";
  throw( GenericException( __FILE__, __LINE__, 
         "void combinedFunction::Split( double const&, ElmPtr& a, ElmPtr& b ) const", 
         uic.str().c_str() ) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//combinedFunction& combinedFunction::operator=(const combinedFunction& x) {
//  bmlnElmnt::operator=(x);	// This operator needs to be fixed.
//  bml_ = (beamline*)x.bml_->Clone();
//}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void combinedFunction::append(bmlnElmnt& x) {
  bml_->append( ElmPtr( x.Clone() ));
  length_ += x.Length();
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void combinedFunction::setField(WHICH_MULTIPOLE mult, double field) {

  ElmPtr element;
 
  double newstrength;
  std::list<ElmPtr> foundElements;

  for (beamline::deep_iterator it  = bml_->deep_begin();
                               it !=  bml_->deep_end(); ++it ) {
    element = (*it); 

    if ( hasMultipole( element, mult) ) { foundElements.push_back( element ) ;}

  }

  if( foundElements.size() > 0 ) {
    newstrength = field/( (double) foundElements.size() );

     for (std::list<ElmPtr>::iterator it = foundElements.begin();  it != foundElements.end(); ++it ) {
         if(  typeid( *(*it).get() ) == typeid(combinedFunction) ) {
                static_cast<combinedFunction*>( (*it).get() )->setField( mult, newstrength );
         }
         else {
           element->setStrength( newstrength );
         }
     }// for
   } // if
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void combinedFunction::setField(  boost::function<bool(bmlnElmnt const&)> crit, double s )
{

  std::list<ElmPtr>  foundElements;

  for (beamline::deep_iterator it= bml_->deep_begin();
                               it !=  bml_->deep_end(); ++it ) {
    if( crit(**it) ) foundElements.push_back( *it );
  }


  if( foundElements.size()  == 0 )  return; 

  double newstrength = 0.0;
  newstrength = s/( (double) foundElements.size() );
 
  for (std::list<ElmPtr>::iterator it = foundElements.begin();  it != foundElements.end(); ++it)  {

       if( typeid( **it ) == typeid(combinedFunction) ) {
          boost::static_pointer_cast<combinedFunction>(*it)->setField( crit, newstrength );
       }
       else {
        (*it)->setStrength( newstrength );
       }
  
 } // for
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double combinedFunction::Field(WHICH_MULTIPOLE mult) 
{

  ElmPtr element;
  double multStrength = 0.0;

  for (beamline::deep_iterator it= bml_->deep_begin();
                               it !=  bml_->deep_end(); ++it ) 
  {
    element = (*it);

    if ( hasMultipole( element, mult) ) {
	multStrength += element->Strength();
    }
  }
  return multStrength;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void combinedFunction::setStrength( double const& )
{
  (*pcerr) <<   "*** WARNING ****: "
              "\n*** WARNING ****: "  << __FILE__ << "," << __LINE__
           << "\n*** WARNING ****: void combinedFunction::setStrength( double const& )" 
              "\n*** WARNING ****: Attempt to explicitly set the strength of a combinedFunction. "
              "\n*** WARNING ****: This is most likely an error.  Will not comply."
              "\n*** WARNING ****: Continuing, nonetheless... "
              "\n*** WARNING ****: " 
           << std::endl;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void combinedFunction::setLength( double const& )
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

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void combinedFunction::setSkew(WHICH_MULTIPOLE mult, alignmentData& alignD) 
{

  ElmPtr element;

  for (beamline::deep_iterator it= bml_->deep_begin();
                               it !=  bml_->deep_end(); ++it ) 
  {
    element = (*it); 

    if ( hasMultipole( element, mult) ) {
	element->setAlignment(alignD);
    }
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

alignmentData combinedFunction::Skew(WHICH_MULTIPOLE mult) 
{

  ElmPtr element;
  alignmentData alignD;
  bool foundIt = false;

  for (beamline::deep_iterator it  = bml_->deep_begin();
                               it !=  bml_->deep_end(); ++it ) {
 
    if ( foundIt ) break;

    if ( hasMultipole( element, mult) ) {
	alignD = element->Alignment();
	foundIt = true;
    } 

  }
  return alignD;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& combinedFunction::writeTo(ostream& os) {
  os << *bml_;
  os << "combinedFunction_END " << ident_ << " 0 0 0 0 0\n";
  return os;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

istream& combinedFunction::readFrom(istream& is)
{
  bml_ = BmlPtr( new beamline() );

  is >> *bml_;

  length_ = bml_->Length();

  bmlnElmnt* e = read_istream(is);  /// !!!! FIX ME !!!! 

  if ( e ) {
    (*pcerr) << " **** WARNING **** Expecting an end of combinedFunction but got another bmlnElmnt\n";
    (*pcerr) << " **** WARNING **** Will attempt to proceed, but all bets are off!\n";
    delete e;
  }
  return is;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* combinedFunction::Type() const 
{ 
  return "combinedFunction"; 
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool combinedFunction::isMagnet() const
{
  return true;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double combinedFunction::AdjustPosition( const Particle& arg_p )
{
  JetParticle* myJPPtr = new JetParticle(arg_p);
  // This probably won't work properly.
  double ret = AdjustPosition( *myJPPtr );
  delete myJPPtr;
  return ret;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double combinedFunction::AdjustPosition( JetParticle const& arg_jp )
{
  enum { x = 0, y, cdt, xp, yp, dpop };

  JetParticle jetparticle(arg_jp);
  Particle    particle(jetparticle);

  Vector& state   = particle.State();
  Vector  inState = particle.State();

  double x_i  = inState[x];
  double xp_i = inState[xp];

  jetparticle.setState( inState );
  particle.State() =  inState;

  propagate( jetparticle );

  double f, m, z;

  m = ( jetparticle.State().Jacobian() )( x, x );
  m -= 1.0;
  if( std::abs(m) < 1.0e-12 ) {
    throw( GenericException( __FILE__, __LINE__, 
           "double combinedFunction::AdjustPosition( JetParticle const& arg_jp )", 
           "Horrible, inexplicable error: a multi-valued solution is suspected." ) );
  }
  m = 1.0 / m;

  z = x_i;
  for( int i = 0; i < 75; i++ ) {
    inState[x] = z;
    particle.State() = inState;
    propagate( particle );
    f = state[x] - z;
    z -= m*f;
  }


  // Set the alignment of the internal beamline.
  // align->getAlignment().xOffset -= z;
  alignmentData v;	// Assumes zero alignment constructed this way.
  if ( align_ )
    v = align_->getAlignment();
  v.xOffset -= z;
  // ??? Does not work: bml_->setAlignment( v );
  // ??? The reason is that the alignment stategy is
  // ??? not correct for elements whose faces are not
  // ??? parallel.
  setAlignment( v );
  // ??? This will work only if the in and out faces
  // ??? of the combinedFunction element are parallel.

  return z;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool combinedFunction::hasMultipole( ElmPtr elm, WHICH_MULTIPOLE mult )  
{
  bool ret = false;

  switch (mult) {

    case DIPOLE_FIELD:
      if( (typeid(*elm) == typeid(sbend)) ||
          (typeid(*elm) == typeid(rbend))    ) {
 	      ret = true;
      }
      break;
    case QUADRUPOLE_FIELD:
      if(     (typeid(*elm) == typeid(quadrupole)  ) 
	      || (typeid(*elm) == typeid(thinQuad) ) ) {
	ret = true;
      }
      break;
    case SEXTUPOLE_FIELD:
      if(   (typeid(*elm) == typeid(sextupole)     )
	 || (typeid(*elm) == typeid(thinSextupole )) ) {
	ret = true;
      }
      break;
    case OCTUPOLE_FIELD:
      if( typeid(*elm) == typeid(thinOctupole) ) 
	ret = true;
      break;
    case DECAPOLE_FIELD:
      if( typeid(*elm) == typeid(thinDecapole) )
	ret = true;
      break;
    case TWELVEPOLE_FIELD:
      if( typeid(*elm) == typeid(thin12pole) )
	ret = true;
      break;
    case FOURTEENPOLE_FIELD:
      if( typeid(*elm) == typeid(thin14pole) )
	ret = true;
      break;
    case SIXTEENPOLE_FIELD:
      if( typeid(*elm) == typeid(thin16pole) )
	ret = true;
      break;
    case EIGHTEENPOLE_FIELD:
      if( typeid(*elm) == typeid(thin18pole) )
	 ret = true;
      break;
    default:
      break;
    } //switch

  return ret;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void combinedFunction::accept( BmlVisitor& v )            
{ 
  v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void combinedFunction::accept( ConstBmlVisitor& v ) const 
{ 
 v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void combinedFunction::usePropagator( PropagatorPtr& x )
{
  propagator_ = PropagatorPtr( x->Clone() );
  propagator_->setup( *this );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void combinedFunction::localPropagate( Particle& p ) 
{ 
  (*propagator_)( *this,p ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void combinedFunction::localPropagate( JetParticle& p ) 
{ 
  (*propagator_)( *this,p ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
void combinedFunction::localPropagate( ParticleBunch& b ) 
{ 
  (*propagator_)( *this,b ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void combinedFunction::localPropagate( JetParticleBunch& b ) 
{ 
  (*propagator_)( *this,b ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


