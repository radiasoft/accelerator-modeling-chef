#include "beamline.inc"


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
                        PROPFUNC pf, JETPROPFUNC jpf ) 
: bmlnElmnt( n, l, s, pf, jpf )
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

 double xm = m;
 double frontLength     = 6.0*(l/4.0)/15.0;
 double sepLength       = ( l - 2.0*frontLength ) / 3.0;
 double quarterStrength = strength*length/4.0;
 JetStrength.setVariable(index);
// JetStrength += s;
// cout << "constructor " << JetStrength << endl;
 strengthIndex = index;
 if(pf == JetQuadrupole::P_LikeMAD) {
   p_bml = 0;
 } else {
   p_bml = new beamline;

   if( m == 1 ) {           // One thin element ..........
     p_bml->append( new drift( length / 2.0 ) );
     p_bml->append( p_bml_e = new JetthinQuad( strength*length,index ) );
     p_bml->append( new drift( length / 2.0 ) );
   }
   
   else if( m == 4 ) {      // TEAPOT-like schema .........
     p_bml->append( new drift( frontLength ) );
     p_bml->append( new JetthinQuad( quarterStrength, index ) );
     for( int i = 0; i < 3; i++ ) {
       p_bml->append( new drift( sepLength ) );
       p_bml->append( new JetthinQuad( quarterStrength, index ) );
     }
     p_bml->append( new drift( frontLength ) );
   }
   
   else {                   // Equal spacing ...............
     p_bml->append( new drift( length / ( 2.0*xm ) ) );
     p_bml->append( new JetthinQuad( strength*length/ xm, index ) );
     for( int i = 0; i < m-1; i++ ) {
       p_bml->append( new drift( length / xm ) );
       p_bml->append( new JetthinQuad( strength*length/ xm, index ) );
     }
     p_bml->append( new drift( length / ( 2.0*xm ) ) );
   }
 }
}


JetQuadrupole::JetQuadrupole( double l, double s, int index, int m,
                        PROPFUNC pf, JETPROPFUNC jpf ) 
: bmlnElmnt( l, s, pf, jpf ) 
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

 double xm = m;
 double frontLength     = 6.0*(l/4.0)/15.0;
 double sepLength       = ( l - 2.0*frontLength ) / 3.0;
 double quarterStrength = strength*length/4.0;
 JetStrength.setVariable(index);
// JetStrength += s;
// cout << "constructor " << JetStrength << endl;
 strengthIndex = index;
 if(pf == JetQuadrupole::P_LikeMAD)
   p_bml = 0;
 else {
   p_bml = new beamline;
   
   if( m == 1 ) {           // One thin element ..........
     p_bml->append( new drift( length / 2.0 ) );
     p_bml->append( p_bml_e = new JetthinQuad( strength*length, index ) );
     p_bml->append( new drift( length / 2.0 ) );
   }
   
   else if( m == 4 ) {      // TEAPOT-like schema .........
     p_bml->append( new drift( frontLength ) );
     p_bml->append( new JetthinQuad( quarterStrength, index ) );
     for( int i = 0; i < 3; i++ ) {
       p_bml->append( new drift( sepLength ) );
       p_bml->append( new JetthinQuad( quarterStrength, index ) );
     }
     p_bml->append( new drift( frontLength ) );
   }
   
   else {                   // Equal spacing ...............
     p_bml->append( new drift( length / ( 2.0*xm ) ) );
     p_bml->append( new JetthinQuad( strength*length/ xm, index ) );
     for( int i = 0; i < m-1; i++ ) {
       p_bml->append( new drift( length / xm ) );
       p_bml->append( new JetthinQuad( strength*length/ xm, index ) );
     }
     p_bml->append( new drift( length / ( 2.0*xm ) ) );
   }
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


// ??? REMOVE void JetQuadrupole::peekAt() {
// ??? REMOVE  bmlnElmnt::peekAt();
// ??? REMOVE }

// ??? REMOVE void JetQuadrupole::image( int d, slist* s, BMLN_posInfo* cg ) {
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
void JetQuadrupole::eliminate() {
 delete this;
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
                       4, Propagator, JetPropagator ); // ??? Fix
  *b = new JetQuadrupole( ident,( 1.0 - pc )*length, strength, strengthIndex,
                       4, Propagator, JetPropagator ); // ??? this
  // We assume "strength" means field, not field*length.
}

ostream& JetQuadrupole::writeTo(ostream& os)
{
  os << strengthIndex << " ";
  if ( Propagator == 		&JetQuadrupole::P_LikeTPOT )
    os << "JetQuadrupole::P_LikeTPOT ";

  if ( JetPropagator == 	&JetQuadrupole::J_LikeTPOT )
    os << "JetQuadrupole::J_LikeTPOT";

  os << "\n";
  return os;
}

istream& JetQuadrupole::readFrom(istream& is)
{
  int index;
  char prop_fun[100], jet_prop_fun[100];
  
  is >> index >> prop_fun >> jet_prop_fun;
  setStrength(strength, index);

  if ( strcasecmp(prop_fun, 	"JetQuadrupole::P_LikeTPOT" ) == 0 )
    setPropFunction(&JetQuadrupole::P_LikeTPOT );
  else
    {
      cerr << " **** WARNING **** JetQuadrupole::readFrom(istream)\n";
      cerr << " **** WARNING **** unknown propagator function specified:\n";
      cerr << " **** WARNING **** " << prop_fun << "\n";
      cerr << " **** WARNING **** Substituting JetQuadrupole::P_LikeTPOT\n";
      setPropFunction(&JetQuadrupole::P_LikeTPOT);
    }

  if ( strcasecmp(jet_prop_fun, 	"JetQuadrupole::J_LikeTPOT" ) == 0 )
    setJPropFunction(&JetQuadrupole::J_LikeTPOT);
  else
    {
      cerr << " **** WARNING **** JetQuadrupole::readFrom(istream)\n";
      cerr << " **** WARNING **** unknown jet propagator function specified:\n";
      cerr << " **** WARNING **** " << jet_prop_fun << "\n";
      cerr << " **** WARNING **** Substituting JetQuadrupole::J_LikeTPOT\n";
      setJPropFunction(&JetQuadrupole::J_LikeTPOT);
    }
  return is;
}

