#include "sextupole.h"
#include "drift.h"

// **************************************************
//   class sextupole 
// **************************************************

sextupole::sextupole ( double l, double s ) : bmlnElmnt( l, s ) {
 p_bml = new beamline;
 p_bml->append( new drift( length / 2.0 ) );
 p_bml->append( p_bml_e = new thinSextupole( strength*length ) );
 p_bml->append( new drift( length / 2.0 ) );
}


sextupole::sextupole ( char* n, double l, double s ) : bmlnElmnt( n, l, s ) {
 p_bml = new beamline;
 p_bml->append( new drift( length / 2.0 ) );
 p_bml->append( p_bml_e = new thinSextupole( strength*length ) );
 p_bml->append( new drift( length / 2.0 ) );
}


sextupole::sextupole( bmlnElmntData& x ) : bmlnElmnt( x ) {
 p_bml = new beamline;
 p_bml->append( new drift( length / 2.0 ) );
 p_bml->append( p_bml_e = new thinSextupole( strength*length ) );
 p_bml->append( new drift( length / 2.0 ) );
}

sextupole::sextupole( const sextupole& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{

}

sextupole::~sextupole() {
  p_bml_e = 0;
}


void sextupole::setStrength( double s ) {
  strength = s - getShunt()*IToField();
  p_bml_e->setStrength( strength*length );
}


void sextupole::setStrength( double, int ) {
 cerr << "\nCall to sextupole::setStrength( double s, int index )"
      << endl;
 exit(1);
}


void sextupole::setCurrent( double s ) {
 p_bml_e->setCurrent( s );
}


const char* sextupole::Type() const 
{ 
  return "sextupole"; 
}


void sextupole::Split( double pc, bmlnElmnt** a, bmlnElmnt** b )
{
  if( ( pc <= 0.0 ) || ( pc >= 1.0 ) ) {
    cerr << "\n"
            "*** ERROR ***                                    \n"
            "*** ERROR *** sextupole::Split                   \n"
            "*** ERROR *** pc = " << pc << 
                               "and is out of bounds.         \n"
            "*** ERROR ***                                    \n"
         << endl;
    exit (1);
  }

  // We assume "strength" means field, not field*length.
  *a = new sextupole(         pc  *length, strength );
  *b = new sextupole( ( 1.0 - pc )*length, strength );

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



// **************************************************
//   class thinSextupole
// **************************************************

thinSextupole::thinSextupole ( double s ) : bmlnElmnt( 0.0, s ) {
 // The strength is to be interpreted as
 // B''l in  Tesla / meter
}

thinSextupole::thinSextupole ( char* n, double s ) : bmlnElmnt( n, 0.0, s ) {
 // The strength is to be interpreted as
 // B''l in  Tesla / meter
}


thinSextupole::thinSextupole( bmlnElmntData& x ) : bmlnElmnt( x ) {
}

thinSextupole::thinSextupole( const thinSextupole& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
}

thinSextupole::~thinSextupole() {
}


const char* thinSextupole::Type() const { 
  return "thinSextupole"; 
}
