/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      CF_sbend.cc
******                                                                
******  Copyright (c) 1991 Universities Research Association, Inc.    
******                All Rights Reserved                             
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
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iomanip>

#include "PhysicsConstants.h"
#include "CF_sbend.h"
#include "quadrupole.h"
#include "sextupole.h"
#include "sbend.h"
#include "octupole.h"
#include "Particle.h"

using namespace std;

CF_sbend::CF_sbend( double        lng,  // length     [ meter    ]
                    double        fld,  // field      [ tesla    ]
                    double        ang,  // bend angle [ radians  ]
                    int )
: bmlnElmnt( lng, fld )
  , _angle(ang)
  , _usEdgeAngle(0.0)
  , _dsEdgeAngle(0.0)
  , _usAngle(0.0)
  , _dsAngle(0.0)
  , _usTan(0.0)
  , _dsTan(0.0)
{
  _finishConstructor();
}


CF_sbend::CF_sbend( const char*   nm,   // name
                    double        lng,  // length     [ meter    ]
                    double        fld,  // field      [ tesla    ]
                    double        ang,  // angle      [ radians  ]
                    int )
: bmlnElmnt( nm, lng, fld )
  , _angle(ang)
  , _usEdgeAngle(0.0)
  , _dsEdgeAngle(0.0)
  , _usAngle(0.0)
  , _dsAngle(0.0)
  , _usTan(0.0)
  , _dsTan(0.0)
{
  _finishConstructor();
}


CF_sbend::CF_sbend( double        lng,  // length     [ meter    ]
                    double        fld,  // field      [ tesla    ]
                    double        ang,  // bend angle [ radians ]
                    double        us,   // upstream edge angle [radians]
                    double        ds,   // downstream edge angle [radians]
                                        // signs of previous two parameters
                                        // are as defined for sbends by MAD
                    int           n  )  // number of blocks: 4n+1 bends + 2(4n) multipoles
: bmlnElmnt( lng, fld )
  , _angle(ang)
  , _usEdgeAngle(us)
  , _dsEdgeAngle(ds)
  , _usAngle(us)
  , _dsAngle(-ds)
  , _usTan(tan(us))
  , _dsTan(-tan(ds))
{
  _finishConstructor();
}


CF_sbend::CF_sbend( const char*   nm,   // name
                    double        lng,  // length     [ meter    ]
                    double        fld,  // field      [ tesla    ]
                    double        ang,  // bend angle [ radians ]
                    double        us,   // upstream edge angle [radians]
                    double        ds,   // downstream edge angle [radians]
                                        // signs of previous two parameters
                                        // are as defined for sbends by MAD
                    int           n  )  // number of blocks: 4n+1 bends + 2(4n) multipoles
: bmlnElmnt( nm, lng, fld )
  , _angle(ang)
  , _usEdgeAngle(us)
  , _dsEdgeAngle(ds)
  , _usAngle(us)
  , _dsAngle(-ds)
  , _usTan(tan(us))
  , _dsTan(-tan(ds))
{
  _finishConstructor();
}


CF_sbend::CF_sbend( const CF_sbend& x )
: bmlnElmnt( (const bmlnElmnt&) x )
  , _angle(x._angle)
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


void CF_sbend::_finishConstructor()
{
// Insertion for CF_sbend constructors
// 
  double field       = this->strength;
  double frontLength =  6.0*(this->length/4.0)/15.0;
  double sepLength   = 16.0*(this->length/4.0)/15.0;

  sbend inEdge  ( frontLength,     field, (frontLength/this->length)*_angle, 
                  _usEdgeAngle, 0.0, &sbend::InEdge  );
  sbend outEdge ( frontLength,     field, (frontLength/this->length)*_angle, 
                  0.0, _dsEdgeAngle, &sbend::OutEdge );
  sbend body    ( sepLength,   field, (sepLength/this->length)*_angle,
                  0.0,          0.0, &sbend::NoEdge );

  thinSextupole ts( 0.0 );
  thinQuad      tq( 0.0 );

  _u = new bmlnElmnt* [ 13 ];
  _v = _u;

  *(_v++) = new sbend          ( inEdge    );
  *(_v++) = new thinSextupole  ( ts      );
  *(_v++) = new thinQuad       ( tq      );
  *(_v++) = new sbend          ( body    );
  *(_v++) = new thinSextupole  ( ts      );
  *(_v++) = new thinQuad       ( tq      );
  *(_v++) = new sbend          ( body    );
  *(_v++) = new thinSextupole  ( ts      );
  *(_v++) = new thinQuad       ( tq      );
  *(_v++) = new sbend          ( body    );
  *(_v++) = new thinSextupole  ( ts      );
  *(_v++) = new thinQuad       ( tq      );
  *(_v  ) = new sbend          ( outEdge    );
}


CF_sbend::~CF_sbend()
{
  // NOTE: If this code is ever modified, you 
  // must also modify CF_sbend::readFrom.
  while( _v >= _u ) {
    delete (*(_v--));
  }
  delete [] _u;
}


void CF_sbend::acceptInner( BmlVisitor& v )
{
  _ctRef = 0.0;
  bmlnElmnt** x = _u;
  while( x <= _v ) {
    (*x)->accept( v );
    _ctRef += (*x)->getReferenceTime();
    x++;
  }
}


void CF_sbend::acceptInner( ConstBmlVisitor& v )
{
  _ctRef = 0.0;
  bmlnElmnt** x = _u;
  while( x <= _v ) {
    (*x)->accept( v );
    _ctRef += (*x)->getReferenceTime();
    x++;
  }
}


void CF_sbend::peekAt( double& s, Particle* p_prt )
{
 cout << setw(12) << s;
 s += OrbitLength( *p_prt );
 cout << setw(12) << s           
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


void CF_sbend::localPropagate( Particle& p )
{
  bmlnElmnt** x = _u;
  while( x <= _v ) {
    (*(x++))->localPropagate( p );
  }
}


void CF_sbend::localPropagate( JetParticle& p )
{
  bmlnElmnt** x = _u;
  while( x <= _v ) {
    (*(x++))->localPropagate( p );
  }
}


double sbend::setEntryAngle( const Particle& p )
{
  return this->setEntryAngle( atan2( p.get_npx(), p.get_npz() ) );
  // i.e. tan(phi) = px/pz, where pz = longitudinal momentum
}


double sbend::setExitAngle( const Particle& p )
{
  return this->setExitAngle( atan2( p.get_npx(), p.get_npz() ) );
  // i.e. tan(phi) = px/pz, where pz = longitudinal momentum
}


double sbend::setEntryAngle( double phi /* radians */ )
{
  double ret = _usAngle;
  _usAngle = phi;
  _usTan = tan(phi);
  return ret;
}


double sbend::setExitAngle( double phi /* radians */ )
{
  double ret = _dsAngle;
  _dsAngle = phi;  
  _dsTan  = tan(phi);
  return ret;
}


int CF_sbend::setOctupole( double arg_x )
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

int CF_sbend::setSextupole( double arg_x )
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

int CF_sbend::setQuadrupole( double arg_x )
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


int CF_sbend::setDipoleField( double arg_x )
{
  this->strength = arg_x;

  int m = 1 + ( ( int(_v) - int(_u) )/sizeof( bmlnElmnt* ) );
  sbend** w = new sbend* [ m ];
  int counter = -1;
  bmlnElmnt** x = _u;

  while( x <= _v ) {
    if( 0 == strcmp( (*x)->Type(), "sbend" ) ) {
      // w[++counter] = dynamic_cast<sbend*>(*x);
      w[++counter] = (sbend*)(*x);
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


double CF_sbend::getOctupole() const
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


double CF_sbend::getSextupole() const
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

double CF_sbend::getQuadrupole() const
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


double CF_sbend::getDipoleField() const
{
  return this->strength;
}


double CF_sbend::getBendAngle() const
{
  return _angle;
}


void CF_sbend::Split( double pc, bmlnElmnt** a, bmlnElmnt** b )
{
  static bool firstTime = true;
  if( firstTime ) {
    firstTime = false;
    cerr << "\n"
            "\n*** WARNING ***"
            "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** void sbend::Split( double pc, bmlnElmnt** a, bmlnElmnt** b )"
            "\n*** WARNING *** The new, split elements must be commissioned with"
            "\n*** WARNING *** RefRegVisitor before being used."
            "\n*** WARNING *** "
         << endl;
    cerr << "\n*** WARNING ***                             "
            "\n*** WARNING *** void CF_sbend::Split        "
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
           "void CF_sbend::Split( double pc, bmlnElmnt** a, bmlnElmnt** b )", 
           uic.str().c_str() ) );
  }


  // We assume "strength" means field, not field*length.
  // "length," "strength," and "_angle" are private data members.
  *a = new CF_sbend(         pc*length, strength,         pc*_angle, _usEdgeAngle, 0.0 );
  *b = new CF_sbend( (1.0 - pc)*length, strength, (1.0 - pc)*_angle, 0.0, _dsEdgeAngle );


  // Assign quadrupole strength
  double quadStrength = this->getQuadrupole();  
  // quadStrength = B'l

  ((CF_sbend*) *a)->setQuadrupole( pc*quadStrength );
  ((CF_sbend*) *b)->setQuadrupole( (1.0 - pc)*quadStrength );


  // Rename
  char* newname;
  newname = new char [ strlen(ident) + 6 ];

  strcpy( newname, ident );
  strcat( newname, "_1" );
  (*a)->Rename( newname );

  strcpy( newname, ident );
  strcat( newname, "_2" );
  (*b)->Rename( newname );

  delete [] newname;
}


ostream& CF_sbend::writeTo( ostream& os )
{
  os << OSTREAM_DOUBLE_PREC << _angle << " ";
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


istream& CF_sbend::readFrom( istream& is )
{
  double quadStrength = 0.0;  // Assignment mitigates 
  double sextStrength = 0.0;  // obnoxious warning message
  double octStrength  = 0.0;  // from compiler.


  is >> ( this->_angle ) 
     >> ( this->_usEdgeAngle )
     >> ( this->_dsEdgeAngle )
     >> ( this->_usAngle )
     >> ( this->_dsAngle )
     >> quadStrength 
     >> sextStrength
     >> octStrength;

  _usTan = tan(_usAngle);
  _dsTan = tan(_dsAngle);

  // Rebuild basic element ...
  // ... First deconstruct (identical to CF_sbend destructor)
  while( _v >= _u ) {
    delete (*(_v--));
  }
  delete [] _u;
  _v = 0;
  _u = 0;

  // ... Then reconstruct
  _finishConstructor();


  // Set multipoles
  this->setQuadrupole( quadStrength );
  this->setSextupole( sextStrength );
  this->setOctupole( octStrength );

  return is;
}


void CF_sbend::eliminate( void )
{
  delete this;
}


const char* CF_sbend::Type() const  
{ 
  return "CF_sbend"; 
}


double CF_sbend::OrbitLength( const Particle& x )
{
  return length;
}
