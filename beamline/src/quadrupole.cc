#include "beamline.inc"

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

quadrupole::quadrupole( char* n, double l, double s, int m,
                        PROPFUNC pf, JETPROPFUNC jpf ) 
: bmlnElmnt( n, l, s, pf, jpf )
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

 double xm = m;
 double frontLength     = 6.0*(l/4.0)/15.0;
 double sepLength       = ( l - 2.0*frontLength ) / 3.0;
 double quarterStrength = strength*length/4.0;
 if(pf == quadrupole::P_LikeMAD)
   p_bml = 0;
 else {
   p_bml = new beamline;
  
   if( m == 1 ) {           // One thin element ..........
     p_bml->append( new drift( length / 2.0 ) );
     p_bml->append( p_bml_e = new thinQuad( strength*length ) );
     p_bml->append( new drift( length / 2.0 ) );
   }
  
   else if( m == 4 ) {      // TEAPOT-like schema .........
     p_bml->append( new drift( frontLength ) );
     p_bml->append( new thinQuad( quarterStrength ) );
     for( int i = 0; i < 3; i++ ) {
       p_bml->append( new drift( sepLength ) );
       p_bml->append( new thinQuad( quarterStrength ) );
     }
     p_bml->append( new drift( frontLength ) );
   }
  
   else {                   // Equal spacing ...............
     p_bml->append( new drift( length / ( 2.0*xm ) ) );
     p_bml->append( new thinQuad( strength*length/ xm ) );
     for( int i = 0; i < m-1; i++ ) {
       p_bml->append( new drift( length / xm ) );
       p_bml->append( new thinQuad( strength*length/ xm ) );
     }
     p_bml->append( new drift( length / ( 2.0*xm ) ) );
   }
 }
}


quadrupole::quadrupole( double l, double s, int m,
                        PROPFUNC pf, JETPROPFUNC jpf ) 
: bmlnElmnt( l, s, pf, jpf ) 
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

 double xm = m;
 double frontLength     = 6.0*(l/4.0)/15.0;
 double sepLength       = ( l - 2.0*frontLength ) / 3.0;
 double quarterStrength = strength*length/4.0;
 if(pf == quadrupole::P_LikeMAD)
   p_bml = 0;
 else {
   p_bml = new beamline;
   
   if( m == 1 ) {           // One thin element ..........
     p_bml->append( new drift( length / 2.0 ) );
     p_bml->append( p_bml_e = new thinQuad( strength*length ) );
     p_bml->append( new drift( length / 2.0 ) );
   }
  
   else if( m == 4 ) {      // TEAPOT-like schema .........
     p_bml->append( new drift( frontLength ) );
     p_bml->append( new thinQuad( quarterStrength ) );
     for( int i = 0; i < 3; i++ ) {
       p_bml->append( new drift( sepLength ) );
       p_bml->append( new thinQuad( quarterStrength ) );
     }
     p_bml->append( new drift( frontLength ) );
   }
  
   else {                   // Equal spacing ...............
     p_bml->append( new drift( length / ( 2.0*xm ) ) );
     p_bml->append( new thinQuad( strength*length/ xm ) );
     for( int i = 0; i < m-1; i++ ) {
       p_bml->append( new drift( length / xm ) );
       p_bml->append( new thinQuad( strength*length/ xm ) );
     }
     p_bml->append( new drift( length / ( 2.0*xm ) ) );
   }
 }
}


quadrupole::quadrupole( bmlnElmntData& x ) 
: bmlnElmnt( x ) 
{
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
 if(p_bml_e != 0)
   p_bml_e->setStrength( strength*length );
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
  *a = new quadrupole(         pc  *length, strength, 
                       4, Propagator, JetPropagator ); // ??? Fix
  *b = new quadrupole( ( 1.0 - pc )*length, strength, 
                       4, Propagator, JetPropagator ); // ??? this

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
    setPropFunction(&quadrupole::P_LikeMAD );
  else if ( strcasecmp(prop_fun,        "quadrupole::P_LikeTPOT" ) == 0 )
    setPropFunction(&quadrupole::P_LikeTPOT );
  else
    {
      cerr << " **** WARNING **** quadrupole::readFrom(istream)\n";
      cerr << " **** WARNING **** unknown propagator function specified:\n";
      cerr << " **** WARNING **** " << prop_fun << "\n";
      cerr << " **** WARNING **** Substituting quadrupole::P_LikeMAD\n";
      setPropFunction(&quadrupole::P_LikeMAD);
    }

  if ( strcasecmp(jet_prop_fun,         "quadrupole::J_LikeMAD" ) == 0 )
    setJPropFunction(&quadrupole::J_LikeMAD);
  else if ( strcasecmp(jet_prop_fun,    "quadrupole::J_LikeTPOT" ) == 0 )
    setJPropFunction(&quadrupole::J_LikeTPOT);
  else
   {
      cerr << " **** WARNING **** quadrupole::readFrom(istream)\n";
      cerr << " **** WARNING **** unknown jet propagator function specified:\n";
      cerr << " **** WARNING **** " << jet_prop_fun << "\n";
      cerr << " **** WARNING **** Substituting quadrupole::J_LikeMAD\n";
      setJPropFunction(&quadrupole::J_LikeMAD);
    }
  
  return is;
}

ostream& quadrupole::writeTo(ostream& os)
{
  if ( Propagator == 		&quadrupole::P_LikeMAD )
    os << "quadrupole::P_LikeMAD ";
  else if ( Propagator == 	&quadrupole::P_LikeTPOT )
    os << "quadrupole::P_LikeTPOT ";

  if ( JetPropagator == 	&quadrupole::J_LikeMAD )
    os << "quadrupole::J_LikeMAD";
  else if ( JetPropagator == 	&quadrupole::J_LikeTPOT )
    os << "quadrupole::J_LikeTPOT";

  os << "\n";
  return os;
}
