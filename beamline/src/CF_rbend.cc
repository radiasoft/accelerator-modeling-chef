#include "CF_rbend.h"

CF_rbend::CF_rbend( double        lng,  // length     [ meter    ]
                    double        fld,  // field      [ tesla    ]
                    double        ang,  // angle      [ radians  ]
                    int )
: bmlnElmnt( lng, fld ), _poleFaceAngle( ang  )
{
  #include "CF_rbendConstructor.ins"
}


CF_rbend::CF_rbend( const char*   nm,   // name
                    double        lng,  // length     [ meter    ]
                    double        fld,  // field      [ tesla    ]
                    double        ang,  // angle      [ radians  ]
                    int )
: bmlnElmnt( nm, lng, fld ), _poleFaceAngle( ang  )
{
  #include "CF_rbendConstructor.ins"
}


CF_rbend::CF_rbend( double lng,
                    double fld,
                    double ang,
                    const Vector& xmlt,
                    int  )
: bmlnElmnt( lng, fld ), _poleFaceAngle( ang  )
{
  #include "CF_rbendConstructor.ins"
}


CF_rbend::CF_rbend( const char*  nm,
                    double lng,
                    double fld,
                    double ang,
                    const Vector& xmlt,
                    int  )
: bmlnElmnt( nm, lng, fld ), _poleFaceAngle( ang  )
{
  #include "CF_rbendConstructor.ins"
}


CF_rbend::CF_rbend( const CF_rbend& x )
: bmlnElmnt( (const bmlnElmnt&) x ), _poleFaceAngle( x._poleFaceAngle )
{
  int m = 1 + ( ( int(x._v) - int(x._u) )/sizeof( bmlnElmnt* ) );
  _u = new bmlnElmnt* [ m ];
  _v = &( _u[m-1] );
  
  for( int k = 0; k < m; k++ ) {
    _u[k] = ( x._u[k] )->Clone();
  }
}


CF_rbend::~CF_rbend()
{
  while( _v >= _u ) {
    delete (*(_v--));
  }
  delete [] _u;
}


void CF_rbend::localPropagate( Particle& p )
{
  bmlnElmnt** x = _u;
  while( x <= _v ) {
    (*(x++))->localPropagate( p );
  }
}


void CF_rbend::localPropagate( JetParticle& p )
{
  bmlnElmnt** x = _u;
  while( x <= _v ) {
    (*(x++))->localPropagate( p );
  }
}


int CF_rbend::setSextupole( double arg_x )
{
  int m = 1 + ( ( int(_v) - int(_u) )/sizeof( bmlnElmnt* ) );
  thinSextupole** w = new thinSextupole* [ m ];
  int counter = -1;
  bmlnElmnt** x = _u;

  while( x <= _v ) {
    if( 0 == strcmp( (*x)->Type(), "thinSextupole" ) ) {
      // w[++counter] = dynamic_cast<thinSextupole*>(*x);
      w[++counter] = (thinSextupole*)(*x);
    }
    x++;
  }

  if( counter < 0 ) {
    delete [] w;
    return 1;
  }
  
  double s = arg_x/((double) ( counter + 1 ));
  for( int i = 0; i <= counter; i++ ) {
    w[i]->setStrength( s );
  }
  
  delete [] w;
  return 0;
}

int CF_rbend::setQuadrupole( double arg_x )
{
  int m = 1 + ( ( int(_v) - int(_u) )/sizeof( bmlnElmnt* ) );
  thinQuad** w = new thinQuad* [ m ];
  int counter = -1;
  bmlnElmnt** x = _u;

  while( x <= _v ) {
    if( 0 == strcmp( (*x)->Type(), "thinQuad" ) ) {
      // w[++counter] = dynamic_cast<thinQuad*>(*x);
      w[++counter] = (thinQuad*)(*x);
    }
    x++;
  }

  if( counter < 0 ) {
    delete [] w;
    return 1;
  }
  
  double s = arg_x/((double) ( counter + 1 ));
  for( int i = 0; i <= counter; i++ ) {
    w[i]->setStrength( s );
  }
  
  delete [] w;
  return 0;
}

double CF_rbend::getSextupole()
{
  int m = 1 + ( ( int(_v) - int(_u) )/sizeof( bmlnElmnt* ) );
  thinSextupole** w = new thinSextupole* [ m ];
  int counter = -1;
  bmlnElmnt** x = _u;

  while( x <= _v ) {
    if( 0 == strcmp( (*x)->Type(), "thinSextupole" ) ) {
      // w[++counter] = dynamic_cast<thinSextupole*>(*x);
      w[++counter] = (thinSextupole*)(*x);
    }
    x++;
  }

  if( counter < 0 ) {
    delete [] w;
    return 1;
  }
  
  double ret = 0.0;
  for( int i = 0; i <= counter; i++ ) {
    ret += w[i]->Strength();
  }
  
  delete [] w;
  return ret;
}

double CF_rbend::getQuadrupole()
{
  int m = 1 + ( ( int(_v) - int(_u) )/sizeof( bmlnElmnt* ) );
  thinQuad** w = new thinQuad* [ m ];
  int counter = -1;
  bmlnElmnt** x = _u;

  while( x <= _v ) {
    if( 0 == strcmp( (*x)->Type(), "thinQuad" ) ) {
      // w[++counter] = dynamic_cast<thinQuad*>(*x);
      w[++counter] = (thinQuad*)(*x);
    }
    x++;
  }

  if( counter < 0 ) {
    delete [] w;
    return 1;
  }
  
  double ret = 0.0;
  for( int i = 0; i <= counter; i++ ) {
    ret += w[i]->Strength();
  }
  
  delete [] w;
  return ret;
}


void CF_rbend::Split( double, bmlnElmnt**, bmlnElmnt** )
{
  cerr << "*** ERROR ***                                 \n" 
          "*** ERROR *** CF_rbend::Split                 \n" 
          "*** ERROR *** This function is not written.   \n" 
          "*** ERROR ***                                 \n" 
       << endl;
  exit(1);
}

ostream& CF_rbend::writeTo( ostream& os )
{
  cerr << "*** ERROR ***                                 \n" 
          "*** ERROR *** CF_rbend::writeTo               \n" 
          "*** ERROR *** This function is not written.   \n" 
          "*** ERROR ***                                 \n" 
       << endl;
  exit(1);
  return os;
}

istream& CF_rbend::readFrom( istream& is )
{
  cerr << "*** ERROR ***                                 \n" 
          "*** ERROR *** CF_rbend::readFrom              \n" 
          "*** ERROR *** This function is not written.   \n" 
          "*** ERROR ***                                 \n" 
       << endl;
  exit(1);
  return is;
}

void CF_rbend::eliminate( void )
{
  delete this;
}


double CF_rbend::OrbitLength( const Particle& x )
{
  static double tworho;
  tworho  = 2.0 * ( x.Momentum() / PH_CNV_brho_to_p ) / strength;
  return tworho * asin( length / tworho );
}
