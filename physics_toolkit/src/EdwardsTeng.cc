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
****** REVISION HISTORY
******
****** Mar 2006   ostiguy@fnal.gov
******
******  - reference counted elements/beamlines 
******  - eliminated references to slist/dlist
******  - use new-style STL compatible beamline iterators
******  - new type safe Barnacles
******  - initialization optimizations
********************************************************************************
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
#include <beamline/JetParticle.h>
#include <beamline/beamline.h>
#include <physics_toolkit/EdwardsTeng.h>

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


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
double          EdwardsTeng::csH_, 
                EdwardsTeng::csV_, 
                EdwardsTeng::snH_, 
                EdwardsTeng::snV_;
Mapping         EdwardsTeng::theMap_;


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


int EdwardsTeng::attachETLattFuncs( ElmPtr lbe ) 
{
 double  dcos, cos2phi, sin2phi, tanphi;
 static MatrixD M( 2, 2 ), N( 2, 2 ), m( 2, 2 ), n( 2, 2 ),
                D( 2, 2 ), S( "J", 2 ), A( 2, 2 ), B( 2, 2 ),
                U( "I", 2 );
  
 BarnacleList::iterator it = lbe->dataHook.find( "EdwardsTeng" );

 ETinfo etinfo = boost::any_cast<ETinfo>(it->info);
 lbe->dataHook.erase(it);

 Mapping localMap = etinfo.map(  theMap_( etinfo.mapInv ) );
 MatrixD     mtrx = localMap.Jacobian();
 etinfo.EV = mtrx.eigenVectors(); 

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

  if( fabs( csH_ - csV_ ) < 1.0e-4 ) {
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

  dcos    =  csH_ - csV_;
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
   sin2phi =   csH_;  // Variable used as dummy register.
   csH_    =   csV_;
   csV_    =   sin2phi;
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
 etinfo.phi       = atan( tanphi ) / 2.0;
 etinfo.D         = D;

 // .......... Lattice functions ..........................

 // ......  First the "horizontal" ......
 MatrixD JH;
 JH = A - csH_ * U;
 if( JH( 0, 1 ) > 0.0 )
  snH_ =   sqrt( 1.0 - csH_ * csH_ );
 else {
  snH_ = - sqrt( 1.0 - csH_ * csH_ );
 }

 etinfo.beta .hor = JH( 0, 1 ) / snH_;
 etinfo.alpha.hor = JH( 0, 0 ) / snH_;

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
 JV = B -  csV_ * U;
 if( JV( 0, 1 ) > 0.0 )
  snV_ =   sqrt( 1.0 - csV_*csV_ );
 else {
  snV_ = - sqrt( 1.0 - csV_*csV_ );
 }

 etinfo.beta .ver = JV( 0, 1 ) / snV_;
 etinfo.alpha.ver = JV( 0, 0 ) / snV_;

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

 lbe->dataHook.append( Barnacle("EdwardsTeng", etinfo) );

 return 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


EdwardsTeng::EdwardsTeng( beamline const* x ) 
{
  myBeamline_ = const_cast<beamline*>(x);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


EdwardsTeng::~EdwardsTeng()
{
 // Will remove barnacles from its beamline.
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void EdwardsTeng::eraseAll() 
{
  for (beamline::deep_iterator it  = myBeamline_->deep_begin();
       it != myBeamline_->deep_end(); ++it ) {
    (*it)->dataHook.eraseAll( "EdwardsTeng" ); 
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int EdwardsTeng::doCalc( JetParticle& jp, ET_CRITFUNC Crit ) 
{
 int                  ret;
 double               t;

 // .......... Propagate a JetParticle element by element
 // .......... It is assumed to be on a closed orbit!!
 Particle particle(jp);

 // is deleted before returning


 double lng = 0.0;

 for (beamline::deep_iterator it  = myBeamline_->deep_begin();  
                              it != myBeamline_->deep_end(); ++it ) {

   lng += (*it)->OrbitLength( particle );
   (*it)->propagate( jp );
   if( !Crit ) {
    ETinfo etinfo;
    etinfo.arcLength = lng;
    etinfo.map = jp.getState();   
    etinfo.mapInv =etinfo.map.Inverse();
     (*it)->dataHook.append( Barnacle( "EdwardsTeng", etinfo) );
   }
   else if( (*Crit)( (*it).get() ) ) {
    ETinfo etinfo;
    etinfo.arcLength = lng;
    etinfo.map = jp.getState();   
    etinfo.mapInv = etinfo.map.Inverse();
     (*it)->dataHook.append( Barnacle( "EdwardsTeng", etinfo) );
   }
 }



 // .......... Calculating tunes .........................

 theMap_        = jp.getState();
 MatrixD mtrx   = EdwardsTeng::theMap_.Jacobian();
 MatrixC lambda = mtrx.eigenValues();

 for( int i=0; i < BMLN_dynDim;  ++i)
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
   eraseAll();
   return 11;
 }

 csH_ = real( lambda(0) );    
 csV_ = real( lambda(1) );
 // EdwardsTeng::snH_ = imag( lambda(0) );    
 // EdwardsTeng::snV_ = imag( lambda(1) );

 // Go through the accelerator again and attach
 // the Edwards Teng lattice info to each element.

 for (beamline::deep_iterator it  = myBeamline_->deep_begin();  
                              it != myBeamline_->deep_end(); ++it ) {
  if( !Crit ) {
   if( ( ret = attachETLattFuncs( (*it) ) ) != 0 ) {
    eraseAll();
    return ret;
   }
  }
  else if( (*Crit)( (*it).get() ) ) {
   if( ( ret = attachETLattFuncs( (*it) ) ) != 0 ) {
    eraseAll();
    return ret;
   }
  }
 }


 ETtunes tunes;
 t = atan2( snH_, csH_ );
 if( t < 0.0 )   t += M_TWOPI;
 tunes.hor = ( t / M_TWOPI );
 t = atan2( snV_, csV_ );
 if( t < 0.0 )   t += M_TWOPI;
 tunes.ver = ( t / M_TWOPI );

 myBeamline_->dataHook.append( Barnacle( "Tunes", tunes ) );

 return 0;
}

