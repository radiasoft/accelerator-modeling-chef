/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      EdwardsTeng.cc
******  Version:   1.1
******                                                                
******  Copyright (c) 2001  Universities Research Association, Inc.   
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
#if HAVE_CONFIG_H
#include <config.h>
#endif

/*
 *  This "lattice function" method is based on
 *  D.A.Edwards and L.C.Teng
 *  "Parametrization of Linear Coupled Motion in Periodic Systems"
 *  IEEE Tr.NucSci.
 *  NS-20(3), 885(1973).
 *       -- Leo Michelotti
 *          June 6, 1994   ( D-Day + 50 years )
 */

#include <iomanip>
#include <basic_toolkit/iosetup.h>

#include <beamline/Particle.h>
#include <beamline/BeamlineIterator.h>
#include <physics_toolkit/EdwardsTeng.h>

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;


ETinfo::ETinfo( const ETinfo& x ) 
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


// ... Globals:
double          EdwardsTeng::csH, 
                EdwardsTeng::csV, 
                EdwardsTeng::snH, 
                EdwardsTeng::snV;
Mapping*        EdwardsTeng::theMap;



int EdwardsTeng::attachETLattFuncs( bmlnElmnt* lbe ) 
{
 double  dcos, cos2phi, sin2phi, tanphi;
 MatrixD mtrx;
 static MatrixD M( 2, 2 ), N( 2, 2 ), m( 2, 2 ), n( 2, 2 ),
                D( 2, 2 ), S( "J", 2 ), A( 2, 2 ), B( 2, 2 ),
                U( "I", 2 );
 ETinfo*   ETptr;
 Barnacle* ETbarn;
 Mapping   localMap;

 ETbarn = lbe->dataHook.lift( "EdwardsTeng" );
 ETptr = (ETinfo*) ETbarn->info;
 localMap = ETptr->map( (*EdwardsTeng::theMap)( ETptr->mapInv ) );
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

  if( fabs( EdwardsTeng::csH - EdwardsTeng::csV ) < 1.0e-4 ) {
    (*pcerr) << "\n"
         << "*** ERROR *** EdwardsTeng()                        \n"
         << "*** ERROR *** \"Horizontal\" and \"vertical\" tunes\n"
         << "*** ERROR *** are too near each other for          \n"
         << "*** ERROR *** reasonable results.                  \n"
         << "*** ERROR ***                                      \n"
         << "*** ERROR *** The calculation is meaningless.      \n"
         << "*** ERROR ***                                        " << endl;
    return 2;
  }

  dcos    = EdwardsTeng::csH - EdwardsTeng::csV;
  cos2phi = ( M - N ).trace() / ( 2.0 *( dcos ) );
  if( fabs( cos2phi - 1.0 ) < 1.0e-4 ) cos2phi =   1.0;  // ??? Rather coarse,
  if( fabs( cos2phi + 1.0 ) < 1.0e-4 ) cos2phi = - 1.0;  // ??? isn't it?

  if( fabs( cos2phi ) > 1.0 ) {
   (*pcerr) << "\n"
        << "*** ERROR: EdwardsTeng()                        \n"
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
   sin2phi = EdwardsTeng::csH;  // Variable used as dummy register.
   EdwardsTeng::csH = EdwardsTeng::csV;
   EdwardsTeng::csV = sin2phi;
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
         << "*** ERROR *** EdwardsTeng()                        \n"
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
 JH = A - EdwardsTeng::csH*U;
 if( JH( 0, 1 ) > 0.0 )
  EdwardsTeng::snH =   sqrt( 1.0 - EdwardsTeng::csH*EdwardsTeng::csH );
 else {
  EdwardsTeng::snH = - sqrt( 1.0 - EdwardsTeng::csH*EdwardsTeng::csH );
 }

 ETptr->beta .hor = JH( 0, 1 ) / EdwardsTeng::snH;
 ETptr->alpha.hor = JH( 0, 0 ) / EdwardsTeng::snH;

 // .......... A little test to keep everyone honest .....
 if( JH( 0, 0 ) != 0.0 )
  if( fabs( ( JH(0,0) + JH(1,1) ) / ( JH(0,0) - JH(1,1) ) ) > 1.0e-4 ) {
   (*pcerr) << endl;
   (*pcerr) << "*** WARNING ***                                " << endl;
   (*pcerr) << "*** WARNING *** EdwardsTeng()                  " << endl;
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
 JV = B - EdwardsTeng::csV*U;
 if( JV( 0, 1 ) > 0.0 )
  EdwardsTeng::snV =   sqrt( 1.0 - EdwardsTeng::csV*EdwardsTeng::csV );
 else {
  EdwardsTeng::snV = - sqrt( 1.0 - EdwardsTeng::csV*EdwardsTeng::csV );
 }

 ETptr->beta .ver = JV( 0, 1 ) / EdwardsTeng::snV;
 ETptr->alpha.ver = JV( 0, 0 ) / EdwardsTeng::snV;

 // .......... A little test to keep everyone honest .....
 if( JV( 0, 0 ) != 0.0 )
  if( fabs( ( JV(0,0) + JV(1,1) ) / ( JV(0,0) - JV(1,1) ) ) > 1.0e-4 ) {
   (*pcerr) << endl;
   (*pcerr) << "*** WARNING ***                                " << endl;
   (*pcerr) << "*** WARNING *** EdwardsTeng()                  " << endl;
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


EdwardsTeng::EdwardsTeng( const beamline* x ) 
{
 myBeamline = (beamline*) x;
}

EdwardsTeng::~EdwardsTeng()
{
 // Will remove barnacles from its beamline.
}


/* ============================================================== */

void EdwardsTeng::eraseAll() 
{
 DeepBeamlineIterator dbi( myBeamline );
 bmlnElmnt* be;
 while((  be = dbi++  )) 
 {  be->dataHook.eraseAll( "EdwardsTeng" ); }
}

int EdwardsTeng::doCalc( JetParticle* ptr_jp, ET_CRITFUNC Crit ) 
{
 int                  ret;
 bmlnElmnt*           be;
 DeepBeamlineIterator dbi( myBeamline );
 double               t;
 int                  i;
 MatrixD              mtrx( BMLN_dynDim, BMLN_dynDim );
 ETinfo*              ETptr;
 ETtunes*             tuneptr;

 EdwardsTeng::theMap = new Mapping;

 // .......... Propagate a JetParticle element by element
 // .......... It is assumed to be on a closed orbit!!
 Particle* ptr_particle = ptr_jp->ConvertToParticle();
 // is deleted before returning


 double lng = 0.0;
 while ((  be = dbi++  )) {
   lng += be->OrbitLength( *ptr_particle );
   be->propagate( *ptr_jp );
   if( !Crit ) {
     ETptr = new ETinfo;
     ETptr->arcLength = lng;
     ptr_jp->getState( ETptr->map );   // ??? Change statements?  Use pointer?
     ETptr->mapInv = ETptr->map.Inverse();
     be->dataHook.append( "EdwardsTeng", ETptr );
   }
   else if( (*Crit)( be ) ) {
     ETptr = new ETinfo;
     ETptr->arcLength = lng;
     ptr_jp->getState( ETptr->map );   // ??? Change statements?  Use pointer?
     ETptr->mapInv = ETptr->map.Inverse();
     be->dataHook.append( "EdwardsTeng", ETptr );
   }
 }
 dbi.reset();


 // .......... Calculating tunes .........................
 ptr_jp->getState( *EdwardsTeng::theMap );
 mtrx = EdwardsTeng::theMap->Jacobian();
 MatrixC lambda;
 lambda = mtrx.eigenValues();

 for( i = 0; i < BMLN_dynDim; i++ )
  if( fabs( abs(lambda(i)) - 1.0 ) > 1.0e-4 ) {
   (*pcerr) << "\n"
        << "*** ERROR ***                                     \n"
        << "*** ERROR ***                                     \n"
        << "*** ERROR *** EdwardsTeng()                       \n"
        << "*** ERROR *** The lattice is linearly unstable.   \n"
        << "*** ERROR *** lambda( " << i << " ) has magnitude = " 
        << abs(lambda(i))
        << "\n"
        << "*** ERROR ***                                     \n"
        << endl;
   delete EdwardsTeng::theMap;
   delete ptr_particle;
   eraseAll();
   return 10;
  }

 if( ( abs( lambda(0) - conj( lambda(3) ) ) > 1.0e-4 )  ||
     ( abs( lambda(1) - conj( lambda(4) ) ) > 1.0e-4 )
   ) {
   (*pcerr) << "\n"
        << "*** ERROR *** EdwardsTeng()                        \n"
        << "*** ERROR *** Conjugacy condition has been violated\n"
        << "*** ERROR *** The lattice may be linearly unstable.\n"
        << "*** ERROR *** Eigenvalues =                        \n"
        << "*** ERROR *** " << lambda << endl;
   delete EdwardsTeng::theMap;
   delete ptr_particle;
   eraseAll();
   return 11;
 }

 EdwardsTeng::csH = real( lambda(0) );    
 EdwardsTeng::csV = real( lambda(1) );
 // EdwardsTeng::snH = imag( lambda(0) );    
 // EdwardsTeng::snV = imag( lambda(1) );

 // Go through the accelerator again and attach
 // the Edwards Teng lattice info to each element.

 while ((  be = dbi++  )) {
  if( !Crit ) {
   if( ( ret = attachETLattFuncs( be ) ) != 0 ) {
    delete EdwardsTeng::theMap;
    delete ptr_particle;
    eraseAll();
    return ret;
   }
  }
  else if( (*Crit)( be ) ) {
   if( ( ret = attachETLattFuncs( be ) ) != 0 ) {
    delete EdwardsTeng::theMap;
    delete ptr_particle;
    eraseAll();
    return ret;
   }
  }
 }
 dbi.reset();


 tuneptr = new ETtunes;
 t = atan2( EdwardsTeng::snH, EdwardsTeng::csH );
 if( t < 0.0 )   t += M_TWOPI;
 tuneptr->hor = ( t / M_TWOPI );
 t = atan2( EdwardsTeng::snV, EdwardsTeng::csV );
 if( t < 0.0 )   t += M_TWOPI;
 tuneptr->ver = ( t / M_TWOPI );

 myBeamline->dataHook.append( "Tunes", tuneptr );

 delete EdwardsTeng::theMap;
 delete ptr_particle;

 return 0;
}

