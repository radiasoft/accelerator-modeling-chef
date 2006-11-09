/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      CF_rbend.cc
******  Version:   3.0
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws.
******                                                                
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
******                                                                
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iomanip>

#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/MathConstants.h>
#include <basic_toolkit/PhysicsConstants.h>
#include <beamline/CF_rbend.h>
#include <beamline/quadrupole.h>
#include <beamline/sextupole.h>
#include <beamline/rbend.h>
#include <beamline/octupole.h>
#include <beamline/Particle.h>
#include <beamline/Alignment.h>


using namespace std;

using FNAL::pcout;
using FNAL::pcerr;


CF_rbend::CF_rbend()
: bmlnElmnt( 1.0, 0.0 )
  , _usEdgeAngle(0.0)
  , _dsEdgeAngle(0.0)
  , _usAngle(M_TWOPI)
  , _dsAngle(-M_TWOPI)
  , _usTan(0.0)
  , _dsTan(0.0)
{
  _finishConstructor(1);
}


CF_rbend::CF_rbend( double        lng,  // length      [ meter    ]
                    double        fld,  // field       [ tesla    ]
                    int           n  )  // number of blocks: 4n+1 bends + 2(4n) multipoles
: bmlnElmnt( lng, fld )
  , _usEdgeAngle(0.0)
  , _dsEdgeAngle(0.0)
  , _usAngle(M_TWOPI)
  , _dsAngle(-M_TWOPI)
  , _usTan(0.0)
  , _dsTan(0.0)
{
  _finishConstructor(n);
}


CF_rbend::CF_rbend( const char*   nm,   // name
                    double        lng,  // length      [ meter    ]
                    double        fld,  // field       [ tesla    ]
                    int           n  )  // number of blocks: 4n+1 bends + 2(4n) multipoles
: bmlnElmnt( nm, lng, fld )
  , _usEdgeAngle(0.0)
  , _dsEdgeAngle(0.0)
  , _usAngle(M_TWOPI)
  , _dsAngle(-M_TWOPI)
  , _usTan(0.0)
  , _dsTan(0.0)
{
  _finishConstructor(n);
}


CF_rbend::CF_rbend( double        lng,  // length      [ meter    ]
                    double        fld,  // field       [ tesla    ]
                    double        ang,  // entry angle [ radians  ]
                    int           n  )  // number of blocks: 4n+1 bends + 2(4n) multipoles
: bmlnElmnt( lng, fld )
  , _usEdgeAngle(0.0)
  , _dsEdgeAngle(0.0)
  , _usAngle(ang)
  , _dsAngle(-ang)
  , _usTan(tan(ang))
  , _dsTan(-tan(ang))
{
  _finishConstructor(n);
}


CF_rbend::CF_rbend( const char*   nm,   // name
                    double        lng,  // length      [ meter    ]
                    double        fld,  // field       [ tesla    ]
                    double        ang,  // entry angle [ radians  ]
                    int           n  )  // number of blocks: 4n+1 bends + 2(4n) multipoles
: bmlnElmnt( nm, lng, fld )
  , _usEdgeAngle(0.0)
  , _dsEdgeAngle(0.0)
  , _usAngle(ang)
  , _dsAngle(-ang)
  , _usTan(tan(ang))
  , _dsTan(-tan(ang))
{
  _finishConstructor(n);
}


CF_rbend::CF_rbend( double        lng,  // length     [ meter    ]
                    double        fld,  // field      [ tesla    ]
                    double        us,   // upstream edge angle [radians]
                    double        ds,   // downstream edge angle [radians]
                                        // signs of previous two parameters
                                        // are as defined for rbends by MAD
                    int           n  )  // number of blocks: 4n+1 bends + 2(4n) multipoles
: bmlnElmnt( lng, fld )
  , _usEdgeAngle(us)
  , _dsEdgeAngle(ds)
  , _usAngle(M_TWOPI)
  , _dsAngle(-M_TWOPI)
  , _usTan(0.0)
  , _dsTan(0.0)
{
  _finishConstructor(n);
}


CF_rbend::CF_rbend( const char*   nm,   // name
                    double        lng,  // length     [ meter    ]
                    double        fld,  // field      [ tesla    ]
                    double        us,   // upstream edge angle [radians]
                    double        ds,   // downstream edge angle [radians]
                                        // signs of previous two parameters
                                        // are as defined for rbends by MAD
                    int           n  )  // number of blocks: 4n+1 bends + 2(4n) multipoles
: bmlnElmnt( nm, lng, fld )
  , _usEdgeAngle(us)
  , _dsEdgeAngle(ds)
  , _usAngle(M_TWOPI)
  , _dsAngle(-M_TWOPI)
  , _usTan(0.0)
  , _dsTan(0.0)
{
  _finishConstructor(n);
}


CF_rbend::CF_rbend( double        lng,  // length      [meter]
                    double        fld,  // field       [tesla]
                    double        ang,  // entry angle [radians]
                    double        us,   // upstream edge angle [radians]
                    double        ds,   // downstream edge angle [radians]
                                        // signs of previous two parameters
                                        // are as defined for rbends by MAD
                    int           n  )  // number of blocks: 4n+1 bends + 2(4n) multipoles
: bmlnElmnt( lng, fld )
  , _usEdgeAngle(us)
  , _dsEdgeAngle(ds)
  , _usAngle(ang + us)
  , _dsAngle(-(ang + ds))
  , _usTan(tan(ang + us))
  , _dsTan(-tan(ang + ds))
{
  _finishConstructor(n);
}


CF_rbend::CF_rbend( const char*   nm,   // name
                    double        lng,  // length      [meter]
                    double        fld,  // field       [tesla]
                    double        ang,  // entry angle [radians]
                    double        us,   // upstream edge angle [radians]
                    double        ds,   // downstream edge angle [radians]
                                        // signs of previous two parameters
                                        // are as defined for rbends by MAD
                    int           n  )  // number of blocks: 4n+1 bends + 2(4n) multipoles
: bmlnElmnt( nm, lng, fld )
  , _usEdgeAngle(us)
  , _dsEdgeAngle(ds)
  , _usAngle(ang + us)
  , _dsAngle(-(ang + ds))
  , _usTan(tan(ang + us))
  , _dsTan(-tan(ang + ds))
{
  _finishConstructor(n);
}


CF_rbend::CF_rbend( const CF_rbend& x )
: bmlnElmnt( (const bmlnElmnt&) x )
  , _usEdgeAngle(x._usEdgeAngle)
  , _dsEdgeAngle(x._dsEdgeAngle)
  , _usAngle(x._usAngle)
  , _dsAngle(x._dsAngle)
  , _usTan(x._usTan)
  , _dsTan(x._dsTan)
{
  int m = 1 + ( ( int(x._v) - int(x._u) )/sizeof( bmlnElmnt* ) );
  _u = new bmlnElmnt* [ m ];
  _v = &( _u[m-1] );
  
  for( int k = 0; k < m; k++ ) {
    _u[k] = ( x._u[k] )->Clone();
  }
}


void CF_rbend::_finishConstructor(int n)
{
// NOTE: If this code is ever modified, you 
// must also modify CF_rbend::readFrom and
// CF_rbend::writeTo

// NOTE: n refers to the number of blocks of twelve pieces!

  if( n < 1 || n > 9 ) {
    (*pcerr) << "*** WARNING ***                                         \n"
            "*** WARNING *** CF_rbend constructor                    \n"
            "*** WARNING *** n = " << n << " is out of range [1-9].  \n"
            "*** WARNING *** Constructor is resetting n = 1.         \n"
            "*** WARNING ***                                         \n"
         << endl;
    n = 1;
  }

  double field       = this->strength;
  double frontLength =  (6.0*(this->length/4.0)/15.0)/((double) n);
  double sepLength   = (16.0*(this->length/4.0)/15.0)/((double) n);

  rbend inEdge    ( frontLength,     field, _usEdgeAngle, 0.0, &rbend::InEdge  );
  rbend outEdge   ( frontLength,     field, 0.0, _dsEdgeAngle, &rbend::OutEdge );
  rbend separator ( 2.0*frontLength, field, 0.0,          0.0, &rbend::NoEdge  );
  rbend body      ( sepLength,       field, 0.0,          0.0, &rbend::NoEdge  );

  thinSextupole ts( 0.0 );
  thinQuad      tq( 0.0 );

  _u = new bmlnElmnt* [ 121 ];    // Paranoia: should need only 109
  _v = _u;

  *(_v  ) = new rbend          ( inEdge  );
  dynamic_cast<rbend*>(*_v)->setEntryAngle( _usAngle );
    _v++;
  *(_v++) = new thinSextupole  ( ts      );
  *(_v++) = new thinQuad       ( tq      );
  *(_v++) = new rbend          ( body    );
  *(_v++) = new thinSextupole  ( ts      );
  *(_v++) = new thinQuad       ( tq      );
  *(_v++) = new rbend          ( body    );
  *(_v++) = new thinSextupole  ( ts      );
  *(_v++) = new thinQuad       ( tq      );
  *(_v++) = new rbend          ( body    );
  *(_v++) = new thinSextupole  ( ts      );
  *(_v++) = new thinQuad       ( tq      );

  for( int i = 1; i < n; i++ ) {
    *(_v++) = new rbend          ( separator );
    *(_v++) = new thinSextupole  ( ts      );
    *(_v++) = new thinQuad       ( tq      );
    *(_v++) = new rbend          ( body    );
    *(_v++) = new thinSextupole  ( ts      );
    *(_v++) = new thinQuad       ( tq      );
    *(_v++) = new rbend          ( body    );
    *(_v++) = new thinSextupole  ( ts      );
    *(_v++) = new thinQuad       ( tq      );
    *(_v++) = new rbend          ( body    );
    *(_v++) = new thinSextupole  ( ts      );
    *(_v++) = new thinQuad       ( tq      );
  }

  *(_v  ) = new rbend          ( outEdge );
  dynamic_cast<rbend*>(*_v)->setExitAngle( _dsAngle );

  // Paranoid test.
  if( (12*n + 1) != (1 + ( ( int(_v) - int(_u) )/sizeof( bmlnElmnt* ) )) ) {
    (*pcerr) << "*** ERROR ***                                         \n"
            "*** ERROR *** CF_rbend constructor                    \n"
            "*** ERROR *** Impossible! "
         << (11*n + 2)
         << " != " 
         << (1 + ( ( int(_v) - int(_u) )/sizeof( bmlnElmnt* ) ))
         << "\n*** ERROR *** Constructor is aborting program.        \n"
            "*** ERROR ***                                         \n"
         << endl;
    exit(9);
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


double CF_rbend::setEntryAngle( const Particle& p )
{
  return this->setEntryAngle( atan2( p.get_npx(), p.get_npz() ) );
  // i.e. tan(phi) = px/pz, where pz = longitudinal momentum
}


double CF_rbend::setExitAngle( const Particle& p )
{
  return this->setExitAngle( atan2( p.get_npx(), p.get_npz() ) );
  // i.e. tan(phi) = px/pz, where pz = longitudinal momentum
}


double CF_rbend::setEntryAngle( double phi /* radians */ )
{
  double ret = _usAngle;
  _usAngle = phi;
  _usTan = tan(phi);
  dynamic_cast<rbend*>(*_u)->setEntryAngle(phi);
  return ret;
}


double CF_rbend::setExitAngle( double phi /* radians */ )
{
  double ret = _dsAngle;
  _dsAngle = phi;  
  _dsTan  = tan(phi);
  dynamic_cast<rbend*>(*_v)->setExitAngle(phi);
  return ret;
}


int CF_rbend::setOctupole( double arg_x )
{
  int m = 1 + ( ( int(_v) - int(_u) )/sizeof( bmlnElmnt* ) );
  thinOctupole** w = new thinOctupole* [ m ];
  int counter = -1;
  bmlnElmnt** x = _u;

  while( x <= _v ) {
    if( 0 == strcmp( (*x)->Type(), "thinOctupole" ) ) {
      // w[++counter] = dynamic_cast<thinOctupole*>(*x);
      w[++counter] = (thinOctupole*)(*x);
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


bool CF_rbend::hasParallelFaces() const
{
  return ( std::abs( _usEdgeAngle + _dsEdgeAngle ) <  1.0e-9 );
}


bool CF_rbend::hasStandardFaces() const
{
  return ( (std::abs(_usEdgeAngle) < 1.0e-9) && (std::abs(_dsEdgeAngle) < 0.5e-9) );
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


int CF_rbend::setDipoleField( double arg_x )
{
  this->strength = arg_x;

  int m = 1 + ( ( int(_v) - int(_u) )/sizeof( bmlnElmnt* ) );
  rbend** w = new rbend* [ m ];
  int counter = -1;
  bmlnElmnt** x = _u;

  while( x <= _v ) {
    if( 0 == strcmp( (*x)->Type(), "rbend" ) ) {
      // w[++counter] = dynamic_cast<rbend*>(*x);
      w[++counter] = (rbend*)(*x);
    }
    x++;
  }

  if( counter < 0 ) {
    delete [] w;
    return 1;
  }
  
  for( int i = 0; i <= counter; i++ ) {
    w[i]->setStrength( this->strength );
  }
  
  delete [] w;
  return 0;
}


double CF_rbend::getOctupole() const
{
  int m = 1 + ( ( int(_v) - int(_u) )/sizeof( bmlnElmnt* ) );
  thinOctupole** w = new thinOctupole* [ m ];
  int counter = -1;
  bmlnElmnt** x = _u;

  while( x <= _v ) {
    if( 0 == strcmp( (*x)->Type(), "thinOctupole" ) ) {
      // w[++counter] = dynamic_cast<thinOctupole*>(*x);
      w[++counter] = (thinOctupole*)(*x);
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


double CF_rbend::getSextupole() const
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

double CF_rbend::getQuadrupole() const
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


double CF_rbend::getDipoleField() const
{
  return this->strength;
}


void CF_rbend::Split( double pc, bmlnElmnt** a, bmlnElmnt** b ) const
{
  static bool firstTime = true;
  if( firstTime ) {
    firstTime = false;
    (*pcerr) << "\n"
            "\n*** WARNING ***"
            "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** void rbend::Split( double pc, bmlnElmnt** a, bmlnElmnt** b )"
            "\n*** WARNING *** The new, split elements must be commissioned with"
            "\n*** WARNING *** RefRegVisitor before being used."
            "\n*** WARNING *** "
         << endl;
    (*pcerr) << "\n*** WARNING ***                             "
            "\n*** WARNING *** void CF_rbend::Split        "
            "\n*** WARNING *** Combined split elements     "
            "\n*** WARNING *** are not identical to the    "
            "\n*** WARNING *** original.                   "
            "\n*** WARNING ***                           \n"
         << endl;
  }

  if( ( pc <= 0.0 ) || ( pc >= 1.0 ) ) {
    ostringstream uic;
    uic << "Requested percentage = " << pc << "; should be in [0,1].";
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "void CF_rbend::Split( double pc, bmlnElmnt** a, bmlnElmnt** b )", 
           uic.str().c_str() ) );
  }


  // We assume "strength" means field, not field*length.
  // "length," "strength," and "_angle" are private data members.
  *a = new CF_rbend(         pc*length, strength, _usEdgeAngle, 0.0 );
  dynamic_cast<CF_rbend*>(*a)->setEntryAngle( this->getEntryAngle() );
  dynamic_cast<CF_rbend*>(*a)->setExitAngle( 0.0 );    // Will matter
  *b = new CF_rbend( (1.0 - pc)*length, strength, 0.0, _dsEdgeAngle );
  dynamic_cast<CF_rbend*>(*b)->setEntryAngle( 0.0 );   // Will matter
  dynamic_cast<CF_rbend*>(*b)->setExitAngle( this->getExitAngle() );


  // Assign quadrupole strength
  double quadStrength = this->getQuadrupole();  
  // quadStrength = B'l

  ((CF_rbend*) *a)->setQuadrupole( pc*quadStrength );
  ((CF_rbend*) *b)->setQuadrupole( (1.0 - pc)*quadStrength );


  // Rename
  char* newname;
  newname = new char [ strlen(ident) + 6 ];

  strcpy( newname, ident );
  strcat( newname, "_1" );
  (*a)->rename( newname );

  strcpy( newname, ident );
  strcat( newname, "_2" );
  (*b)->rename( newname );

  delete [] newname;
}


void CF_rbend::acceptInner( BmlVisitor& v )
{
  _ctRef = 0.0;
  bmlnElmnt** x = _u;
  while( x <= _v ) {
    (*x)->accept( v );
    _ctRef += (*x)->getReferenceTime();
    x++;
  }
}


void CF_rbend::acceptInner( ConstBmlVisitor& v )
{
  _ctRef = 0.0;
  bmlnElmnt** x = _u;
  while( x <= _v ) {
    (*x)->accept( v );
    _ctRef += (*x)->getReferenceTime();
    x++;
  }
}


void CF_rbend::peekAt( double& s, const Particle& prt ) const
{
 (*pcout) << setw(12) << s;
 s += OrbitLength( prt );
 (*pcout) << setw(12) << s           
                  << " : " 
      << setw(10) << (int) this  
      << setw(15) << ident       
      << setw(15) << Type()      
      << setw(12) << length      
      << setw(12) << strength    
      << setw(12) << ((this->getQuadrupole())/length)
      << setw(12) << (2.0*(this->getSextupole())/length)
      << setw(12) << (6.0*(this->getOctupole())/length)
      << setw(12) << shuntCurrent
      << endl;
}


ostream& CF_rbend::writeTo( ostream& os )
{
  int n = ( ( int(_v) - int(_u) )/sizeof( bmlnElmnt* ) );
  if( 0 != n%12 ) {
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "ostream& CF_rbend::writeTo( ostream& os )", 
           "Unexpected number of blocks." ) );
  }

  os << (n/12) << "  ";
  os << OSTREAM_DOUBLE_PREC << _usEdgeAngle << " "
     << OSTREAM_DOUBLE_PREC << _dsEdgeAngle << endl;
  os << OSTREAM_DOUBLE_PREC << _usAngle << " "
     << OSTREAM_DOUBLE_PREC << _dsAngle << endl;
  os << OSTREAM_DOUBLE_PREC << getQuadrupole() << " ";
  os << OSTREAM_DOUBLE_PREC << getSextupole() << " ";
  os << OSTREAM_DOUBLE_PREC << getOctupole() << " ";
  os << "\n";
  return os;
}


istream& CF_rbend::readFrom( istream& is )
{
  double quadStrength = 0.0;
  double sextStrength = 0.0;
  double octStrength  = 0.0;
  int    n            = 0;

  is >> n
     >> (this->_usEdgeAngle)
     >> (this->_dsEdgeAngle)
     >> (this->_usAngle)
     >> (this->_dsAngle)
     >> quadStrength 
     >> sextStrength
     >> octStrength;

  _usTan = tan(_usAngle);
  _dsTan = tan(_dsAngle);

  // Rebuild basic element ...
  // ... First deconstruct (identical to CF_rbend destructor)
  while( _v >= _u ) {
    delete (*(_v--));
  }
  delete [] _u;
  _v = 0;
  _u = 0;

  // ... Then reconstruct
  _finishConstructor(n);


  // Set multipoles
  this->setQuadrupole( quadStrength );
  this->setSextupole( sextStrength );


  return is;
}


const char* CF_rbend::Type() const  
{ 
  return "CF_rbend"; 
}


bool CF_rbend::isMagnet() const
{
  return true;
}


double CF_rbend::OrbitLength( const Particle& x )
{
  double tworho = 2.0 * ( x.Momentum() / PH_CNV_brho_to_p ) / strength;
  return tworho * asin( length / tworho );
}


void CF_rbend::eliminate( void )
{
  delete this;
}


double CF_rbend::AdjustPosition( const Particle& arg_p )
{
  JetParticle* myJPPtr = arg_p.ConvertToJetParticle();
  // This probably won't work properly.
  double ret = AdjustPosition( *myJPPtr );
  delete myJPPtr;
  return ret;
}


double CF_rbend::AdjustPosition( const JetParticle& arg_jp )
{
  if( ( 0.0 != _usEdgeAngle ) || ( 0.0 != _dsEdgeAngle ) ) {
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "double CF_rbend::AdjustPosition( const JetParticle& arg_jp )", 
           "In this version: only implemented for parallel faces." ) );
  }

  enum { x = 0, y, cdt, xp, yp, dpop };

  JetParticle* myJPPtr = arg_jp.Clone();
  Particle*      p_myP = myJPPtr->ConvertToParticle();
  // These are deleted before returning.

  double x_i  = p_myP->State( x  );
  double xp_i = p_myP->State( xp );

  double inState [] = { 0, 0, 0, 0, 0, 0 };
  inState[x]  = x_i;
  inState[xp] = xp_i;

  double f, m, z;

  // Initialize the derivative...
  myJPPtr->setState( inState );
  this->propagate( *myJPPtr );

  m = ( myJPPtr->State().Jacobian() )( xp, x );
  if( fabs(m) < 1.0e-12 ) {
    delete p_myP;   p_myP   = 0;
    delete myJPPtr; myJPPtr = 0;
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "double CF_rbend::AdjustPosition( const JetParticle& arg_jp )", 
           "Horrible, inexplicable error: a multi-valued solution is suspected." ) );
  }
  m = 1.0 / m;

  // Initialize the difference ...
  z = x_i;
  inState[x] = z;
  p_myP->setState( inState );
  this->propagate( *p_myP );
  f = ( p_myP->State() )( xp ) + xp_i;

  int i = 0;
  while( ( i < 5 ) || ( ( i < 15 ) && (fabs(f) > 1.0e-9) ) ) 
  {
    i++;

    // One Newton's step ...
    z -= m*f;
    inState[x]  = z;

    // Recalculate inverse derivative ...
    myJPPtr->setState( inState );
    this->propagate( *myJPPtr );
    m = ( myJPPtr->State().Jacobian() )( xp, x );
    if( fabs(m) < 1.0e-12 ) {
      delete p_myP;   p_myP   = 0;
      delete myJPPtr; myJPPtr = 0;
      throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
             "double CF_rbend::AdjustPosition( const JetParticle& arg_jp )", 
             "Horrible, inexplicable error: a multi-valued solution is suspected." ) );
    }
    m = 1.0 / m;

    // Recalculate difference ...
    p_myP->setState( inState );
    this->propagate( *p_myP );
    f = ( p_myP->State() )( xp ) + xp_i;
  }


  // Step procedure when Newton's method fails ...
  if( i >= 15 ) {
    (*pcerr) << "*** WARNING ***                                      \n"
            "*** WARNING *** CF_rbend::AdjustPosition             \n"
            "*** WARNING *** No convergence within 15 Newton      \n"
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

    (*pcerr) << "*** WARNING ***                                      \n"
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
  delete p_myP;   p_myP   = 0;
  delete myJPPtr; myJPPtr = 0;

  return z;
}
