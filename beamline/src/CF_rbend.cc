#ifdef __VISUAL_CPP__
#include <iomanip>
#else
#include <iomanip.h>
#endif

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
  // NOTE: If this code is ever modified, you 
  // must also modify CF_rbend::readFrom.
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
    return 0.0;
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
    return 0.0;
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
  os << OSTREAM_DOUBLE_PREC << ( this->_poleFaceAngle ) << " ";
  os << OSTREAM_DOUBLE_PREC << getQuadrupole() << " ";
  os << OSTREAM_DOUBLE_PREC << getSextupole() << " ";
  os << "\n";
  return os;
}

istream& CF_rbend::readFrom( istream& is )
{
  double quadStrength, sextStrength;
  is >> ( this->_poleFaceAngle ) 
     >> quadStrength 
     >> sextStrength;


  // Rebuild basic element ...
  // ... First deconstruct (identical to CF_rbend destructor)
  while( _v >= _u ) {
    delete (*(_v--));
  }
  delete [] _u;
  _v = 0;
  _u = 0;

  // ... Then reconstruct
  #include "CF_rbendConstructor.ins"


  // Set multipoles
  this->setQuadrupole( quadStrength );
  this->setSextupole( sextStrength );


  return is;
}

void CF_rbend::eliminate( void )
{
  delete this;
}



char* CF_rbend::Type() const  
{ 
  return "CF_rbend"; 
}


double CF_rbend::OrbitLength( const Particle& x )
{
  static double tworho;
  tworho  = 2.0 * ( x.Momentum() / PH_CNV_brho_to_p ) / strength;
  return tworho * asin( length / tworho );
}


double CF_rbend::AdjustPosition( const Proton& arg_p )
{
  JetProton myJP( arg_p );  // This probably won't work properly.
  return AdjustPosition( myJP );
}

double CF_rbend::AdjustPosition( const JetProton& arg_jp )
{
  /* static */ const enum { x = 0, y, cdt, xp, yp, dpop };
  JetProton  myJP( arg_jp );
  Proton*    p_myP = (Proton*) myJP.ConvertToParticle();
  // This is deleted before returning.

  double x_i  = p_myP->State( x  );
  double xp_i = p_myP->State( xp );

  double inState [] = { 0, 0, 0, 0, 0, 0 };
  inState[x]  = x_i;
  inState[xp] = xp_i;

  double f, m, z;

  // Initialize the derivative...
  myJP  .setState( inState );
  this->propagate( myJP );

  m = ( myJP.State().Jacobian() )( xp, x );
  if( fabs(m) < 1.0e-12 ) {
    cerr << "*** ERROR ***                                       \n"
         << "*** ERROR *** CF_rbend::AdjustPosition              \n"
         << "*** ERROR *** A horrible, inexplicable error has    \n"
         << "*** ERROR *** occurred. A multi-valued solution     \n"
         << "*** ERROR *** is suspected.                         \n"
         << "*** ERROR ***                                       \n";
    exit(0);
  }
  m = 1.0 / m;

  // Initialize the difference ...
  z = x_i;
  inState[x] = z;
  p_myP->setState( inState );
  this->propagate( *p_myP );
  f = ( p_myP->State() )( xp ) + xp_i;

  int i = 0;
  while( (i++ < 10) && (fabs(f) > 1.0e-9) ) 
  {
    // One Newton's step ...
    z -= m*f;
    inState[x]  = z;

    // Recalculate inverse derivative ...
    myJP.setState( inState );
    this->propagate( myJP );
    m = ( myJP.State().Jacobian() )( xp, x );
    if( fabs(m) < 1.0e-12 ) {
      cerr << "*** ERROR ***                                       \n"
              "*** ERROR *** CF_rbend::AdjustPosition              \n"
              "*** ERROR *** A horrible, inexplicable error has    \n"
              "*** ERROR *** occurred at step "
                                          <<  i
                                          << " . A multi-valued solution \n"
              "*** ERROR *** is suspected.                         \n"
              "*** ERROR ***                                       \n";
      exit(0);
    }
    m = 1.0 / m;

    // Recalculate difference ...
    p_myP->setState( inState );
    this->propagate( *p_myP );
    f = ( p_myP->State() )( xp ) + xp_i;
  }


  // Step procedure when Newton's method fails ...
  if( i >= 10 ) {
    cerr << "*** WARNING ***                                      \n"
            "*** WARNING *** CF_rbend::AdjustPosition             \n"
            "*** WARNING *** No convergence within 10 Newton      \n"
            "*** WARNING *** iterations for magnet "
         <<                               this->Name() 
         <<                           ". Proceeding to find       \n"
            "*** WARNING *** best solution by stepping.           \n"
            "*** WARNING ***                                      \n"
         << endl;

    inState[x] = 0.0;
    double delta = 1.0e-4;           // step 0.1 mm

    p_myP->setState( inState );
    this->propagate( *p_myP );
    double error = ( p_myP->State() )( xp ) + xp_i;

    inState[x] = delta;
    p_myP->setState( inState );
    this->propagate( *p_myP );
    f = ( p_myP->State() )( xp ) + xp_i;

    if(      ( ( f >= 0.0 && error >= 0.0 ) || ( f <= 0.0 && error <= 0.0 ) ) 
          && ( fabs(error) < fabs(f) ) ) 
    {
      delta = - delta;
    }

    inState[x] = 0.0;
    while( fabs(delta) > 0.9e-6 ) {
      inState[x] += delta;
      p_myP->setState( inState );
      this->propagate( *p_myP );
      f = ( p_myP->State() )( xp ) + xp_i;

      while( ( ( f <= 0.0 && error <= 0.0 ) || ( f >= 0.0 && error >= 0.0 ) ) && 
             ( fabs(f) < fabs(error) ) )
      {
        error = f;
        inState[x] += delta;
      	p_myP->setState( inState );
      	this->propagate( *p_myP );
      	f = ( p_myP->State() )( xp ) + xp_i;
      }

      inState[x] -= delta;
      delta      *= (-0.1);
    }

    cerr << "*** WARNING ***                                      \n"
            "*** WARNING *** CF_rbend::AdjustPosition             \n"
            "*** WARNING *** The step procedure suggests a best   \n"
            "*** WARNING *** solution with magnet displacement "
         <<                                  (-1000.0)*inState[x]
         <<                                           " mm with   \n"
            "*** WARNING *** bend angle error "
         <<                                   (2.0e6)*error
         <<                                       " microradians. \n"
            "*** WARNING ***                                      \n"
         << endl;

    z = inState[x];
  }


  // Set the alignment of the internal beamline.
  // this->align->getAlignment().xOffset -= z;
  alignmentData v;	// Assumes zero alignment constructed this way.
  if ( align != 0 )
    v = align->getAlignment();
  v.xOffset -= z;
  // ??? Does not work: p_bml->setAlignment( v );
  // ??? The reason is that the alignment stategy is
  // ??? not correct for elements whose faces are not
  // ??? parallel.
  this->setAlignment( v );
  // ??? This will work only if the in and out faces
  // ??? of the CF_rbend element are parallel.


  // Clean up and return.
  delete p_myP;
  return z;
}
