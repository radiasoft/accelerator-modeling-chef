#include "beamline.inc"

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


char* JetthinQuad::Type() const 
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

