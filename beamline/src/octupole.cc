#include "beamline.inc"

// **************************************************
//   class octupole
// **************************************************

octupole::octupole( double l, double s ) : bmlnElmnt(l, s) {
 p_bml = new beamline;
 p_bml->append( new drift( length / 2.0 ) );
 p_bml->append( p_bml_e = new thinOctupole( strength*length ) );
 p_bml->append( new drift( length / 2.0 ) );
}

octupole::octupole( char* n, double l, double s ) : bmlnElmnt( n, l, s ) {
 p_bml = new beamline;
 p_bml->append( new drift( length / 2.0 ) );
 p_bml->append( p_bml_e = new thinOctupole( strength*length ) );
 p_bml->append( new drift( length / 2.0 ) );
}

octupole::octupole( bmlnElmntData& x ) : bmlnElmnt( x ) {
 p_bml = new beamline;
 p_bml->append( new drift( length / 2.0 ) );
 p_bml->append( p_bml_e = new thinOctupole( strength*length ) );
 p_bml->append( new drift( length / 2.0 ) );
}

octupole::octupole( const octupole& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
}

octupole::~octupole() {
  p_bml_e = 0;
}


void octupole::setStrength( double s ) {
 strength = s - getShunt()*IToField();
 p_bml_e->setStrength( strength*length );
}


void octupole::setStrength( double, int ) {
 cerr << "\nCall to octupole::setStrength( double s, int index )"
      << endl;
 exit(1);
}


void octupole::setCurrent( double s ) {
 p_bml_e->setCurrent( s );
}


// ??? REMOVE void octupole::image( int d, slist* s, BMLN_posInfo* cg ) {
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


char* octupole::Type() const 
{ 
  return "octupole"; 
}


void octupole::Split( double pc, bmlnElmnt** a, bmlnElmnt** b )
{
  if( ( pc <= 0.0 ) || ( pc >= 1.0 ) ) {
    cerr << "\n"
            "*** ERROR ***                                    \n"
            "*** ERROR *** octupole::Split                    \n"
            "*** ERROR *** pc = " << pc << 
                               "and is out of bounds.         \n"
            "*** ERROR ***                                    \n"
         << endl;
    exit (1);
  }

  // We assume "strength" means field, not field*length.
  *a = new octupole(         pc  *length, strength );
  *b = new octupole( ( 1.0 - pc )*length, strength );

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
