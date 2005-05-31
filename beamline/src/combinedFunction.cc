/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      combinedFunction.cc
******                                                                
******  Copyright (c) 1991 Universities Research Association, Inc.    
******                All Rights Reserved                             
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
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iosetup.h>
#include <combinedFunction.h>
#include <BeamlineIterator.h>
#include <Particle.h>

using namespace std;

using FNAL::pcout;
using FNAL::pcerr;

// **************************************************
//   class combinedFunction
// **************************************************

combinedFunction::combinedFunction()
: bmlnElmnt()
{
  throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
         "combinedFunction::combinedFunction()", 
         "Proper instatiation requires parameters." ) );
}

combinedFunction::combinedFunction( const char* n ) : bmlnElmnt(n) 
{
  p_bml = new beamline();
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
  DeepBeamlineIterator dbi( p_bml );
  bmlnElmnt* element;
  double newstrength;
  slist  foundElements;

  while((  element = dbi++  )) {
    if( crit( element ) ) foundElements.append( element );
  }

  if( foundElements.size() > 0 ) {
    newstrength = s/( (double) foundElements.size() );
    slist_iterator getNext( foundElements );
    while((  element = (bmlnElmnt*) getNext()  )) {
      if( 0 == strcmp( "combinedFunction", element->Type() ) ) {
        ((combinedFunction*) element)->setField( crit, newstrength );
      }
      else {
        element->setStrength( newstrength );
      }
    }
  }
}


void combinedFunction::setField(WHICH_MULTIPOLE mult, double field) {
  DeepBeamlineIterator dbi( p_bml );
  bmlnElmnt* element;
  double newstrength;
  slist  foundElements;

  while((  element = dbi++  )) 
  {
    switch (mult) {
    case DIPOLE_FIELD:
      if(strcasecmp(element->Type(),"sbend") == 0 ||
	 strcasecmp(element->Type(),"rbend") == 0 )
	foundElements.append( element );
      break;
    case QUADRUPOLE_FIELD:
      if(strcasecmp(element->Type(),"quadrupole") == 0 ||
	 strcasecmp(element->Type(),"thinQuad") == 0 )
	foundElements.append( element );
      break;
    case SEXTUPOLE_FIELD:
      if(strcasecmp(element->Type(),"sextupole") == 0 ||
	 strcasecmp(element->Type(),"thinSextupole") == 0 )
	foundElements.append( element );
      break;
    case OCTUPOLE_FIELD:
      if(strcasecmp(element->Type(),"thinOctupole") == 0 )
	foundElements.append( element );
      break;
    case DECAPOLE_FIELD:
      if(strcasecmp(element->Type(),"thinDecapole") == 0 )
	foundElements.append( element );
      break;
    case TWELVEPOLE_FIELD:
      if(strcasecmp(element->Type(),"thin12pole") == 0 )
	foundElements.append( element );
      break;
    case FOURTEENPOLE_FIELD:
      if(strcasecmp(element->Type(),"thin14pole") == 0 )
	foundElements.append( element );
      break;
    case SIXTEENPOLE_FIELD:
      if(strcasecmp(element->Type(),"thin16pole") == 0 )
	foundElements.append( element );
      break;
    case EIGHTEENPOLE_FIELD:
      if(strcasecmp(element->Type(),"thin18pole") == 0 )
	foundElements.append( element );
      break;
    default:
      break;
    }
  }

  if( foundElements.size() > 0 ) {
    newstrength = field/( (double) foundElements.size() );
    slist_iterator getNext( foundElements );
    while((  element = (bmlnElmnt*) getNext()  )) {
      if( 0 == strcmp( "combinedFunction", element->Type() ) ) {
        ((combinedFunction*) element)->setField( mult, newstrength );
      }
      else {
        element->setStrength( newstrength );
      }
    }
  }
}

double combinedFunction::Field(WHICH_MULTIPOLE mult) {
  DeepBeamlineIterator dbi( p_bml );
  bmlnElmnt* element;
  double multStrength = 0.0;

  while((  element = dbi++  )) 
  {
    switch (mult) {
    case DIPOLE_FIELD:
      if(strcasecmp(element->Type(),"sbend") == 0 ||
	 strcasecmp(element->Type(),"rbend") == 0 ) {
	multStrength += element->Strength();
      }
      break;
    case QUADRUPOLE_FIELD:
      if(strcasecmp(element->Type(),"quadrupole") == 0 ||
	 strcasecmp(element->Type(),"thinQuad") == 0 ) {
	multStrength += element->Strength();
      }
      break;
    case SEXTUPOLE_FIELD:
      if(strcasecmp(element->Type(),"sextupole") == 0 ||
	 strcasecmp(element->Type(),"thinSextupole") == 0 ) {
	multStrength += element->Strength();
      }
      break;
    case OCTUPOLE_FIELD:
      if(strcasecmp(element->Type(),"thinOctupole") == 0 ) {
	multStrength += element->Strength();
      }
      break;
    case DECAPOLE_FIELD:
      if(strcasecmp(element->Type(),"thinDecapole") == 0 ) {
	multStrength += element->Strength();
      }
      break;
    case TWELVEPOLE_FIELD:
      if(strcasecmp(element->Type(),"thin12pole") == 0 ) {
	multStrength += element->Strength();
      }
      break;
    case FOURTEENPOLE_FIELD:
      if(strcasecmp(element->Type(),"thin14pole") == 0 ) {
	multStrength += element->Strength();
      }
      break;
    case SIXTEENPOLE_FIELD:
      if(strcasecmp(element->Type(),"thin16pole") == 0 ) {
	multStrength += element->Strength();
      }
      break;
    case EIGHTEENPOLE_FIELD:
      if(strcasecmp(element->Type(),"thin18pole") == 0 ) {
	multStrength += element->Strength();
      }
      break;
    default:
      break;
    }
  }

  return multStrength;
}

void combinedFunction::setSkew(WHICH_MULTIPOLE mult, alignmentData& alignD) {
  DeepBeamlineIterator dbi( p_bml );
  bmlnElmnt* element;

  while((  element = dbi++  )) 
  {
    switch (mult) {
    case DIPOLE_FIELD:
      if(strcasecmp(element->Type(),"sbend") == 0 ||
	 strcasecmp(element->Type(),"rbend") == 0 )
	element->setAlignment(alignD);
      break;
    case QUADRUPOLE_FIELD:
      if(strcasecmp(element->Type(),"quadrupole") == 0 ||
	 strcasecmp(element->Type(),"thinQuad") == 0 )
	element->setAlignment(alignD);
      break;
    case SEXTUPOLE_FIELD:
      if(strcasecmp(element->Type(),"sextupole") == 0 ||
	 strcasecmp(element->Type(),"thinSextupole") == 0 )
	element->setAlignment(alignD);
      break;
    case OCTUPOLE_FIELD:
      if(strcasecmp(element->Type(),"thinOctupole") == 0 )
	element->setAlignment(alignD);
      break;
    case DECAPOLE_FIELD:
      if(strcasecmp(element->Type(),"thinDecapole") == 0 )
	element->setAlignment(alignD);
      break;
    case TWELVEPOLE_FIELD:
      if(strcasecmp(element->Type(),"thin12pole") == 0 ) {
	element->setAlignment(alignD);
      }
      break;
    case FOURTEENPOLE_FIELD:
      if(strcasecmp(element->Type(),"thin14pole") == 0 ) {
	element->setAlignment(alignD);
      }
      break;
    case SIXTEENPOLE_FIELD:
      if(strcasecmp(element->Type(),"thin16pole") == 0 ) {
	element->setAlignment(alignD);
      }
      break;
    case EIGHTEENPOLE_FIELD:
      if(strcasecmp(element->Type(),"thin18pole") == 0 ) {
	element->setAlignment(alignD);
      }
      break;
    default:
      break;
    }
  }
}

alignmentData combinedFunction::Skew(WHICH_MULTIPOLE mult) {
  dlist_iterator getNext(*(dlist*)p_bml);
  bmlnElmnt* element;
  alignmentData alignD;
  int foundIt = 0;

  while((element = (bmlnElmnt*)getNext()) != 0 && foundIt == 0) {
    switch (mult) {
    case DIPOLE_FIELD:
      if(strcasecmp(element->Type(),"sbend") == 0 ||
	 strcasecmp(element->Type(),"rbend") == 0 ) {
	alignD = element->Alignment();
	foundIt = 1;
      }
      break;
    case QUADRUPOLE_FIELD:
      if(strcasecmp(element->Type(),"quadrupole") == 0 ||
	 strcasecmp(element->Type(),"thinQuadrupole") == 0 ) {
	alignD = element->Alignment();
	foundIt = 1;
      }
      break;
    case SEXTUPOLE_FIELD:
      if(strcasecmp(element->Type(),"sextupole") == 0 ||
	 strcasecmp(element->Type(),"thinSextupole") == 0 ) {
	alignD = element->Alignment();
	foundIt = 1;
      }
      break;
    case OCTUPOLE_FIELD:
      if(strcasecmp(element->Type(),"thinOctupole") == 0 ) {
	alignD = element->Alignment();
	foundIt = 1;
      }
      break;
    case DECAPOLE_FIELD:
      if(strcasecmp(element->Type(),"thinDecapole") == 0 ) {
	alignD = element->Alignment();
	foundIt = 1;
      }
      break;
    case TWELVEPOLE_FIELD:
      if(strcasecmp(element->Type(),"thin12pole") == 0 ) {
	alignD = element->Alignment();
	foundIt = 1;
      }
      break;
    case FOURTEENPOLE_FIELD:
      if(strcasecmp(element->Type(),"thin14pole") == 0 ) {
	alignD = element->Alignment();
	foundIt = 1;
      }
      break;
    case SIXTEENPOLE_FIELD:
      if(strcasecmp(element->Type(),"thin16pole") == 0 ) {
	alignD = element->Alignment();
	foundIt = 1;
      }
      break;
    case EIGHTEENPOLE_FIELD:
      if(strcasecmp(element->Type(),"thin18pole") == 0 ) {
	alignD = element->Alignment();
	foundIt = 1;
      }
      break;
    default:
      break;
    }
  }
  return alignD;
}

ostream& combinedFunction::writeTo(ostream& os) {
  os << *p_bml;
  os << "combinedFunction_END " << ident << " 0 0 0 0 0\n";
  return os;
}

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


const char* combinedFunction::Type() const 
{ 
  return "combinedFunction"; 
}

double combinedFunction::AdjustPosition( const Proton& arg_p )
{
  JetProton myJP( arg_p );  // This probably won't work properly.
  return AdjustPosition( myJP );
}

double combinedFunction::AdjustPosition( const JetProton& arg_jp )
{
  /* static */ enum { x = 0, y, cdt, xp, yp, dpop };
  JetProton  myJP( arg_jp );
  Proton*    p_myP = (Proton*) myJP.ConvertToParticle();
  // This is deleted before returning.

  double x_i  = p_myP->State( x  );
  double xp_i = p_myP->State( xp );

  double inState [] = { 0, 0, 0, 0, 0, 0 };
  inState[x]  = x_i;
  inState[xp] = xp_i;

  myJP  .setState( inState );
  p_myP->setState( inState );

  this->propagate( myJP );

  double f, m, z;

  m = ( myJP.State().Jacobian() )( x, x );
  m -= 1.0;
  if( fabs(m) < 1.0e-12 ) {
  throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
         "double combinedFunction::AdjustPosition( const JetProton& arg_jp )", 
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
  delete p_myP;
  return z;
}
