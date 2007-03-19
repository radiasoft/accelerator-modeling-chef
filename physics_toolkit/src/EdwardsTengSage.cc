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
******  REVISION HISTORY:
******
******  Dec 2006 - Jean-Francois Ostiguy 
******             ostiguy@fnal
******    
******  - interface based on Particle& rather than ptrs. 
******    Stack allocated local Particle objects.
******  - changes to accomodate new boost::any based Barnacle objects.
******  - use new style STL-compatible beamline iterators
******  - calcs_ array is now an STL vector. LF are now returned by 
******    returning a const reference to the entire vector.
******  - misc cleanup.  
******
******  Mar 2007   ostiguy@fnal.gov
******  - eliminated references to slist/dlist
******  - efficiency: use linear maps (as opposed to nonlinear until now)
******    to perform linear lattice functions computations
******  - object initialization optimizations
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

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

EdwardsTengSage::Info::Info() 
: D(6,6), EV(2,1) 
{
  arcLength = -1.0;
  beta.hor  = 0.0;
  beta.ver  = 0.0;
  alpha.hor = 0.0;
  alpha.ver = 0.0;
  phi       = 0.0;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


EdwardsTengSage::EdwardsTengSage( BmlPtr x ) 
  : Sage( x )
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

EdwardsTengSage::EdwardsTengSage( beamline const& x ) 
  : Sage( x )
{}



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


int EdwardsTengSage::eigenTuneCalc( JetParticle const& ptr_jp, 
                                    EdwardsTengSage::Tunes& answer )
{
  const int x  = Particle::xIndex();
  const int y  = Particle::yIndex();
  const int xp = Particle::npxIndex();
  const int yp = Particle::npyIndex();

  MatrixD mtrx   = ptr_jp.State().Jacobian();  
  MatrixC lambda = mtrx.Matrix::eigenValues();

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


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


int EdwardsTengSage::attachETFuncs( ElmPtr lbe, MatrixD const& one_turn_map ) 
{

 BarnacleList::iterator itb = lbe->dataHook.find( "EdwardsTengSage" );

 Info etinfo;

 if ( itb != lbe->dataHook.end() ) { 
  etinfo = boost::any_cast<Info>(itb->info);
  
  lbe->dataHook.erase(itb);
 }

 // ---------------------------------------------------------------------------------
 // (1) the struct etinfo holds a cumulative map (from the beamline first element) 
 //     for the current element. 
 // (2) one_turn_map contains a one-turn map from the origin
 //
 //  To compute lattice functions, we need the one turn map *at the element location*. 
 //   
 //  we map back from the current location to the origin, then go around the ring once
 //  and finally return to the current location. This is what the map composition below
 //  accomplishes. 
 //  
 //  Note: In previous versions of this function, "map" was a full-fledged Mapping, albeit a 
 //        only a first-order one  (i.e. a First order Jet Vector ).
 //        We now perform the same task with regular matrices because this is 
 //        significantly faster (Map inversion and composition are slower). 
 //
 //  - JFO        
 //--------------------------------------------------------------------------------------
 
 Matrix  mtrx = etinfo.map * one_turn_map * etinfo.map.inverse();

 // OLD VERSION:: Matrix  mtrx = etinfo.map(  one_turn_map( etinfo.map.Inverse() ) ).Jacobian();

 etinfo.EV = mtrx.Matrix::eigenVectors();

  // In the above, it is necessary to qualify the member 
  // function eigenValues(). Without explicit class qualifier, 
  // the complex form of the algorithm is called. 
  // This should not be so. Compiler bug ???

 MatrixD M( 2, 2 ), N( 2, 2 ),   m( 2, 2 ), n( 2, 2 ),
                    D( 2, 2 ), A( 2, 2 ),   B( 2, 2 );

 static MatrixD const S( "J", 2 ), U( "I", 2 );

 M( 0, 0 ) = mtrx( 0, 0 );     n( 0, 0 ) = mtrx( 0, 1 );
 M( 0, 1 ) = mtrx( 0, 3 );     n( 0, 1 ) = mtrx( 0, 4 );
 M( 1, 0 ) = mtrx( 3, 0 );     n( 1, 0 ) = mtrx( 3, 1 );
 M( 1, 1 ) = mtrx( 3, 3 );     n( 1, 1 ) = mtrx( 3, 4 );

 m( 0, 0 ) = mtrx( 1, 0 );     N( 0, 0 ) = mtrx( 1, 1 );
 m( 0, 1 ) = mtrx( 1, 3 );     N( 0, 1 ) = mtrx( 1, 4 );
 m( 1, 0 ) = mtrx( 4, 0 );     N( 1, 0 ) = mtrx( 4, 1 );
 m( 1, 1 ) = mtrx( 4, 3 );     N( 1, 1 ) = mtrx( 4, 4 );


 double  dcos    = 0.0;
 double  cos2phi = 0.0;
 double  sin2phi = 0.0;
 double  tanphi  = 0.0;


 if( ( m == 0.0 ) && ( n == 0.0 ) ) {  // Uncoupled lattice

  A = M;
  B = N;

  sin2phi = tanphi = 0.0;
  cos2phi = 1.0;
  D(0,0)  = 1.0;  D(0,1) = 0.0;
  D(1,0)  = 0.0;  D(1,1) = 1.0;
 }

 else {                                // Coupled lattice

  if(  std::abs( csH_ - csV_ ) < 1.0e-4 ) {
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
 
  dcos    = (csH_ - csV_);
  cos2phi = ( M - N ).trace() / ( 2.0 *( dcos ) );

  if( std::abs( cos2phi - 1.0 ) < 1.0e-4 ) cos2phi =   1.0;  // ??? Rather coarse,
  if( std::abs( cos2phi + 1.0 ) < 1.0e-4 ) cos2phi = - 1.0;  // ??? isn't it?

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
   sin2phi =   csH_;         // Variable used as dummy register.
   csH_    =   csV_;
   csV_    =   sin2phi;
   dcos    = - dcos;
   cos2phi = - cos2phi;
 }
  sin2phi = sqrt( 1.0 - cos2phi*cos2phi );
  tanphi  = sin2phi / ( 1.0 + cos2phi );

  if( fabs( sin2phi ) > 1.0e-8 ) {
    D = -( m + S*n.transpose()*S.transpose() ) * ( 1.0 / ( dcos*sin2phi ));
  } else {
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
 } // if (coupled lattice case)

 // .......... Save the ET information
 etinfo.phi       = atan( tanphi ) / 2.0;
 etinfo.D         = D;

 // .......... Lattice functions ..........................

 // ......  First the "horizontal" ......

 MatrixD JH = A - EdwardsTengSage::csH_*U;

 if( JH( 0, 1 ) > 0.0 ) { 
  EdwardsTengSage::snH_ =   sqrt( 1.0 - EdwardsTengSage::csH_*EdwardsTengSage::csH_ );
 } else {
  EdwardsTengSage::snH_ = - sqrt( 1.0 - EdwardsTengSage::csH_*EdwardsTengSage::csH_ );
 }

 etinfo.beta .hor = JH( 0, 1 ) / EdwardsTengSage::snH_;
 etinfo.alpha.hor = JH( 0, 0 ) / EdwardsTengSage::snH_;

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
 MatrixD JV = B - csV_*U;

 if( JV( 0, 1 ) > 0.0 )
   snV_ =   sqrt( 1.0 - csV_*csV_ );
 else {
   snV_ = - sqrt( 1.0 - csV_*csV_ );
 }

 etinfo.beta .ver = JV( 0, 1 ) / snV_;
 etinfo.alpha.ver = JV( 0, 0 ) / snV_;

 // .......... A little test to keep everyone honest .....
 if( JV( 0, 0 ) != 0.0 ) 
  {
    if( std::abs( ( JV(0,0) + JV(1,1) ) / ( JV(0,0) - JV(1,1) ) ) > 1.0e-4 ) 
    {
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
 }
 
 lbe->dataHook.append( Barnacle("EdwardsTengSage", etinfo) );
  
 return 0;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void EdwardsTengSage::eraseAll() 
{
  myBeamlinePtr_->dataHook.eraseAll( "EdwardsTengSage" );
  myBeamlinePtr_->dataHook.eraseAll( "eigentunes" );

  for( beamline::deep_iterator it = myBeamlinePtr_->deep_begin(); it != myBeamlinePtr_->deep_end(); ++it) {
    (*it)->dataHook.eraseAll( "EdwardsTengSage" );
    (*it)->dataHook.eraseAll( "eigentunes" );
  }

  calcs_.clear();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int EdwardsTengSage::doCalc( JetParticle& jp, beamline::Criterion& crit )
{
  // PRECONDITION:   The JetParticle must be on the closed
  //                 orbit with the identity mapping for its state.
  //                 Its Jet environment's reference point 
  //                 should be the closed orbit. It is not reset.
  //
  // POSTCONDITIONS: The JetParticle has the one-turn mapping for its state.
  //                 Every element in the beamline that
  //                 matches the criterion has attached a Barnacle
  //                 labelled "EdwardsTengSage" that possesses an
  //                 EdwardsTengSage::Info struct.
  //                 The beamline has attached to it
  //                 a Barnacle labelled "eigentunes" that possesses
  //                 an EdwardsTengSage::Tunes data struct. 

  int             ret = 0;
  double          t;


  // .......... Propagate a JetParticle element by element
  // .......... It is assumed to be on a closed orbit!!

  Particle particle(jp);

  double lng = 0.0;

  for( beamline::deep_iterator it  = myBeamlinePtr_->deep_begin(); 
                               it != myBeamlinePtr_->deep_end(); ++it) {

    lng += (*it)->OrbitLength( particle );

    (*it)->propagate( jp );

    if( crit(**it) ) {
      Info etinfo;
      etinfo.arcLength = lng;

      // PREVIOUS VERSION: etinfo.map= jp.getState();
      etinfo.map= jp.getState().Jacobian(); 

      (*it)->dataHook.append( Barnacle("EdwardsTengSage", etinfo) );
    }
  }

  // .......... Calculating tunes .........................

  Mapping one_turn_map( jp.getState() );

  MatrixD mtrx   = one_turn_map.Jacobian();
  MatrixC lambda = mtrx.eigenValues();

  for( int i = 0; i < BMLN_dynDim; ++i)
   if( abs( abs(lambda(i)) - 1.0 ) > 1.0e-4 ) {
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
    return 11;
  }

  csH_ = real( lambda(0) );    
  csV_ = real( lambda(1) );

  // Go through the accelerator again and attach
  //   the Edwards Teng lattice info to each element
  //   satisfying the selection criterion.


  for( beamline::deep_iterator it = myBeamlinePtr_->deep_begin(); it != myBeamlinePtr_->deep_end(); ++it) {

    if(  crit( **it ) ) 
    {
      // PREVIOUS VERSION:: int err = attachETFuncs( (*it), one_turn_map );  
      int err = attachETFuncs( (*it), mtrx );  

      if ( err != 0) 
      {  
        eraseAll();
        return -1; // failure
      }
    }
  }

  Tunes tunes;
  t = atan2( snH_, csH_ );

  if( t < 0.0 )   t += M_TWOPI;
  tunes.hor = ( t / M_TWOPI );

  t = atan2( snV_, csV_ );

  if( t < 0.0 )   t += M_TWOPI;
  tunes.ver = ( t / M_TWOPI );

  myBeamlinePtr_->dataHook.append( Barnacle("eigentunes", tunes) );

  
  // ------------------------------------------------------------------------------------
  // If we got here, the calculation was successful. Populate the calc_ array. 
  // 
  // 
  // IMPORTANT NOTE: with shared bmlnElmmnts, the Barnacle mechanism is *broken* because
  //                 the same element ptr may refer to multiple locations within a beamline. 
  //                 As a temporary work around, one can ***remove*** the barnacles in a FIFO manner. 
  //
  //-------------------------------------------------------------------------------------


  for ( beamline::deep_iterator it = myBeamlinePtr_->deep_begin(); it != myBeamlinePtr_->deep_end(); ++it) 
  {
         
        BarnacleList::iterator itb;
        if( ( itb = (*it)->dataHook.find( "EdwardsTengSage" )) !=  (*it)->dataHook.end() ) {

            calcs_.push_back( boost::any_cast<EdwardsTengSage::Info>(itb->info) );
 
            // NOTE: the step below needed if *SHARED elements* are allowed !!! 
            //  (*it)->dataHook.erase(itb); 
        }
   
  }

  return 0;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<EdwardsTengSage::Info> const& 
  EdwardsTengSage::getETArray()
{

  return  calcs_;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
