#include "Slot.h"

// --------------------------------------------------
// --- Constructors ---------------------------------
// --------------------------------------------------

Slot::Slot()
: bmlnElmnt()
{
  p_bml   = 0;
  p_bml_e = 0;
  align = new alignment;
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
    cerr << "*** ERROR ***                                 \n"
            "*** ERROR *** Slot::Slot                      \n"
            "*** ERROR *** Current implementation requires \n"
            "*** ERROR *** that frames be orthonormal.     \n"
            "*** ERROR ***                                 \n"
         << endl;
    exit(1);
  }

  p_bml   = 0;
  p_bml_e = 0;
  align   = 0;

  length = out.getOrigin() .Norm();
}


Slot::Slot( const char* nm, const Frame& y )
: bmlnElmnt(nm), out(y)
{
  if( !out.isOrthonormal() )
  {
    cerr << "*** ERROR ***                                 \n"
            "*** ERROR *** Slot::Slot                      \n"
            "*** ERROR *** Current implementation requires \n"
            "*** ERROR *** that frames be orthonormal.     \n"
            "*** ERROR ***                                 \n"
         << endl;
    exit(1);
  }

  p_bml   = 0;
  p_bml_e = 0;
  align   = 0;

  length = out.getOrigin() .Norm();
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
  short int ret = checkFrame( frm );

  if( 0 == ret ) {
    out = frm;
  }

  return ret;
}


short int Slot::checkFrame( const Frame& f ) const
{
  static const Frame zero;
  static const int y = 1;
  static const int z = 2;
  static short int ret;

  ret = 0;

  if( !f.isOrthonormal() ) {
    cerr << "*** ERROR ***                                 \n"
            "*** ERROR *** Slot::Slot                      \n"
            "*** ERROR *** Current implementation requires \n"
            "*** ERROR *** that frames be orthonormal.     \n"
            "*** ERROR ***                                 \n"
         << endl;
    ret = 1;
    exit(1);
  }

  if( f.getOrigin() != zero.getOrigin() ) {
    cerr << "*** ERROR ***                                 \n"
            "*** ERROR *** Slot::Slot                      \n"
            "*** ERROR *** Current implementation requires \n"
            "*** ERROR *** no displacement of origin.      \n"
            "*** ERROR ***                                 \n"
         << endl;
    ret = 2;
    exit(2);
  }

  else if( (  f.getAxis(y) != zero.getAxis(y) ) && 
           (  f.getAxis(z) != zero.getAxis(z) )
         ) {
    cerr << "*** ERROR ***                                 \n"
            "*** ERROR *** Slot::Slot                      \n"
            "*** ERROR *** Current implementation requires \n"
            "*** ERROR *** rotation only about either      \n"
            "*** ERROR *** y or z axis, but not both.      \n"
            "*** ERROR ***                                 \n"
         << endl;
    ret = 3;
    exit(3);
  }

  return ret;
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
      cerr << " **** ERROR ****" << endl;
      cerr << " **** ERROR **** Slot:readFrom(istream&): Expecting \"slot_END\"" << endl;
      cerr << " **** ERROR **** but got \"" << type << "\"\n";
      cerr << " **** ERROR ****" << endl;
      exit(1);
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


void Slot::Split( double, bmlnElmnt**, bmlnElmnt** )
{
  cerr << "*** ERROR *** Slot::Split is not implemented." << endl;
  exit(1);
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

    double tauZero = length / p.ReferenceBeta();
    double tau;
    double betaParallel = beta * u_3;

    if( betaParallel > 0.0 ) {
      tau = ( q - r )*u_3 / betaParallel;
    }
    else {
      cerr << "*** ERROR ***                                 \n"
              "*** ERROR *** Slot::localPropagate(Proton)    \n"
              "*** ERROR *** Velocity is not forward.        \n"
              "*** ERROR ***                                 \n"
           << endl;
      cerr << this->Name() << endl;
      cerr << in << "\n" << out << endl;
      cerr << "p.VectorBeta(): " << p.VectorBeta() << endl;
      cerr << "betaParallel: " << betaParallel << endl;
      exit(3);
    }

    r += tau*beta;
    r -= q;

    p.state[ p.xIndex()   ]  = r*u_1;
    p.state[ p.yIndex()   ]  = r*u_2;
    p.state[ p.cdtIndex() ] += ( tau - tauZero );

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

    double tauZero = length / p.ReferenceBeta();
    Jet    tau;
    Jet    betaParallel = beta * u_3;

    if( betaParallel.standardPart() > 0.0 ) {
      tau = ( q - r )*u_3 / betaParallel;
    }
    else {
      cerr << "*** ERROR ***                                 \n"
              "*** ERROR *** Slot::localPropagate(JetProton) \n"
              "*** ERROR *** Velocity is not forward.        \n"
              "*** ERROR ***                                 \n"
           << endl;
      cerr << this->Name() << endl;
      cerr << in << "\n" << out << endl;
      cerr << "p.VectorBeta(): " << p.VectorBeta() << endl;
      cerr << "betaParallel: " << betaParallel << endl;
      exit(3);
    }

    r += tau*beta;
    r -= q;

    p.state( p.xIndex()   )  = r*u_1;
    p.state( p.yIndex()   )  = r*u_2;
    p.state( p.cdtIndex() ) += ( tau - tauZero );

    // Momentum transformation
    JetVector mom( p.NormalizedVectorMomentum() );
    p.state( p.npxIndex() ) = mom*u_1;
    p.state( p.npyIndex() ) = mom*u_2;
  }
}


void Slot::localPropagate( ParticleBunch& x )
{
  if     ( 0 != p_bml   ) p_bml   ->propagate( x );
  else if( 0 != p_bml_e ) p_bml_e ->propagate( x );

  else {
    Particle* p;
    slist_iterator getNext( (slist&) x );
    while((  p = (Particle*) getNext()  )) {
      this->localPropagate( *p );
    }
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

void Slot::setAlignment  ( const alignmentData& x )
{
  if     ( 0 != p_bml   ) p_bml   ->setAlignment( x );
  else if( 0 != p_bml_e ) p_bml_e ->setAlignment( x );
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
  else                    {
    if( firstTime ) {
      firstTime = false;
      cerr << "*** WARNING ***                                 \n"
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
