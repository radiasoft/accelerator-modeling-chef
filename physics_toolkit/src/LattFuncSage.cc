/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      LattFuncSage.cc
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
******  REVISION HISTORY
******
******  Dec 2006 - Jean-Francois Ostiguy 
******             ostiguy@fnal
******    
******  - interface based on Particle& rather than ptrs. 
******    Stack allocated local Particle objects.
******  - changes to accomodate new boost::any based Barnacle objects.
******  - use new style STL-compatible beamline iterators
******  - calcs_ array is now an STL vector. LF are now returned 
******    by returning a const reference to the entire vector.
******  - misc cleanup.  
******
**************************************************************************
*************************************************************************/

/*
 *  File: LattFuncSage.cc
 *  
 *  Implementation for a sage for computing
 *  traditional, and untraditional,  lattice 
 *  functions.
 *  
 *  Leo Michelotti
 *  Nov. 19, 1998
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iomanip>

#include <basic_toolkit/iosetup.h>
#include <physics_toolkit/LattFuncSage.h>
#include <physics_toolkit/ClosedOrbitSage.h>
#include <physics_toolkit/FPSolver.h>
#include <physics_toolkit/QBpropVisitor.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/rbend.h>
#include <beamline/CF_rbend.h>
#include <beamline/Slot.h>

using namespace std;

using FNAL::pcout;
using FNAL::pcerr;

extern int filterTransverseTunes( /* const */ MatrixD&, Vector& );


namespace {

 Particle::PhaseSpaceIndex const& i_x     = Particle::xIndex;
 Particle::PhaseSpaceIndex const& i_npx   = Particle::npxIndex;
 Particle::PhaseSpaceIndex const& i_y     = Particle::yIndex;
 Particle::PhaseSpaceIndex const& i_npy   = Particle::npyIndex;
 Particle::PhaseSpaceIndex const& i_cdt   = Particle::cdtIndex;
 Particle::PhaseSpaceIndex const& i_ndp   = Particle::ndpIndex;

  int const BMLN_dynDim =6;


 bool checkForCoupling ( Matrix const& mtrx)
 { 
  
  bool ret = false;
 
  if( ( mtrx( i_y,   i_x   ) != 0.0 )  ||
      ( mtrx( i_x,   i_y   ) != 0.0 )  ||
      ( mtrx( i_x,   i_npy ) != 0.0 )  ||
      ( mtrx( i_y,   i_npx ) != 0.0 )  ||
      ( mtrx( i_npy, i_x   ) != 0.0 )  ||
      ( mtrx( i_npx, i_y   ) != 0.0 )  ||
      ( mtrx( i_npy, i_npx ) != 0.0 )  ||
      ( mtrx( i_npx, i_npy ) != 0.0 )     )
  {
    (*pcerr) << "*** WARNING ***                                 \n"
            "*** WARNING *** LattFuncSage::Slow_CS_Calc          \n"
            "*** WARNING *** Coupling detected. Calculation is   \n"
            "*** WARNING *** proceeding but results are suspect. \n"
            "*** WARNING ***                                     \n"
            "*** WARNING *** Suggest you use EdwardsTeng         \n"
            "*** WARNING *** instead.                            \n"
            "*** WARNING ***                                     \n"
         << endl;
     ret = true;
   }

  return ret;
 }


} // namespace


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


LattFuncSage::lattFunc::lattFunc() {

  arcLength      = 0.0;
  dispersion.hor = 0.0;
  dispersion.ver = 0.0;
  dPrime.hor     = 0.0;
  dPrime.ver     = 0.0;
  beta.hor       = 0.0;   // Indicates value is not
  beta.ver       = 0.0;   // calculated.
  alpha.hor      = 0.0;
  alpha.ver      = 0.0;
  psi.hor        = 0.0;
  psi.ver        = 0.0;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


LattFuncSage::LattFuncSage( BmlPtr x ) 
  : Sage( x ), dpp_( 0.00005 )
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


LattFuncSage::LattFuncSage( beamline const& x) 
  : Sage( x ), dpp_( 0.00005 )
{}




//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


double LattFuncSage::get_dpp()
{
  return dpp_;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void LattFuncSage::set_dpp( double x )
{
  if( x > 0.0 ) {
    dpp_ = x;
  }
  else {
    (*pcerr) << "*** WARNING ***                        \n"
            "*** WARNING *** LattFuncSage::set_dpp   \n"
            "*** WARNING *** Non-positive argument  \n"
            "*** WARNING *** changed.               \n"
            "*** WARNING ***                        \n"
	 << endl;
    if( x != 0.0 ) dpp_ = -x;
  }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


std::vector<LattFuncSage::lattFunc> const& 
 LattFuncSage::getTwissArray()
{
 
  return  lfvec_; 

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LattFuncSage::lattRing const& LattFuncSage::getLattRing()
{
  return lr_;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int LattFuncSage::pushCalc( Particle const& prt, LattFuncSage::lattFunc const& initialConditions )
{

  if( verbose_ ) {
    *outputStreamPtr_ << "LattFuncSage -- Entering LattFuncSage::pushCalc" << endl;
    outputStreamPtr_->flush();
  }

  int ret = 0;

  const int N = prt.State().Dim();

  // Preserve the current Jet environment

  Jet__environment_ptr  storedEnv  = Jet__environment::getLastEnv();
  JetC__environment_ptr storedEnvC = JetC__environment::getLastEnv();

  // Create a new Jet environment
  double scale[N];
  //   scale is probably no longer needed ... oh, well ...

  for( int i=0; i < N; ++i) {
    scale[i] = 0.001;
  }

  std::vector<coord*>  coord_vec; 

  Jet__environment::BeginEnvironment( 1 );
 
  for( int i=0; i < N; ++i) {
    coord_vec.push_back(new coord( prt.State()[i] ) );
  }

  JetC__environment::setLastEnv( Jet__environment::EndEnvironment(scale) ); // implicit conversion 
 

  Particle    p0(prt);
  JetParticle jp(prt);

  const double beta_x_0  = initialConditions.beta.hor;
  const double alpha_x_0 = initialConditions.alpha.hor;
  const double gamma_x_0 = ( 1.0 + alpha_x_0*alpha_x_0 )/beta_x_0;

  const double beta_y_0  = initialConditions.beta.ver;
  const double alpha_y_0 = initialConditions.alpha.ver;
  const double gamma_y_0 = ( 1.0 + alpha_y_0*alpha_y_0 )/beta_y_0;


  MatrixD mtrx(N,N,0.0);

  double const momentum = jp.ReferenceMomentum();

  lfvec_.clear();

  double arcLength = 0.0;
  double psi_x     = 0.0;
  double psi_y     = 0.0;


  for (beamline::deep_iterator it =  myBeamlinePtr_->deep_begin(); 
                               it != myBeamlinePtr_->deep_end();  ++it) 

  {

    arcLength += (*it) -> OrbitLength( p0 );
    (*it) -> propagate( jp);
    mtrx = jp.State().Jacobian();


    double a = mtrx(  i_x,   i_x   );
    double b = mtrx(  i_x,   i_npx );
    double c = mtrx(  i_npx, i_x   );
    double d = mtrx(  i_npx, i_npx );

    // I allow for the possibility of RF cavities by scaling

    double beta_x =  ( a*a*beta_x_0 - 2.0*a*b*alpha_x_0 + b*b*gamma_x_0 )
                    *( jp.ReferenceMomentum()/momentum );

    double alpha_x = ( - a*c*beta_x_0 + (a*d+b*c)*alpha_x_0 - d*b*gamma_x_0 )
                    *( jp.ReferenceMomentum()/momentum );

    a = mtrx( i_y,    i_y   );
    b = mtrx( i_y,    i_npy );
    c = mtrx( i_npy,  i_y   );
    d = mtrx( i_npy,  i_npy );

    psi_x    =  (( psi_x = atan( (d-a)/(d+a)) ) > 0.0 ) ?  psi_x : 2*M_PI + psi_x;

    double beta_y =  ( a*a*beta_y_0 - 2.0*a*b*alpha_y_0 + b*b*gamma_y_0 )
                    *( jp.ReferenceMomentum()/momentum );

    double alpha_y = ( - a*c*beta_y_0 + (a*d+b*c)*alpha_y_0 - d*b*gamma_y_0 )
                    *( jp.ReferenceMomentum()/momentum );

    psi_y    =  (( psi_y = atan( (d-a)/(d+a)) ) > 0.0 ) ?  psi_y : 2*M_PI + psi_y;

    // Output

    LattFuncSage::lattFunc lf;

    lf.arcLength = arcLength;
    lf.beta.hor  = beta_x;
    lf.beta.ver  = beta_y;
    lf.alpha.hor = alpha_x;
    lf.alpha.ver = alpha_y;
    lf.psi.hor   = psi_x;
    lf.psi.ver   = psi_y;

    lfvec_.push_back(lf);


  } // end loop over the beamline elements ..............

  
  // Clean up before exit
  Jet__environment::setLastEnv( storedEnv );
  JetC__environment::setLastEnv( storedEnvC );

  for( std::vector<coord*>::iterator it = coord_vec.begin();  it != coord_vec.end(); ++it) {
    delete (*it);
  }
  
  if( verbose_ ) {
    *outputStreamPtr_ << "LattFuncSage -- Leaving LattFuncSage::pushCalc" << endl;
    outputStreamPtr_->flush();
  }

  return ret;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LattFuncSage::eraseAll() 
{


  for (beamline::deep_iterator it =  myBeamlinePtr_->deep_begin(); 
                               it != myBeamlinePtr_->deep_end();  ++it ) { 
    (*it)->dataHook.eraseAll( "LattFuncSage" );
  }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int LattFuncSage::CourantSnyderLatticeFunctions(  JetParticle const& jp, Sage::CRITFUNC Crit )
{
  // PRECONDITIONS: 
  //    jp  is already on the closed orbit and its
  //        state is the one-turn map.
  //
  // *myBeamlinePtr_  is an uncoupled ring.
  // 
  // POSTCONDITIONS:
  // On every element filtered by Crit is attached a lattFunc
  // Barnacle.

  if( verbose_ ) {
    (*pcout) << "LattFuncSage -- Entering LattFuncSage::CourantSnyderLatticeFunctions" << endl;
    (*pcout).flush();
  }

  // Preserve the current Jet environment
  Jet__environment_ptr  storedEnv  = Jet__environment::getLastEnv();
  JetC__environment_ptr storedEnvC = JetC__environment::getLastEnv();

  // Reset current environment

  Jet__environment::setLastEnv(  jp.State().Env() );
  JetC__environment::setLastEnv( Jet__environment::getLastEnv() ); // implicit conversion 

  JetParticle  jparticle(jp);
  Particle      particle(jp);

  int ret = 0;
  int i   = 0;

  // .......... Check coupling ............................


  MatrixD mtrx = jp.State().Jacobian();
  ::checkForCoupling(mtrx);
  
  // Calculate initial lattice functions ...

  // ... first horizontal

  double cs = ( mtrx( i_x, i_x ) + mtrx( i_npx, i_npx ) )/2.0;

  if( fabs( cs ) > 1.0 ) {
    (*pcerr) << "*** ERROR ***                                     \n"
                "*** ERROR *** LattFuncSage::CourantSnyderLatticeFunctions \n"
                "*** ERROR *** cos( psi_H ) = "
         << cs
         << "\n"
            "*** ERROR *** Lattice is unstable.                \n"
            "*** ERROR *** Cannot continue with calculation.   \n"
            "*** ERROR ***                                     \n"
         << endl;

    Jet__environment::setLastEnv(storedEnv );
    JetC__environment::setLastEnv( storedEnvC );
    return LattFuncSage::UNSTABLE;
  }

  double sn = ( mtrx( i_x, i_npx ) > 0.0 ) ? sqrt( 1.0 - cs*cs ): - sqrt( 1.0 - cs*cs );

  if( sn == 0.0 ) {
    (*pcerr) << "*** ERROR ***                                     \n"
            "*** ERROR *** LattFuncSage::CourantSnyderLatticeFunctions \n"
            "*** ERROR *** Integer horizontal tune.            \n"
            "*** ERROR ***                                     \n"
         << endl;

    Jet__environment::setLastEnv( storedEnv );
    JetC__environment::setLastEnv( storedEnvC );
    return LattFuncSage::INTEGER_TUNE;
  }

  double beta_x  = mtrx( i_x, i_npx ) / sn;
  double alpha_x = ( mtrx( i_x, i_x ) - mtrx( i_npx, i_npx ) ) / ( 2.0*sn );


  // ... then vertical.

  cs = ( mtrx( i_y, i_y ) + mtrx( i_npy, i_npy ) )/2.0;
  if( fabs( cs ) <= 1.0 ) {
    if( mtrx( i_y, i_npy ) > 0.0 )  sn =   sqrt( 1.0 - cs*cs );
    else                            sn = - sqrt( 1.0 - cs*cs );
  }
  else {
    (*pcerr) << "*** ERROR ***                                     \n"
            "*** ERROR *** LattFuncSage::CourantSnyderLatticeFunctions \n"
            "*** ERROR *** cos( psi_V ) = "
         << cs
         << "\n"
            "*** ERROR *** Lattice is unstable.                \n"
            "*** ERROR *** Cannot continue with calculation.   \n"
            "*** ERROR ***                                     \n"
         << endl;

    Jet__environment::setLastEnv( storedEnv );
    JetC__environment::setLastEnv( storedEnvC );
    return LattFuncSage::UNSTABLE;
  }

  if( sn == 0.0 ) {
    (*pcerr) << "*** ERROR ***                                     \n"
            "*** ERROR *** LattFuncSage::CourantSnyderLatticeFunctions \n"
            "*** ERROR *** Integer vertical tune.              \n"
            "*** ERROR ***                                     \n"
         << endl;


    Jet__environment::setLastEnv( storedEnv );
    JetC__environment::setLastEnv( storedEnvC );
    return LattFuncSage::INTEGER_TUNE;
  }

  double  beta_y  = mtrx( i_y, i_npy ) / sn;
  double alpha_y  = ( mtrx( i_y, i_y ) - mtrx( i_npy, i_npy ) ) / ( 2.0*sn );


  double beta0H  = beta_x;
  double beta0V  = beta_y;
  double alpha0H = alpha_x;
  double alpha0V = alpha_y;

  double oldpsiH = 0.0;
  double oldpsiV = 0.0;

  double tb      = 0.0;
  double t       = 0.0;
  double lng     = 0.0;
  double psi_x   = 0.0;
  double psi_y   = 0.0;

  jparticle.setState( particle.State() );

  lfvec_.clear();

  for( beamline::deep_iterator it = myBeamlinePtr_->deep_begin(); it != myBeamlinePtr_->deep_end(); ++it) 
  {
    ElmPtr lbe = (*it);

    bool is_regular = ( ( typeid(*lbe) != typeid(rbend)    ) && 
                      (   typeid(*lbe) != typeid(CF_rbend) ) && 
                      (   typeid(*lbe) != typeid(Slot)     ) &&
                      (     (*lbe).hasStandardFaces()      )  );

    // bool is_regular = true;

    lng += lbe->OrbitLength( particle );
    lbe -> propagate( jparticle );

    mtrx = jparticle.State().Jacobian();
    
  
    tb      = mtrx[0][0] * beta0H -  mtrx[0][3] * alpha0H;
    beta_x  = ( tb * tb + mtrx(0,3) * mtrx(0,3))/beta0H;
   
    alpha_x = -1.0*(tb * (mtrx(3,0)*beta0H - mtrx(3,3)*alpha0H) +
   			     mtrx(0,3)*mtrx(3,3))/beta0H;
   
    if ( is_regular ) {
       t = atan2(mtrx(0,3),tb);
       while(t < oldpsiH) t += M_TWOPI;
       psi_x = oldpsiH = t;
     }
     else { 
       psi_x = oldpsiH;
     }

     tb     = mtrx(1,1) * beta0V -  mtrx(1,4) * alpha0V;
     beta_y = (tb * tb + mtrx(1,4) * mtrx(1,4))/beta0V;
   
     alpha_y = -1.0*(tb * (mtrx(4,1)*beta0V - mtrx(4,4)*alpha0V) +
   			     mtrx(1,4)*mtrx(4,4))/beta0V;
   
     if ( is_regular ) {
       t = atan2(mtrx(1,4),tb);
       while(t < oldpsiV) t += M_TWOPI;
       psi_y = oldpsiV = t;
     } 
     else { 
       psi_y = oldpsiV;
     }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++

    // Store the calculation if appropriate ...
    if( ( !Crit ) || ( Crit( lbe ) ) ) 
    {
      LattFuncSage::lattFunc lf;

      lf.arcLength = lng;
      lf.beta.hor  = beta_x;
      lf.beta.ver  = beta_y;
      lf.alpha.hor = alpha_x;
      lf.alpha.ver = alpha_y;
      lf.psi.hor   = psi_x;
      lf.psi.ver   = psi_y;

      lfvec_.push_back(lf);

      if( localData_ ) {
        BarnacleList::iterator it;
        if( (it = lbe->dataHook.find("Twiss") )==  lbe->dataHook.end() ) {
          lbe->dataHook.insert( Barnacle( "Twiss", lf) );
        }
        else {
          it->info = lf;
        }
      }
    }

  }  // End loop on lbe ...


  // Finished....
  if( verbose_ ) {
    (*pcout) << "LattFuncSage -- Leaving LattFuncSage::CourantSnyderLatticeFunctions" << endl;
    (*pcout).flush();
  }


  Jet__environment::setLastEnv( storedEnv );
  JetC__environment::setLastEnv( storedEnvC );
  return ret;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int LattFuncSage::TuneCalc( JetParticle& jp, bool forceClosedOrbitCalc )
{
  // This method does the following:
  // 
  // (1) Puts  jp on the closed orbit by
  //     (1.a) instantiating a ClosedOrbitSage
  //     (1.b) invoking ClosedOrbitSage::findClosedOrbit.
  //     (1.c) Note: if forceClosedOrbitCalc is false, the
  //           ClosedOrbitSage first checks to see if *arg_jp
  //           is already on a closed orbit. 
  // 
  // (2) Calculates tunes:
  //     (2.a) projects separately the x-x' and y-y' sectors
  //           of the one-turn matrix onto 2x2 matrices
  //     (2.b) separately obtains "horizontal" and "vertical"
  //           tunes from the eigenvalues of those matrices.
  //     (2.c) stores the results on its beamline's dataHook
  //           with a Barnacle labelled "Tunes."
  //     (2.d) Note: this implicitly assumes an uncoupled machine!
  // 
  // (3) Upon returning, 
  //     (3.a) jp is on the closed orbit 
  //     (3.b) its state is the one-turn mapping.
  //     (3.c) the object's beamline contains a barnacle
  //           with the (uncoupled) tune information

  if( verbose_ ) {
    (*pcout) << "LattFuncSage -- Entering LattFuncSage::TuneCalc" << endl;
    (*pcout).flush();
  }


  int ret = 0;


  // Check for closed orbit ...
  ClosedOrbitSage clsg( this->myBeamlinePtr_ );

  if( forceClosedOrbitCalc ) { clsg.setForcedCalc(); }

  if( verbose_ ) clsg.set_verbose();
  else           clsg.unset_verbose();  // Unnecessary line.


  if( ( ret = clsg.findClosedOrbit( jp ) ) == 0 )
  {
    if( verbose_ ) {
      (*pcout) << "LattFuncSage -- Closed orbit successfully calculated." << endl;
      (*pcout).flush();
    }
  }
  else
  {
    if( verbose_ ) {
      (*pcout) << "LattFuncSage -- Closed orbit not successfully calculated." << endl;
      (*pcout).flush();
      return ret;
    }
  }


  // :::::::::::::::::::::::::::::::::::::::::::::::::::

 
  // .......... Calculating tunes .........................
  // .......... (lifted from EdwardsTeng) .................
  
  MatrixD mtrx = jp.State().Jacobian();

  ::checkForCoupling(mtrx);

   MatrixD M(2,2);
   M( 0, 0 ) = mtrx( i_x,   i_x  );
   M( 0, 1 ) = mtrx( i_x,   i_npx );
   M( 1, 0 ) = mtrx( i_npx, i_x  );
   M( 1, 1 ) = mtrx( i_npx, i_npx );

   MatrixC lambda = M.eigenValues();

   if( fabs( abs(lambda(0)) - 1.0 ) > 1.0e-4 ) {
      (*pcout) << "\n"
           << "*** ERROR ***                                     \n"
           << "*** ERROR ***                                     \n"
           << "*** ERROR *** LattFuncSage::TuneCalc              \n"
           << "*** ERROR *** The lattice is linearly unstable.   \n"
           << "*** ERROR *** horizontal lambda has magnitude = "
           << abs(lambda(0))
           << "\n"
           << "*** ERROR ***                                     \n"
           << endl;
      ret = 10; return ret;
   }

  

  // :::::::::::::::::::::::::::::::::::::::::::::::::::


  if( ( std::abs( lambda(0) - conj( lambda(1) ) ) > 1.0e-4 ) )
  {
  	(*pcout) << "\n"
  	     << "*** ERROR *** LattFuncSage::TuneCalc               \n"
  	     << "*** ERROR *** Conjugacy condition has been violated\n"
  	     << "*** ERROR *** The lattice may be linearly unstable.\n"
  	     << "*** ERROR *** Eigenvalues =                        \n"
  	     << "*** ERROR *** " << lambda << endl;
  	ret = 11; return ret;
  }
  

  // :::::::::::::::::::::::::::::::::::::::::::::::::::

   double csH = real( lambda(0) );
   double snH = sqrt( 1.0 - csH*csH );

   if( M(0,1) < 0.0 ) snH = - snH;
    

    // Calculation in vertical plane

   M( 0, 0 ) = mtrx( i_y,   i_y   );
   M( 0, 1 ) = mtrx( i_y,   i_npy );
   M( 1, 0 ) = mtrx( i_npy, i_y   );
   M( 1, 1 ) = mtrx( i_npy, i_npy );

   lambda = M.eigenValues();
   if( std::abs( std::abs(lambda(0)) - 1.0 ) > 1.0e-4 ) {
      (*pcout) << "\n"
           << "*** ERROR ***                                     \n"
           << "*** ERROR ***                                     \n"
           << "*** ERROR *** LattFuncSage::TuneCalc              \n"
           << "*** ERROR *** The lattice is linearly unstable.   \n"
           << "*** ERROR *** vertical lambda has magnitude = "
           << abs(lambda(0))
           << "\n"
           << "*** ERROR ***                                     \n"
           << endl;
       ret = 12; return ret;
  }

  // :::::::::::::::::::::::::::::::::::::::::::::::::::

  if( ( std::abs( lambda(0) - conj( lambda(1) ) ) > 1.0e-4 ) )
  {
  	(*pcout) << "\n"
  	     << "*** ERROR *** LattFuncSage::TuneCalc               \n"
  	     << "*** ERROR *** Conjugacy condition has been violated\n"
  	     << "*** ERROR *** The lattice may be linearly unstable.\n"
  	     << "*** ERROR *** Eigenvalues =                        \n"
  	     << "*** ERROR *** " << lambda << endl;
          ret = 13; return ret;
  }

  // :::::::::::::::::::::::::::::::::::::::::::::::::::
  
  double csV = real( lambda(0) );
  double snV = sqrt( 1.0 - csV*csV );

  if( M(0,1) < 0.0 ) snV = - snV;

    
  // Attach data to the beamline

  LattFuncSage::tunes  lftunes;

  double t = atan2( snH, csH );
  if( t < 0.0 )   t += M_TWOPI;
  lftunes.hor = ( t / M_TWOPI );
  t = atan2( snV, csV );
  if( t < 0.0 )   t += M_TWOPI;
  lftunes.ver = ( t / M_TWOPI );

  myBeamlinePtr_->dataHook.eraseAll( "Tunes" );
  myBeamlinePtr_->dataHook.append( Barnacle( "Tunes", lftunes ) );
  

  // :::::::::::::::::::::::::::::::::::::::::::::::::::

  // Final operations ....................................
  if( verbose_ ) {
    (*pcout) << "LattFuncSage -- Leaving LattFuncSage::TuneCalc" << endl;
    (*pcout).flush();
  }
  
  return ret;
}



//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int LattFuncSage::NewDisp_Calc( JetParticle const& arg_jp,  bool onClosedOrbit )
{
  if( verbose_ ) {
    (*pcout) << "LattFuncSage -- Entering LattFuncSage::Disp_Calc" << endl;
    (*pcout).flush();
  }

  JetParticle jp(arg_jp);

  int ret = 0;


   double lng = 0.0;

  // Preliminary steps ...

  ClosedOrbitSage clsg( myBeamlinePtr_ );
  if( verbose_ ) {
    clsg.set_verbose();
  }

  if( !onClosedOrbit ) { 
    clsg.setForcedCalc(); 
    ret = clsg.findClosedOrbit( jp );
    clsg.unsetForcedCalc();
  }

  if( ret == 0 ) {
    if( verbose_ ) {
      (*pcout) << "LattFuncSage -- Closed orbit successfully calculated." << endl;
      (*pcout).flush();
    }
  }

  else {
    if( verbose_ ) {
      (*pcout) << "LattFuncSage -- Closed orbit not successfully calculated." << endl;
      (*pcout) << "LattFuncSage -- Leaving LattFuncSage::Disp_Calc" << endl;
      (*pcout).flush();
    }
    return ret;
  }

  Particle firstParticle(jp);
  MatrixD firstJacobian  = jp.State().Jacobian();


  // Calculate the closed orbit for an off-momentum particle ...
  if( verbose_ ) {
    (*pcout) << "LattFuncSage --- Starting calculation of offset closed orbit." << endl;
    (*pcout).flush();
  }

  double dpp = get_dpp();

  Particle tmp_p(jp);
  tmp_p.State()[i_ndp] = dpp;
  jp = JetParticle(tmp_p); 

  clsg.setForcedCalc();
  ret = clsg.findClosedOrbit( jp );
  clsg.unsetForcedCalc();

  if( ret == 0 ) {
    if( verbose_ ) {
      (*pcout) << "LattFuncSage -- Offset closed orbit successfully calculated." << endl;
      (*pcout).flush();
    }
  }
  else {
    if( verbose_ ) {
      (*pcout) << "LattFuncSage -- Off-momentum closed orbit not successfully calculated." << endl;
      (*pcout) << "LattFuncSage -- Leaving LattFuncSage::Disp_Calc" << endl;
      (*pcout).flush();
    }
    return ret;
  }

  Particle secondParticle(jp);
  MatrixD secondJacobian  = jp.State().Jacobian();


  // Attach dispersion data wherever desired ...
  if( verbose_ ) {
    (*pcout) << "LattFuncSage --- Attaching dispersion data to the elements." << endl;
    (*pcout).flush();
  }

  // Attach initial dispersion data to the beamline ...

  Vector d( firstParticle.State().Dim() );

  bool stand_alone_disp_calc = ( lfvec_.size() == 0 );   

  std::vector<LattFuncSage::lattFunc>::iterator lf_it =  lfvec_.begin();

  for( beamline::deep_iterator it = myBeamlinePtr_->deep_begin(); it != myBeamlinePtr_->deep_end(); ++it ) {

    (*it)->propagate( firstParticle );
    (*it)->propagate( secondParticle );

    lng += (*it)->OrbitLength( firstParticle );

    d = ( secondParticle.State()  -  firstParticle.State() ) / dpp;

    if ( stand_alone_disp_calc ) {     	
       LattFuncSage::lattFunc lf;
       lf.dispersion.hor = d( i_x  );
       lf.dPrime.hor     = d( i_npx );
       lf.dispersion.ver = d( i_y  );
       lf.dPrime.ver     = d( i_npy );
       lf.arcLength        = lng;
       lfvec_.push_back(lf);
    } 
    else {
       lf_it->dispersion.hor = d( i_x  );
       lf_it->dPrime.hor     = d( i_npx );
       lf_it->dispersion.ver = d( i_y  );
       lf_it->dPrime.ver     = d( i_npy );      
       if( localData_ ) {
		BarnacleList::iterator bit;
        	if( (bit = (*it)->dataHook.find("Dispersion") )==  (*it)->dataHook.end() ) {
          		(*it)->dataHook.insert( Barnacle( "Dispersion", *lf_it) );
        	}
        	else {
          		bit->info = *lf_it;
        	}
	}
       
       
       
       ++lf_it;
      
    }
    	
  }  


  // Attach tune and chromaticity to the beamline ........

  Vector    firstNu(2), secondNu(2);
  if( ( 0 == filterTransverseTunes( firstJacobian, firstNu   ) ) && 
      ( 0 == filterTransverseTunes( secondJacobian, secondNu ) ) )
  {
    lr_.tune.hor = firstNu(0);
    lr_.tune.ver = firstNu(1);
    lr_.chromaticity.hor = ( secondNu(0) - firstNu(0) ) / dpp;
    lr_.chromaticity.ver = ( secondNu(1) - firstNu(1) ) / dpp;
  }
  else {
    (*pcerr) << "*** ERROR ***                                        \n"
            "*** ERROR *** LattFuncSage::Disp_Calc                \n"
            "*** ERROR ***                                        \n"
            "*** ERROR *** Horrible error occurred while trying   \n"
            "*** ERROR *** to filter the tunes.                   \n"
            "*** ERROR ***                                        \n"
         << endl;
    ret = 111;

    return ret;
  }




  if( verbose_ ) {
    (*pcout) << "LattFuncSage -- Leaving LattFuncSage::Disp_Calc" << endl;
    (*pcout).flush();
  }

  return ret;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

