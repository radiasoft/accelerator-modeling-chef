/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      CF_sbend.cc
******  Version:   3.1
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
*****
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
*******                                                                
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iomanip>

#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/PhysicsConstants.h>
#include <beamline/CF_sbend.h>
#include <beamline/quadrupole.h>
#include <beamline/sextupole.h>
#include <beamline/sbend.h>
#include <beamline/octupole.h>
#include <beamline/Particle.h>
#include <beamline/BmlVisitor.h>

using namespace std;

using FNAL::pcout;
using FNAL::pcerr;

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_sbend::CF_sbend()
: bmlnElmnt( 1.0, 0.0 )
  , angle_(0.001)
  , usEdgeAngle_(0.0)
  , dsEdgeAngle_(0.0)
  , usAngle_(0.0)
  , dsAngle_(0.0)
  , usTan_(0.0)
  , dsTan_(0.0)
{
  finishConstructor();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


CF_sbend::CF_sbend( double const&        lng,  // length     [ meter    ]
                    double const&        fld,  // field      [ tesla    ]
                    double const&        ang,  // bend angle [ radians  ]
                    int )
: bmlnElmnt( lng, fld )
  , angle_(ang)
  , usEdgeAngle_(0.0)
  , dsEdgeAngle_(0.0)
  , usAngle_(0.0)
  , dsAngle_(0.0)
  , usTan_(0.0)
  , dsTan_(0.0)
{
  finishConstructor();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


CF_sbend::CF_sbend( const char*   nm,   // name
                    double const&        lng,  // length     [ meter    ]
                    double const&        fld,  // field      [ tesla    ]
                    double const&        ang,  // angle      [ radians  ]
                    int )
: bmlnElmnt( nm, lng, fld )
  , angle_(ang)
  , usEdgeAngle_(0.0)
  , dsEdgeAngle_(0.0)
  , usAngle_(0.0)
  , dsAngle_(0.0)
  , usTan_(0.0)
  , dsTan_(0.0)
{
  finishConstructor();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


CF_sbend::CF_sbend( double const&        lng,  // length     [ meter    ]
                    double const&        fld,  // field      [ tesla    ]
                    double const&        ang,  // bend angle [ radians ]
                    double const&        us,   // upstream edge angle [radians]
                    double const&        ds,   // downstream edge angle [radians]
                                        // signs of previous two parameters
                                        // are as defined for sbends by MAD
                    int           n  )  // number of blocks: 4n+1 bends + 2(4n) multipoles
: bmlnElmnt( lng, fld )
  , angle_(ang)
  , usEdgeAngle_(us)
  , dsEdgeAngle_(ds)
  , usAngle_(us)
  , dsAngle_(-ds)
  , usTan_(tan(us))
  , dsTan_(-tan(ds))
{
  finishConstructor();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


CF_sbend::CF_sbend( const char*   nm,   // name
                    double const&        lng,  // length     [ meter    ]
                    double const&        fld,  // field      [ tesla    ]
                    double const&        ang,  // bend angle [ radians ]
                    double const&        us,   // upstream edge angle [radians]
                    double const&        ds,   // downstream edge angle [radians]
                                        // signs of previous two parameters
                                        // are as defined for sbends by MAD
                    int           n  )  // number of blocks: 4n+1 bends + 2(4n) multipoles
: bmlnElmnt( nm, lng, fld )
  , angle_(ang)
  , usEdgeAngle_(us)
  , dsEdgeAngle_(ds)
  , usAngle_(us)
  , dsAngle_(-ds)
  , usTan_(tan(us))
  , dsTan_(-tan(ds))
{
  finishConstructor();
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_sbend::CF_sbend( CF_sbend const& x )
: bmlnElmnt( x )
  , angle_(x.angle_)
  , usEdgeAngle_(x.usEdgeAngle_)
  , dsEdgeAngle_(x.dsEdgeAngle_)
  , usAngle_(x.usAngle_)
  , dsAngle_(x.dsAngle_)
  , usTan_(x.usTan_)
  , dsTan_(x.dsTan_)
{
  int m = 1 + ( ( int(x.v_) - int(x.u_) )/sizeof( bmlnElmnt* ) );
  u_ = new bmlnElmnt* [ m ];
  v_ = &( u_[m-1] );
  
  for( int k=0; k < m; ++k) {
    u_[k] = ( x.u_[k] )->Clone();
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void CF_sbend::finishConstructor()
{
// Insertion for CF_sbend constructors
// 
  double field       =  strength_;
  double frontLength =  6.0*(length_/4.0)/15.0;
  double sepLength   = 16.0*(length_/4.0)/15.0;

  sbend inEdge  ( frontLength,     field, (frontLength/length_)*angle_, 
                  usEdgeAngle_, 0.0, &sbend::InEdge  );
  sbend outEdge ( frontLength,     field, (frontLength/length_)*angle_, 
                  0.0, dsEdgeAngle_, &sbend::OutEdge );
  sbend body    ( sepLength,       field, (sepLength/length_)*angle_,
                  0.0,          0.0, &sbend::NoEdge );

  thinSextupole ts( 0.0 );
  thinQuad      tq( 0.0 );

  u_ = new bmlnElmnt* [ 13 ];
  v_ = u_;

  (*v_)   = new sbend          ( inEdge    );

  dynamic_cast<sbend*>(*v_)->setEntryAngle( usAngle_ );
    v_++;
  *(v_++) = new thinSextupole  ( ts      );
  *(v_++) = new thinQuad       ( tq      );
  *(v_++) = new sbend          ( body    );
  *(v_++) = new thinSextupole  ( ts      );
  *(v_++) = new thinQuad       ( tq      );
  *(v_++) = new sbend          ( body    );
  *(v_++) = new thinSextupole  ( ts      );
  *(v_++) = new thinQuad       ( tq      );
  *(v_++) = new sbend          ( body    );
  *(v_++) = new thinSextupole  ( ts      );
  *(v_++) = new thinQuad       ( tq      );
  *(v_  ) = new sbend          ( outEdge );

  dynamic_cast<sbend*>(*v_)->setExitAngle( dsAngle_ );

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_sbend::~CF_sbend()
{
  // NOTE: If this code is ever modified, you 
  // must also modify CF_sbend::readFrom.
  while( v_ >= u_ ) {
    delete (*(v_--));
  }
  delete [] u_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_sbend::peekAt( double& s, Particle const& prt ) 
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
      << setw(12) << getQuadrupole()/length_
      << setw(12) << 2.0*getSextupole()/length_
      << setw(12) << 6.0*getOctupole()/length_
      << setw(12) << shuntCurrent_
      << endl;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_sbend::localPropagate( Particle& p )
{
  bmlnElmnt** x = u_;
  while( x <= v_ ) {
    (*(x++))->localPropagate( p );
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_sbend::localPropagate( JetParticle& p )
{
  bmlnElmnt** x = u_;
  while( x <= v_ ) {
    (*(x++))->localPropagate( p );
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_sbend::setEntryAngle( Particle const& p )
{
  return this->setEntryAngle( atan2( p.get_npx(), p.get_npz() ) );
  // i.e. tan(phi) = px/pz, where pz = longitudinal momentum
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_sbend::setExitAngle( const Particle& p )
{
  return this->setExitAngle( atan2( p.get_npx(), p.get_npz() ) );
  // i.e. tan(phi) = px/pz, where pz = longitudinal momentum
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_sbend::setEntryAngle( double const& phi /* radians */ )
{
  double ret = usAngle_;
  usAngle_   = phi;
  usTan_     = tan(phi);
  dynamic_cast<sbend*>(*u_)->setEntryAngle(phi);
  return ret;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_sbend::setExitAngle( double const& phi /* radians */ )
{
  double ret = dsAngle_;
  dsAngle_   = phi;  
  dsTan_     = tan(phi);
  dynamic_cast<sbend*>(*v_)->setExitAngle(phi);
  return ret;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int CF_sbend::setOctupole( double const& arg_x )
{
  thinOctupole* q = 0;
  bmlnElmnt** x   = u_;
  
  int counter = 0;
  while( x <= v_ ) {
    if( (q=dynamic_cast<thinOctupole*>(*x) ) ) ++counter;
    ++x;
  }

  if (counter==0) return 1;
 
  x   = u_;

  while( x <= v_ ) {
   if( (q=dynamic_cast<thinOctupole*>(*x) ) )  q->setStrength( arg_x/counter );
  }
  
  return 0;
 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int CF_sbend::setSextupole( double const& arg_x )
{

  thinSextupole* q = 0;
  bmlnElmnt** x    = u_;
  
  int counter = 0;
  while( x <= v_ ) {
    if( (q=dynamic_cast<thinSextupole*>(*x) ) ) ++counter;
    ++x;
  }

  if (counter==0) return 1;
 
  x   = u_;

  while( x <= v_ ) {
   if( (q=dynamic_cast<thinSextupole*>(*x) ) )  q->setStrength( arg_x/counter );
  }
  
  return 0;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool CF_sbend::hasParallelFaces() const
{
  return ( std::abs( usEdgeAngle_ - dsEdgeAngle_ ) <  1.0e-9 );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


bool CF_sbend::hasStandardFaces() const
{
  return ( (std::abs(usEdgeAngle_) < 1.0e-9) && (std::abs(dsEdgeAngle_) < 0.5e-9) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int CF_sbend::setQuadrupole( double const& arg_x )
{

  thinQuad* q = 0;
  bmlnElmnt** x    = u_;
  
  int counter = 0;
  while( x <= v_ ) {
    if( (q=dynamic_cast<thinQuad*>(*x) ) ) ++counter;
    ++x;
  }

  if (counter==0) return 1;
 
  x   = u_;

  while( x <= v_ ) {
   if( (q=dynamic_cast<thinQuad*>(*x) ) )  q->setStrength( arg_x/counter );
   ++x; 
 }
  
  return 0;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int CF_sbend::setDipoleField( double const& arg_x )
{

  strength_ = arg_x;

  sbend*      q  = 0;
  bmlnElmnt** x  = u_;
  
  int counter = 0;
  while( x <= v_ ) {
    if( (q=dynamic_cast<sbend*>(*x) ) ) ++counter;
    ++x;
  }

  if (counter==0) return 1;
 
  x   = u_;

  while( x <= v_ ) {
   if( (q=dynamic_cast<sbend*>(*x) ) )  q->setStrength( arg_x/counter );
  }
  
  return 0;


}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_sbend::setStrength( double const& s )
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

double CF_sbend::getOctupole() const
{

  // Returns the **integrated** octupole

  thinOctupole*  q = 0;
  bmlnElmnt**    x = u_;

  double ret = 0.0;
  
  while( x <= v_ ) {
    if( (q = dynamic_cast<thinOctupole*>(*x)) ) {
      ret += q->Strength(); 
    }
  ++x;
  }
  return ret;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_sbend::getSextupole() const
{

  // Returns the **integrated** sextupole

   thinSextupole* q   = 0;
   bmlnElmnt** x    = u_;

   double ret         = 0.0;

   while( x <= v_ ) {
    if( (q = dynamic_cast<thinSextupole*>(*x)) ) 
    {
      ret += q->Strength();
    }
    ++x;
   }

   return ret;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_sbend::getQuadrupole() const
{

  // Returns the **integrated** quadrupole

   thinQuad* q   = 0;
   double ret    = 0.0;

   bmlnElmnt** x = u_;
   while( x <= v_ ) {
    if( (q = dynamic_cast<thinQuad*>(*x)) ) 
    {
      ret += q->Strength();
    }
    ++x;
   }

   return ret;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_sbend::getDipoleField() const
{
  return strength_;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_sbend::getBendAngle() const
{
  return angle_;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_sbend::Split( double const& pc, ElmPtr& a, ElmPtr& b ) const
{
  static bool firstTime = true;
  if( firstTime ) {
    firstTime = false;
    (*pcerr) << "\n"
            "\n*** WARNING ***"
            "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** void sbend::Split( double pc, bmlnElmnt** a, bmlnElmnt** b )"
            "\n*** WARNING *** The new, split elements must be commissioned with"
            "\n*** WARNING *** RefRegVisitor before being used."
            "\n*** WARNING *** "
         << endl;
    (*pcerr) << "\n*** WARNING ***                             "
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
           "void CF_sbend::Split( double const& pc, bmlnElmnt** a, bmlnElmnt** b )", 
           uic.str().c_str() ) );
  }


  // We assume "strength_" means field, not field*length_.
  // "length_," "strength_," and "angle_" are private data members.

  CF_sbend* p_a = 0;
  CF_sbend* p_b = 0;

  a = CFSbendPtr( p_a = new CF_sbend(         pc*length_, strength_,         pc*angle_, usEdgeAngle_, 0.0 ) );

  p_a->setEntryAngle( getEntryAngle() );
  p_a->setExitAngle( 0.0 );    // Will matter

  b =  CFSbendPtr( p_b = new CF_sbend( (1.0 - pc)*length_, strength_, (1.0 - pc)*angle_, 0.0, dsEdgeAngle_ ) );

  p_b->setEntryAngle( 0.0 );   // Will matter
  p_b->setExitAngle( getExitAngle() );


  // Assign quadrupole strength
  double quadStrength = getQuadrupole();  // quadStrength = B'l

  p_a->setQuadrupole( pc*quadStrength );
  p_b->setQuadrupole( (1.0 - pc)*quadStrength );


  // Rename

  p_a->rename( ident_ + string("_1") );
  p_b->rename( ident_ + string("_2") );

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& CF_sbend::writeTo( ostream& os )
{
  os << OSTREAM_DOUBLE_PREC << angle_       << " ";
  os << OSTREAM_DOUBLE_PREC << usEdgeAngle_ << " "
     << OSTREAM_DOUBLE_PREC << dsEdgeAngle_ << endl;
  os << OSTREAM_DOUBLE_PREC << usAngle_     << " "
     << OSTREAM_DOUBLE_PREC << dsAngle_     << endl;
  os << OSTREAM_DOUBLE_PREC << getQuadrupole() << " ";
  os << OSTREAM_DOUBLE_PREC << getSextupole()  << " ";
  os << OSTREAM_DOUBLE_PREC << getOctupole()   << " ";
  os << "\n";
  return os;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

istream& CF_sbend::readFrom( istream& is )
{
  double quadStrength = 0.0;  // Assignment mitigates 
  double sextStrength = 0.0;  // obnoxious warning message
  double octStrength  = 0.0;  // from compiler.


  is >> angle_ 
     >> usEdgeAngle_ 
     >> dsEdgeAngle_ 
     >> usAngle_ 
     >> dsAngle_ 
     >> quadStrength 
     >> sextStrength
     >> octStrength;

  usTan_ = tan(usAngle_);
  dsTan_ = tan(dsAngle_);

  // Rebuild basic element ...
  // ... First deconstruct (identical to CF_sbend destructor)
  while( v_ >= u_ ) {
    delete ( *(v_--) );
  }
  delete [] u_;
  v_ = 0;
  u_ = 0;

  // ... Then reconstruct
  finishConstructor();


  // Set multipoles
  setQuadrupole( quadStrength );
   setSextupole( sextStrength );
    setOctupole( octStrength );

  return is;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* CF_sbend::Type() const  
{ 
  return "CF_sbend"; 
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool CF_sbend::isMagnet() const
{
  return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CF_sbend::OrbitLength( const Particle& x )
{
  return length_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_sbend::acceptInner( BmlVisitor& v )
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

void CF_sbend::acceptInner( ConstBmlVisitor& v ) const
{
  ctRef_        = 0.0;
  bmlnElmnt** x = u_;

  while( x <= v_ ) {
    (*x)->accept( v );
    ctRef_ += (*x)->getReferenceTime();
    ++x;
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_sbend::accept( BmlVisitor& v )
{
  v.visit(*this);
} 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void CF_sbend::accept( ConstBmlVisitor& v ) const
{
  v.visit(*this);
} 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
