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

#include <iosetup.h>
#include <marker.h>
#include <Slot.h>
#include <Particle.h>
#include <ParticleBunch.h>

using namespace std;

using FNAL::pcerr;
using FNAL::pcout;

// --------------------------------------------------
// --- Constructors ---------------------------------
// --------------------------------------------------

Slot::Slot()
  : bmlnElmnt()
{
  p_bml   = 0;
  p_bml_e = 0;
  align = new alignment;  // ??? why???
}


Slot::Slot( const char* nm )
: bmlnElmnt(nm)
{
  p_bml   = 0;
  p_bml_e = 0;
  align = new alignment;
}


Slot::Slot( const Frame& y )
: bmlnElmnt(), out(y)
{
  if( !out.isOrthonormal() )
  {
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__,
           "Slot::Slot( const Frame& y )", 
           "Current implementation requires that frames be orthonormal." ) );
  }

  p_bml   = 0;
  p_bml_e = 0;
  align   = 0;

  length = out.getOrigin() .Norm();
  _ctRef = length;
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

  p_bml   = 0;
  p_bml_e = 0;
  align   = 0;

  length = out.getOrigin() .Norm();
  _ctRef = length;
}


Slot::Slot( const Frame&     x, 
            const Frame&     y, 
            const beamline&  bl
          )
: bmlnElmnt(), in(x), out(y)
{
  if( ( 0 == this->checkFrame(in)  ) && 
      ( 0 == this->checkFrame(out) ) ) 
  {
    p_bml_e = 0;
    p_bml   = (beamline*) bl.Clone();
  }
  align = new alignment;
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
    p_bml_e = 0;
    p_bml   = (beamline*) bl.Clone();
  }
  align = new alignment;
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
    p_bml_e = be.Clone();
    p_bml   = 0;
  }
  align = new alignment;
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
    p_bml_e = be.Clone();
    p_bml   = 0;
  }
  align = new alignment;
}


Slot::Slot( const Slot& x )
: bmlnElmnt( (const bmlnElmnt&) x ), in( x.in ), out( x.out )
{
  p_bml   = 0;
  p_bml_e = 0;
  if ( x.align != 0 )
    align = new alignment( *(x.align) );
  else 
    align = NULL;

  if     ( 0 != x.p_bml   ) p_bml   = (beamline*) x.p_bml->Clone();
  else if( 0 != x.p_bml_e ) p_bml_e = x.p_bml_e ->Clone();
}


Slot::~Slot()
{
  if( p_bml ) {
    p_bml->eliminate();
    p_bml = 0;
  }
  if( p_bml_e ) {
    delete p_bml_e;
    p_bml_e = 0;
  }

  if( align) {
    delete align;
    align = 0;
  }
}


void Slot::makeUpstreamHorizontal   ( double lng, double ang )
{
  length = lng;
  in.reset();
  out.reset();

  static Vector driftOffset(3); 
  driftOffset(2) = lng;
  
  out.rotate( - ang, out.getyAxis() );
  out.translate( driftOffset );
}


void Slot::makeDownstreamHorizontal ( double lng, double ang )
{
  length = lng;
  in.reset();
  out.reset();

  static Vector driftOffset(3); 
  driftOffset(2) = lng;
  
  out.translate( driftOffset );
  out.rotate( - ang, out.getyAxis() );
}


void Slot::makeUpstreamVertical   ( double lng, double ang )
{
  length = lng;
  in.reset();
  out.reset();

  static Vector driftOffset(3); 
  driftOffset(2) = lng;
  
  out.rotate( - ang, out.getxAxis() );
  out.translate( driftOffset );
}


void Slot::makeDownstreamVertical ( double lng, double ang )
{
  length = lng;
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

short int Slot::setInFrame( const Frame& frm )
{
  short int ret = checkFrame( frm );

  if( 0 == ret ) {
    in = frm;
  }

  return ret;
}


short int Slot::setOutFrame( const Frame& frm )
{
  // REMOVE: short int ret = checkFrame( frm );

  if( frm.isOrthonormal() ) { 
    out = frm;   
    return 0;
  }

  throw( bmlnElmnt::GenericException( __FILE__, __LINE__,
         "short int Slot::setOutFrame( const Frame& frm )", 
         "Current implementation requires that frames be orthonormal." ) );
}


short int Slot::checkFrame( const Frame& f ) const
{
  static const Frame zero;
  static const int y = 1;
  static const int z = 2;
  static short int ret;

  ret = 0;

  if( !f.isOrthonormal() ) {
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__,
           "short int Slot::checkFrame( const Frame& f ) const", 
           "Current implementation requires that frames be orthonormal." ) );
  }

  if( f.getOrigin() != zero.getOrigin() ) {
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__,
           "short int Slot::checkFrame( const Frame& f ) const", 
           "Current implementation requires no displacement of origin." ) );
  }

  else if( (  f.getAxis(y) != zero.getAxis(y) ) && 
           (  f.getAxis(z) != zero.getAxis(z) )
         ) {
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__,
           "short int Slot::checkFrame( const Frame& f ) const", 
           "Current implementation allows rotation about "
           "y or z axis, but not both." ) );
  }

  return ret;
}


double Slot::setReferenceTime( double x )
{
  double oldValue = _ctRef;
  _ctRef = x;
  if( fabs(_ctRef) < 1.0e-12 ) { _ctRef = 0.0; }
  return oldValue;
}


double Slot::setReferenceTime( const Particle& prtn )
{
  // This probably should never be used.
  // Code copied from Slot::localPropagate
  // and edited slightly. The two functions
  // must stay synchronized.

  Particle* localParticle = prtn.Clone();

  // This part correlates with bmlnElmnt.h
  if( this->align != 0 ) {
    this->enterLocalFrame( *localParticle );
  }

  // This assumes the in face corresponds to the
  // in Frame. It is attached to the out face
  // of the previous element.
  Vector r(3);
  r(0) = localParticle->get_x();
  r(1) = localParticle->get_y();
  // r(2) = 0.0, set in constructor

  Vector beta ( localParticle->VectorBeta() );
  Vector u_3  ( out.getAxis(2) );
  Vector q    ( out.getOrigin() );

  double betaParallel = beta * u_3;

  if( betaParallel > 0.0 ) {
    _ctRef = ( q - r )*u_3 / betaParallel;
  }
  else {
    (*pcerr) << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** double Slot::setReferenceTime( const Particle& prtn )"
            "\n*** WARNING *** Velocity is not forward.        "
            "\n*** WARNING ***                                 ";
    (*pcerr) << "\n*** WARNING *** " << this->Name();
    (*pcerr) << "\n*** WARNING *** " << in;
    (*pcerr) << "\n*** WARNING *** " << out;
    (*pcerr) << "\n*** WARNING *** " << "VectorBeta(): " << localParticle->VectorBeta();
    (*pcerr) << "\n*** WARNING *** " << "betaParallel: " << betaParallel;
    (*pcerr) << endl;

    _ctRef = 0.0;
  }

  delete localParticle;
  return _ctRef;
}


// --------------------------------------------------
// --- Istream and Ostream support ------------------
// --------------------------------------------------

ostream& Slot::writeTo ( ostream& os )
{
  // Write out private attributes, which are the "in" and "out" Frame's.
  os << in ;
  if ( p_bml != NULL ) {
    // print out the beamline contained in this slot
    os << "slot_BEGIN " << Name() << " 0 0 0 0 0\n";
    os << (*p_bml);
    os << "slot_END " << Name() << " 0 0 0 0 0\n";
  } else if ( p_bml_e != NULL ) {
    os << "slot_BEGIN " << Name() << " 0 0 0 0 0\n";
    os << (*p_bml_e);
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
  p_bml   = 0;
  p_bml_e = 0;

  is >> type >> name >> Length >> Strength >> x >> y >> t;
  if ( strcasecmp(type, "slot_BEGIN") == 0 ) {
    p_bml_e = read_istream(is);	// Recursively read the bmlnElmnt.
    if ( p_bml_e != 0 && strcasecmp(p_bml_e->Type(), "beamline") == 0 ) 
      p_bml = (beamline*) p_bml_e;
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
    if( align != 0 ) {
      delete align;
      align = 0;
    }
    // Skip the next line--it is not "slot_BEGIN" (assume it is "slot_empty")
    ;
  }
    
  // Finally, read in the "out" frame information.
  is >> out;
  length = ( out.getOrigin() - in.getOrigin()) .Norm();

  return is;
}


void Slot::Split( double pct, bmlnElmnt** a, bmlnElmnt** b ) const
{
  if( pct < 0.0 || 1.0 < pct ) {
    (*pcerr) << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** void Slot::Split( double, bmlnElmnt**, bmlnElmnt** )"
         << "\n*** WARNING *** Function called with first argument = " 
         << pct <<             ", outside [0,1]."
         << "\n*** WARNING *** Null pointers are being returned." 
         << endl;
    *a = 0;
    *b = 0;
    return;
  }
  if( pct == 0.0 ) {
    *a = new marker( "Null Slot" );
    *b = new Slot( *this );
    return;
  }
  if( pct == 1.0 ) {
    *a = new Slot( *this );
    *b = new marker( "Null Slot" );
    return;
  }

  Vector d( out.getOrigin() - in.getOrigin() );
  Vector av( pct*d );
  Vector bv( ( 1. - pct )*d );  
  
  Frame aOutFrame( in );
  aOutFrame.setOrigin( in.getOrigin() + pct*d );
  Frame bOutFrame( out.relativeTo( aOutFrame ) );

  *a = new Slot( aOutFrame );
  *b = new Slot( bOutFrame );

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


// --------------------------------------------------
// --- Frame functions ------------------------------
// --------------------------------------------------


void Slot::processFrame( const Frame& frm, Particle& p ) const
{
  static const int x    = p.xIndex();
  static const int y    = p.yIndex();
  static const int cdt  = p.cdtIndex();
  static const int xp   = p.npxIndex();
  static const int yp   = p.npyIndex();
  static const int dpop = p.ndpIndex();

  static Vector u_z(3);
  static Vector u_y(3);
  static bool firstTime = 1;
  
  static double* inState;
  static double temp;
  static double cs, sn;
  

  if( firstTime ) {
    firstTime = false;
    u_y(0) = 0.0; u_y(1) = 1.0; u_y(2) = 0.0;
    u_z(0) = 0.0; u_z(1) = 0.0; u_z(2) = 1.0;
    inState = new double[ p.psd() ];
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
    Vector r        ( p.State(x)*e_1 + p.State(y)*e_2 );
    Vector dummy    ( p.VectorBeta() );
    Vector beta     ( dummy(0)*e_1 +
                      dummy(1)*e_2 +
                      dummy(2)*e_3 );
  
    double tau      ( - r(2) / beta(2) );
  
    p.state[x]    = r(0) + tau*beta(0);
    p.state[y]    = r(1) + tau*beta(1);
    p.state[cdt] += tau;
  
    // Momentum transformation
    double p1( p.State( xp ) );
    double p2( p.State( yp ) );
    double p3divpbar = sqrt( ( 1.0 + p.state[dpop] ) * ( 1.0 + p.state[dpop] )
                              - p1*p1 - p2*p2 );
  
    p.state[xp] = cs*p.State( xp ) + sn*p3divpbar;
  }


  // Roll ------------------------------------
  else if( ( frm.getAxis(2) == u_z ) && ( frm.getAxis(1) != u_y ) ) 
  {
    cs = ( frm.getAxis(0) )(0);
    sn = ( frm.getAxis(1) )(0); // ??? right?
    // sin of angle by which magnet is rolled

    p.getState( inState );
  
    temp       = inState[0] * cs + inState[1] * sn;
    inState[1] = inState[1] * cs - inState[0] * sn;
    inState[0] = temp;
  
    temp       = inState[3] * cs + inState[4] * sn;
    inState[4] = inState[4] * cs - inState[3] * sn;
    inState[3] = temp;
   
    p.setState( inState) ;

  }

}


void Slot::processFrame( const Frame& frm, JetParticle& p ) const
{
  static const int x    = p.xIndex();
  static const int y    = p.yIndex();
  static const int cdt  = p.cdtIndex();
  static const int xp   = p.npxIndex();
  static const int yp   = p.npyIndex();
  static const int dpop = p.ndpIndex();

  static Vector u_z(3);
  static Vector u_y(3);
  static bool firstTime = 1;
  
  Mapping   inState ( p.State() );
  Jet       temp    ( inState.Env() );
  static    double  cs, sn;


  if( firstTime ) {
    firstTime = false;
    u_y(0) = 0.0; u_y(1) = 1.0; u_y(2) = 0.0;
    u_z(0) = 0.0; u_z(1) = 0.0; u_z(2) = 1.0;
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
    JetVector r        ( p.State(x)*e_1 + p.State(y)*e_2 );
    JetVector dummy    ( p.VectorBeta() );
    JetVector beta     ( dummy(0)*e_1 +
                         dummy(1)*e_2 +
                         dummy(2)*e_3 );
  
    Jet tau            ( - r(2) / beta(2) );
  
    ( p.state ).SetComponent( x,   r(0) + tau*beta(0) );
    ( p.state ).SetComponent( y,   r(1) + tau*beta(1) );
    ( p.state ).SetComponent( cdt, p.state(cdt) + tau );
  
    // Momentum transformation
    Jet p1( p.State( xp ) );
    Jet p2( p.State( yp ) );
    Jet p3divpbar = sqrt( ( 1.0 + p.state(dpop) ) * ( 1.0 + p.state(dpop) )
                              - p1*p1 - p2*p2 );
  
    ( p.state ).SetComponent( xp, cs*p.State( xp ) + sn*p3divpbar );
  }

  // Roll ------------------------------------
  else if( ( frm.getAxis(2) == u_z ) && ( frm.getAxis(1) != u_y ) ) 
  {
    cs = ( frm.getAxis(0) )(0);
    sn = ( frm.getAxis(1) )(0); // ??? right?
    // sin of angle by which magnet is rolled

    p.getState( inState );
  
    temp       = inState(0) * cs + inState(1) * sn;
    inState(1) = inState(1) * cs - inState(0) * sn;
    inState(0) = temp;

    temp       = inState(3) * cs + inState(4) * sn;
    inState(4) = inState(4) * cs - inState(3) * sn;
    inState(3) = temp;

    p.setState( inState) ;
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

  if      ( 0 != p_bml_e ) p_bml_e ->propagate( p );
  else if ( 0 != p_bml   ) p_bml   ->propagate( p );

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

    // REMOVE: double tauZero = length / p.ReferenceBeta();
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

    p.state[ p.xIndex()   ]  = r*u_1;
    p.state[ p.yIndex()   ]  = r*u_2;
    // REMOVE: p.state[ p.cdtIndex() ] += ( tau - tauZero );
    p.state[ p.cdtIndex() ] += ( tau - _ctRef );

    // Momentum transformation
    Vector momntm(3);
    momntm = ( p.NormalizedVectorMomentum() );
    p.state[ p.npxIndex() ] = momntm*u_1;
    p.state[ p.npyIndex() ] = momntm*u_2;
  }
}


void Slot::localPropagate( JetParticle& p )
{
  if     ( 0 != p_bml   ) p_bml   ->propagate( p );
  else if( 0 != p_bml_e ) p_bml_e ->propagate( p );

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

    // REMOVE: double tauZero = length / p.ReferenceBeta();
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

    p.state( p.xIndex()   )  = r*u_1;
    p.state( p.yIndex()   )  = r*u_2;
    // REMOVE: p.state( p.cdtIndex() ) += ( tau - tauZero );
    p.state( p.cdtIndex() ) += ( tau - _ctRef );

    // Momentum transformation
    JetVector mom( p.NormalizedVectorMomentum() );
    p.state( p.npxIndex() ) = mom*u_1;
    p.state( p.npyIndex() ) = mom*u_2;
  }
}


void Slot::localPropagate( ParticleBunch& x )
{
  if     ( 0 != p_bml   ) { p_bml   ->propagate( x ); }
  else if( 0 != p_bml_e ) { p_bml_e ->propagate( x ); }
  else {
    Particle* p;
    ParticleBunch::Iterator get( x );
    while((  p = (Particle*) get.next()  )) { this->localPropagate( *p ); }
  }
}


 
// --------------------------------------------------
// --- Functions passed on to tenant ----------------
// --------------------------------------------------

void Slot::setStrength   ( double x )
{
  if     ( 0 != p_bml   ) p_bml   ->setStrength( x );
  else if( 0 != p_bml_e ) p_bml_e ->setStrength( x );
}

void Slot::setStrength   ( double x , int i )
{
  if     ( 0 != p_bml   ) p_bml   ->setStrength( x, i );
  else if( 0 != p_bml_e ) p_bml_e ->setStrength( x, i );
}

void Slot::setCurrent    ( double x )
{
  if     ( 0 != p_bml   ) p_bml   ->setStrength( x );
  else if( 0 != p_bml_e ) p_bml_e ->setStrength( x );
}

bool Slot::setAlignment  ( const alignmentData& x )
{
  bool ret = true;
  if     ( 0 != p_bml   ) {
    ret = p_bml->setAlignment( x );
  }
  else if( 0 != p_bml_e ) {
    ret = p_bml_e->setAlignment( x );
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
  if     ( 0 != p_bml   ) return p_bml   ->Current();
  else if( 0 != p_bml_e ) return p_bml_e ->Current();
  else                    return 0.0;
}


double Slot::OrbitLength( const Particle& x )
{
  static bool firstTime = true;

  if     ( 0 != p_bml   ) return p_bml   ->OrbitLength( x );
  else if( 0 != p_bml_e ) return p_bml_e ->OrbitLength( x );
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
    return length;
  }
}
 

const char* Slot::Name() const
{
  if     ( 0 != p_bml   ) return p_bml   ->Name();
  else if( 0 != p_bml_e ) return p_bml_e ->Name();
  else                    return bmlnElmnt::Name();
}
