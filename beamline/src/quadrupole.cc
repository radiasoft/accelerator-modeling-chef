#include "quadrupole.h"
#include "drift.h"
#include "BeamlineIterator.h"

// **************************************************
//   class quadrupole
// **************************************************

quadrupole::quadrupole()
: bmlnElmnt()
{
  cerr << "\n*** ERROR *** Cannot define quadrupole without parameters.\n" 
       << endl;
  exit(1);
}

quadrupole::quadrupole( char* n, double l, double s, bmlnElmnt::PropFunc* pf )
: bmlnElmnt( n, l, s, pf )
{
 if( l<= 0.0 ) {
  cerr << "*** ERROR ***                                   \n"
          "*** ERROR *** quadrupole::quadrupole            \n"
          "*** ERROR *** You ignorant cretin!  The length  \n"
          "*** ERROR *** of a quadrupole must be positive! \n"
          "*** ERROR ***                                   \n"
       << endl;
  exit(1);
 }

 if( 0 == strcmp( pf->Type(), "quadrupole::TPOT_Prop" ) ) 
 {
   ((quadrupole::TPOT_Prop*) pf)->setup( this );
 }
 else 
 {
   p_bml = 0;
 }
}


quadrupole::quadrupole( double l, double s, bmlnElmnt::PropFunc* pf )
: bmlnElmnt( l, s, pf )
{
 if( l<= 0.0 ) {
  cerr << "*** ERROR ***                                   \n"
          "*** ERROR *** quadrupole::quadrupole            \n"
          "*** ERROR *** You ignorant cretin!  The length  \n"
          "*** ERROR *** of a quadrupole must be positive! \n"
          "*** ERROR ***                                   \n"
       << endl;
  exit(1);
 }

 if( 0 == strcmp( pf->Type(), "quadrupole::TPOT_Prop" ) ) 
 {
   ((quadrupole::TPOT_Prop*) pf)->setup( this );
 }
 else 
 {
   p_bml = 0;
 }
}


quadrupole::quadrupole( bmlnElmntData& x ) 
: bmlnElmnt( x ) 
{
 // ??? GET RID OF THIS FUNCTION.
  static char firstTime = 1;
  if( firstTime ) {
    cerr << "Warning: call to quadrupole::quadrupole( bmlnElmntData& x )" << endl;
    firstTime = 0;
  }

 double xm = 3.0;
 p_bml = new beamline;

 p_bml->append( new drift( length / ( 2.0*xm ) ) );
 p_bml->append( new thinQuad( strength*length/ xm ) );
 for( int i = 0; i < 2; i++ ) {
   p_bml->append( new drift( length / xm ) );
   p_bml->append( new thinQuad( strength*length/ xm ) );
 }
 p_bml->append( new drift( length / ( 2.0*xm ) ) );

}


quadrupole::quadrupole( const quadrupole& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
}


quadrupole::~quadrupole() {
}


void quadrupole::setStrength( double s ) {
 strength = s - getShunt()*IToField();
 double integratedStrength = strength*length;

 if( p_bml != 0 ) 
 {
   double counter = 0.0;
   BeamlineIterator bi( p_bml );
   bmlnElmnt* q;
   while( 0 != ( q = bi++  ) ) {
     if( 0 == strcmp( q->Type(), "thinQuad" ) )  counter++;
   }
   if( counter <= 0.0 ) {
     cerr << "*** ERROR ***                                    \n"
             "*** ERROR *** quadrupole::setStrength            \n"
             "*** ERROR *** No thin quads in the internal      \n"
             "*** ERROR *** beamline.                          \n"
             "*** ERROR ***                                    \n"
          << endl;
     exit(1);
   }
   else if( counter == 1.0 ) {
     if(p_bml_e != 0) 
     {
       p_bml_e->setStrength( integratedStrength );
     }
     else 
     {
       cerr << "*** ERROR ***                                    \n"
               "*** ERROR *** quadrupole::setStrength            \n"
               "*** ERROR *** p_bml_e not set.                   \n"
               "*** ERROR ***                                    \n"
            << endl;
       exit(2);
     }
   }
   else {
     bi.reset();
     while( 0 != ( q = bi++  ) ) {
       if( 0 == strcmp( q->Type(), "thinQuad" ) ) {
         q->setStrength( integratedStrength/counter );
       }
     }
   }
 }
}


void quadrupole::setStrength( double, int ) {
 cerr << "\nCall to quadrupole::setStrength( double s, int index )"
      << endl;
 exit(1);
}


// ??? REMOVE void quadrupole::peekAt() {
// ??? REMOVE  bmlnElmnt::peekAt();
// ??? REMOVE }

// ??? REMOVE void quadrupole::image( int d, slist* s, BMLN_posInfo* cg ) {
// ??? REMOVE  bmlnElmntData* p = bmlnElmnt::image();
// ??? REMOVE 
// ??? REMOVE  strcpy( p->type, Type() );
// ??? REMOVE  p->address   = this;
// ??? REMOVE  p->depth     = d;
// ??? REMOVE 
// ??? REMOVE  geomToEnd( *cg );
// ??? REMOVE  cg->outPoint = geometry.outPoint;
// ??? REMOVE  for( int j = 0; j < 3; j++ ) cg->outAxes[j] = geometry.outAxes[j];
// ??? REMOVE 
// ??? REMOVE      p->geometry.inPoint  = geometry.inPoint;
// ??? REMOVE      p->geometry.outPoint = geometry.outPoint;
// ??? REMOVE      for( j = 0; j < 3; j++ ) p->geometry.inAxes[j]  = geometry.inAxes[j];
// ??? REMOVE      for( j = 0; j < 3; j++ ) p->geometry.outAxes[j] = geometry.outAxes[j];
// ??? REMOVE 
// ??? REMOVE  s->append( p );
// ??? REMOVE }
// ??? REMOVE 


void quadrupole::eliminate() {
 delete this;
}


const char* quadrupole::Type() const 
{ 
  return "quadrupole"; 
}


void quadrupole::Split( double pc, bmlnElmnt** a, bmlnElmnt** b )
{
  if( ( pc <= 0.0 ) || ( pc >= 1.0 ) ) {
    cerr << "\n"
            "*** ERROR ***                                    \n"
            "*** ERROR *** quadrupole::Split                  \n"
            "*** ERROR *** pc = " << pc << 
                               "and is out of bounds.         \n"
            "*** ERROR ***                                    \n"
         << endl;
    exit (1);
  }

  // We assume "strength" means field, not field*length.
  *a = new quadrupole(         pc  *length, strength, Propagator ); // ??? Fix
  *b = new quadrupole( ( 1.0 - pc )*length, strength, Propagator ); // ??? this

  // Rename
  char* newname;
  newname = new char [ strlen(ident) + 6 ];

  strcpy( newname, ident );
  strcat( newname, "_CL_A" );
  (*a)->Rename( newname );

  strcpy( newname, ident );
  strcat( newname, "_CL_B" );
  (*b)->Rename( newname );

  delete [] newname;
}

istream& quadrupole::readFrom(istream& is)
{
  char prop_fun[100], jet_prop_fun[100];

  is >> prop_fun >> jet_prop_fun;
  if ( strcasecmp(prop_fun,             "quadrupole::P_LikeMAD" ) == 0 )
    setPropFunction(&quadrupole::LikeMAD );
  else if ( strcasecmp(prop_fun,        "quadrupole::P_LikeTPOT" ) == 0 )
    setPropFunction(&quadrupole::LikeTPOT );
  else
    {
      cerr << " **** WARNING **** quadrupole::readFrom(istream)\n";
      cerr << " **** WARNING **** unknown propagator function specified:\n";
      cerr << " **** WARNING **** " << prop_fun << "\n";
      cerr << " **** WARNING **** Substituting quadrupole::P_LikeMAD\n";
      setPropFunction(&quadrupole::LikeMAD);
    }

  return is;
}

ostream& quadrupole::writeTo(ostream& os)
{
  if ( Propagator == 		&quadrupole::LikeMAD )
    os << "quadrupole::P_LikeMAD   quadrupole::J_LikeMAD";
  else if ( Propagator == 	&quadrupole::LikeTPOT )
    os << "quadrupole::P_LikeTPOT  quadrupole::J_LikeTPOT";
  else 
    os << "UNKNOWN  UNKNOWN";

  os << "\n";
  return os;
}



// **************************************************
//   class thinQuad
// **************************************************

thinQuad::thinQuad( double s ) : bmlnElmnt() {
 strength = s;      // B'L in Tesla
}

thinQuad::thinQuad( char* n, double s ) : bmlnElmnt(n) {
 strength = s;
}

thinQuad::thinQuad( bmlnElmntData& x ) : bmlnElmnt( x ) {
}

thinQuad::thinQuad( const thinQuad& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
}

thinQuad::~thinQuad() {
}

// ??? REMOVE void thinQuad::image( int d, slist* s, BMLN_posInfo* cg ) {
// ??? REMOVE  bmlnElmntData* p = bmlnElmnt::image();
// ??? REMOVE 
// ??? REMOVE  strcpy( p->type, Type() );
// ??? REMOVE  p->address   = this;
// ??? REMOVE  p->depth     = d;
// ??? REMOVE 
// ??? REMOVE  geomToEnd( *cg );
// ??? REMOVE  cg->outPoint = geometry.outPoint;
// ??? REMOVE  for( int j = 0; j < 3; j++ ) cg->outAxes[j] = geometry.outAxes[j];
// ??? REMOVE 
// ??? REMOVE      p->geometry.inPoint  = geometry.inPoint;
// ??? REMOVE      p->geometry.outPoint = geometry.outPoint;
// ??? REMOVE      for( j = 0; j < 3; j++ ) p->geometry.inAxes[j]  = geometry.inAxes[j];
// ??? REMOVE      for( j = 0; j < 3; j++ ) p->geometry.outAxes[j] = geometry.outAxes[j];
// ??? REMOVE 
// ??? REMOVE  s->append( p );
// ??? REMOVE }
// ??? REMOVE 
void thinQuad::eliminate() {
 delete this;
}


const char* thinQuad::Type() const { 
  return "thinQuad"; 
}
