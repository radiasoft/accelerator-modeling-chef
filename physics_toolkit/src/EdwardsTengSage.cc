/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      EdwardsTengSage.cc
******  Version:   1.0
******                                                                
******  Copyright (c) 2004  Universities Research Association, Inc.   
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
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******                                                                
**************************************************************************
*************************************************************************/

//
//  This "lattice function" method is based on
//  D.A.Edwards and L.C.Teng
//  "Parametrization of Linear Coupled Motion in Periodic Systems"
//  IEEE Tr.NucSci.
//  NS-20(3), 885(1973).
//       -- Leo Michelotti
//          June 6, 1994   ( D-Day + 50 years )
//
//  The Sage version is modified from class EdwardsTeng
//       -- Leo Michelotti
//          April 14, 2004
//

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iomanip>

#include <basic_toolkit/iosetup.h>
#include <physics_toolkit/EdwardsTengSage.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/BeamlineIterator.h>

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;

// ============================================================== //

// ... Globals:
double   EdwardsTengSage::_csH, 
         EdwardsTengSage::_csV, 
         EdwardsTengSage::_snH, 
         EdwardsTengSage::_snV;
Mapping* EdwardsTengSage::_theMapPtr;


// ============================================================== //

EdwardsTengSage::Info::Info() 
: D(2,2), EV(6,6)
{
  arcLength = -1.0;
  beta.hor  = 0.0;
  beta.ver  = 0.0;
  alpha.hor = 0.0;
  alpha.ver = 0.0;
  phi       = 0.0;
}


EdwardsTengSage::Info::Info( const Info& x ) 
{
  arcLength = x.arcLength;
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


// ============================================================== //

EdwardsTengSage::EdwardsTengSage( const beamline* x, bool doClone ) 
: Sage( x, doClone ), _calcs(0), _n(0)
{
}


EdwardsTengSage::EdwardsTengSage( const beamline& x, bool doClone ) 
: Sage( &x, doClone ), _calcs(0), _n(0)
{
}


EdwardsTengSage::~EdwardsTengSage()
{
  _deleteCalcs();
}

void EdwardsTengSage::_deleteCalcs()
{
  if( 0 != _calcs ) {
    for( int i = 0; i < _n; i++ ) 
    { if( 0 != _calcs[i] ) {delete _calcs[i];} }
    delete [] _calcs;
    _calcs = 0;
    _n = 0;
  }
}


// ============================================================== //


int EdwardsTengSage::eigenTuneCalc( const JetParticle& ptr_jp, 
                                    EdwardsTengSage::Tunes& answer )
{
  const int x  = Particle::xIndex();
  const int y  = Particle::yIndex();
  const int xp = Particle::npxIndex();
  const int yp = Particle::npyIndex();

  MatrixD mtrx;
  mtrx = ptr_jp.State().Jacobian();  
  MatrixC lambda;
  lambda = mtrx.Matrix::eigenValues();
  // In the above, it is necessary to qualify the member 
  // function eigenValues(). Without explicit class qualifier, 
  // the complex form of the algorithm is called. 
  // This should not be so. Compiler bug ???
            

  if( ( std::abs(1.0 - std::abs(lambda(0)) > 1.0e-8 ) ) || 
      ( std::abs(1.0 - std::abs(lambda(1)) > 1.0e-8 ) )    ){
    (*pcerr) << "\n*** ERROR ***                                   "
         << "\n*** ERROR *** EdwardsTengSage::TuneCalc()       "
         << "\n*** ERROR *** The lattice is linearly unstable. "
         << "\n*** ERROR *** | lambda(0) | = " 
         << std::abs(lambda(0))
         << "\n*** ERROR *** | lambda(1) | = " 
         << std::abs(lambda(1))
         << "\n*** ERROR ***                                   "
         << endl;
    return 10;
  }

  if( ( std::abs( lambda(0) - conj( lambda(3) ) ) > 1.0e-4 )  ||
      ( std::abs( lambda(1) - conj( lambda(4) ) ) > 1.0e-4 )
    ) {
    (*pcerr) << "\n*** ERROR ***                                      "
         << "\n*** ERROR *** EdwardsTengSage::TuneCalc()          "
         << "\n*** ERROR *** Conjugacy condition has been violated"
         << "\n*** ERROR *** The lattice may be linearly unstable."
         << "\n*** ERROR *** Eigenvalues =                        "
         << "\n*** ERROR *** " 
         << lambda 
         << endl;
    return 11;
  }

  double cs = real( lambda(0) );
  double sn = sqrt( 1.0 - cs*cs );
  if( mtrx(x,xp) < 0.0 ) { sn = - sn; }
  double t = atan2( sn, cs );
  while( t < 0.0 ) { t += M_TWOPI; }
  answer.hor = ( t / M_TWOPI );

  cs = real( lambda(1) );
  sn = sqrt( 1.0 - cs*cs );
  if( mtrx(y,yp) < 0.0 ) { sn = - sn; }
  t = atan2( sn, cs );
  while( t < 0.0 ) { t += M_TWOPI; }
  answer.ver = ( t / M_TWOPI );

  return 0;
}


// ============================================================== //


int EdwardsTengSage::_attachETFuncs( bmlnElmnt* lbe ) 
{
 double  dcos, cos2phi, sin2phi, tanphi;
 MatrixD mtrx;
 MatrixD M( 2, 2 ), N( 2, 2 ), m( 2, 2 ), n( 2, 2 ),
         D( 2, 2 ), S( "J", 2 ), A( 2, 2 ), B( 2, 2 ),
         U( "I", 2 );
 Info*     ETptr;
 Barnacle* ETbarn;
 Mapping   localMap;

 ETbarn = lbe->dataHook.lift( "EdwardsTengSage" );
 ETptr = (Info*) ETbarn->info;
 // REMOVE: localMap = ETptr->map( (*EdwardsTengSage::_theMapPtr)( ETptr->mapInv ) );
 localMap = ETptr->map( (*EdwardsTengSage::_theMapPtr)( ETptr->map.Inverse() ) );
 mtrx = localMap.Jacobian();
 ETptr->EV = mtrx.Matrix::eigenVectors();
  // In the above, it is necessary to qualify the member 
  // function eigenValues(). Without explicit class qualifier, 
  // the complex form of the algorithm is called. 
  // This should not be so. Compiler bug ???

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
  D(0,0) = 1.0;  D(0,1) = 0.0;
  D(1,0) = 0.0;  D(1,1) = 1.0;
 }

 else {                                // Coupled lattice

  if( fabs( EdwardsTengSage::_csH - EdwardsTengSage::_csV ) < 1.0e-4 ) {
    (*pcerr) << "\n"
         << "*** ERROR *** EdwardsTengSage()                        \n"
         << "*** ERROR *** \"Horizontal\" and \"vertical\" tunes\n"
         << "*** ERROR *** are too near each other for          \n"
         << "*** ERROR *** reasonable results.                  \n"
         << "*** ERROR ***                                      \n"
         << "*** ERROR *** The calculation is meaningless.      \n"
         << "*** ERROR ***                                        " << endl;
    return 2;
  }

  dcos    = EdwardsTengSage::_csH - EdwardsTengSage::_csV;
  cos2phi = ( M - N ).trace() / ( 2.0 *( dcos ) );
  if( fabs( cos2phi - 1.0 ) < 1.0e-4 ) cos2phi =   1.0;  // ??? Rather coarse,
  if( fabs( cos2phi + 1.0 ) < 1.0e-4 ) cos2phi = - 1.0;  // ??? isn't it?

  if( fabs( cos2phi ) > 1.0 ) {
   (*pcerr) << "\n"
        << "*** ERROR: EdwardsTengSage()                        \n"
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
   sin2phi = EdwardsTengSage::_csH;  // Variable used as dummy register.
   EdwardsTengSage::_csH = EdwardsTengSage::_csV;
   EdwardsTengSage::_csV = sin2phi;
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
    (*pcerr) << "\n"
         << "*** ERROR *** EdwardsTengSage()                        \n"
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
 JH = A - EdwardsTengSage::_csH*U;
 if( JH( 0, 1 ) > 0.0 )
  EdwardsTengSage::_snH =   sqrt( 1.0 - EdwardsTengSage::_csH*EdwardsTengSage::_csH );
 else {
  EdwardsTengSage::_snH = - sqrt( 1.0 - EdwardsTengSage::_csH*EdwardsTengSage::_csH );
 }

 ETptr->beta .hor = JH( 0, 1 ) / EdwardsTengSage::_snH;
 ETptr->alpha.hor = JH( 0, 0 ) / EdwardsTengSage::_snH;

 // .......... A little test to keep everyone honest .....
 if( JH( 0, 0 ) != 0.0 )
  if( fabs( ( JH(0,0) + JH(1,1) ) / ( JH(0,0) - JH(1,1) ) ) > 1.0e-4 ) {
   (*pcerr) << endl;
   (*pcerr) << "*** WARNING ***                                " << endl;
   (*pcerr) << "*** WARNING *** EdwardsTengSage()                  " << endl;
   (*pcerr) << "*** WARNING *** \"Horizontal\" matrix does not " << endl;
   (*pcerr) << "*** WARNING *** pass symplecticity test.       " << endl;
   (*pcerr) << "*** WARNING *** JH( 0, 0 ) = " << JH( 0, 0 )     << endl;
   (*pcerr) << "*** WARNING *** JH( 1, 1 ) = " << JH( 1, 1 )     << endl;
   (*pcerr) << "*** WARNING ***                                " << endl;
   (*pcerr) << "*** WARNING *** The ratio is " 
        << fabs( ( JH(0,0) + JH(1,1) ) / ( JH(0,0) - JH(1,1) ) )
        << endl;
   (*pcerr) << "*** WARNING ***                                " << endl;
   // return 5;
  }


 // ......  Then  the "vertical" ......
 MatrixD JV;
 JV = B - EdwardsTengSage::_csV*U;
 if( JV( 0, 1 ) > 0.0 )
  EdwardsTengSage::_snV =   sqrt( 1.0 - EdwardsTengSage::_csV*EdwardsTengSage::_csV );
 else {
  EdwardsTengSage::_snV = - sqrt( 1.0 - EdwardsTengSage::_csV*EdwardsTengSage::_csV );
 }

 ETptr->beta .ver = JV( 0, 1 ) / EdwardsTengSage::_snV;
 ETptr->alpha.ver = JV( 0, 0 ) / EdwardsTengSage::_snV;

 // .......... A little test to keep everyone honest .....
 if( JV( 0, 0 ) != 0.0 )
  if( fabs( ( JV(0,0) + JV(1,1) ) / ( JV(0,0) - JV(1,1) ) ) > 1.0e-4 ) {
   (*pcerr) << endl;
   (*pcerr) << "*** WARNING ***                                " << endl;
   (*pcerr) << "*** WARNING *** EdwardsTengSage()                  " << endl;
   (*pcerr) << "*** WARNING *** \"Vertical\" matrix does not   " << endl;
   (*pcerr) << "*** WARNING *** pass symplecticity test.       " << endl;
   (*pcerr) << "*** WARNING *** JV( 0, 0 ) = " << JV( 0, 0 )     << endl;
   (*pcerr) << "*** WARNING *** JV( 1, 1 ) = " << JV( 1, 1 )     << endl;
   (*pcerr) << "*** WARNING ***                                " << endl;
   (*pcerr) << "*** WARNING *** The ratio is " 
        << fabs( ( JV(0,0) + JV(1,1) ) / ( JV(0,0) - JV(1,1) ) )
        << endl;
   (*pcerr) << "*** WARNING ***                                " << endl;
   // return 6;
 }


 lbe->dataHook.append( ETbarn );

 return 0;
}


// ============================================================== //

void EdwardsTengSage::eraseAll() 
{
  _myBeamlinePtr->dataHook.eraseAll( "EdwardsTengSage" );
  _myBeamlinePtr->dataHook.eraseAll( "eigentunes" );

  DeepBeamlineIterator dbi( *_myBeamlinePtr );
  bmlnElmnt* be;
  while((  be = dbi++  )) {
    be->dataHook.eraseAll( "EdwardsTengSage" );
    be->dataHook.eraseAll( "eigentunes" );
  }

  _deleteCalcs();
}


int EdwardsTengSage::doCalc( JetParticle* ptr_jp, beamline::Criterion& crit )
{
  // PRECONDITION:   The JetParticle must be on the closed
  //   orbit with the identity mapping for its state.
  //                 Its Jet environment's reference point 
  //   should be the closed orbit. It is not reset.
  // POSTCONDITIONS: The JetParticle has the one-turn
  //   mapping for its state.
  //                 Every element in the beamline that
  //   matches the criterion has attached a Barnacle
  //   labelled EdwardsTengSage that possesses and
  //   EdwardsTengSage::Info data struct.
  //                 The beamline has attached to it
  //   a Barnacle labelled eigentunes that possesses
  //   an EdwardsTengSage::Tunes data struct. 
  int             ret;
  bmlnElmnt*      be;
  double          t;
  int             i;
  MatrixD         mtrx( BMLN_dynDim, BMLN_dynDim );
  Info*           ETptr;

  EdwardsTengSage::_theMapPtr = new Mapping;

  // .......... Propagate a JetParticle element by element
  // .......... It is assumed to be on a closed orbit!!
  Particle* ptr_particle = new Particle(*ptr_jp);

  // Note: ptr_particle is deleted before returning

  double lng = 0.0;
  DeepBeamlineIterator dbi( *_myBeamlinePtr );
   
  // REMOVE: Mapping* tmp_map = 0;  

  while ((  be = dbi++  )) {
    lng += be->OrbitLength( *ptr_particle );
    be->propagate( *ptr_jp );
    if( crit(be) ) {
      ETptr = new Info;
      ETptr->arcLength = lng;
      ETptr->map= ptr_jp->getState();   // ??? Change statements?  Use pointer?
      // REMOVE: ETptr->mapInv = ETptr->map.Inverse();
      be->dataHook.append( "EdwardsTengSage", ETptr );
    }
  }

  // .......... Calculating tunes .........................
  *EdwardsTengSage::_theMapPtr = ptr_jp->getState();
  mtrx = EdwardsTengSage::_theMapPtr->Jacobian();
  MatrixC lambda;
  lambda = mtrx.eigenValues();

  for( i = 0; i < BMLN_dynDim; i++ )
   if( fabs( abs(lambda(i)) - 1.0 ) > 1.0e-4 ) {
    (*pcerr) << "\n"
         << "*** ERROR ***                                     \n"
         << "*** ERROR ***                                     \n"
         << "*** ERROR *** EdwardsTengSage()                       \n"
         << "*** ERROR *** The lattice is linearly unstable.   \n"
         << "*** ERROR *** lambda( " << i << " ) has magnitude = " 
         << abs(lambda(i))
         << "\n"
         << "*** ERROR ***                                     \n"
         << endl;
    delete EdwardsTengSage::_theMapPtr;
    delete ptr_particle;
    eraseAll();
    return 10;
   }

  if( ( abs( lambda(0) - conj( lambda(3) ) ) > 1.0e-4 )  ||
      ( abs( lambda(1) - conj( lambda(4) ) ) > 1.0e-4 )
    ) {
    (*pcerr) << "\n"
         << "*** ERROR *** EdwardsTengSage()                        \n"
         << "*** ERROR *** Conjugacy condition has been violated\n"
         << "*** ERROR *** The lattice may be linearly unstable.\n"
         << "*** ERROR *** Eigenvalues =                        \n"
         << "*** ERROR *** " << lambda << endl;
    delete EdwardsTengSage::_theMapPtr;
    delete ptr_particle;
    eraseAll();
    return 11;
  }

  EdwardsTengSage::_csH = real( lambda(0) );    
  EdwardsTengSage::_csV = real( lambda(1) );
  // EdwardsTengSage::_snH = imag( lambda(0) );    
  // EdwardsTengSage::_snV = imag( lambda(1) );

  // Go through the accelerator again and attach
  //   the Edwards Teng lattice info to each element
  //   satisfying the selection criterion.
  dbi.reset();
  while ((  be = dbi++  )) {
    if( crit(be) ) {
      if( 0 != ( ret = _attachETFuncs(be) ) ) {
        delete EdwardsTengSage::_theMapPtr;
        delete ptr_particle;
        eraseAll();
        return ret;
      }
    }
  }

  EdwardsTengSage::Tunes* tuneptr = new EdwardsTengSage::Tunes;
  t = atan2( EdwardsTengSage::_snH, EdwardsTengSage::_csH );
  if( t < 0.0 )   t += M_TWOPI;
  tuneptr->hor = ( t / M_TWOPI );
  t = atan2( EdwardsTengSage::_snV, EdwardsTengSage::_csV );
  if( t < 0.0 )   t += M_TWOPI;
  tuneptr->ver = ( t / M_TWOPI );

  _myBeamlinePtr->dataHook.append( "eigentunes", tuneptr );

  delete EdwardsTengSage::_theMapPtr;
  delete ptr_particle;
  return 0;
}


const EdwardsTengSage::Info* EdwardsTengSage::get_ETFuncPtr( int j )
{
  if( j < 0 || _arrayPtr->size() <= j ) { return 0; }

  if( _calcs == 0 ) {
    int i;
    _n = _arrayPtr->size();
    _calcs = new Info* [_n];
    for( i = 0; i < _n; i++ ) { _calcs[i] = 0;  }
    
    EdwardsTengSage::Info* infoPtr;
    Barnacle* barnaclePtr;
    i = 0;
    DeepBeamlineIterator dbi( *_myBeamlinePtr );
    bmlnElmnt* be;
    while( ( 0 != (be = dbi++)  ) && ( i < _n ) ) {
      if( 0 != ( barnaclePtr = be->dataHook.lift( "EdwardsTengSage" ) ) ) {
        infoPtr = dynamic_cast<EdwardsTengSage::Info*>(barnaclePtr->info);
        _calcs[i] = infoPtr;
        i++;
      }
    }
  }

  return _calcs[j];
}
