#include "beamline.inc"

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
}


void sextupole::setStrength( const double s ) {
  strength = s - getShunt()*IToField();
  p_bml_e->setStrength( strength*length );
}


void sextupole::setStrength( const double, const int ) {
 cerr << "\nCall to sextupole::setStrength( const double s, const int index )"
      << endl;
 exit(1);
}


void sextupole::setCurrent( const double s ) {
 p_bml_e->setCurrent( s );
}


// ??? REMOVE void sextupole::image( int d, slist* s, BMLN_posInfo* cg ) {
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
