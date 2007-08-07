/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.1
******                                    
******  File:      Slot.cc
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
******                                                                
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <basic_toolkit/iosetup.h>
#include <beamline/marker.h>
#include <beamline/Slot.h>
#include <beamline/beamline.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/ParticleBunch.h>
#include <beamline/Alignment.h>
#include <beamline/BmlVisitor.h>

using namespace std;

using FNAL::pcerr;
using FNAL::pcout;

// --------------------------------------------------
// --- Constructors ---------------------------------
// --------------------------------------------------

Slot::Slot()
  : bmlnElmnt()
{
  align_ = new alignment;  // ??? why???
}


Slot::Slot( const char* nm )
: bmlnElmnt(nm)
{
  align_ = new alignment;
}


Slot::Slot( Frame const& y )
: bmlnElmnt(), out(y)
{
  if( !out.isOrthonormal() )
  {
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__,
           "Slot::Slot( const Frame& y )", 
           "Current implementation requires that frames be orthonormal." ) );
  }

  align_   = 0;

  length_ = out.getOrigin() .Norm();
  ctRef_  = length_;
}


Slot::Slot( const char* nm, const Frame& y )
: bmlnElmnt(nm), out(y)
{
  if( !out.isOrthonormal() )
  {
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__,
           "Slot::Slot( const char* nm, const Frame& y )", 
           "Current implementation requires that frames be orthonormal." ) );
  }

  align_   = 0;

  length_ = out.getOrigin() .Norm();
  ctRef_  = length_;
}


Slot::Slot( const Frame&     x, 
            const Frame&     y, 
            const beamline&  bl
          )
: bmlnElmnt(), in(x), out(y)
{
  if( ( 0 == checkFrame(in)  ) && 
      ( 0 == checkFrame(out) ) ) 
  {
    p_bml_   = BmlPtr( bl.Clone() );
  }
  align_ = new alignment;
}


Slot::Slot( const char*      nm,
            const Frame&     x, 
            const Frame&     y, 
            const beamline&  bl
          )
: bmlnElmnt(nm), in(x), out(y)
{
  if( ( 0 == this->checkFrame(in)  ) && 
      ( 0 == this->checkFrame(out) ) ) 
  {
    p_bml_   = BmlPtr( bl.Clone() );
  }
  align_ = new alignment;
}


Slot::Slot( const Frame&     x, 
            const Frame&     y, 
            const bmlnElmnt& be
          )
: bmlnElmnt(), in(x), out(y)
{
  if( ( 0 == this->checkFrame(in)  ) && 
      ( 0 == this->checkFrame(out) ) &&
      ( 0 != strcasecmp( be.Type(), "Slot" ) ) ) 
  {
    bml_e_ = ElmPtr( be.Clone() );
  }
  align_ = new alignment;
}


Slot::Slot( const char*     nm,
            const Frame&     x, 
            const Frame&     y, 
            const bmlnElmnt& be
          )
: bmlnElmnt(nm), in(x), out(y)
{
  if( ( 0 == this->checkFrame(in)  ) && 
      ( 0 == this->checkFrame(out) ) &&
      ( 0 != strcasecmp( be.Type(), "Slot" ) ) ) 
  {
     bml_e_ = ElmPtr( be.Clone() );
  }
  align_ = new alignment;
}


Slot::Slot( Slot const& x )
: bmlnElmnt( x), in( x.in ), out( x.out )
{
  if ( x.align_ != 0 )
    align_ = new alignment( *(x.align_) );
  else 
    align_ = NULL;

  if     ( x.p_bml_   ) p_bml_   = BmlPtr( x.p_bml_->Clone()   );
  else if( x.bml_e_   ) bml_e_   = ElmPtr( x.bml_e_->Clone() );
}


Slot::~Slot()
{

  if( align_) {
    delete align_;
    align_ = 0;
  }
}


void Slot::makeUpstreamHorizontal   ( double const& lng, double const& ang )
{
  length_ = lng;
  in.reset();
  out.reset();

  static Vector driftOffset(3); 
  driftOffset(2) = lng;
  
  out.rotate( - ang, out.getyAxis() );
  out.translate( driftOffset );
}


void Slot::makeDownstreamHorizontal ( double const& lng, double const& ang )
{
  length_ = lng;
  in.reset();
  out.reset();

  static Vector driftOffset(3); 
  driftOffset(2) = lng;
  
  out.translate( driftOffset );
  out.rotate( - ang, out.getyAxis() );
}


void Slot::makeUpstreamVertical   ( double const& lng, double const& ang )
{
  length_ = lng;
  in.reset();
  out.reset();

  static Vector driftOffset(3); 
  driftOffset(2) = lng;
  
  out.rotate( - ang, out.getxAxis() );
  out.translate( driftOffset );
}


void Slot::makeDownstreamVertical ( double const& lng, double const& ang )
{
  length_ = lng;
  in.reset();
  out.reset();

  static Vector driftOffset(3); 
  driftOffset(2) = lng;
  
  out.translate( driftOffset );
  out.rotate( - ang, out.getxAxis() );
}


const char*  Slot::Type()  const  
{ 
  return "Slot"; 
}

bool Slot::isMagnet()  const  
{ 
  return false; 
}

 int Slot::setInFrame( const Frame& frm )
{
   int ret = checkFrame( frm );

  if( 0 == ret ) {
    in = frm;
  }

  return ret;
}


 int Slot::setOutFrame( const Frame& frm )
{
  // REMOVE:  int ret = checkFrame( frm );

  if( frm.isOrthonormal() ) { 
    out = frm;   
    return 0;
  }

  throw( bmlnElmnt::GenericException( __FILE__, __LINE__,
         " int Slot::setOutFrame( const Frame& frm )", 
         "Current implementation requires that frames be orthonormal." ) );
}


 int Slot::checkFrame( const Frame& f ) const
{
  static const Frame zero;
  static const int y = 1;
  static const int z = 2;
 
  int ret = 0;

  if( !f.isOrthonormal() ) {
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__,
           " int Slot::checkFrame( const Frame& f ) const", 
           "Current implementation requires that frames be orthonormal." ) );
  }

  if( f.getOrigin() != zero.getOrigin() ) {
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__,
           " int Slot::checkFrame( const Frame& f ) const", 
           "Current implementation requires no displacement of origin." ) );
  }

  else if( (  f.getAxis(y) != zero.getAxis(y) ) && 
           (  f.getAxis(z) != zero.getAxis(z) )
         ) {
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__,
           " int Slot::checkFrame( const Frame& f ) const", 
           "Current implementation allows rotation about "
           "y or z axis, but not both." ) );
  }

  return ret;
}


double Slot::setReferenceTime( double const& x )
{
  double oldValue = ctRef_;
  ctRef_ = x;
  return oldValue;
}


double Slot::setReferenceTime( Particle const& p)
{
  // This probably should never be used.
  // Code copied from Slot::localPropagate
  // and edited slightly. The two functions
  // must stay synchronized.

  Particle localParticle(p);

  // This part correlates with bmlnElmnt.h
  if( align_ != 0 ) {
    enterLocalFrame( localParticle );
  }

  // This assumes the in face corresponds to the
  // in Frame. It is attached to the out face
  // of the previous element.

  Vector r(3);
  r(0) = localParticle.get_x();
  r(1) = localParticle.get_y();
  // r(2) = 0.0, set in constructor

  Vector beta ( localParticle.VectorBeta() );
  Vector u_3  ( out.getAxis(2) );
  Vector q    ( out.getOrigin() );
  double betaParallel = beta * u_3;


  if( betaParallel > 0.0 ) {
    ctRef_ = ( q - r )*u_3 / betaParallel;
  }
  else {
    (*pcerr) << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** double Slot::setReferenceTime( const Particle& prtn )"
            "\n*** WARNING *** Velocity is not forward.        "
            "\n*** WARNING ***                                 ";
    (*pcerr) << "\n*** WARNING *** " << this->Name();
    (*pcerr) << "\n*** WARNING *** " << in;
    (*pcerr) << "\n*** WARNING *** " << out;
    (*pcerr) << "\n*** WARNING *** " << "VectorBeta(): " << localParticle.VectorBeta();
    (*pcerr) << "\n*** WARNING *** " << "betaParallel: " << betaParallel;
    (*pcerr) << endl;

    ctRef_ = 0.0;
  }

  return ctRef_;
}


// --------------------------------------------------
// --- Istream and Ostream support ------------------
// --------------------------------------------------

ostream& Slot::writeTo ( ostream& os )
{
  // Write out private attributes, which are the "in" and "out" Frame's.
  os << in ;
  if ( p_bml_ != NULL ) {
    // print out the beamline contained in this slot
    os << "slot_BEGIN " << Name() << " 0 0 0 0 0\n";
    os << (*p_bml_);
    os << "slot_END " << Name() << " 0 0 0 0 0\n";
  } else if ( bml_e_ != NULL ) {
    os << "slot_BEGIN " << Name() << " 0 0 0 0 0\n";
    os << (*bml_e_);
    os << "slot_END " << Name() << " 0 0 0 0 0\n";
  } else {
    os << "no_slot_contents " << Name() << " 0 0 0 0 0\n";
  }
  os << out ;
  return os;
}


istream& Slot::readFrom( istream& is )
{
  // Read in my private attributes

  // First, get the "in" frame"
  is >> in;

  // Second, read in the stuff contained in the Slot.
  char type[30], name[60];
  double Length, Strength, x, y, t;

  // Now read in the bmlnElmnt, if there is one

  is >> type >> name >> Length >> Strength >> x >> y >> t;
  if ( strcasecmp(type, "slot_BEGIN") == 0 ) {
    bml_e_ = ElmPtr( read_istream(is) );  // Recursively read the bmlnElmnt.

    if ( bml_e_  && strcasecmp(bml_e_->Type(), "beamline") == 0 ) 

      p_bml_ = BmlPtr( (beamline*) bml_e_.get() );

    // The only element in this Slot is a single bmlnElmnt.  There is a 
    // "slot_END" line to read in.

    is >> type >> name >> Length >> Strength >> x >> y >> t;
    if ( strcasecmp(type, "slot_END") != 0 ) {
      ostringstream uic;
      uic << "Expecting \"slot_END\" but got " << type;
      throw( bmlnElmnt::GenericException( __FILE__, __LINE__,
             "istream& Slot::readFrom( istream& is )", 
             uic.str().c_str() ) );
    }
  } else {
    if( align_ != 0 ) {
      delete align_;
      align_ = 0;
    }
    // Skip the next line--it is not "slot_BEGIN" (assume it is "slot_empty")
    ;
  }
    
  // Finally, read in the "out" frame information.
  is >> out;
  length_ = ( out.getOrigin() - in.getOrigin()) .Norm();

  return is;
}


void Slot::Split( double const& pct, ElmPtr& a, ElmPtr& b ) const
{
  if( pct < 0.0 || 1.0 < pct ) {
    (*pcerr) << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** void Slot::Split( double, ElmPtr&, ElmPtr& )"
         << "\n*** WARNING *** Function called with first argument = " 
         << pct <<             ", outside [0,1]."
         << "\n*** WARNING *** Null pointers are being returned." 
         << endl;
    a = ElmPtr();
    b = ElmPtr();
    return;
  }

  
  if( pct == 0.0 ) {
     a = MarkerPtr( new marker( "Null Slot" ) );
     b =   SlotPtr( new Slot( *this ) );
    return;
  }
  if( pct == 1.0 ) {
     a = SlotPtr( new Slot( *this ) );
     b = MarkerPtr( new marker( "Null Slot" ) );
    return;
  }

  Vector d( out.getOrigin() - in.getOrigin() );
  Vector av( pct*d );
  Vector bv( ( 1. - pct )*d );  
  
  Frame aOutFrame( in );
  aOutFrame.setOrigin( in.getOrigin() + pct*d );
  Frame bOutFrame( out.relativeTo( aOutFrame ) );

  a = SlotPtr( new Slot( aOutFrame ) );
  b = SlotPtr( new Slot( bOutFrame ) );

  // Rename

  a->rename( ident_ + string("_1") );
  b->rename( ident_ + string("_2") );

}


// --------------------------------------------------
// --- Frame functions ------------------------------
// --------------------------------------------------


void Slot::processFrame( const Frame& frm, Particle& p ) const
{

  const int x    = p.xIndex();
  const int y    = p.yIndex();
  const int cdt  = p.cdtIndex();
  const int xp   = p.npxIndex();
  const int yp   = p.npyIndex();
  const int dpop = p.ndpIndex();

  static bool firstTime = true;
  static Vector u_z(3);
  static Vector u_y(3);

  Vector& state = p.State();


  if( firstTime ) {
    firstTime = false;
    u_y[0] = 0.0; u_y[1] = 1.0; u_y[2] = 0.0;
    u_z[0] = 0.0; u_z[1] = 0.0; u_z[2] = 1.0;

    // This is never deleted.
  }


  // Yaw -------------------------------------
  if( ( frm.getAxis(1) == u_y ) && ( frm.getAxis(2) != u_z ) ) 
  {
    // "Old" frame represented in "new" frame coordinates.
    Vector e_1(3), e_2(3), e_3(3);

    e_1 = frm.getAxis(0);
    e_2 = frm.getAxis(1);
    e_3 = frm.getAxis(2);
    
    double cs = e_1(0);
    double sn = e_3(0);
    
    // Coordinate transformation.
    Vector r        ( state[x]*e_1 + state[y]*e_2 );
    Vector dummy    ( p.VectorBeta() );
    Vector beta     ( dummy(0)*e_1 +
                      dummy(1)*e_2 +
                      dummy(2)*e_3 );
  
    double tau      ( - r(2) / beta(2) );
  
    state[x]    = r(0) + tau*beta(0);
    state[y]    = r(1) + tau*beta(1);
    state[cdt] += tau;
  
    // Momentum transformation

    double p1( state[ xp ] );
    double p2( state[ yp ] );
    double p3divpbar = sqrt( ( 1.0 + state[dpop] ) * ( 1.0 + state[dpop] )
                              - p1*p1 - p2*p2 );
  
    state[xp] = cs* state[ xp ] + sn*p3divpbar;
  }


  // Roll ------------------------------------
  else if( ( frm.getAxis(2) == u_z ) && ( frm.getAxis(1) != u_y ) ) 
  {
    double cs = ( frm.getAxis(0) )(0);
    double sn = ( frm.getAxis(1) )(0); // ??? right?
    // sin of angle by which magnet is rolled

  
    double temp     = state[0] * cs + state[1] * sn;
    state[1]        = state[1] * cs - state[0] * sn;
    state[0]        = temp;
  
    temp       = state[3] * cs + state[4] * sn;
    state[4]   = state[4] * cs - state[3] * sn;
    state[3]   = temp;
   

  }

}


void Slot::processFrame( const Frame& frm, JetParticle& p ) const
{
  const int x    = p.xIndex();
  const int y    = p.yIndex();
  const int cdt  = p.cdtIndex();
  const int xp   = p.npxIndex();
  const int yp   = p.npyIndex();
  const int dpop = p.ndpIndex();

  static bool firstTime = true;
  static Vector u_z(3);
  static Vector u_y(3);
 

  if( firstTime ) {
    firstTime = false;
    u_y[0] = 0.0; u_y[1] = 1.0; u_y[2] = 0.0;
    u_z[0] = 0.0; u_z[1] = 0.0; u_z[2] = 1.0;
  }

   
  Mapping&  state = p.State();


  // Yaw -------------------------------------
  if( ( frm.getAxis(1) == u_y ) && ( frm.getAxis(2) != u_z ) ) 
  {
    // "Old" frame represented in "new" frame coordinates.
    Vector e_1(3), e_2(3), e_3(3);
    e_1 = frm.getAxis(0);
    e_2 = frm.getAxis(1);
    e_3 = frm.getAxis(2);
    
    double cs = e_1(0);
    double sn = e_3(0);

    // Coordinate transformation.

    JetVector r        ( state[x]*e_1 + state[y]*e_2 );
    JetVector dummy    ( p.VectorBeta() );
    JetVector beta     ( dummy(0)*e_1 +
                         dummy(1)*e_2 +
                         dummy(2)*e_3 );
  
    Jet tau            ( - r(2) / beta(2) );
  
    state[x]    =   r(0) + tau*beta(0);
    state[y]    =   r(1) + tau*beta(1);
    state[cdt] +=   tau ;
  
    // Momentum transformation
    Jet p1( state[xp] );
    Jet p2( state[yp] );
    Jet p3divpbar = sqrt( ( 1.0 + state[dpop] ) * ( 1.0 + state[dpop] )
                              - p1*p1 - p2*p2 );
  
     state[xp] =  cs*state[xp] + sn*p3divpbar ;
  }

  // Roll ------------------------------------
  else if( ( frm.getAxis(2) == u_z ) && ( frm.getAxis(1) != u_y ) ) 
  {
    double cs = ( frm.getAxis(0) )(0);
    double sn = ( frm.getAxis(1) )(0); // ??? right?

    // sin of angle by which magnet is rolled

  
    Jet temp   = state[0] * cs + state[1] * sn;
    state[1]   = state[1] * cs - state[0] * sn;
    state[0]   = temp;

    temp      = state[3] * cs + state[4] * sn;
    state[4]  = state[4] * cs - state[3] * sn;
    state[3]  = temp;

  }

}


void Slot::enterLocalFrame( Particle& p ) const
{
  processFrame( in, p );
}


void Slot::enterLocalFrame( JetParticle& p ) const
{
  processFrame( in, p );
}


void Slot::leaveLocalFrame( Particle& p ) const
{
  processFrame( out, p );
}


void Slot::leaveLocalFrame( JetParticle& p ) const
{
  processFrame( out, p );
}



// --------------------------------------------------
// --- Local propagate functions --------------------
// --------------------------------------------------


void Slot::localPropagate( Particle& p )
{
  // If this is modified, one should
  // simultaneously modify Slot::setReferenceTime( const Particle& )

  Vector& state = p.State();

  if      ( bml_e_   ) bml_e_->propagate( p );
  else if ( p_bml_   ) p_bml_->propagate( p );

  else {
    // Propagate as drift to the out-plane

    Vector r(3);
    r(0) = p.get_x();
    r(1) = p.get_y();

    Vector beta ( p.VectorBeta() );

    Vector q    ( out.getOrigin() );
    Vector u_1  ( out.getAxis(0) );
    Vector u_2  ( out.getAxis(1) );
    Vector u_3  ( out.getAxis(2) );

    // REMOVE: double tauZero = length_ / p.ReferenceBeta();

    double tau;
    double betaParallel = beta * u_3;

    if( betaParallel > 0.0 ) {
      tau = ( q - r )*u_3 / betaParallel;
    }
    else {
      ostringstream uic;
      uic << this->Type() << "  " << this->Name()
          << ": Velocity is not forward: it may be NAN.";
      throw( bmlnElmnt::GenericException( __FILE__, __LINE__,
             "void Slot::localPropagate( Particle& p )", 
             uic.str().c_str() ) );
    }

    r += tau*beta;
    r -= q;

    state[ p.xIndex()   ]  = r*u_1;
    state[ p.yIndex()   ]  = r*u_2;
    // REMOVE: p.state[ p.cdtIndex() ] += ( tau - tauZero );
    state[ p.cdtIndex() ] += ( tau - ctRef_ );

    // Momentum transformation
    Vector momntm(3);
    momntm = ( p.NormalizedVectorMomentum() );
    state[ p.npxIndex() ] = momntm*u_1;
    state[ p.npyIndex() ] = momntm*u_2;
  }
}


void Slot::localPropagate( JetParticle& p )
{
 
  Mapping& state = p.State();

  if     (  p_bml_   ) p_bml_->propagate( p );
  else if(  bml_e_   ) bml_e_->propagate( p );

  else {
    // Propagate as drift to the out-plane

    JetVector r(3);
    r(0) = p.get_x();
    r(1) = p.get_y();

    JetVector beta ( p.VectorBeta() );

    Vector q    ( out.getOrigin() );
    Vector u_1  ( out.getAxis(0) );
    Vector u_2  ( out.getAxis(1) );
    Vector u_3  ( out.getAxis(2) );

    // REMOVE: double tauZero = length_ / p.ReferenceBeta();
    Jet    tau;

    Jet    betaParallel = beta * u_3;

    if( betaParallel.standardPart() > 0.0 ) {
      tau = ( q - r )*u_3 / betaParallel;
    }
    else {

      ostringstream uic;
      uic << this->Type() << "  " << this->Name()
          << ": Velocity is not forward: it may be NAN.";
      throw( bmlnElmnt::GenericException( __FILE__, __LINE__,
             "void Slot::localPropagate( JetParticle& p )", 
             uic.str().c_str() ) );
    }

    r += tau*beta;
    r -= q;

    state( p.xIndex()   )   = r*u_1;
    state( p.yIndex()   )   = r*u_2;
    // REMOVE: p.state( p.cdtIndex() ) += ( tau - tauZero );
    state( p.cdtIndex() )  += ( tau - ctRef_ );

    // Momentum transformation
    JetVector mom( p.NormalizedVectorMomentum() );
    state( p.npxIndex() ) = mom*u_1;
    state( p.npyIndex() ) = mom*u_2;
  }
}


void Slot::localPropagate( ParticleBunch& x )
{
  if     ( p_bml_   ) { p_bml_->propagate( x ); }
  else if( bml_e_ )   { bml_e_->propagate( x ); }
  else {
    for ( ParticleBunch::iterator it = x.begin();  it != x.end(); ++it )  
    { localPropagate( *it ); }
  }
}


 
// --------------------------------------------------
// --- Functions passed on to tenant ----------------
// --------------------------------------------------

void Slot::setStrength   ( double const& x )
{
  if     ( p_bml_   ) p_bml_->setStrength( x );
  else if( bml_e_   ) bml_e_->setStrength( x );
}

void Slot::setCurrent    ( double const& x )
{
  if     ( p_bml_   ) p_bml_->setStrength( x );
  else if( bml_e_   ) bml_e_->setStrength( x );
}

bool Slot::setAlignment  ( const alignmentData& x )
{
  bool ret = true;
  if     ( p_bml_   ) {
    ret = p_bml_->setAlignment( x );
  }
  else if( bml_e_ ) {
    ret = bml_e_->setAlignment( x );
  }
  if( !ret ) {
    (*pcerr) << "\n*** ERROR *** "
         << "\n*** ERROR *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** ERROR *** bool Slot::setAlignment  ( const alignmentData& x )"
            "\n*** ERROR *** Unable to perform operation on internal elements."
            "\n*** ERROR *** Subsequent calculations prone to error."
            "\n*** ERROR *** "
         << endl;
  }
  return ret;
}


double Slot::Current() const
{
  if     ( p_bml_   ) return p_bml_->Current();
  else if( bml_e_   ) return bml_e_->Current();
  else                    return 0.0;
}


double Slot::OrbitLength( const Particle& x )
{
  static bool firstTime = true;

  if     ( p_bml_   ) return p_bml_->OrbitLength( x );
  else if( bml_e_   ) return bml_e_->OrbitLength( x );
  else {
    if( firstTime ) {
      firstTime = false;
      (*pcerr) << "*** WARNING ***                                 \n"
              "*** WARNING *** Slot::OrbitLength               \n"
              "*** WARNING *** Returning the design length of  \n"
              "*** WARNING *** empty slots rather then the     \n"
              "*** WARNING *** orbit length.                   \n"
              "*** WARNING ***                                 \n"
           << endl;
    }    
    return length_;
  }
}
 

std::string Slot::Name() const
{
  if     ( p_bml_   )     return p_bml_->Name();
  else if( bml_e_ )       return bml_e_->Name();
  else                    return bmlnElmnt::Name();
}



void Slot::accept( BmlVisitor& v ) 
{
 v.visit(*this);
}

void Slot::accept( ConstBmlVisitor& v ) const
{

  v.visit(*this);
}
