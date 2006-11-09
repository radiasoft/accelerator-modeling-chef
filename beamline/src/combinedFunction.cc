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
******                                                                
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <basic_toolkit/iosetup.h>
#include <beamline/beamline.h>
#include <beamline/combinedFunction.h>
#include <beamline/Particle.h>
#include <beamline/sbend.h>
#include <beamline/rbend.h>
#include <beamline/quadrupole.h>
#include <beamline/sextupole.h>
#include <beamline/octupole.h>
#include <beamline/decapole.h>
#include <beamline/thinpoles.h>

using namespace std;

using FNAL::pcout;
using FNAL::pcerr;

// **************************************************
//   class combinedFunction
// **************************************************

combinedFunction::combinedFunction()
: bmlnElmnt()
{
  p_bml = new beamline;
}

combinedFunction::combinedFunction( const char* n ) : bmlnElmnt(n) 
{
  p_bml = new beamline;
}

combinedFunction::combinedFunction( const char* n, const beamline& b ) 
: bmlnElmnt(n) 
{
  p_bml = (beamline*) b.Clone();
  length = b.Length();
}

combinedFunction::combinedFunction( const beamline& b ) 
: bmlnElmnt(b.Name())
{
  p_bml = (beamline*) b.Clone();
  length = b.Length();
}

combinedFunction::~combinedFunction() {
  p_bml->zap();
}

combinedFunction::combinedFunction( const combinedFunction& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
  p_bml = (beamline*)x.p_bml->Clone();
}

//combinedFunction& combinedFunction::operator=(const combinedFunction& x) {
//  bmlnElmnt::operator=(x);	// This operator needs to be fixed.
//  p_bml = (beamline*)x.p_bml->Clone();
//}

void combinedFunction::append(bmlnElmnt& x) {
  p_bml->append(x.Clone());
  length += x.Length();
}


void combinedFunction::setField( bmlnElmnt::CRITFUNC crit, 
                                 double s )
{

  slist  foundElements;

  for (beamline::deep_iterator it= p_bml->deep_begin();
                               it !=  p_bml->deep_end(); ++it ) {
    if( crit( *it ) ) foundElements.append( *it );
  }

  double newstrength = 0.0;
  if( foundElements.size() > 0 ) {
    newstrength = s/( (double) foundElements.size() );
    slist_iterator getNext( foundElements );

    bmlnElmnt* element;
  
    while((  element = (bmlnElmnt*) getNext()  )) {
      if( typeid(*element) == typeid(combinedFunction) ) {
        ((combinedFunction*) element)->setField( crit, newstrength );
      }
      else {
        element->setStrength( newstrength );
      }
    }
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void combinedFunction::setField(WHICH_MULTIPOLE mult, double field) {

  bmlnElmnt* element;
 
  double newstrength;
  slist  foundElements;

  for (beamline::deep_iterator it= p_bml->deep_begin();
                               it !=  p_bml->deep_end(); ++it ) {
    element = *it; 

    switch (mult) {

    case DIPOLE_FIELD:
      if( (typeid(*element) == typeid(sbend)) ||
          (typeid(*element) == typeid(rbend))    ) {
 	       foundElements.append( element );
      }
      break;
    case QUADRUPOLE_FIELD:
      if(     (typeid(*element) == typeid(quadrupole)  ) 
	      || (typeid(*element) == typeid(thinQuad) ) ) {
	foundElements.append( element );
      }
      break;
    case SEXTUPOLE_FIELD:
      if(   (typeid(*element) == typeid(sextupole)     )
	 || (typeid(*element) == typeid(thinSextupole )) ) {
	foundElements.append( element );
      }
      break;
    case OCTUPOLE_FIELD:
      if( typeid(*element) == typeid(thinOctupole) ) 
	foundElements.append( element );
      break;
    case DECAPOLE_FIELD:
      if( typeid(*element) == typeid(thinDecapole) )
	foundElements.append( element );
      break;
    case TWELVEPOLE_FIELD:
      if( typeid(*element) == typeid(thin12pole) )
	foundElements.append( element );
      break;
    case FOURTEENPOLE_FIELD:
      if( typeid(*element) == typeid(thin14pole) )
	foundElements.append( element );
      break;
    case SIXTEENPOLE_FIELD:
      if( typeid(*element) == typeid(thin16pole) )
	foundElements.append( element );
      break;
    case EIGHTEENPOLE_FIELD:
      if( typeid(*element) == typeid(thin18pole) )
	foundElements.append( element );
      break;
    default:
      break;
    } //switch
  }// for


  if( foundElements.size() > 0 ) {
    newstrength = field/( (double) foundElements.size() );
    slist_iterator getNext( foundElements );

     while((  element = (bmlnElmnt*) getNext()  )) {
         if(  typeid(*element) == typeid(combinedFunction) ) {
                ((combinedFunction*) element)->setField( mult, newstrength );
         }
         else {
           element->setStrength( newstrength );
         }
     }// while
  } // if
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double combinedFunction::Field(WHICH_MULTIPOLE mult) {

  bmlnElmnt* element;
  double multStrength = 0.0;

  for (beamline::deep_iterator it= p_bml->deep_begin();
                               it !=  p_bml->deep_end(); ++it ) 
  {
    element = *it;

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

  for (beamline::deep_iterator it= p_bml->deep_begin();
                               it !=  p_bml->deep_end(); ++it ) 
  {
    element = *it; 

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

  for (beamline::deep_iterator it  = p_bml->deep_begin();
                               it !=  p_bml->deep_end(); ++it ) {
 
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
  os << *p_bml;
  os << "combinedFunction_END " << ident << " 0 0 0 0 0\n";
  return os;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

istream& combinedFunction::readFrom(istream& is)
{
  p_bml = new beamline();
  is >> *p_bml;
  length = p_bml->Length();
  bmlnElmnt *e = read_istream(is);
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
  JetParticle* myJPPtr = arg_p.ConvertToJetParticle();
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

  JetParticle* myJPPtr = arg_jp.Clone();
  Particle*      p_myP = myJPPtr->ConvertToParticle();
  // This is deleted before returning.

  double x_i  = p_myP->State( x  );
  double xp_i = p_myP->State( xp );

  double inState [] = { 0, 0, 0, 0, 0, 0 };
  inState[x]  = x_i;
  inState[xp] = xp_i;

  myJPPtr->setState( inState );
  p_myP->setState( inState );

  this->propagate( *myJPPtr );

  double f, m, z;

  m = ( myJPPtr->State().Jacobian() )( x, x );
  m -= 1.0;
  if( fabs(m) < 1.0e-12 ) {
    delete p_myP;   p_myP   = 0;
    delete myJPPtr; myJPPtr = 0;
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "double combinedFunction::AdjustPosition( const JetParticle& arg_jp )", 
           "Horrible, inexplicable error: a multi-valued solution is suspected." ) );
  }
  m = 1.0 / m;

  z = x_i;
  int i;
  for( i = 0; i < 75; i++ ) {
    inState[x] = z;
    p_myP->setState( inState );
    this->propagate( *p_myP );
    f = ( p_myP->State() )( x ) - z;
    z -= m*f;
  }


  // Set the alignment of the internal beamline.
  // this->align->getAlignment().xOffset -= z;
  alignmentData v;	// Assumes zero alignment constructed this way.
  if ( align != 0 )
    v = align->getAlignment();
  v.xOffset -= z;
  // ??? Does not work: p_bml->setAlignment( v );
  // ??? The reason is that the alignment stategy is
  // ??? not correct for elements whose faces are not
  // ??? parallel.
  this->setAlignment( v );
  // ??? This will work only if the in and out faces
  // ??? of the combinedFunction element are parallel.


  // Clean up and return.
  delete p_myP;   p_myP   = 0;
  delete myJPPtr; myJPPtr = 0;

  return z;
}
