/*
 *  This "lattice function" method is based on
 *  D.A.Edwards and L.C.Teng
 *  "Parametrization of Linear Coupled Motion in Periodic Systems"
 *  IEEE Tr.NucSci.
 *  NS-20(3), 885(1973).
 *       -- Leo Michelotti
 *          June 6, 1994   ( D-Day + 50 years )
 */

#ifdef __VISUAL_CPP__
#include <iomanip>
#else
#include <iomanip.h>
#endif

#include "LattFuncSage.h"
#include "FPSolver.h"

ETinfo::ETinfo( const ETinfo& x ) {
 map       = x.map;
 mapInv    = x.mapInv;
 beta.hor  = x.beta.hor;
 beta.ver  = x.beta.ver;
 alpha.hor = x.alpha.hor;
 alpha.ver = x.alpha.ver;
 phi       = x.phi;
 D         = x.D;
 EV        = x.EV;
}


// ... Globals:
double          LattFuncSage::_csH, 
                LattFuncSage::_csV, 
                LattFuncSage::_snH, 
                LattFuncSage::_snV;
Mapping*        LattFuncSage::_theMap;



void LattFuncSage::set_dpp( double x )
{
  if( x > 0.0 ) {
    _dpp = x;
  }
  else {
    cerr << "*** WARNING ***                        \n"
            "*** WARNING *** LattFuncSage::set_dpp   \n"
            "*** WARNING *** Non-positive argument  \n"
            "*** WARNING *** changed.               \n"
            "*** WARNING ***                        \n"
	 << endl;
    if( x != 0.0 ) _dpp = -x;
  }
}



int LattFuncSage::attachETLattFuncs( bmlnElmnt* lbe ) 
{
 double          dcos, cos2phi, sin2phi, tanphi;
 MatrixD mtrx;
 static
 MatrixD M( 2, 2 ), N( 2, 2 ), m( 2, 2 ), n( 2, 2 ),
              D( 2, 2 ), S( "J", 2 ), A( 2, 2 ), B( 2, 2 ),
              U( "I", 2 );
 ETinfo*   ETptr;
 Barnacle* ETbarn;
 Mapping   localMap;

 ETbarn = lbe->dataHook.lift( "LattFuncSage" );
 ETptr = (ETinfo*) ETbarn->info;
 localMap = ETptr->map( (*LattFuncSage::_theMap)( ETptr->mapInv ) );
 mtrx = localMap.Jacobian();
 ETptr->EV = mtrx.eigenVectors(); 

 M( 0, 0 ) = mtrx( 0, 0 );     n( 0, 0 ) = mtrx( 0, 1 );
 M( 0, 1 ) = mtrx( 0, 3 );     n( 0, 1 ) = mtrx( 0, 4 );
 M( 1, 0 ) = mtrx( 3, 0 );     n( 1, 0 ) = mtrx( 3, 1 );
 M( 1, 1 ) = mtrx( 3, 3 );     n( 1, 1 ) = mtrx( 3, 4 );

 m( 0, 0 ) = mtrx( 1, 0 );     N( 0, 0 ) = mtrx( 1, 1 );
 m( 0, 1 ) = mtrx( 1, 3 );     N( 0, 1 ) = mtrx( 1, 4 );
 m( 1, 0 ) = mtrx( 4, 0 );     N( 1, 0 ) = mtrx( 4, 1 );
 m( 1, 1 ) = mtrx( 4, 3 );     N( 1, 1 ) = mtrx( 4, 4 );


 if( ( m == 0.0 ) && ( n == 0.0 ) ) {  // Uncoupled lattice

  A = M;
  B = N;
  sin2phi = tanphi = 0.0;
  cos2phi = 1.0;
 }

 else {                                // Coupled lattice

  if( fabs( LattFuncSage::_csH - LattFuncSage::_csV ) < 1.0e-4 ) {
    cout << "\n"
         << "*** ERROR *** LattFuncSage()                        \n"
         << "*** ERROR *** \"Horizontal\" and \"vertical\" tunes\n"
         << "*** ERROR *** are too near each other for          \n"
         << "*** ERROR *** reasonable results.                  \n"
         << "*** ERROR ***                                      \n"
         << "*** ERROR *** The calculation is meaningless.      \n"
         << "*** ERROR ***                                        " << endl;
    return 2;
  }

  dcos    = LattFuncSage::_csH - LattFuncSage::_csV;
  cos2phi = ( M - N ).trace() / ( 2.0 *( dcos ) );
  if( fabs( cos2phi - 1.0 ) < 1.0e-4 ) cos2phi =   1.0;  // ??? Rather coarse,
  if( fabs( cos2phi + 1.0 ) < 1.0e-4 ) cos2phi = - 1.0;  // ??? isn't it?

  if( fabs( cos2phi ) > 1.0 ) {
   cout << "\n"
        << "*** ERROR: LattFuncSage()                        \n"
        << "*** ERROR: cos( 2 phi ) = " 
        <<           setprecision(10) << cos2phi 
        <<                       "; has magnitude larger    \n"
        << "*** ERROR: than one.                            \n"
        << "*** ERROR:                                      \n"
        << "*** ERROR: Cannot continue calculation.         \n"
        << "*** ERROR:                                        " << endl;
   return 3;
  }

  if( cos2phi < 0.0 ) {
   sin2phi = LattFuncSage::_csH;  // Variable used as dummy register.
   LattFuncSage::_csH = LattFuncSage::_csV;
   LattFuncSage::_csV = sin2phi;
   dcos    = - dcos;
   cos2phi = - cos2phi;
  }
  sin2phi = sqrt( 1.0 - cos2phi*cos2phi );
  tanphi  = sin2phi / ( 1.0 + cos2phi );

  if( fabs( sin2phi ) > 1.0e-8 ) 
    D = -( m + S*n.transpose()*S.transpose() ) * ( 1.0 / ( dcos*sin2phi ));
  else {
    D(0,0) = 1.0;  D(0,1) = 0.0;
    D(1,0) = 0.0;  D(1,1) = 1.0;
  }

  if( fabs( D.determinant() - 1.0 ) > 1.0e-4 ) {
    cout << "\n"
         << "*** ERROR *** LattFuncSage()                        \n"
         << "*** ERROR *** The matrix D is non-symplectic.      \n"
         << "*** ERROR *** |D| = " << D.determinant() << "      \n"
         << "*** ERROR ***                                        " << endl;
    return 4;
  }

  // ...... Edwards-Teng sign convention.
  if( D.trace() < 0.0 ) {
   D = -D;
   sin2phi = -sin2phi;
   tanphi  = -tanphi;
  }
  A = M - D.inverse()*m*tanphi;
  B = N + D*n*tanphi;
 }

 // .......... Save the ET information
 ETptr->phi       = atan( tanphi ) / 2.0;
 ETptr->D         = D;

 // .......... Lattice functions ..........................

 // ......  First the "horizontal" ......
 MatrixD JH;
 JH = A - LattFuncSage::_csH*U;
 if( JH( 0, 1 ) > 0.0 )
  LattFuncSage::_snH =   sqrt( 1.0 - LattFuncSage::_csH*LattFuncSage::_csH );
 else {
  LattFuncSage::_snH = - sqrt( 1.0 - LattFuncSage::_csH*LattFuncSage::_csH );
 }

 ETptr->beta .hor = JH( 0, 1 ) / LattFuncSage::_snH;
 ETptr->alpha.hor = JH( 0, 0 ) / LattFuncSage::_snH;

 // .......... A little test to keep everyone honest .....
 if( JH( 0, 0 ) != 0.0 )
  if( fabs( ( JH(0,0) + JH(1,1) ) / ( JH(0,0) - JH(1,1) ) ) > 1.0e-4 ) {
   cout << endl;
   cout << "*** WARNING ***                                " << endl;
   cout << "*** WARNING *** LattFuncSage()                  " << endl;
   cout << "*** WARNING *** \"Horizontal\" matrix does not " << endl;
   cout << "*** WARNING *** pass symplecticity test.       " << endl;
   cout << "*** WARNING *** JH( 0, 0 ) = " << JH( 0, 0 )     << endl;
   cout << "*** WARNING *** JH( 1, 1 ) = " << JH( 1, 1 )     << endl;
   cout << "*** WARNING ***                                " << endl;
   cout << "*** WARNING *** The ratio is " 
        << fabs( ( JH(0,0) + JH(1,1) ) / ( JH(0,0) - JH(1,1) ) )
        << endl;
   cout << "*** WARNING ***                                " << endl;
   // return 5;
  }


 // ......  Then  the "vertical" ......
 MatrixD JV;
 JV = B - LattFuncSage::_csV*U;
 if( JV( 0, 1 ) > 0.0 )
  LattFuncSage::_snV =   sqrt( 1.0 - LattFuncSage::_csV*LattFuncSage::_csV );
 else {
  LattFuncSage::_snV = - sqrt( 1.0 - LattFuncSage::_csV*LattFuncSage::_csV );
 }

 ETptr->beta .ver = JV( 0, 1 ) / LattFuncSage::_snV;
 ETptr->alpha.ver = JV( 0, 0 ) / LattFuncSage::_snV;

 // .......... A little test to keep everyone honest .....
 if( JV( 0, 0 ) != 0.0 )
  if( fabs( ( JV(0,0) + JV(1,1) ) / ( JV(0,0) - JV(1,1) ) ) > 1.0e-4 ) {
   cout << endl;
   cout << "*** WARNING ***                                " << endl;
   cout << "*** WARNING *** LattFuncSage()                  " << endl;
   cout << "*** WARNING *** \"Vertical\" matrix does not   " << endl;
   cout << "*** WARNING *** pass symplecticity test.       " << endl;
   cout << "*** WARNING *** JV( 0, 0 ) = " << JV( 0, 0 )     << endl;
   cout << "*** WARNING *** JV( 1, 1 ) = " << JV( 1, 1 )     << endl;
   cout << "*** WARNING ***                                " << endl;
   cout << "*** WARNING *** The ratio is " 
        << fabs( ( JV(0,0) + JV(1,1) ) / ( JV(0,0) - JV(1,1) ) )
        << endl;
   cout << "*** WARNING ***                                " << endl;
   // return 6;
 }


 lbe->dataHook.append( ETbarn );

 return 0;
}


LattFuncSage::LattFuncSage( const beamline* x ) 
: _dpp( 0.00005 ), _verbose(0)
{
 _myBeamline = (beamline*) x;
 _theCalculator = LattFuncSage::Orig_RX_Calc;
}

LattFuncSage::~LattFuncSage() {
 // Will remove barnacles from its beamline.
}


/* ============================================================== */

void LattFuncSage::eraseAll() {
 dlist_iterator getNext ( *(dlist*) _myBeamline );
 bmlnElmnt*      be;
 while((  be = (bmlnElmnt*) getNext()  )) 
  be->dataHook.eraseAll( "LattFuncSage" );
}


char LattFuncSage::no( bmlnElmnt* )
{
  return 0;
}

char LattFuncSage::yes( bmlnElmnt* )
{
  return 1;
}


int LattFuncSage::Orig_RX_Calc( LattFuncSage* ptr_et,
                               JetParticle*   ptr_jp,
                               ET_CRITFUNC  Crit
                             ) 
{
 int             ret;
 bmlnElmnt*      be;
 dlist_looper    getNext ( *(dlist*) ptr_et->_myBeamline );
 double*         zero = new double[ BMLN_dynDim ];
 double          t;
 int             i;
 MatrixD         mtrx( BMLN_dynDim, BMLN_dynDim );
 ETinfo*         ETptr;
 ETtunes*        tuneptr;

 LattFuncSage::_theMap = new Mapping;

 // .......... Propagate a JetParticle element by element
 // .......... It is assumed to be on a closed orbit!!
 for( i = 0; i < BMLN_dynDim; i++ ) 
   zero[i] = ptr_jp->State(i).standardPart();
 // ??? REMOVE Jet::FixReference( zero );

 while (( be = (bmlnElmnt*) getNext() )) {
   be->propagate( *ptr_jp );
   if( !Crit ) {
     ETptr = new ETinfo;
     ptr_jp->getState( ETptr->map );   // ??? Change statements?  Use pointer?
     ETptr->mapInv = ETptr->map.Inverse();
     be->dataHook.append( "LattFuncSage", ETptr );
   }
   else if( (*Crit)( be ) ) {
     ETptr = new ETinfo;
     ptr_jp->getState( ETptr->map );   // ??? Change statements?  Use pointer?
     ETptr->mapInv = ETptr->map.Inverse();
     be->dataHook.append( "LattFuncSage", ETptr );
   }
 }

 // .......... Calculating tunes .........................
 ptr_jp->getState( *LattFuncSage::_theMap );
 mtrx = LattFuncSage::_theMap->Jacobian();
 MatrixC lambda;
 lambda = mtrx.eigenValues();

 for( i = 0; i < BMLN_dynDim; i++ )
  if( fabs( abs(lambda(i)) - 1.0 ) > 1.0e-4 ) {
   cout << "\n"
        << "*** ERROR ***                                     \n"
        << "*** ERROR ***                                     \n"
        << "*** ERROR *** LattFuncSage()                       \n"
        << "*** ERROR *** The lattice is linearly unstable.   \n"
        << "*** ERROR *** lambda( " << i << " ) has magnitude = " 
        << abs(lambda(i))
        << "\n"
        << "*** ERROR ***                                     \n"
        << endl;
   delete [] zero;
   delete LattFuncSage::_theMap;
   ptr_et->eraseAll();
   return 10;
  }

 if( ( abs( lambda(0) - conj( lambda(3) ) ) > 1.0e-4 )  ||
     ( abs( lambda(1) - conj( lambda(4) ) ) > 1.0e-4 )
   ) {
   cout << "\n"
        << "*** ERROR *** LattFuncSage()                        \n"
        << "*** ERROR *** Conjugacy condition has been violated\n"
        << "*** ERROR *** The lattice may be linearly unstable.\n"
        << "*** ERROR *** Eigenvalues =                        \n"
        << "*** ERROR *** " << lambda << endl;
   delete [] zero;
   delete LattFuncSage::_theMap;
   ptr_et->eraseAll();
   return 11;
 }

 LattFuncSage::_csH = real( lambda(0) );    
 LattFuncSage::_csV = real( lambda(1) );
 // LattFuncSage::_snH = imag( lambda(0) );    
 // LattFuncSage::_snV = imag( lambda(1) );

 // Go through the accelerator again and attach
 // the Edwards Teng lattice info to each element.

 while ((  be = (bmlnElmnt*) getNext()  )) {

  if( !Crit ) {
   if( ( ret = attachETLattFuncs( be ) ) != 0 ) {
    delete [] zero;
    delete LattFuncSage::_theMap;
    ptr_et->eraseAll();
    return ret;
   }
  }
  else if( (*Crit)( be ) ) {
   if( ( ret = attachETLattFuncs( be ) ) != 0 ) {
    delete [] zero;
    delete LattFuncSage::_theMap;
    ptr_et->eraseAll();
    return ret;
   }
  }
 }

 tuneptr = new ETtunes;
 t = atan2( LattFuncSage::_snH, LattFuncSage::_csH );
 if( t < 0.0 )   t += M_TWOPI;
 tuneptr->hor = ( t / M_TWOPI );
 t = atan2( LattFuncSage::_snV, LattFuncSage::_csV );
 if( t < 0.0 )   t += M_TWOPI;
 tuneptr->ver = ( t / M_TWOPI );

 ptr_et->_myBeamline->dataHook.append( "Tunes", tuneptr );

 delete [] zero;
 delete LattFuncSage::_theMap;
 return 0;
}


int LattFuncSage::Fast_CS_Calc( LattFuncSage*, JetParticle*, ET_CRITFUNC )
{
  return 1;
}


int LattFuncSage::Disp_Calc( LattFuncSage* p_et, 
                            JetParticle* p_jp, 
                            ET_CRITFUNC  Crit )
{
  if( p_et->_verbose ) {
    cout << "LattFuncSage -- Entering LattFuncSage::Disp_Calc" << endl;
  }

  // Preliminary steps ...
  beamline* bml = p_et->_myBeamline;
  Jet__environment* storedEnv = Jet::lastEnv;   // Just precautionary
  Jet::lastEnv = (p_jp->State()).Env();


  // Data structures for handling RF ...
  struct strengthData
  {
    bmlnElmnt*    address;
    double        strength;
  };
  strengthData* sd;

  slist cavityStrengths;


  // Helper particle ...
  Particle* p_prt =  p_jp->ConvertToParticle();
  short int i_x   =  p_prt->xIndex();
  short int i_y   =  p_prt->yIndex();
  short int i_z   =  p_prt->cdtIndex();
  short int i_px  =  p_prt->npxIndex();
  short int i_py  =  p_prt->npyIndex();
  short int i_dpp =  p_prt->ndpIndex();

  Vector inState(  p_prt->State() );
  inState( i_dpp ) = 0.0;
  p_prt->setState( inState );

  Vector co     (  inState );
  

  // Turn off all RF ...
  bmlnElmnt* q = 0;
  DeepBeamlineIterator getNext( bml );

  while((  q = getNext++  )) 
  {
    if( 0 == strcmp( "thinrfcavity", q->Type() ) ) 
    {
      sd = new strengthData;
      sd->address = q;
      sd->strength = q->Strength();
      cavityStrengths.append( sd );
      q->setStrength( 0.0 );
    }
  }


  // Set up an FPSolver ...
  int i;
  double jumpScale [] = { 1.0e-3, 1.0e-3, 1.0e-4, 1.0e-4 };
  double zeroScale [] = { 1.0e-9, 1.0e-9, 1.0e-9, 1.0e-9 };

  FPSolver fp( bml );
  for( i = 0; i < 6; i++ ) fp.ZeroScale(i) = zeroScale[i];
  for( i = 0; i < 6; i++ ) fp.JumpScale(i) = jumpScale[i];


  // Check to see if the Particle is on a transverse closed orbit.
  bml->propagate( *p_prt );
  if( ( fabs( inState(i_x)  - p_prt->State(i_x)  ) > 1.0e-6 ) ||
      ( fabs( inState(i_y)  - p_prt->State(i_y)  ) > 1.0e-6 ) ||
      ( fabs( inState(i_px) - p_prt->State(i_px) ) > 1.0e-6 ) ||
      ( fabs( inState(i_py) - p_prt->State(i_py) ) > 1.0e-6 )
  ){

    if( p_et->_verbose ) {
      cout << "LattFuncSage --- Starting calculation of closed orbit." << endl;
    }

    // If not, solve for the closed orbit ...
    p_prt->setState( inState );
    fp( (Proton*) p_prt, "transverse", LattFuncSage::no );  // ???
              // ??? The recast is a kludge which must be changed.
    
    // Test the new closed orbit ...
    co = p_prt->State();
    bml->propagate( *p_prt );
    if( ( fabs( co(i_x)  - p_prt->State(i_x)  ) > 1.0e-6 ) ||
        ( fabs( co(i_y)  - p_prt->State(i_y)  ) > 1.0e-6 ) ||
        ( fabs( co(i_px) - p_prt->State(i_px) ) > 1.0e-6 ) ||
        ( fabs( co(i_py) - p_prt->State(i_py) ) > 1.0e-6 )
    ){
      cerr << "*** ERROR ***                                   \n"
              "*** ERROR *** LattFuncSage::Disp_Calc            \n"
              "*** ERROR *** Closed orbit not correct.         \n"
              "*** ERROR ***                                   \n"
	   << endl;
      if( p_et->_verbose ) {
        cout << "LattFuncSage -- Leaving LattFuncSage::Disp_Calc" << endl;
      }

      return 1;
    }

    if( p_et->_verbose ) {
      cout << "LattFuncSage --- Closed orbit successfully calculated." << endl;
    }

    p_prt->setState( co );
    inState = co;
  }


  // Calculate the closed orbit for an off-momentum particle ...
  if( p_et->_verbose ) {
    cout << "LattFuncSage --- Starting calculation of offset closed orbit." << endl;
  }


  Particle* secondParticle = p_prt->Clone();
  double dpp = p_et->get_dpp();
  double energy = p_prt->ReferenceEnergy();
  double mass = p_prt->Mass();
  double momentum = sqrt( energy*energy - mass*mass )*( 1.0 + dpp );
  energy = sqrt( momentum*momentum + mass*mass );

  secondParticle->SetReferenceEnergy( energy );
  fp( (Proton*) secondParticle, "transverse", LattFuncSage::no );  // ???
                     // ??? The recast is a kludge which must be changed.
  co = secondParticle->State();


  // Check the off-momentum closed orbit ...
  bml->propagate( *secondParticle );
  if( ( fabs( co(i_x)  - secondParticle->State(i_x)  ) > 1.0e-6 ) ||
      ( fabs( co(i_y)  - secondParticle->State(i_y)  ) > 1.0e-6 ) ||
      ( fabs( co(i_px) - secondParticle->State(i_px) ) > 1.0e-6 ) ||
      ( fabs( co(i_py) - secondParticle->State(i_py) ) > 1.0e-6 )
  ){
    cerr << "*** ERROR ***                                   \n"
            "*** ERROR *** LattFuncSage::Disp_Calc            \n"
            "*** ERROR *** Offset closed orbit not correct.  \n"
            "*** ERROR ***                                   \n"
         << endl;
    if( p_et->_verbose ) {
      cout << "LattFuncSage -- Leaving LattFuncSage::Disp_Calc" << endl;
    }
    return 2;
  }

  if( p_et->_verbose ) {
    cout << "LattFuncSage --- Offset closed orbit successfully calculated." << endl;
  }

  secondParticle->setState( co );



  // Attach initial dispersion data to the beamline ...
  lattFunc* lf;
  Vector d( co.Dim() );
  double lng = 0.0;

  d = ( secondParticle->State()  -  p_prt->State() ) / dpp;

  lf = new lattFunc;
  lf->dispersion.hor = d( i_x  );
  lf->dPrime.hor     = d( i_px );
  lf->dispersion.ver = d( i_y  );
  lf->dPrime.ver     = d( i_py );
  lf->arcLength      = lng;
  
  bml->dataHook.eraseAll( "Dispersion" );
  bml->dataHook.insert( new Barnacle( "Dispersion", lf ) );



  // Attach dispersion data wherever desired ...
  if( p_et->_verbose ) {
    cout << "LattFuncSage --- Attaching dispersion data to the elements." << endl;
  }

  getNext.reset();
  while((  q = getNext++  )) 
  {
    q->propagate( *p_prt );
    q->propagate( *secondParticle );

    lng += q->OrbitLength( *p_prt );

    if( Crit( q ) ) 
    {
      d = ( secondParticle->State()  -  p_prt->State() ) / dpp;
  
      lf = new lattFunc;
      lf->dispersion.hor = d( i_x  );
      lf->dPrime.hor     = d( i_px );
      lf->dispersion.ver = d( i_y  );
      lf->dPrime.ver     = d( i_py );
      lf->arcLength      = lng;
  
      q->dataHook.eraseAll( "Dispersion" );
      q->dataHook.insert( new Barnacle( "Dispersion", lf ) );
    }    
  }  


  // Final operations ....................................

  // Return RF to original settings before leaving ...
  while((  sd = (strengthData*) cavityStrengths.get()  )) 
  {
    ( sd->address )->setStrength( sd->strength );
    delete sd;
  }

  // Reset the Jet environment ...
  Jet::lastEnv = storedEnv;

  delete p_prt;


  if( p_et->_verbose ) {
    cout << "LattFuncSage -- Leaving LattFuncSage::Disp_Calc" << endl;
  }
  return 0;
}


int LattFuncSage::ET_Disp_Calc( LattFuncSage*, JetParticle*, ET_CRITFUNC )
{
  return 1;
}


int LattFuncSage::CS_Disp_Calc( LattFuncSage*, JetParticle*, ET_CRITFUNC )
{
  return 1;
}




int LattFuncSage::doCalc( const lattFunc& W, JetParticle& p, ET_CRITFUNC Crit )
{
  // This function replaces int beamline::twiss( lattFunc& W, JetParticle& p)

  bmlnElmnt*      be;
  dlist_iterator  getNext ( *(dlist*) _myBeamline );
  double          t, oldpsiH, oldpsiV, lng;
  int             elmntPos = 0;
  int             count;
  Jet*            z;
  double*         zero;
  lattFunc*       lf;
  lattFunc*       lfp = 0;
  lattFunc*       latticeFunctions = new lattFunc;
  MatrixD         mtrx(BMLN_dynDim,BMLN_dynDim,0.0);
  int             i;
  Mapping         map;

  zero           = new double   [ BMLN_dynDim ];
  for(i=0; i<BMLN_dynDim; i++) zero[i] = 0.0;
  z              = new Jet      [ BMLN_dynDim ];

  // .......... Calculating betas and alphas ..............
  latticeFunctions->beta .hor = W.beta.hor;
  latticeFunctions->beta .ver = W.beta.ver;
  latticeFunctions->alpha.hor = W.alpha.hor;
  latticeFunctions->alpha.ver = W.alpha.ver;
  latticeFunctions->psi  .hor = 0.0;
  latticeFunctions->psi  .ver = 0.0;
  latticeFunctions->dispersion.hor = W.dispersion.hor;
  latticeFunctions->dPrime.hor = W.dPrime.hor;
  latticeFunctions->dispersion.ver = W.dispersion.ver;
  latticeFunctions->dPrime.ver = W.dPrime.ver;

  count   = 0;
  oldpsiH = 0.0;
  oldpsiV = 0.0;
  p.setState(zero);

  double alpha0H = latticeFunctions->alpha.hor;
  double beta0H = latticeFunctions->beta.hor;
    
  double alpha0V = latticeFunctions->alpha.ver;
  double beta0V = latticeFunctions->beta.ver;

  MatrixD dispFinal(6,1);
  MatrixD dispVector(6,1);
  dispVector(0,0) = latticeFunctions->dispersion.hor;
  dispVector(1,0) = latticeFunctions->dispersion.ver;
  dispVector(2,0) = 0.0;
  dispVector(3,0) = latticeFunctions->dPrime.hor;
  dispVector(4,0) = latticeFunctions->dPrime.ver;
  dispVector(5,0) = 1.0;

  double tb;                  // use temp variables to save calc time
  Particle* ptr_dummy = p.ConvertToParticle();  // This must be deleted before returning.

  while ((  be = (bmlnElmnt*) getNext()  )) {
    lng = be -> OrbitLength( *ptr_dummy );
    elmntPos++;

    if( strcasecmp( be->Type(), "beamline" ) == 0 ) {
      cerr << "\n *** SORRY: beamline::twiss cannot "
              "handle inserted beamlines yet."
           << endl;
      exit(0);
    } else
      be -> propagate( p );

    // .......... While calculating lattice functions .......
    lf = new lattFunc;

    be->dataHook.eraseFirst( "Twiss" );
    be->dataHook.insert( new Barnacle( "Twiss", lf ) );
    p.getState(map);
    mtrx = map.Jacobian();
    dispFinal = mtrx * dispVector;

    if( ( 0 != strcmp( be->Type(), "rbend"    ) ) && 
        ( 0 != strcmp( be->Type(), "CF_rbend" ) ) && 
        ( 0 != strcmp( be->Type(), "Slot"     ) ) )
    {
      tb = mtrx(0,0) * beta0H -  mtrx(0,3) * alpha0H;
      lf->beta.hor = ( tb * tb + mtrx(0,3) * mtrx(0,3))/beta0H;

      lf->alpha.hor = -1.0*(tb * (mtrx(3,0)*beta0H - mtrx(3,3)*alpha0H) +
                            mtrx(0,3)*mtrx(3,3))/beta0H;

      t = atan2(mtrx(0,3),tb);
      while(t < oldpsiH) t += M_TWOPI;
      lf->psi.hor = oldpsiH = t;

      tb = mtrx(1,1) * beta0V -  mtrx(1,4) * alpha0V;
      lf->beta.ver = (tb * tb + mtrx(1,4) * mtrx(1,4))/beta0V;
 
      lf->alpha.ver = -1.0*(tb * (mtrx(4,1)*beta0V - mtrx(4,4)*alpha0V) +
                            mtrx(1,4)*mtrx(4,4))/beta0V;

      t = atan2(mtrx(1,4),tb);
      while(t < oldpsiV) t += M_TWOPI;
      lf->psi.ver = oldpsiV = t;
    }

    else { // ??? This is a kludge.
      tb = mtrx(0,0) * beta0H -  mtrx(0,3) * alpha0H;
      lf->beta.hor = ( tb * tb + mtrx(0,3) * mtrx(0,3))/beta0H;

      lf->alpha.hor = -1.0*(tb * (mtrx(3,0)*beta0H - mtrx(3,3)*alpha0H) +
                            mtrx(0,3)*mtrx(3,3))/beta0H;

      lf->psi.hor = oldpsiH;

      tb = mtrx(1,1) * beta0V -  mtrx(1,4) * alpha0V;
      lf->beta.ver = (tb * tb + mtrx(1,4) * mtrx(1,4))/beta0V;
 
      lf->alpha.ver = -1.0*(tb * (mtrx(4,1)*beta0V - mtrx(4,4)*alpha0V) +
                            mtrx(1,4)*mtrx(4,4))/beta0V;

      lf->psi.ver = oldpsiV;
    }

    lf->dispersion.hor = dispFinal(0,0);
    lf->dPrime.hor = dispFinal(3,0);

    lf->dispersion.ver = dispFinal(1,0);
    lf->dPrime.ver = dispFinal(4,0);

    if ( count == 0 ) 
      lf->arcLength = lng;
    else 
      lf->arcLength = lng + lfp->arcLength;
    lfp = lf;

    count++;
  } // end while loop over the beamline elements ..............

  // .......... A little test to keep everyone honest .....
  if ( count != _myBeamline->countHowMany() ) {
    cerr << "*** ERROR: beamline::twiss(LatticeFunct, JetParticle):" << endl;
    cerr << "*** ERROR: A horrible, inexplicable error has occurred!" << endl;
    cerr << "*** ERROR: num elements seen, " << count << endl;
    cerr << "*** ERROR: is not equal to num elements expected, " 
         << _myBeamline->countHowMany() << endl;
    cerr << "*** ERROR: Bailing out" << endl;
    exit(1);
  }

  // .......... Cleaning up and leaving ...................

   delete [] zero;
   delete [] z;
   delete latticeFunctions;

   delete ptr_dummy;
   return 0;
}
