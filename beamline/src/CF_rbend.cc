#include "CF_rbend.h"

CF_rbend::CF_rbend( double        lng,  // length     [ meter    ]
                    double        fld,  // field      [ tesla    ]
                    double        ang,  // angle      [ radians  ]
                    const Vector& xmlt  // multipoles [ meter^-n ]
                    // Normal and skew multipoles alternate.
                    // xmlt(0)       = b_0    xmlt(1)     = a_0
                    // xmlt(2)       = b_1    xmlt(3)     = a_1
                    // xmlt(4)       = b_2    xmlt(5)     = a_2
                    // ...
                    // xmlt(2*m)     = b_[m]  xmlt(2*m+1) = a_[m]
                  )
: bmlnElmnt()
{
  #include "CF_rbendConstructor.ins"
}


CF_rbend::CF_rbend( char*  nm,         // name
                    double lng,
                    double fld,
                    double ang,
                    const Vector& xmlt )
: bmlnElmnt( nm )
{
  #include "CF_rbendConstructor.ins"
}


CF_rbend::CF_rbend( const CF_rbend& x )
: bmlnElmnt( (const bmlnElmnt&) x )
{
  int i, j, k, l;
  for( i = 0; i < 4; i++ ) {
    u[i] = x.u[i]->Clone();
  }
  j = 4;
  for( i = 0; i < 3; i++ ) {
    u[j++] = u[1];
    u[j++] = u[2];
    u[j++] = u[3];
  }
  --j;
  u[j++] = u[0];

  for( k = 0; k < 3; k++ ) {
    i = 0;
    for( l = 0; l < 13; l++ ) {
      u[j++] = u[i++];
    }
  }  

  if( j != 52 ) {
    cerr << "*** ERROR ***                                 \n"
            "*** ERROR *** CF_rbend::CF_rbend( CF_rbend )  \n"
            "*** ERROR *** A horrible, inexplicable error  \n"
            "*** ERROR *** has occurred.                   \n"
            "*** ERROR ***                                 \n"
         << endl;
    exit(1);
  }

}


CF_rbend::~CF_rbend()
{
  delete u[0];
  delete u[1];
  delete u[2];
  delete u[3];
}


void CF_rbend::localPropagate( Particle& p )
{
  for( int i = 0; i < 52; i++ ) {
    u[i]->localPropagate( p );
  }
}


void CF_rbend::localPropagate( JetParticle& p )
{
  for( int i = 0; i < 52; i++ ) {
    u[i]->localPropagate( p );
  }
}
