#include "JetQuadrupole.h"


// **************************************************
//   class JetQuadrupole
// **************************************************

JetQuadrupole::JetQuadrupole()
: bmlnElmnt()
{
  cerr << "\n*** ERROR *** Cannot define JetQuadrupole without parameters.\n" 
       << endl;
  exit(1);
}

JetQuadrupole::JetQuadrupole( char* n, double l, double s, int index, int m,
                              PropFunc* pf ) 
: bmlnElmnt( n, l, s, pf )
{
 if( l<= 0.0 ) {
  cerr << "*** ERROR ***                                   \n"
          "*** ERROR *** JetQuadrupole::JetQuadrupole            \n"
          "*** ERROR *** You ignorant cretin!  The length  \n"
          "*** ERROR *** of a JetQuadrupole must be positive! \n"
          "*** ERROR ***                                   \n"
       << endl;
  exit(1);
 }

 JetStrength.setVariable(index);
 strengthIndex = index;

 if( 0 == strcmp( pf->Type(), "JetQuadrupole::TPOT_Prop" ) ) 
 {
   ((JetQuadrupole::TPOT_Prop*) pf)->setup( this );
 }
 else 
 {
   p_bml = 0;
 }

}


JetQuadrupole::JetQuadrupole( double l, double s, int index, int m,
                              PropFunc* pf ) 
: bmlnElmnt( l, s, pf ) 
{
 // This body is identical to that of the previous constructor.

 if( l<= 0.0 ) {
  cerr << "*** ERROR ***                                   \n"
          "*** ERROR *** JetQuadrupole::JetQuadrupole            \n"
          "*** ERROR *** You ignorant cretin!  The length  \n"
          "*** ERROR *** of a JetQuadrupole must be positive! \n"
          "*** ERROR ***                                   \n"
       << endl;
  exit(1);
 }

 JetStrength.setVariable(index);
 strengthIndex = index;

 if( 0 == strcmp( pf->Type(), "JetQuadrupole::TPOT_Prop" ) ) 
 {
   ((JetQuadrupole::TPOT_Prop*) pf)->setup( this );
 }
 else 
 {
   p_bml = 0;
 }
}


JetQuadrupole::JetQuadrupole( bmlnElmntData& x ) 
: bmlnElmnt( x ) 
{
 cout << "*** ERROR *** \n"
      << "*** ERROR *** JetQuadrupole::JetQuadrupole( bmlnElmntData& x ) \n"
      << "*** ERROR *** Sorry, Jack!  You can't do this.\n"
      << endl;
 exit (1);
}


JetQuadrupole::JetQuadrupole( const JetQuadrupole& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
  JetStrength = x.JetStrength;
  strengthIndex = x.strengthIndex;
//  cout << "Copy constructor " << JetStrength << endl;
}


JetQuadrupole::~JetQuadrupole() {
}


void JetQuadrupole::setStrength( double s ) {
 JetStrength.setVariable(strengthIndex);
// JetStrength += s;
 if(p_bml_e != 0)
 p_bml_e->setStrength( strength*length, strengthIndex );
}


void JetQuadrupole::setStrength( double s, int index) {
 strength = s;
 strengthIndex = index;
 JetStrength.setVariable(s,index);
// JetStrength += s;
 if(p_bml_e != 0)
 p_bml_e->setStrength( strength*length, index );
}


void JetQuadrupole::eliminate() {
 delete this;
}



const char* JetQuadrupole::Type() const 
{ 
  return "JetQuadrupole"; 
}


void JetQuadrupole::Split( double pc, bmlnElmnt** a, bmlnElmnt** b )
{
  if( ( pc <= 0.0 ) || ( pc >= 1.0 ) ) {
    cerr << "\n"
            "*** ERROR ***                                    \n"
            "*** ERROR *** JetQuadrupole::Split                  \n"
            "*** ERROR *** pc = " << pc << 
                               "and is out of bounds.         \n"
            "*** ERROR ***                                    \n"
         << endl;
    exit (1);
  }

  *a = new JetQuadrupole( ident,        pc  *length, strength,strengthIndex, 
                       4, Propagator ); // ??? Fix
  *b = new JetQuadrupole( ident,( 1.0 - pc )*length, strength, strengthIndex,
                       4, Propagator ); // ??? this
  // We assume "strength" means field, not field*length.
}

ostream& JetQuadrupole::writeTo(ostream& os)
{
  os << strengthIndex << " ";
  if ( Propagator == &JetQuadrupole::LikeTPOT )
    os << "JetQuadrupole::P_LikeTPOT  JetQuadrupole::J_LikeTPOT";
  else 
    os << "UNKNOWN   UNKNOWN";

  os << "\n";
  return os;
}

istream& JetQuadrupole::readFrom(istream& is)
{
  int index;
  char prop_fun[100], jet_prop_fun[100];
  
  is >> index >> prop_fun >> jet_prop_fun;
  setStrength(strength, index);

  if ( strcasecmp( prop_fun, "JetQuadrupole::P_LikeTPOT" ) == 0 )
    setPropFunction( &JetQuadrupole::LikeTPOT );
  else
    {
      cerr << " **** WARNING **** JetQuadrupole::readFrom(istream)\n";
      cerr << " **** WARNING **** unknown propagator function specified:\n";
      cerr << " **** WARNING **** " << prop_fun << "\n";
      cerr << " **** WARNING **** Substituting JetQuadrupole::P_LikeTPOT\n";
      setPropFunction( &JetQuadrupole::LikeTPOT );
    }

  return is;
}


// **************************************************
//   class JetthinQuad
// **************************************************

JetthinQuad::JetthinQuad( double s, int index ) 
 : bmlnElmnt() 
{
 KL.setVariable( s, index );
 strength = s;
 strengthIndex = index;
}

JetthinQuad::JetthinQuad( char*  n, 
                          double s,
                          int    index ) 
 : bmlnElmnt(n) 
{
 KL.setVariable( s, index );
 strength = s;
 strengthIndex = index;
}

JetthinQuad::JetthinQuad( bmlnElmntData& x ) : bmlnElmnt( x ) {
 cout << "*** ERROR *** \n"
      << "*** ERROR *** JetthinQuad::JetthinQuad( bmlnElmntData& x ) \n"
      << "*** ERROR *** Sorry, Jack!  You can't do this.\n"
      << endl;
 exit (1);
}


JetthinQuad::JetthinQuad( const JetthinQuad& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
  KL = x.KL;
  strengthIndex = x.strengthIndex;
}

JetthinQuad::~JetthinQuad() {
}

void JetthinQuad::setStrength( double s ) {
 KL.setVariable( s, strengthIndex );
 strength = s;
}


void JetthinQuad::setStrength( double s, int index ) {
 KL.setVariable( s, index );
 strength = s;
 strengthIndex = index;
}

// ??? REMOVEvoid JetthinQuad::image( int d, slist* s, BMLN_posInfo* cg ) {
// ??? REMOVE bmlnElmntData* p = bmlnElmnt::image();

// ??? REMOVE p->address   = this;
// ??? REMOVE p->depth     = d;

// ??? REMOVE geomToEnd( *cg );
// ??? REMOVE cg->outPoint = geometry.outPoint;
// ??? REMOVE for( int j = 0; j < 3; j++ ) cg->outAxes[j] = geometry.outAxes[j];

// ??? REMOVE     p->geometry.inPoint  = geometry.inPoint;
// ??? REMOVE     p->geometry.outPoint = geometry.outPoint;
// ??? REMOVE     for( j = 0; j < 3; j++ ) p->geometry.inAxes[j]  = geometry.inAxes[j];
// ??? REMOVE     for( j = 0; j < 3; j++ ) p->geometry.outAxes[j] = geometry.outAxes[j];

// ??? REMOVE s->append( p );
// ??? REMOVE}


void JetthinQuad::eliminate() {
 delete this;
}


const char* JetthinQuad::Type() const 
{ 
  return "JetthinQuad"; 
}


ostream& JetthinQuad::writeTo(ostream& os)
{
  os << strengthIndex << "\n";
  return os;
}

istream& JetthinQuad::readFrom(istream& is)
{
  is >> strengthIndex;
  return is;
}

