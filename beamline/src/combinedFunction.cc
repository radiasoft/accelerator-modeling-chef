/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      combinedFunction.cc
******  Version:   2.1
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
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <basic_toolkit/iosetup.h>
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

using namespace std;

using FNAL::pcout;
using FNAL::pcerr;

// **************************************************
//   class combinedFunction
// **************************************************

combinedFunction::combinedFunction()
: bmlnElmnt()
{
  p_bml_ = BmlPtr( new beamline );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

combinedFunction::combinedFunction( const char* n ) : bmlnElmnt(n) 
{
  p_bml_ =  BmlPtr( new beamline );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

combinedFunction::combinedFunction( const char* n, beamline const& b ) 
: bmlnElmnt(n) 
{
  p_bml_ = BmlPtr( b.Clone() );
  length_ = b.Length();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

combinedFunction::combinedFunction( beamline const& b ) 
: bmlnElmnt(b.Name().c_str())
{
  p_bml_ =  BmlPtr( b.Clone() );
  length_ = b.Length();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

combinedFunction::~combinedFunction() 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

combinedFunction::combinedFunction( combinedFunction const& x ) 
: bmlnElmnt( x )
{
  p_bml_ =  BmlPtr(  x.p_bml_->Clone() );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//combinedFunction& combinedFunction::operator=(const combinedFunction& x) {
//  bmlnElmnt::operator=(x);	// This operator needs to be fixed.
//  p_bml_ = (beamline*)x.p_bml_->Clone();
//}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void combinedFunction::append(bmlnElmnt& x) {
  p_bml_->append( ElmPtr( x.Clone() ));
  length_ += x.Length();
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void combinedFunction::setField( bmlnElmnt::CRITFUNC crit, double s )
{

  std::list<ElmPtr>  foundElements;

  for (beamline::deep_iterator it= p_bml_->deep_begin();
                               it !=  p_bml_->deep_end(); ++it ) {
    if( crit( (*it).get() ) ) foundElements.push_back( *it );
  }

  double newstrength = 0.0;
  if( foundElements.size() > 0 ) {
    newstrength = s/( (double) foundElements.size() );
 

    bmlnElmnt* element;
  
    for (std::list<ElmPtr>::iterator it = foundElements.begin();  it != foundElements.end(); ++it)  {

      if( typeid( *(*it).get()) == typeid(combinedFunction) ) {
         static_cast<combinedFunction*>((*it).get() )->setField( crit, newstrength );
      }
      else {
        (*it)->setStrength( newstrength );
      }
    }
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void combinedFunction::setField(WHICH_MULTIPOLE mult, double field) {

  ElmPtr element;
 
  double newstrength;
  std::list<ElmPtr> foundElements;

  for (beamline::deep_iterator it  = p_bml_->deep_begin();
                               it !=  p_bml_->deep_end(); ++it ) {
    element = *it; 

    switch (mult) {

    case DIPOLE_FIELD:
      if( (typeid(*element) == typeid(sbend)) ||
          (typeid(*element) == typeid(rbend))    ) {
 	       foundElements.push_back( element );
      }
      break;
    case QUADRUPOLE_FIELD:
      if(     (typeid(*element) == typeid(quadrupole)  ) 
	      || (typeid(*element) == typeid(thinQuad) ) ) {
	foundElements.push_back( element );
      }
      break;
    case SEXTUPOLE_FIELD:
      if(   (typeid(*element) == typeid(sextupole)     )
	 || (typeid(*element) == typeid(thinSextupole )) ) {
	foundElements.push_back( element );
      }
      break;
    case OCTUPOLE_FIELD:
      if( typeid(*element) == typeid(thinOctupole) ) 
	foundElements.push_back( element );
      break;
    case DECAPOLE_FIELD:
      if( typeid(*element) == typeid(thinDecapole) )
	foundElements.push_back( element );
      break;
    case TWELVEPOLE_FIELD:
      if( typeid(*element) == typeid(thin12pole) )
	foundElements.push_back( element );
      break;
    case FOURTEENPOLE_FIELD:
      if( typeid(*element) == typeid(thin14pole) )
	foundElements.push_back( element );
      break;
    case SIXTEENPOLE_FIELD:
      if( typeid(*element) == typeid(thin16pole) )
	foundElements.push_back( element );
      break;
    case EIGHTEENPOLE_FIELD:
      if( typeid(*element) == typeid(thin18pole) )
	foundElements.push_back( element );
      break;
    default:
      break;
    } //switch
  }// for


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

double combinedFunction::Field(WHICH_MULTIPOLE mult) {

  bmlnElmnt* element;
  double multStrength = 0.0;

  for (beamline::deep_iterator it= p_bml_->deep_begin();
                               it !=  p_bml_->deep_end(); ++it ) 
  {
    element = (*it).get();

    switch (mult) {

    case DIPOLE_FIELD:
      if(  (typeid(*element) == typeid(sbend) ) 
	 ||(typeid(*element) == typeid(rbend) ) ) {
	multStrength += element->Strength();
      }
      break;
    case QUADRUPOLE_FIELD:
      if(   (typeid(*element) == typeid(quadrupole) )
         || (typeid(*element) == typeid(thinQuad)   )  ){
	multStrength += element->Strength();
      }
      break;
    case SEXTUPOLE_FIELD:
      if(     ( typeid(*element) == typeid(sextupole)     )
	   || ( typeid(*element) == typeid(thinSextupole) ) ) {
	multStrength += element->Strength();
     }
      break;
    case OCTUPOLE_FIELD:
      if(typeid(*element) == typeid(thinOctupole) ) {
	multStrength += element->Strength();
      }
      break;
    case DECAPOLE_FIELD:
      if(typeid(*element) == typeid(thinDecapole) ) {
	multStrength += element->Strength();
      }
      break;
    case TWELVEPOLE_FIELD:
      if(typeid(*element) == typeid(thin12pole) ) {
	multStrength += element->Strength();
      }
      break;
    case FOURTEENPOLE_FIELD:
      if(typeid(*element) == typeid(thin14pole) ) {
	multStrength += element->Strength();
      }
      break;
    case SIXTEENPOLE_FIELD:
      if(typeid(*element) == typeid(thin16pole) ) {
	multStrength += element->Strength();
      }
      break;
    case EIGHTEENPOLE_FIELD:
      if( typeid(*element) == typeid(thin18pole) ) {
	multStrength += element->Strength();
      }
      break;
    default:
      break;
    } // switch
  } //for

  return multStrength;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void combinedFunction::setSkew(WHICH_MULTIPOLE mult, alignmentData& alignD) {

  bmlnElmnt* element;

  for (beamline::deep_iterator it= p_bml_->deep_begin();
                               it !=  p_bml_->deep_end(); ++it ) 
  {
    element = (*it).get(); 

    switch (mult) {
    case DIPOLE_FIELD:
      if(  (typeid(*element) == typeid(sbend) )
        || (typeid(*element) == typeid(rbend) ) ) {
	element->setAlignment(alignD);
      }
      break;
    case QUADRUPOLE_FIELD:
      if(   (typeid(*element) == typeid(quadrupole) )
         || (typeid(*element) == typeid(thinQuad)   ) ) {
	element->setAlignment(alignD);
      }
      break;
    case SEXTUPOLE_FIELD:
      if(    (typeid(*element) == typeid(sextupole)    ) 
          || (typeid(*element) == typeid(thinSextupole)) ) {
	element->setAlignment(alignD);
      }
      break;
    case OCTUPOLE_FIELD:
      if( typeid(*element) == typeid(thinOctupole))
	element->setAlignment(alignD);
      break;
    case DECAPOLE_FIELD:
      if( typeid(*element) == typeid(thinDecapole))
	element->setAlignment(alignD);
      break;
    case TWELVEPOLE_FIELD:
      if( typeid(*element) == typeid(thin12pole) ){
	element->setAlignment(alignD);
      }
      break;
    case FOURTEENPOLE_FIELD:
      if(typeid(*element) == typeid(thin14pole) ){
	element->setAlignment(alignD);
      }
      break;
    case SIXTEENPOLE_FIELD:
      if(typeid(*element) == typeid(thin16pole) ){
	element->setAlignment(alignD);
      }
      break;
    case EIGHTEENPOLE_FIELD:
      if(typeid(*element) == typeid(thin18pole) ){
	element->setAlignment(alignD);
      }
      break;
    default:
      break;
    }
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

alignmentData combinedFunction::Skew(WHICH_MULTIPOLE mult) {

  bmlnElmnt* element;
  alignmentData alignD;
  bool foundIt = false;

  for (beamline::deep_iterator it  = p_bml_->deep_begin();
                               it !=  p_bml_->deep_end(); ++it ) {
 
    if ( foundIt ) break;

    switch (mult) {
    case DIPOLE_FIELD:
      if(  (typeid(*element) == typeid(sbend) )
        || (typeid(*element) == typeid(rbend) ) ) {
	alignD = element->Alignment();
	foundIt = true;
      }
      break;
    case QUADRUPOLE_FIELD:
      if(  ( typeid(*element) == typeid(quadrupole)     ) 
        || ( typeid(*element) == typeid(thinQuad) ) ) {
	alignD = element->Alignment();
	foundIt = true;
      }
      break;
    case SEXTUPOLE_FIELD:
      if(   ( typeid(*element) == typeid(sextupole)       ) 
         || ( typeid(*element) == typeid(thinSextupole)   ) ) {
	alignD = element->Alignment();
	foundIt = true;
      }
      break;
    case OCTUPOLE_FIELD:
      if(typeid(*element) == typeid(thinOctupole)) {
	alignD = element->Alignment();
	foundIt = true;
      }
      break;
    case DECAPOLE_FIELD:
      if(typeid(*element) == typeid(thinDecapole)) {
	alignD = element->Alignment();
	foundIt = true;
      }
      break;
    case TWELVEPOLE_FIELD:
      if(typeid(*element) == typeid(thin12pole)) {
	alignD = element->Alignment();
	foundIt = true;
      }
      break;
    case FOURTEENPOLE_FIELD:
      if(typeid(*element) == typeid(thin14pole)) {
	alignD = element->Alignment();
	foundIt = true;
      }
      break;
    case SIXTEENPOLE_FIELD:
      if(typeid(*element) == typeid(thin16pole)) {
	alignD = element->Alignment();
	foundIt = true;
      }
      break;
    case EIGHTEENPOLE_FIELD:
      if(typeid(*element) == typeid(thin18pole)) {
	alignD = element->Alignment();
	foundIt = true;
      }
      break;
    default:
      break;
    }
  }
  return alignD;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& combinedFunction::writeTo(ostream& os) {
  os << *p_bml_;
  os << "combinedFunction_END " << ident_ << " 0 0 0 0 0\n";
  return os;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

istream& combinedFunction::readFrom(istream& is)
{
  p_bml_ = BmlPtr( new beamline() );

  is >> *p_bml_;

  length_ = p_bml_->Length();

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

double combinedFunction::AdjustPosition( const JetParticle& arg_jp )
{
  enum { x = 0, y, cdt, xp, yp, dpop };

  JetParticle jetparticle(arg_jp);
  Particle    particle(jetparticle);

  Vector inState;
  double x_i  = inState[x]  = particle.State( x  );
  double xp_i = inState[xp] = particle.State( xp );

  jetparticle.setState( inState );
  particle.setState( inState );

  propagate( jetparticle );

  double f, m, z;

  m = ( jetparticle.State().Jacobian() )( x, x );
  m -= 1.0;
  if( std::abs(m) < 1.0e-12 ) {
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "double combinedFunction::AdjustPosition( JetParticle const& arg_jp )", 
           "Horrible, inexplicable error: a multi-valued solution is suspected." ) );
  }
  m = 1.0 / m;

  z = x_i;
  int i;
  for( i = 0; i < 75; i++ ) {
    inState[x] = z;
    particle.setState( inState );
    propagate( particle );
    f = ( particle.State() )( x ) - z;
    z -= m*f;
  }


  // Set the alignment of the internal beamline.
  // align->getAlignment().xOffset -= z;
  alignmentData v;	// Assumes zero alignment constructed this way.
  if ( align_ )
    v = align_->getAlignment();
  v.xOffset -= z;
  // ??? Does not work: p_bml_->setAlignment( v );
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

