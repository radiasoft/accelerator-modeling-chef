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
******   to the virtual method in base class BmlnElmnt.
****** - wrote placeholder implementation for combinedFunction::setLength
******   : evidently, one had never been written
****** - added placeholder setStrength method.
****** May 2008           ostiguy@fnal.gov
****** - attribute changes now dispatched to propagator
****** - added explicit implementation for assigment operator.
******  
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

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
#include <typeinfo> 
using namespace std;

using FNAL::pcout;
using FNAL::pcerr;

// **************************************************
//   class combinedFunction
// **************************************************

combinedFunction::combinedFunction()
: BmlnElmnt()
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

combinedFunction::combinedFunction( std::string const& n ) 
 : BmlnElmnt(n) 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

combinedFunction::combinedFunction( combinedFunction const& x ) 
: BmlnElmnt( x )
{
  // FIXME ! the state of internal multipoles needs to be preserved
  //         as private element data 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

combinedFunction::~combinedFunction() 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::pair<ElmPtr,ElmPtr> combinedFunction::split( double const& pc) const
{
  (*pcerr) <<   "*** WARNING ****: "
              "\n*** WARNING ****: "  << __FILE__ << "," << __LINE__
           << "\n*** WARNING ****: void " << Type() << "::split( double const& ) const"
              "\n*** WARNING ****: Splitting a " << Type() << " is forbidden in this version."
              "\n*** WARNING ****: " 
           << std::endl;
  ostringstream uic;
  uic  <<   "Splitting a " << Type() << " is forbidden in this version.";
  throw(  GenericException( __FILE__, __LINE__, 
         "combinedFunction::split( double const& ) const", 
         uic.str().c_str() ) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//combinedFunction& combinedFunction::operator=(const combinedFunction& x) {
//  BmlnElmnt::operator=(x);	// This operator needs to be fixed.
//  bml_ = (beamline*)x.bml_->clone();
//}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void combinedFunction::append(BmlnElmnt& x) {
#if 0
  bml_->append( ElmPtr( x.clone() ));
  length_ += x.Length();
#endif
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void combinedFunction::setField(WHICH_MULTIPOLE mult, double field) {

#if 0
  ElmPtr element;
 
  double newstrength;
  std::list<ElmPtr> foundElements;

  for (beamline::deep_iterator it  = bml_->deep_begin();
                               it !=  bml_->deep_end(); ++it ) {

    if ( hasMultipole( **it, mult) ) { foundElements.push_back( element ) ;}

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
#endif

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void combinedFunction::setField(  boost::function<bool(BmlnElmnt const&)> crit, double s )
{

#if 0
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
#endif
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double combinedFunction::Field(WHICH_MULTIPOLE mult) 
{
  ElmPtr element;
  double multStrength = 0.0;

#if 0

  for (beamline::deep_iterator it= bml_->deep_begin();
                               it !=  bml_->deep_end(); ++it ) 
  {

    if ( hasMultipole( **it, mult) ) {
	multStrength += element->Strength();
    }
  }
#endif
  return multStrength;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void combinedFunction::setSkew(WHICH_MULTIPOLE mult, Alignment const& align) 
{

#if 0
  for (beamline::deep_iterator it= bml_->deep_begin();
                               it !=  bml_->deep_end(); ++it ) 
  {
    if ( hasMultipole( **it, mult) ) {
	(*it)->setAlignment(align);
    }
  }
#endif
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Alignment combinedFunction::Skew(WHICH_MULTIPOLE mult) 
{

#if 0
  for (beamline::deep_iterator it  = bml_->deep_begin();
                               it !=  bml_->deep_end(); ++it ) {
 
    if ( hasMultipole( (**it), mult) ) {
	return (*it)->alignment();
    } 

  }
#endif
  return Alignment();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& combinedFunction::writeTo(ostream& os) {
#if 0
  os << *bml_;
  os << "combinedFunction_END " << ident_ << " 0 0 0 0 0\n";
#endif
  return os;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

istream& combinedFunction::readFrom(istream& is)
{
#if 0
  bml_ = BmlPtr( new beamline() );

  is >> *bml_;

  length_ = bml_->Length();

  BmlnElmnt* e = read_istream(is);  /// !!!! FIX ME !!!! 

  if ( e ) {
    (*pcerr) << " **** WARNING **** Expecting an end of combinedFunction but got another BmlnElmnt\n";
    (*pcerr) << " **** WARNING **** Will attempt to proceed, but all bets are off!\n";
    delete e;
  }
#endif
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

bool combinedFunction::isThin() const
{
  return false;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool combinedFunction::isPassive() const
{
  return false;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool combinedFunction::isDriftSpace() const
{
  return false;
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

  Vector& state   = particle.state();
  Vector  inState = particle.state();

  double x_i  = inState[x];
  double xp_i = inState[xp];

  jetparticle.setState( inState );
  particle.state() =  inState;

  propagate( jetparticle );

  double m = ( jetparticle.state().jacobian() )[x][x];
  m -= 1.0;
  if( std::abs(m) < 1.0e-12 ) {
    throw( GenericException( __FILE__, __LINE__, 
           "double combinedFunction::AdjustPosition( JetParticle const& arg_jp )", 
           "Horrible, inexplicable error: a multi-valued solution is suspected." ) );
  }
  m = 1.0 / m;

  double z = x_i;
  for( int i = 0; i < 75; i++ ) {
    inState[x] = z;
    particle.state() = inState;
    propagate( particle );
    double f = state[x] - z;
    z -= m*f;
  }


  // Set the alignment of the internal beamline.
  // align->getAlignment().xOffset -= z;

  // ??? Does not work: bml_->setAlignment( v );
  // ??? The reason is that the alignment stategy is
  // ??? not correct for elements whose faces are not
  // ??? parallel.

  if ( align_ )  { 
     align_->setXOffset(align_->xOffset() - z ); }
  else {
    align_  = new Alignment(-z, 0.0, 0.0, 0.0 );
  }

  // ??? This will work only if the in and out faces
  // ??? of the combinedFunction element are parallel.

  return z;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool combinedFunction::hasMultipole( BmlnElmnt const &elm, WHICH_MULTIPOLE mult ) const 
{
  bool ret = false;

  std::type_info const& elmtid = typeid(elm);

  switch (mult) { 

    case DIPOLE_FIELD:
      if( (elmtid == typeid(sbend)) ||
          (elmtid == typeid(rbend))    ) {
 	      ret = true;
      }
      break;
    case QUADRUPOLE_FIELD:
      if(     (elmtid == typeid(quadrupole)  ) 
	      || (elmtid == typeid(thinQuad) ) ) {
	ret = true;
      }
      break;
    case SEXTUPOLE_FIELD:
      if(   (elmtid == typeid(sextupole)     )
	 || (elmtid == typeid(thinSextupole )) ) {
	ret = true;
      }
      break;
    case OCTUPOLE_FIELD:
      if( elmtid == typeid(thinOctupole) ) 
	ret = true;
      break;
    case DECAPOLE_FIELD:
      if( elmtid == typeid(thinDecapole) )
	ret = true;
      break;
    case TWELVEPOLE_FIELD:
      if( elmtid == typeid(thin12pole) )
	ret = true;
      break;
    case FOURTEENPOLE_FIELD:
      if( elmtid == typeid(thin14pole) )
	ret = true;
      break;
    case SIXTEENPOLE_FIELD:
      if( elmtid == typeid(thin16pole) )
	ret = true;
      break;
    case EIGHTEENPOLE_FIELD:
      if( elmtid == typeid(thin18pole) )
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

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


