#include "beamline.inc"
#include "combinedFunction.h"


// **************************************************
//   class combinedFunction
// **************************************************

combinedFunction::combinedFunction()
: bmlnElmnt()
{
  cerr << "\n*** ERROR *** Cannot define combinedFunction without parameters.\n" 
       << endl;
  exit(1);
}

combinedFunction::combinedFunction(char* n) : bmlnElmnt(n) {
  p_bml = new beamline();
}

combinedFunction::combinedFunction(char* n, beamline& b) : bmlnElmnt(n) {
  p_bml = (beamline*)b.Clone();
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

void combinedFunction::setField(WHICH_MULTIPOLE mult, double field) {
  dlist_iterator getNext(*(dlist*)p_bml);
  bmlnElmnt* element;

  while((element = (bmlnElmnt*)getNext()) != 0) {
    switch (mult) {
    case DIPOLE_FIELD:
      if(strcasecmp(element->Type(),"sbend") == 0 ||
	 strcasecmp(element->Type(),"rbend") == 0 )
	element->setStrength(field);
      break;
    case QUADRUPOLE_FIELD:
      if(strcasecmp(element->Type(),"quadrupole") == 0 ||
	 strcasecmp(element->Type(),"thinQuad") == 0 )
	element->setStrength(field);
      break;
    case SEXTUPOLE_FIELD:
      if(strcasecmp(element->Type(),"sextupole") == 0 ||
	 strcasecmp(element->Type(),"thinSextupole") == 0 )
	element->setStrength(field);
      break;
    case OCTUPOLE_FIELD:
      if(strcasecmp(element->Type(),"thinOctupole") == 0 )
	element->setStrength(field);
      break;
    case DECAPOLE_FIELD:
      if(strcasecmp(element->Type(),"thinDecapole") == 0 )
	element->setStrength(field);
      break;
    case TWELVEPOLE_FIELD:
      if(strcasecmp(element->Type(),"thin12pole") == 0 )
	element->setStrength(field);
      break;
    case FOURTEENPOLE_FIELD:
      if(strcasecmp(element->Type(),"thin14pole") == 0 )
	element->setStrength(field);
      break;
    case SIXTEENPOLE_FIELD:
      if(strcasecmp(element->Type(),"thin16pole") == 0 )
	element->setStrength(field);
      break;
    case EIGHTEENPOLE_FIELD:
      if(strcasecmp(element->Type(),"thin18pole") == 0 )
	element->setStrength(field);
      break;
    default:
      break;
    }
  }
}

double combinedFunction::Field(WHICH_MULTIPOLE mult) {
  dlist_iterator getNext(*(dlist*)p_bml);
  bmlnElmnt* element;
  double multStrength = 0;
  int foundIt = 0;

  while((element = (bmlnElmnt*)getNext()) != 0 && foundIt == 0) {
    switch (mult) {
    case DIPOLE_FIELD:
      if(strcasecmp(element->Type(),"sbend") == 0 ||
	 strcasecmp(element->Type(),"rbend") == 0 ) {
	multStrength = element->Strength();
	foundIt = 1;
      }
      break;
    case QUADRUPOLE_FIELD:
      if(strcasecmp(element->Type(),"quadrupole") == 0 ||
	 strcasecmp(element->Type(),"thinQuad") == 0 ) {
	multStrength = element->Strength();
	foundIt = 1;
      }
      break;
    case SEXTUPOLE_FIELD:
      if(strcasecmp(element->Type(),"sextupole") == 0 ||
	 strcasecmp(element->Type(),"thinSextupole") == 0 ) {
	multStrength = element->Strength();
	foundIt = 1;
      }
      break;
    case OCTUPOLE_FIELD:
      if(strcasecmp(element->Type(),"thinOctupole") == 0 ) {
	multStrength = element->Strength();
	foundIt = 1;
      }
      break;
    case DECAPOLE_FIELD:
      if(strcasecmp(element->Type(),"thinDecapole") == 0 ) {
	multStrength = element->Strength();
	foundIt = 1;
      }
      break;
    case TWELVEPOLE_FIELD:
      if(strcasecmp(element->Type(),"thin12pole") == 0 ) {
	multStrength = element->Strength();
	foundIt = 1;
      }
      break;
    case FOURTEENPOLE_FIELD:
      if(strcasecmp(element->Type(),"thin14pole") == 0 ) {
	multStrength = element->Strength();
	foundIt = 1;
      }
      break;
    case SIXTEENPOLE_FIELD:
      if(strcasecmp(element->Type(),"thin16pole") == 0 ) {
	multStrength = element->Strength();
	foundIt = 1;
      }
      break;
    case EIGHTEENPOLE_FIELD:
      if(strcasecmp(element->Type(),"thin18pole") == 0 ) {
	multStrength = element->Strength();
	foundIt = 1;
      }
      break;
    default:
      break;
    }
  }
  return multStrength;
}

void combinedFunction::setSkew(WHICH_MULTIPOLE mult, alignmentData& alignD) {
  dlist_iterator getNext(*(dlist*)p_bml);
  bmlnElmnt* element;

  while((element = (bmlnElmnt*)getNext()) != 0) {
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
    cerr << " **** WARNING **** Expecting an end of combinedFunction but got another bmlnElmnt\n";
    cerr << " **** WARNING **** Will attempt to proceed, but all bets are off!\n";
    delete e;
  }
  return is;
}


double combinedFunction::AdjustPosition( const Proton& arg_p )
{
  JetProton myJP( arg_p );  // This probably won't work properly.
  return AdjustPosition( myJP );
}

double combinedFunction::AdjustPosition( const JetProton& arg_jp )
{
  /* static */ const enum { x = 0, y, cdt, xp, yp, dpop };
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
    cerr << "*** ERROR ***                                       \n"
         << "*** ERROR *** combinedFunction::AdjustPosition      \n"
         << "*** ERROR *** A horrible, inexplicable error has    \n"
         << "*** ERROR *** occurred. A multi-valued solution     \n"
         << "*** ERROR *** is suspected.                         \n"
         << "*** ERROR ***                                       \n";
    exit(0);
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
