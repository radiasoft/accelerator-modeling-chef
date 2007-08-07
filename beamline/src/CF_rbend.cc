/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      CF_rbend.cc
******  Version:   3.1
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
****** REVISION HISTORY
******
****** Mar 2007           ostiguy@fnal.gov
****** - support for reference counted elements
****** - reduced src file coupling due to visitor interface. 
******   visit() takes advantage of (reference) dynamic type.
****** - use std::string for string operations. 
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
#include <beamline/JetParticle.h>
#include <beamline/Alignment.h>


using namespace std;

using FNAL::pcout;
using FNAL::pcerr;


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_rbend::CF_rbend()
: bmlnElmnt( 0.0, 0.0 )
  , usEdgeAngle_(0.0)
  , dsEdgeAngle_(0.0)
  ,     usAngle_(M_TWOPI)
  ,     dsAngle_(-M_TWOPI)
  ,       usTan_(0.0)
  ,       dsTan_(0.0)
{
  finishConstructor(1);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_rbend::CF_rbend( double const&        lng,  // length      [ meter    ]
                    double const&        fld,  // field       [ tesla    ]
                    int           n  )  // number of blocks: 4n+1 bends + 2(4n) multipoles
: bmlnElmnt( lng, fld )
  ,  usEdgeAngle_(0.0)
  ,  dsEdgeAngle_(0.0)
  ,      usAngle_(M_TWOPI)
  ,      dsAngle_(-M_TWOPI)
  ,        usTan_(0.0)
  ,        dsTan_(0.0)
{
  finishConstructor(n);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_rbend::CF_rbend( const char*   nm,   // name
                    double const&        lng,  // length      [ meter    ]
                    double const&        fld,  // field       [ tesla    ]
                    int           n  )  // number of blocks: 4n+1 bends + 2(4n) multipoles
: bmlnElmnt( nm, lng, fld )
  ,  usEdgeAngle_(0.0)
  ,  dsEdgeAngle_(0.0)
  ,      usAngle_(M_TWOPI)
  ,      dsAngle_(-M_TWOPI)
  ,        usTan_(0.0)
  ,        dsTan_(0.0)
{
  finishConstructor(n);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_rbend::CF_rbend( double const&        lng,  // length      [ meter    ]
                    double const&        fld,  // field       [ tesla    ]
                    double const&        ang,  // entry angle [ radians  ]
                    int           n  )  // number of blocks: 4n+1 bends + 2(4n) multipoles
: bmlnElmnt( lng, fld )
  ,  usEdgeAngle_(0.0)
  ,  dsEdgeAngle_(0.0)
  ,      usAngle_(ang)
  ,      dsAngle_(-ang)
  ,        usTan_(tan(ang))
  ,        dsTan_(-tan(ang))
{
  finishConstructor(n);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_rbend::CF_rbend( const char*   nm,   // name
                    double const&        lng,  // length      [ meter    ]
                    double const&        fld,  // field       [ tesla    ]
                    double const&        ang,  // entry angle [ radians  ]
                    int           n  )  // number of blocks: 4n+1 bends + 2(4n) multipoles
: bmlnElmnt( nm, lng, fld )
  ,usEdgeAngle_(0.0)
  ,dsEdgeAngle_(0.0)
  ,    usAngle_(ang)
  ,    dsAngle_(-ang)
  ,      usTan_(tan(ang))
  ,      dsTan_(-tan(ang))
{
  finishConstructor(n);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_rbend::CF_rbend( double const&        lng,  // length     [ meter    ]
                    double const&        fld,  // field      [ tesla    ]
                    double const&        us,   // upstream edge angle [radians]
                    double const&        ds,   // downstream edge angle [radians]
                                        // signs of previous two parameters
                                        // are as defined for rbends by MAD
                    int           n  )  // number of blocks: 4n+1 bends + 2(4n) multipoles
: bmlnElmnt( lng, fld )
  ,  usEdgeAngle_(us)
  ,  dsEdgeAngle_(ds)
  ,      usAngle_(M_TWOPI)
  ,      dsAngle_(-M_TWOPI)
  ,        usTan_(0.0)
  ,        dsTan_(0.0)
{
   finishConstructor(n);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_rbend::CF_rbend( const char*   nm,   // name
                    double const&        lng,  // length     [ meter    ]
                    double const&        fld,  // field      [ tesla    ]
                    double const&        us,   // upstream edge angle [radians]
                    double const&        ds,   // downstream edge angle [radians]
                                        // signs of previous two parameters
                                        // are as defined for rbends by MAD
                    int           n  )  // number of blocks: 4n+1 bends + 2(4n) multipoles
: bmlnElmnt( nm, lng, fld )
  , usEdgeAngle_(us)
  , dsEdgeAngle_(ds)
  ,     usAngle_(M_TWOPI)
  ,     dsAngle_(-M_TWOPI)
  ,       usTan_(0.0)
  ,       dsTan_(0.0)
{
   finishConstructor(n);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_rbend::CF_rbend( double const&        lng,  // length      [meter]
                    double const&        fld,  // field       [tesla]
                    double const&        ang,  // entry angle [radians]
                    double const&        us,   // upstream edge angle [radians]
                    double const&        ds,   // downstream edge angle [radians]
                                        // signs of previous two parameters
                                        // are as defined for rbends by MAD
                    int           n  )  // number of blocks: 4n+1 bends + 2(4n) multipoles
: bmlnElmnt( lng, fld )
  ,  usEdgeAngle_(us)
  ,  dsEdgeAngle_(ds)
  ,      usAngle_(ang + us)
  ,      dsAngle_(-(ang + ds))
  ,        usTan_(tan(ang + us))
  ,        dsTan_(-tan(ang + ds))
{
  finishConstructor(n);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_rbend::CF_rbend( const char*   nm,   // name
                    double const&        lng,  // length      [meter]
                    double const&        fld,  // field       [tesla]
                    double const&        ang,  // entry angle [radians]
                    double const&        us,   // upstream edge angle [radians]
                    double const&        ds,   // downstream edge angle [radians]
                                        // signs of previous two parameters
                                        // are as defined for rbends by MAD
                    int           n  )  // number of blocks: 4n+1 bends + 2(4n) multipoles
: bmlnElmnt( nm, lng, fld )
  ,  usEdgeAngle_(us)
  ,  dsEdgeAngle_(ds)
  ,      usAngle_(ang + us)
  ,      dsAngle_(-(ang + ds))
  ,        usTan_(tan(ang + us))
  ,        dsTan_(-tan(ang + ds))
{
  finishConstructor(n);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


CF_rbend::CF_rbend( CF_rbend const& x )
: bmlnElmnt( x )
  ,  usEdgeAngle_(x.usEdgeAngle_)
  ,  dsEdgeAngle_(x.dsEdgeAngle_)
  ,      usAngle_(x.usAngle_)
  ,      dsAngle_(x.dsAngle_)
  ,        usTan_(x.usTan_)
  ,        dsTan_(x.dsTan_)
{
  int m = 1 + ( ( int(x.v_) - int(x.u_) )/sizeof( bmlnElmnt* ) );
  u_ = new bmlnElmnt* [ m ];
  v_ = &( u_[m-1] );
  
  for( int k = 0; k < m;  ++k) {
    u_[k] = ( x.u_[k] )->Clone();
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_rbend::finishConstructor(int n)
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

  double field       = this->strength_;
  double frontLength =  (6.0*(this->length_/4.0)/15.0)/((double) n);
  double sepLength   = (16.0*(this->length_/4.0)/15.0)/((double) n);

  rbend inEdge    ( frontLength,     field, usEdgeAngle_, 0.0, &rbend::InEdge  );
  rbend outEdge   ( frontLength,     field, 0.0, dsEdgeAngle_, &rbend::OutEdge );
  rbend separator ( 2.0*frontLength, field, 0.0,          0.0, &rbend::NoEdge  );
  rbend body      ( sepLength,       field, 0.0,          0.0, &rbend::NoEdge  );

  thinSextupole ts( 0.0 );
  thinQuad      tq( 0.0 );

  u_ = new bmlnElmnt* [ 121 ];    // Paranoia: should need only 109
  v_ = u_;

  *(v_  ) = new rbend          ( inEdge  );
  dynamic_cast<rbend*>(*v_)->setEntryAngle( usAngle_ );
    v_++;
  *(v_++) = new thinSextupole  ( ts      );
  *(v_++) = new thinQuad       ( tq      );
  *(v_++) = new rbend          ( body    );
  *(v_++) = new thinSextupole  ( ts      );
  *(v_++) = new thinQuad       ( tq      );
  *(v_++) = new rbend          ( body    );
  *(v_++) = new thinSextupole  ( ts      );
  *(v_++) = new thinQuad       ( tq      );
  *(v_++) = new rbend          ( body    );
  *(v_++) = new thinSextupole  ( ts      );
  *(v_++) = new thinQuad       ( tq      );

  for( int i = 1; i < n; i++ ) {
    *(v_++) = new rbend          ( separator );
    *(v_++) = new thinSextupole  ( ts      );
    *(v_++) = new thinQuad       ( tq      );
    *(v_++) = new rbend          ( body    );
    *(v_++) = new thinSextupole  ( ts      );
    *(v_++) = new thinQuad       ( tq      );
    *(v_++) = new rbend          ( body    );
    *(v_++) = new thinSextupole  ( ts      );
    *(v_++) = new thinQuad       ( tq      );
    *(v_++) = new rbend          ( body    );
    *(v_++) = new thinSextupole  ( ts      );
    *(v_++) = new thinQuad       ( tq      );
  }

  *(v_  ) = new rbend          ( outEdge );
  dynamic_cast<rbend*>(*v_)->setExitAngle( dsAngle_ );

  // Paranoid test.
  if( (12*n + 1) != (1 + ( ( int(v_) - int(u_) )/sizeof( bmlnElmnt* ) )) ) {
    (*pcerr) << "*** ERROR ***                                         \n"
            "*** ERROR *** CF_rbend constructor                    \n"
            "*** ERROR *** Impossible! "
         << (11*n + 2)
         << " != " 
         << (1 + ( ( int(v_) - int(u_) )/sizeof( bmlnElmnt* ) ))
         << "\n*** ERROR *** Constructor is aborting program.        \n"
            "*** ERROR ***                                         \n"
         << endl;
    exit(9);
  }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_rbend::~CF_rbend()
{
  // NOTE: If this code is ever modified, you 
  // must also modify CF_rbend::readFrom.
  while( v_ >= u_ ) {
    delete (*(v_--));
  }
  delete [] u_;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_rbend& CF_rbend::operator=( CF_rbend const& rhs)
{
  if ( &rhs == this ) return *this; 
              
  bmlnElmnt::operator=( rhs );

  usEdgeAngle_ =  rhs.usEdgeAngle_;  
  dsEdgeAngle_ =  rhs.dsEdgeAngle_;

  usAngle_     =  rhs.usAngle_;
  dsAngle_     =  rhs.dsAngle_;

  usTan_       =  rhs.usTan_;
  dsTan_       =  rhs.dsTan_;

  if  (!v_ ) { u_ = v_ = 0; return *this; } 

  int nelm = 1 + (rhs.v_ - rhs.u_)/sizeof(bmlnElmnt*);

  u_ = new bmlnElmnt*[nelm];
  
  for ( int i=0; i < nelm; ++i)
  {
    u_[i] = rhs.u_[i]->Clone();   
  }

  v_ = u_ + nelm-1;       
  
  return *this;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_rbend::localPropagate( Particle& p )
{
  bmlnElmnt** x = u_;
  while( x <= v_ ) {
    (*(x++))->localPropagate( p );
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_rbend::localPropagate( JetParticle& p )
{
  bmlnElmnt** x = u_;
  while( x <= v_ ) {
    (*(x++))->localPropagate( p );
  }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_rbend::setEntryAngle( const Particle& p )
{
  return this->setEntryAngle( atan2( p.get_npx(), p.get_npz() ) );
  // i.e. tan(phi) = px/pz, where pz = longitudinal momentum
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_rbend::setExitAngle( const Particle& p )
{
  return this->setExitAngle( atan2( p.get_npx(), p.get_npz() ) );
  // i.e. tan(phi) = px/pz, where pz = longitudinal momentum
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_rbend::setEntryAngle( double const& phi /* radians */ )
{
  double ret = usAngle_;
  usAngle_ = phi;
  usTan_   = tan(phi);
  dynamic_cast<rbend*>(*u_)->setEntryAngle(phi);
  return ret;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_rbend::setExitAngle( double const& phi /* radians */ )
{
  double ret = dsAngle_;
  dsAngle_ = phi;  
  dsTan_  = tan(phi);
  dynamic_cast<rbend*>(*v_)->setExitAngle(phi);
  return ret;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int CF_rbend::setOctupole( double const& arg_x )
{
  int m = 1 + ( ( int(v_) - int(u_) )/sizeof( bmlnElmnt* ) );
  thinOctupole** w = new thinOctupole* [ m ];
  int counter = -1;
  bmlnElmnt** x = u_;

  while( x <= v_ ) {
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

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int CF_rbend::setSextupole( double const& arg_x )
{
  int m = 1 + ( ( int(v_) - int(u_) )/sizeof( bmlnElmnt* ) );
  thinSextupole** w = new thinSextupole* [ m ];
  int counter = -1;
  bmlnElmnt** x = u_;

  while( x <= v_ ) {
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


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool CF_rbend::hasParallelFaces() const
{
  return ( std::abs( usEdgeAngle_ + dsEdgeAngle_ ) <  1.0e-9 );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool CF_rbend::hasStandardFaces() const
{
  return ( (std::abs(usEdgeAngle_) < 1.0e-9) && (std::abs(dsEdgeAngle_) < 0.5e-9) );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int CF_rbend::setQuadrupole( double const& arg_x )
{
  int m = 1 + ( ( int(v_) - int(u_) )/sizeof( bmlnElmnt* ) );
  thinQuad** w = new thinQuad* [ m ];
  int counter = -1;
  bmlnElmnt** x = u_;

  while( x <= v_ ) {
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


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int CF_rbend::setDipoleField( double const& arg_x )
{
  this->strength_ = arg_x;

  int m = 1 + ( ( int(v_) - int(u_) )/sizeof( bmlnElmnt* ) );
  rbend** w = new rbend* [ m ];
  int counter = -1;
  bmlnElmnt** x = u_;

  while( x <= v_ ) {
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
    w[i]->setStrength( this->strength_ );
  }
  
  delete [] w;
  return 0;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_rbend::setStrength( double const& s )
{
  double ratio = s / getDipoleField();
  bmlnElmnt** x = u_;
  while( x <= v_ ) {
    (*x)->setStrength( ratio*((*x)->Strength()) );
    ++x;
  }
  strength_ = s;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_rbend::getOctupole() const
{
  int m = 1 + ( ( int(v_) - int(u_) )/sizeof( bmlnElmnt* ) );
  thinOctupole** w = new thinOctupole* [ m ];
  int counter = -1;
  bmlnElmnt** x = u_;

  while( x <= v_ ) {
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
  for( int i=0; i <= counter; ++i) {
    ret += w[i]->Strength();
  }
  
  delete [] w;
  return ret;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_rbend::getSextupole() const
{
  int m = 1 + ( ( int(v_) - int(u_) )/sizeof( bmlnElmnt* ) );
  thinSextupole** w = new thinSextupole* [ m ];
  int counter = -1;
  bmlnElmnt** x = u_;

  while( x <= v_ ) {
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
  for( int i=0; i <= counter; ++i) {
    ret += w[i]->Strength();
  }
  
  delete [] w;
  return ret;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_rbend::getQuadrupole() const
{
  int m = 1 + ( ( int(v_) - int(u_) )/sizeof( bmlnElmnt* ) );
  thinQuad** w = new thinQuad* [ m ];
  int counter = -1;
  bmlnElmnt** x = u_;

  while( x <= v_ ) {
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


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_rbend::getDipoleField() const
{
  return strength_;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_rbend::Split( double const& pc, ElmPtr& a, ElmPtr& b ) const
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
           "void CF_rbend::Split( double const& pc, bmlnElmnt** a, bmlnElmnt** b )", 
           uic.str().c_str() ) );
  }


  // We assume "strength_" means field, not field*length_.
  // "length_," "strength_," and "_angle" are private data members.
 
  CF_rbend* p_a = 0;
  CF_rbend* p_b = 0;

  a =  CFRbendPtr( p_a = new CF_rbend(         pc*length_, strength_, usEdgeAngle_, 0.0 ) );
  p_a->setEntryAngle( this->getEntryAngle() );
  p_b->setExitAngle( 0.0 );    // Will matter

  b =  CFRbendPtr ( p_b =new CF_rbend( (1.0 - pc)*length_, strength_, 0.0, dsEdgeAngle_ ) );

  p_b->setEntryAngle( 0.0 );   // Will matter
  p_b->setExitAngle( this->getExitAngle() );


  // Assign quadrupole strength

  double quadStrength = getQuadrupole();  // quadStrength = B'l

  p_a->setQuadrupole( pc*quadStrength );
  p_b->setQuadrupole( (1.0 - pc)*quadStrength );


  p_a->rename( ident_ + string("_1") );
  p_b->rename( ident_ + string("_2"));

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_rbend::peekAt( double& s, Particle const& prt ) 
{
 (*pcout) << setw(12) << s;
 s += OrbitLength( prt );
 (*pcout) << setw(12) << s           
                  << " : " 
      << setw(10) << (int) this  
      << setw(15) << ident_       
      << setw(15) << Type()      
      << setw(12) << length_      
      << setw(12) << strength_    
      << setw(12) << ((this->getQuadrupole())/length_)
      << setw(12) << (2.0*(this->getSextupole())/length_)
      << setw(12) << (6.0*(this->getOctupole())/length_)
      << setw(12) << shuntCurrent_
      << endl;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& CF_rbend::writeTo( ostream& os )
{
  int n = ( ( int(v_) - int(u_) )/sizeof( bmlnElmnt* ) );
  if( 0 != n%12 ) {
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "ostream& CF_rbend::writeTo( ostream& os )", 
           "Unexpected number of blocks." ) );
  }

  os << (n/12) << "  ";
  os << OSTREAM_DOUBLE_PREC << usEdgeAngle_ << " "
     << OSTREAM_DOUBLE_PREC << dsEdgeAngle_ << endl;
  os << OSTREAM_DOUBLE_PREC << usAngle_ << " "
     << OSTREAM_DOUBLE_PREC << dsAngle_ << endl;
  os << OSTREAM_DOUBLE_PREC << getQuadrupole() << " ";
  os << OSTREAM_DOUBLE_PREC << getSextupole() << " ";
  os << OSTREAM_DOUBLE_PREC << getOctupole() << " ";
  os << "\n";
  return os;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

istream& CF_rbend::readFrom( istream& is )
{
  double quadStrength = 0.0;
  double sextStrength = 0.0;
  double octStrength  = 0.0;
  int    n            = 0;

  is >> n
     >> usEdgeAngle_
     >> dsEdgeAngle_
     >> usAngle_
     >> dsAngle_
     >> quadStrength 
     >> sextStrength
     >> octStrength;

  usTan_ = tan( usAngle_ );
  dsTan_ = tan( dsAngle_ );

  // Rebuild basic element ...
  // ... First deconstruct (identical to CF_rbend destructor)
  while( v_ >= u_ ) {
    delete (*(v_--));
  }
  delete [] u_;
  v_ = 0;
  u_ = 0;

  // ... Then reconstruct
  finishConstructor(n);


  // Set multipoles
  setQuadrupole( quadStrength );
  setSextupole( sextStrength );


  return is;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* CF_rbend::Type() const  
{ 
  return "CF_rbend"; 
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool CF_rbend::isMagnet() const
{
  return true;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_rbend::OrbitLength( const Particle& x )
{
  double tworho = 2.0 * ( x.Momentum() / PH_CNV_brho_to_p ) / strength_;
  return tworho * asin( length_ / tworho );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


double CF_rbend::AdjustPosition( Particle const& arg_p )
{
  // This probably won't work properly ... 
  // because the current env may not be appropriate 

  JetParticle jetparticle(arg_p);
  return AdjustPosition( jetparticle );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_rbend::AdjustPosition( JetParticle const& arg_jp )
{
  if( ( 0.0 != usEdgeAngle_ ) || ( 0.0 != dsEdgeAngle_ ) ) {
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "double CF_rbend::AdjustPosition( const JetParticle& arg_jp )", 
           "In this version: only implemented for parallel faces." ) );
  }

  enum { x = 0, y, cdt, xp, yp, dpop };

  JetParticle jetparticle(arg_jp);
  Particle       particle(arg_jp);

  Mapping& jetstate =  jetparticle.State();
  Vector&     state =     particle.State();
 
  Jet__environment_ptr env = jetstate.Env(); 

  Vector    instate =     state;

  const double x_i  = state[x];
  const double xp_i = state[xp]; 

  // Initialize the derivative...

  jetparticle.setState(instate); 

  propagate(jetparticle);

  double m = jetstate.Jacobian()( xp, x );
  if( fabs(m) < 1.0e-12 ) {
     throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "double CF_rbend::AdjustPosition( const JetParticle& arg_jp )", 
           "Horrible, inexplicable error: a multi-valued solution is suspected." ) );
  }
  m = 1.0 / m;

  // Initialize the difference ...

  double z = instate[x]  = x_i;
  state = instate;

  propagate( particle );

  double f = state[xp] + xp_i;

  int i = 0;
  while( ( i < 5 ) || ( ( i < 15 ) && (fabs(f) > 1.0e-9) ) ) 
  {
    i++;

    // One Newton's step ...
    z -= m*f;
    
    instate[x] = z;
    state      = instate; 

    // Recalculate inverse derivative ...

    jetparticle.setState(instate); 
    propagate( jetparticle );
 
    m =  (jetstate.Jacobian())( xp, x );
    if( fabs(m) < 1.0e-12 ) {
      throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
             "double CF_rbend::AdjustPosition( const JetParticle& arg_jp )", 
             "Horrible, inexplicable error: a multi-valued solution is suspected." ) );
    }
    m = 1.0 / m;

    // Recalculate difference ...

    propagate( particle);

    f = state[xp] + xp_i;
  }

  //------------------------------------------------------------------------
  // Step procedure when Newton's method fails ...
  //------------------------------------------------------------------------'

  double delta = 1.0e-4;           // step 0.1 mm

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


    instate[x]  = 0.0;
    state       = instate;

    propagate( particle );

    double error = state[xp] + xp_i; // this should be 0 if the angles are equal in magnitude and opposite in sign.

    instate[x] = delta;
    state      = instate;
    
    propagate( particle );

    f = state[xp] + xp_i;

    if(      ( ( f >= 0.0 && error >= 0.0 ) || ( f <= 0.0 && error <= 0.0 ) ) 
          && ( fabs(error) < fabs(f) ) ) 
    {
      delta = - delta;                 
    }

    instate[x]  = 0.0;
    state       = instate;

    while( fabs(delta) > 0.9e-6 ) {

      instate[x] +=  delta;
      state       = instate;
      propagate( particle);

      f = state[xp] + xp_i;

      while( ( ( f <= 0.0 && error <= 0.0 ) || ( f >= 0.0 && error >= 0.0 ) ) && 
             ( fabs(f) < fabs(error) ) )
      {
        error = f;
        instate[x] +=  delta;
        state       = instate;
      	propagate( particle );

      	f = state[xp] + xp_i;
      }

      instate[x] +=  delta;
      state       = instate;
      delta      *= (-0.1);

    } // while

    (*pcerr) << "*** WARNING ***                                  \n"
            "*** WARNING *** CF_rbend::AdjustPosition             \n"
            "*** WARNING *** The step procedure suggests a best   \n"
            "*** WARNING *** solution with magnet displacement "
         <<                                  (-1000.0)*instate[x]
         <<                                           " mm with   \n"
            "*** WARNING *** bend angle error "
         <<                                   (2.0e6)*error
         <<                                       " microradians. \n"
            "*** WARNING ***                                      \n"
         << endl;

    z = state[x];

  } // if 


  // Set the alignment of the internal beamline.
  // this->align_->getAlignment().xOffset -= z;

  alignmentData v; 

  if ( align_ ) {
     v = align_->getAlignment();
  }

  // ??? Does not work: p_bml->setAlignment( v );
  // ??? The reason is that the alignment stategy is
  // ??? not correct for elements whose faces are not
  // ??? parallel.

  setAlignment( v );

  // ??? This will work only if the in and out faces
  // ??? of the CF_rbend element are parallel.

  return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void CF_rbend::acceptInner( BmlVisitor& v )
{
  ctRef_ = 0.0;
  bmlnElmnt** x = u_;
  while( x <= v_ ) {
    (*x)->accept( v );
    ctRef_ += (*x)->getReferenceTime();
    ++x;
  }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_rbend::acceptInner( ConstBmlVisitor& v )
{
  ctRef_ = 0.0;
  bmlnElmnt** x = u_;
  while( x <= v_ ) {
    (*x)->accept( v );
    ctRef_ += (*x)->getReferenceTime();
    ++x;
  }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_rbend::accept( BmlVisitor& v ) 
{
  v.visit(*this);
} 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_rbend::accept( ConstBmlVisitor& v ) const
{
  v.visit(*this);
} 


