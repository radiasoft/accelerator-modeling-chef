/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      LattFuncSage.cc
******  Version:   1.0                    
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


// ... Globals:
const int LattFuncSage::DONE             = 0;
const int LattFuncSage::SLOTS_DETECTED   = 1;
const int LattFuncSage::UNSTABLE         = 2;
const int LattFuncSage::INTEGER_TUNE     = 3;
const int LattFuncSage::PHASE_ERROR      = 4;
const int LattFuncSage::WRONG_COUNT      = 5;
const int LattFuncSage::NOT_WRITTEN      = 6;
const int LattFuncSage::TOO_MANY_VECTORS = 7;

double LattFuncSage::_csH = 0.0;
double LattFuncSage::_csV = 0.0;
double LattFuncSage::_snH = 0.0;
double LattFuncSage::_snV = 0.0;
Mapping* LattFuncSage::_theMapPtr = 0;



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


LattFuncSage::LattFuncSage( const beamline* x, bool doClone ) 
  : Sage( x, doClone ), _dpp( 0.00005 )
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


LattFuncSage::LattFuncSage( const beamline& x, bool doClone ) 
  : Sage( &x, doClone ), _dpp( 0.00005 )
{}




//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


double LattFuncSage::get_dpp()
{
  return _dpp;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void LattFuncSage::set_dpp( double x )
{
  if( x > 0.0 ) {
    _dpp = x;
  }
  else {
    (*pcerr) << "*** WARNING ***                        \n"
            "*** WARNING *** LattFuncSage::set_dpp   \n"
            "*** WARNING *** Non-positive argument  \n"
            "*** WARNING *** changed.               \n"
            "*** WARNING ***                        \n"
	 << endl;
    if( x != 0.0 ) _dpp = -x;
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
  if( _verbose ) {
    *_outputStreamPtr << "LattFuncSage -- Entering LattFuncSage::pushCalc" << endl;
    _outputStreamPtr->flush();
  }

  int ret = 0;

  const int N = prt.State().Dim();

  // Preserve the current Jet environment
  Jet__environment_ptr  storedEnv  = Jet__environment::getLastEnv();
  JetC__environment_ptr storedEnvC = JetC__environment::getLastEnv();

  // Create a new Jet environment
  double scale[N];
  //   scale is probably no longer needed ... oh, well ...

  for( int i = 0; i < N; ++i) {
    scale[i] = 0.001;
  }

  std::vector<coord*>  coord_vec; 

  Jet__environment::BeginEnvironment( 1 );
 
  for( int i=0; i < N; ++i) {
    coord_vec.push_back(new coord( prt.State(i) ) );
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

  double beta_x, beta_y, alpha_x, alpha_y;

  MatrixD mtrx(N,N,0.0);

  int x =  prt.xIndex();
  int y =  prt.yIndex();
  int px = prt.npxIndex();
  int py = prt.npyIndex();

  const double energy = jp.ReferenceEnergy();

  double arcLength = 0.0;
  double a, b, c, d;

  lfvec_.clear();

  for (beamline::deep_iterator it =  _myBeamlinePtr->deep_begin(); 
                               it != _myBeamlinePtr->deep_end();  ++it) 

  {
    arcLength += (*it) -> OrbitLength( p0 );
    (*it) -> propagate( jp);
    mtrx = jp.State().Jacobian();

    a = mtrx(x,x);
    b = mtrx(x,px);
    c = mtrx(px,x);
    d = mtrx(px,px);

    // I allow for the possibility of RF cavities by scaling

    beta_x =  ( a*a*beta_x_0 - 2.0*a*b*alpha_x_0 + b*b*gamma_x_0 )
             *( jp.ReferenceEnergy()/energy );
    alpha_x = ( - a*c*beta_x_0 + (a*d+b*c)*alpha_x_0 - d*b*gamma_x_0 )
             *( jp.ReferenceEnergy()/energy );

    a = mtrx(y,y);
    b = mtrx(y,py);
    c = mtrx(py,y);
    d = mtrx(py,py);

    beta_y =  ( a*a*beta_y_0 - 2.0*a*b*alpha_y_0 + b*b*gamma_y_0 )
             *( jp.ReferenceEnergy()/energy );
    alpha_y = ( - a*c*beta_y_0 + (a*d+b*c)*alpha_y_0 - d*b*gamma_y_0 )
             *( jp.ReferenceEnergy()/energy );

    // Output

    lattFunc lf;

    lf.arcLength = arcLength;
    lf.beta.hor  = beta_x;
    lf.beta.ver  = beta_y;
    lf.alpha.hor = alpha_x;
    lf.alpha.ver = alpha_y;

    lfvec_.push_back(lf);

  } // end loop over the beamline elements ..............

  
  // Clean up before exit
  Jet__environment::setLastEnv( storedEnv );
  JetC__environment::setLastEnv( storedEnvC );

  for( std::vector<coord*>::iterator it = coord_vec.begin();  it != coord_vec.end(); ++it) {
    delete (*it);
  }
  
  if( _verbose ) {
    *_outputStreamPtr << "LattFuncSage -- Leaving LattFuncSage::pushCalc" << endl;
    _outputStreamPtr->flush();
  }

  return ret;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LattFuncSage::eraseAll() 
{


  for (beamline::deep_iterator it =  _myBeamlinePtr->deep_begin(); 
                               it != _myBeamlinePtr->deep_end();  ++it ) { 
    (*it)->dataHook.eraseAll( "LattFuncSage" );
  }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int LattFuncSage::Fast_CS_Calc( JetParticle const& jp, Sage::CRITFUNC Crit )
{
  // PRECONDITIONS: 
  // jp  is already on the closed orbit and its
  //        state is the one-turn map.
  //
  // *_myBeamlinePtr  is an uncoupled ring containing
  //                  no Slots.
  // 
  // POSTCONDITIONS:
  // On every element filtered by Crit is attached a lattFunc
  // Barnacle.

  if( this->_verbose ) {
    (*pcout) << "LattFuncSage -- Entering LattFuncSage::Fast_CS_Calc" << endl;
    (*pcout).flush();
  }

  // Initial check for Slots
  // This may be a redundant check, but it is fast.
  // ??? Perhaps a LattFuncSage variable can toggle
  // ??? this test on or off.
  {

      for (beamline::deep_iterator it =  _myBeamlinePtr->deep_begin(); 
                                   it != _myBeamlinePtr->deep_end();  ++it) {
        if( strstr( "CF_rbend|rbend|Slot", (*it)->Type() ) ) {
          (*pcerr) << "*** ERROR ***                                     \n"
                  "*** ERROR *** LattFuncSage::Fast_CS_Calc          \n"
                  "*** ERROR *** This method  will not work          \n"
                  "*** ERROR *** with Slots.                         \n"
                  "*** ERROR ***                                     \n"
                  "*** ERROR *** Try using another method.           \n"
                  "*** ERROR ***                                     \n"
               << endl;
          return LattFuncSage::SLOTS_DETECTED;
        }
      }
  }



  Particle  p_1(jp);
  Particle  p_2(jp);

  int i_x   =  jp.xIndex();
  int i_y   =  jp.yIndex();
  int i_z   =  jp.cdtIndex();
  int i_px  =  jp.npxIndex();
  int i_py  =  jp.npyIndex();
  int i_dpp =  jp.ndpIndex();

  int ret   = 0;

  // .......... Check coupling ............................

  MatrixD mtrx = ( jp.State() ).Jacobian();
  
  if( ( mtrx( i_y,  i_x  ) != 0.0 )  ||
      ( mtrx( i_x,  i_y  ) != 0.0 )  ||
      ( mtrx( i_x,  i_py ) != 0.0 )  ||
      ( mtrx( i_y,  i_px ) != 0.0 )  ||
      ( mtrx( i_py, i_x  ) != 0.0 )  ||
      ( mtrx( i_px, i_y  ) != 0.0 )  ||
      ( mtrx( i_py, i_px ) != 0.0 )  ||
      ( mtrx( i_px, i_py ) != 0.0 )     )
  {
    (*pcerr) << "*** WARNING ***                                     \n"
            "*** WARNING *** LattFuncSage::Fast_CS_Calc          \n"
            "*** WARNING *** Coupling detected. Calculation is   \n"
            "*** WARNING *** proceeding but results are suspect. \n"
            "*** WARNING ***                                     \n"
            "*** WARNING *** Suggest you use EdwardsTeng         \n"
            "*** WARNING *** instead.                            \n"
            "*** WARNING ***                                     \n"
         << endl;
  }


  // Calculate initial lattice functions ...
  // ... first horizontal

  double cs = ( mtrx( i_x, i_x ) + mtrx( i_px, i_px ) )/2.0;

  if (fabs( cs ) > 1.0)  
 {
    (*pcerr) << "*** ERROR ***                                     \n"
            "*** ERROR *** LattFuncSage::Fast_CS_Calc          \n"
            "*** ERROR *** cos( psi_H ) = "
         << cs
         << "\n"
            "*** ERROR *** Cannot continue with calculation.   \n"
            "*** ERROR ***                                     \n"
         << endl;

    return LattFuncSage::UNSTABLE;
  }

  
  double  sn = ( mtrx( i_x, i_px ) > 0.0 ) ? sqrt( 1.0 - cs*cs ): - sqrt( 1.0 - cs*cs );


  if( sn == 0.0 ) {
    (*pcerr) << "*** ERROR ***                                     \n"
            "*** ERROR *** LattFuncSage::Fast_CS_Calc          \n"
            "*** ERROR *** Integer horizontal tune.            \n"
            "*** ERROR ***                                     \n"
         << endl;

    return LattFuncSage::INTEGER_TUNE;
  }

  double beta_x  = mtrx( i_x, i_px ) / sn;
  double alpha_x = ( mtrx( i_x, i_x ) - mtrx( i_px, i_px ) ) / ( 2.0*sn );


  // ... then vertical.

  cs = ( mtrx( i_y, i_y ) + mtrx( i_py, i_py ) )/2.0;
  if( fabs( cs ) <= 1.0 ) {
    if( mtrx( i_y, i_py ) > 0.0 )  sn =   sqrt( 1.0 - cs*cs );
    else                           sn = - sqrt( 1.0 - cs*cs );
  }
  else {
    (*pcerr) << "*** ERROR ***                                     \n"
            "*** ERROR *** LattFuncSage::Fast_CS_Calc          \n"
            "*** ERROR *** cos( psi_V ) = "
         << cs
         << "\n"
            "*** ERROR *** Cannot continue with calculation.   \n"
            "*** ERROR ***                                     \n"
         << endl;

    return LattFuncSage::UNSTABLE;
  }

  if( sn == 0.0 ) {
    (*pcerr) << "*** ERROR ***                                     \n"
            "*** ERROR *** LattFuncSage::Fast_CS_Calc          \n"
            "*** ERROR *** Integer vertical tune.              \n"
            "*** ERROR ***                                     \n"
         << endl;

    return LattFuncSage::INTEGER_TUNE;
  }

  double beta_y  = mtrx( i_y, i_py ) / sn;
  double alpha_y = ( mtrx( i_y, i_y ) - mtrx( i_py, i_py ) ) / ( 2.0*sn );


  Vector inState(6);
  double resizeFactor = 0.00001;


  // Set up "real" particle ...

  inState[ p_1.xIndex()   ] =   sqrt( beta_x );
  inState[ p_1.npxIndex() ] = - alpha_x / sqrt( beta_x );
  inState[ p_1.yIndex()   ] =   sqrt( beta_y );
  inState[ p_1.npyIndex() ] = - alpha_y / sqrt( beta_y );

  for( int i=0; i < 6; i++ ) {
    inState[i] *= resizeFactor;
  }
  p_1.setState( inState );
  for( int i = 0; i < 6; i++ ) {
    inState[i] = 0.0;
  }
    
  // Set up "imaginary" particle ...
  inState[ p_2.npxIndex() ] = 1.0/sqrt( beta_x );
  inState[ p_2.npyIndex() ] = 1.0/sqrt( beta_y );
  for( int i= 0; i < 6; i++ ) {
    inState[i] *= resizeFactor;
  }
  p_2.setState( inState );
  for( int i = 0; i < 6; i++ ) {
    inState[i] = 0.0;     // Not necessary here, but done for safety.
  }  
  
  
  std::complex<double>  outState[6], phase;

 
  QBpropVisitor zlorfik( p_1 );
  QBpropVisitor fembril( p_2 );
  
  double lng   = 0.0;
  double psi_x = 0.0;
  double psi_y = 0.0;
  double dpsi_x, dpsi_y;


  lfvec_.clear();

  for (beamline::deep_iterator it =  _myBeamlinePtr->deep_begin(); 
                               it != _myBeamlinePtr->deep_end();  ++it) {

    lng += (*it)->OrbitLength( p_1 );

    (*it)->accept( zlorfik );
    (*it)->accept( fembril );

    Vector realState  = zlorfik.getState();
    Vector imagState  = fembril.getState();

    outState[i_x ] = std::complex<double> ( realState[i_x],  imagState[i_x] );
    outState[i_y ] = std::complex<double> ( realState[i_y],  imagState[i_y] );
    outState[i_px] = std::complex<double> ( realState[i_px], imagState[i_px] );
    outState[i_py] = std::complex<double> ( realState[i_py], imagState[i_py] );
  
    phase = outState[i_x] / abs( outState[i_x] );
    outState[i_x]  /= phase;
    outState[i_px] /= phase;

    if(   fabs( imag( outState[i_px] )*real( outState[i_x] ) - 
                resizeFactor*resizeFactor )
        > resizeFactor*resizeFactor*0.01 )
    {
  	(*pcout) << "*** ERROR *** Phase error in horizontal plane at " 
             << (*it)->Type() << "  " << (*it)->Name()
             << endl;
  	(*pcout) << "*** ERROR *** imag( outState[i_px] ) = " 
             << imag( outState[i_px] ) 
             << endl;
  	(*pcout) << "*** ERROR *** " 
             << outState[i_px]/resizeFactor 
             << endl;

  	return LattFuncSage::PHASE_ERROR;
    }
    dpsi_x = atan2( imag(phase), real(phase) );
    while( dpsi_x < psi_x ) dpsi_x += M_TWOPI;
  
    phase = outState[i_y] / abs( outState[i_y] );
    outState[i_y] /= phase;
    outState[i_py] /= phase;
    if(   fabs( imag( outState[i_py] )*real( outState[i_y] ) - 
                resizeFactor*resizeFactor )
        > resizeFactor*resizeFactor*0.01 )
    {
  	(*pcout) << "*** ERROR *** Phase error in vertical plane at " 
             << (*it)->Type() << "  " << (*it)->Name()
             << endl;
  	(*pcout) << "*** ERROR *** imag( outState[i_py] ) = " 
             << imag( outState[i_py] ) 
             << endl;
  	(*pcout) << "*** ERROR *** " 
             << outState[i_py]/resizeFactor 
             << endl;

 	return LattFuncSage::PHASE_ERROR;
    }
    dpsi_y = atan2( imag(phase), real(phase) );
    while( dpsi_y < psi_y ) dpsi_y += M_TWOPI;
  

    // Calculate lattice functions ...
    beta_x  =   real( outState[i_x] )/resizeFactor;
    alpha_x = - real( outState[i_px] )/resizeFactor;
    alpha_x *= beta_x;
    beta_x  *= beta_x;
  
    beta_y  =   real( outState[i_y] )/resizeFactor;
    alpha_y = - real( outState[i_py] )/resizeFactor;
    alpha_y *= beta_y;
    beta_y  *= beta_y;

    if( ( 0 != strcmp( (*it)->Type(), "rbend"    ) ) && 
        ( 0 != strcmp( (*it)->Type(), "CF_rbend" ) ) && 
        ( 0 != strcmp( (*it)->Type(), "Slot"     ) ) )
    {
      psi_x = dpsi_x;
      psi_y = dpsi_y;
    } // ??? This is a real kludge!!
    


    // Attach the calculation to the element ...

    if(  Crit && !Crit( *it )  ) continue;   

    
    LattFuncSage::lattFunc info;
 
    info.arcLength = lng;
    info.beta.hor  = beta_x;
    info.beta.ver  = beta_y;
    info.alpha.hor = alpha_x;
    info.alpha.ver = alpha_y;
    info.psi.hor   = psi_x;
    info.psi.ver   = psi_y;

      
    BarnacleList::iterator itb = (*it)->dataHook.find("Twiss");

    if (  itb == (*it)->dataHook.end() ) 
    	(*it)->dataHook.insert( Barnacle( "Twiss", info ) );
      else
       	 itb->info = info;
    }

   // End loop on beamline elements ...


  // Finished....
  if( this->_verbose ) {
    (*pcout) << "LattFuncSage -- Leaving LattFuncSage::Fast_CS_Calc" << endl;
    (*pcout).flush();
  }


  return ret;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int LattFuncSage::Slow_CS_Calc( JetParticle const& jp, Sage::CRITFUNC Crit )
{

  // PRECONDITIONS: 
  // jp     is already on the closed orbit and its
  //        state is the one-turn map.
  //
  // *_myBeamlinePtr  is an uncoupled ring.
  // 
  // POSTCONDITIONS:
  // On every element filtered by Crit is attached a lattFunc
  // Barnacle.

  if( this->_verbose ) {
    (*pcout) << "LattFuncSage -- Entering LattFuncSage::Slow_CS_Calc" << endl;
    (*pcout).flush();
  }


  JetParticle jprt(jp);
  Particle    prt(jp);

  int i_x   =  jp.xIndex();
  int i_y   =  jp.yIndex();
  int i_z   =  jp.cdtIndex();
  int i_px  =  jp.npxIndex();
  int i_py  =  jp.npyIndex();
  int i_dpp =  jp.ndpIndex();

  int ret = 0;

  // .......... Check coupling ............................
  MatrixD mtrx = ( jp.State() ).Jacobian();
  
  if( ( mtrx( i_y,  i_x  ) != 0.0 )  ||
      ( mtrx( i_x,  i_y  ) != 0.0 )  ||
      ( mtrx( i_x,  i_py ) != 0.0 )  ||
      ( mtrx( i_y,  i_px ) != 0.0 )  ||
      ( mtrx( i_py, i_x  ) != 0.0 )  ||
      ( mtrx( i_px, i_y  ) != 0.0 )  ||
      ( mtrx( i_py, i_px ) != 0.0 )  ||
      ( mtrx( i_px, i_py ) != 0.0 )     )
  {
    (*pcerr) << "*** WARNING ***                                     \n"
            "*** WARNING *** LattFuncSage::Slow_CS_Calc          \n"
            "*** WARNING *** Coupling detected. Calculation is   \n"
            "*** WARNING *** proceeding but results are suspect. \n"
            "*** WARNING ***                                     \n"
            "*** WARNING *** Suggest you use EdwardsTeng         \n"
            "*** WARNING *** instead.                            \n"
            "*** WARNING ***                                     \n"
         << endl;
  }


  // Calculate initial lattice functions ...
  // ... first horizontal

  double cs = ( mtrx( i_x, i_x ) + mtrx( i_px, i_px ) )/2.0;

  if( fabs( cs ) > 1.0 ) 
  {
    (*pcerr) << "*** ERROR ***                                     \n"
            "*** ERROR *** LattFuncSage::Slow_CS_Calc          \n"
            "*** ERROR *** cos( psi_H ) = "
         << cs
         << "\n"
            "*** ERROR *** Lattice is unstable.                \n"
            "*** ERROR *** Cannot continue with calculation.   \n"
            "*** ERROR ***                                     \n"
         << endl;

    return LattFuncSage::UNSTABLE;
  };

  double sn = ( mtrx( i_x, i_px ) > 0.0 ) ? sqrt( 1.0 - cs*cs ) : - sqrt( 1.0 - cs*cs );
 
  if( sn == 0.0 ) {
    (*pcerr) << "*** ERROR ***                                     \n"
            "*** ERROR *** LattFuncSage::Slow_CS_Calc          \n"
            "*** ERROR *** Integer horizontal tune.            \n"
            "*** ERROR ***                                     \n"
         << endl;

    return LattFuncSage::INTEGER_TUNE;
  }

  double beta_x  = mtrx( i_x, i_px ) / sn;
  double alpha_x = ( mtrx( i_x, i_x ) - mtrx( i_px, i_px ) ) / ( 2.0*sn );


  // ... then vertical.

  cs = ( mtrx( i_y, i_y ) + mtrx( i_py, i_py ) )/2.0;
  if( fabs( cs ) <= 1.0 ) {
    if( mtrx( i_y, i_py ) > 0.0 )  sn =   sqrt( 1.0 - cs*cs );
    else                           sn = - sqrt( 1.0 - cs*cs );
  }
  else {
    (*pcerr) << "*** ERROR ***                                     \n"
            "*** ERROR *** LattFuncSage::Slow_CS_Calc          \n"
            "*** ERROR *** cos( psi_V ) = "
         << cs
         << "\n"
            "*** ERROR *** Lattice is unstable.                \n"
            "*** ERROR *** Cannot continue with calculation.   \n"
            "*** ERROR ***                                     \n"
         << endl;

    return LattFuncSage::UNSTABLE;
  }

  if( sn == 0.0 ) {
    (*pcerr) << "*** ERROR ***                                     \n"
            "*** ERROR *** LattFuncSage::Slow_CS_Calc          \n"
            "*** ERROR *** Integer vertical tune.              \n"
            "*** ERROR ***                                     \n"
         << endl;

    return LattFuncSage::INTEGER_TUNE;
  }

  double beta_y  = mtrx( i_y, i_py ) / sn;
  double alpha_y = ( mtrx( i_y, i_y ) - mtrx( i_py, i_py ) ) / ( 2.0*sn );


  // ++++++++++++++++++++++++++++++++++++++++++++++++++++
  // The following is more or less copied from the
  // old beamline::twiss, with some modifications
  // 

  
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

  jprt.setState( prt.State() );

  lfvec_.clear();

  bmlnElmnt* lbe = 0;
  for( beamline::deep_iterator it = _myBeamlinePtr->deep_begin(); it != _myBeamlinePtr->deep_end(); ++it) {

    lbe = (*it);
    lng += lbe->OrbitLength( prt );
    lbe -> propagate( jprt );

    mtrx = jprt.State().Jacobian();
    
    if( ( 0 != strcmp( lbe->Type(), "rbend"    ) ) && 
        ( 0 != strcmp( lbe->Type(), "CF_rbend" ) ) && 
        ( 0 != strcmp( lbe->Type(), "Slot"     ) ) )
    {
      tb = mtrx(0,0) * beta0H -  mtrx(0,3) * alpha0H;
      beta_x = ( tb * tb + mtrx(0,3) * mtrx(0,3))/beta0H;
    
      alpha_x = -1.0*(tb * (mtrx(3,0)*beta0H - mtrx(3,3)*alpha0H) +
    			     mtrx(0,3)*mtrx(3,3))/beta0H;
    
      t = atan2(mtrx(0,3),tb);
      while(t < oldpsiH) t += M_TWOPI;
      psi_x = oldpsiH = t;
    
      tb = mtrx(1,1) * beta0V -  mtrx(1,4) * alpha0V;
      beta_y = (tb * tb + mtrx(1,4) * mtrx(1,4))/beta0V;
    
      alpha_y = -1.0*(tb * (mtrx(4,1)*beta0V - mtrx(4,4)*alpha0V) +
    			     mtrx(1,4)*mtrx(4,4))/beta0V;
    
      t = atan2(mtrx(1,4),tb);
      while(t < oldpsiV) t += M_TWOPI;
      psi_y = oldpsiV = t;
    }
    
    else { // ??? This is a kludge.
      tb = mtrx(0,0) * beta0H -  mtrx(0,3) * alpha0H;
      beta_x = ( tb * tb + mtrx(0,3) * mtrx(0,3))/beta0H;
    
      alpha_x = -1.0*(tb * (mtrx(3,0)*beta0H - mtrx(3,3)*alpha0H) +
    			     mtrx(0,3)*mtrx(3,3))/beta0H;
    
      psi_x = oldpsiH;
    
      tb = mtrx(1,1) * beta0V -  mtrx(1,4) * alpha0V;
      beta_y = (tb * tb + mtrx(1,4) * mtrx(1,4))/beta0V;
    
      alpha_y = -1.0*(tb * (mtrx(4,1)*beta0V - mtrx(4,4)*alpha0V) +
    			     mtrx(1,4)*mtrx(4,4))/beta0V;
    
      psi_y = oldpsiV;
    }
    
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++

    // Attach the calculation to the element ...

    if( (Crit) && !Crit( lbe ) )  continue; 

    
    LattFuncSage::lattFunc info;

    info.arcLength = lng;
    info.beta.hor  = beta_x;
    info.beta.ver  = beta_y;
    info.alpha.hor = alpha_x;
    info.alpha.ver = alpha_y;
    info.psi.hor   = psi_x;
    info.psi.ver   = psi_y;
      
    BarnacleList::iterator it;
    if( (it = lbe->dataHook.find("Twiss") )==  lbe->dataHook.end() ) {
    	lbe->dataHook.insert( Barnacle( "Twiss", info) );
    }
      else {
   	it->info = info;
    }

  }  // End loop on lbe ...


  // Finished....

  if( this->_verbose ) {
    (*pcout) << "LattFuncSage -- Leaving LattFuncSage::Slow_CS_Calc" << endl;
    (*pcout).flush();
  }

  return ret;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int LattFuncSage::NewSlow_CS_Calc(  JetParticle const& jp, Sage::CRITFUNC Crit )
{
  // PRECONDITIONS: 
  //    jp  is already on the closed orbit and its
  //        state is the one-turn map.
  //
  // *_myBeamlinePtr  is an uncoupled ring.
  // 
  // POSTCONDITIONS:
  // On every element filtered by Crit is attached a lattFunc
  // Barnacle.

  if( this->_verbose ) {
    (*pcout) << "LattFuncSage -- Entering LattFuncSage::Slow_CS_Calc" << endl;
    (*pcout).flush();
  }

  // Preserve the current Jet environment
  Jet__environment_ptr  storedEnv  = Jet__environment::getLastEnv();
  JetC__environment_ptr storedEnvC = JetC__environment::getLastEnv();

  // Reset current environment
  Jet__environment::setLastEnv(  jp.State().Env() );
  JetC__environment::setLastEnv( Jet__environment::getLastEnv() ); // implicit conversion 

  MatrixD mtrx;

  JetParticle  jprt(jp);
  Particle     prt(jp);

  int i_x   =  jp.xIndex();
  int i_y   =  jp.yIndex();
  int i_z   =  jp.cdtIndex();
  int i_px  =  jp.npxIndex();
  int i_py  =  jp.npyIndex();
  int i_dpp =  jp.ndpIndex();

  int ret = 0;
  int i   = 0;
  double cs = 0.0;
  double sn = 0.0;

  // .......... Check coupling ............................
  mtrx = ( jp.State() ).Jacobian();
  
  if( ( mtrx( i_y,  i_x  ) != 0.0 )  ||
      ( mtrx( i_x,  i_y  ) != 0.0 )  ||
      ( mtrx( i_x,  i_py ) != 0.0 )  ||
      ( mtrx( i_y,  i_px ) != 0.0 )  ||
      ( mtrx( i_py, i_x  ) != 0.0 )  ||
      ( mtrx( i_px, i_y  ) != 0.0 )  ||
      ( mtrx( i_py, i_px ) != 0.0 )  ||
      ( mtrx( i_px, i_py ) != 0.0 )     )
  {
    (*pcerr) << "*** WARNING ***                                     \n"
            "*** WARNING *** LattFuncSage::Slow_CS_Calc          \n"
            "*** WARNING *** Coupling detected. Calculation is   \n"
            "*** WARNING *** proceeding but results are suspect. \n"
            "*** WARNING ***                                     \n"
            "*** WARNING *** Suggest you use EdwardsTeng         \n"
            "*** WARNING *** instead.                            \n"
            "*** WARNING ***                                     \n"
         << endl;
  }


  // Calculate initial lattice functions ...
  // ... first horizontal
  double alpha_x, beta_x;
  cs = ( mtrx( i_x, i_x ) + mtrx( i_px, i_px ) )/2.0;
  if( fabs( cs ) <= 1.0 ) {
    if( mtrx( i_x, i_px ) > 0.0 )  sn =   sqrt( 1.0 - cs*cs );
    else                           sn = - sqrt( 1.0 - cs*cs );
  }
  else {
    (*pcerr) << "*** ERROR ***                                     \n"
            "*** ERROR *** LattFuncSage::Slow_CS_Calc          \n"
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

  if( sn == 0.0 ) {
    (*pcerr) << "*** ERROR ***                                     \n"
            "*** ERROR *** LattFuncSage::Slow_CS_Calc          \n"
            "*** ERROR *** Integer horizontal tune.            \n"
            "*** ERROR ***                                     \n"
         << endl;

    Jet__environment::setLastEnv( storedEnv );
    JetC__environment::setLastEnv( storedEnvC );
    return LattFuncSage::INTEGER_TUNE;
  }

  beta_x  = mtrx( i_x, i_px ) / sn;
  alpha_x = ( mtrx( i_x, i_x ) - mtrx( i_px, i_px ) ) / ( 2.0*sn );


  // ... then vertical.
  double alpha_y, beta_y;
  cs = ( mtrx( i_y, i_y ) + mtrx( i_py, i_py ) )/2.0;
  if( fabs( cs ) <= 1.0 ) {
    if( mtrx( i_y, i_py ) > 0.0 )  sn =   sqrt( 1.0 - cs*cs );
    else                           sn = - sqrt( 1.0 - cs*cs );
  }
  else {
    (*pcerr) << "*** ERROR ***                                     \n"
            "*** ERROR *** LattFuncSage::Slow_CS_Calc          \n"
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
            "*** ERROR *** LattFuncSage::Slow_CS_Calc          \n"
            "*** ERROR *** Integer vertical tune.              \n"
            "*** ERROR ***                                     \n"
         << endl;


    Jet__environment::setLastEnv( storedEnv );
    JetC__environment::setLastEnv( storedEnvC );
    return LattFuncSage::INTEGER_TUNE;
  }

  beta_y  = mtrx( i_y, i_py ) / sn;
  alpha_y = ( mtrx( i_y, i_y ) - mtrx( i_py, i_py ) ) / ( 2.0*sn );


  // ++++++++++++++++++++++++++++++++++++++++++++++++++++
  // The following is more or less copied from the
  // old beamline::twiss, with some modifications
  // 

  
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

  jprt.setState( prt.State() );

  lfvec_.clear();

  bmlnElmnt* lbe = 0;
  for( beamline::deep_iterator it = _myBeamlinePtr->deep_begin(); it != _myBeamlinePtr->deep_end(); ++it) 
  {
    lbe = (*it);

    lng += lbe->OrbitLength( prt );
    lbe -> propagate( jprt );

    mtrx = jprt.State().Jacobian();
    
    if( ( typeid(*lbe) != typeid(rbend)    ) && 
        ( typeid(*lbe) != typeid(CF_rbend) ) && 
        ( typeid(*lbe) != typeid(Slot)     )   )
    {
      tb = mtrx(0,0) * beta0H -  mtrx(0,3) * alpha0H;
      beta_x = ( tb * tb + mtrx(0,3) * mtrx(0,3))/beta0H;
    
      alpha_x = -1.0*(tb * (mtrx(3,0)*beta0H - mtrx(3,3)*alpha0H) +
    			     mtrx(0,3)*mtrx(3,3))/beta0H;
    
      t = atan2(mtrx(0,3),tb);
      while(t < oldpsiH) t += M_TWOPI;
      psi_x = oldpsiH = t;
    
      tb = mtrx(1,1) * beta0V -  mtrx(1,4) * alpha0V;
      beta_y = (tb * tb + mtrx(1,4) * mtrx(1,4))/beta0V;
    
      alpha_y = -1.0*(tb * (mtrx(4,1)*beta0V - mtrx(4,4)*alpha0V) +
    			     mtrx(1,4)*mtrx(4,4))/beta0V;
    
      t = atan2(mtrx(1,4),tb);
      while(t < oldpsiV) t += M_TWOPI;
      psi_y = oldpsiV = t;
    }
    
    else { // ??? This is a kludge.
      tb = mtrx(0,0) * beta0H -  mtrx(0,3) * alpha0H;
      beta_x = ( tb * tb + mtrx(0,3) * mtrx(0,3))/beta0H;
    
      alpha_x = -1.0*(tb * (mtrx(3,0)*beta0H - mtrx(3,3)*alpha0H) +
    			     mtrx(0,3)*mtrx(3,3))/beta0H;
    
      psi_x = oldpsiH;
    
      tb = mtrx(1,1) * beta0V -  mtrx(1,4) * alpha0V;
      beta_y = (tb * tb + mtrx(1,4) * mtrx(1,4))/beta0V;
    
      alpha_y = -1.0*(tb * (mtrx(4,1)*beta0V - mtrx(4,4)*alpha0V) +
    			     mtrx(1,4)*mtrx(4,4))/beta0V;
    
      psi_y = oldpsiV;
    }
    
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++

    // Store the calculation if appropriate ...
    if( ( !Crit ) || ( Crit( lbe ) ) ) 
    {
      lattFunc lf;

      lf.arcLength = lng;
      lf.beta.hor  = beta_x;
      lf.beta.ver  = beta_y;
      lf.alpha.hor = alpha_x;
      lf.alpha.ver = alpha_y;
      lf.psi.hor   = psi_x;
      lf.psi.ver   = psi_y;

      lfvec_.push_back(lf);
    }
  }  // End loop on lbe ...


  // Finished....
  if( this->_verbose ) {
    (*pcout) << "LattFuncSage -- Leaving LattFuncSage::Slow_CS_Calc" << endl;
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

  if( this->_verbose ) {
    (*pcout) << "LattFuncSage -- Entering LattFuncSage::TuneCalc" << endl;
    (*pcout).flush();
  }

  MatrixD mtrx;
  MatrixC lambda;
  MatrixD M(2,2);
  double  snH, csH, snV, csV;

  int i_x   =  jp.xIndex();
  int i_y   =  jp.yIndex();
  int i_z   =  jp.cdtIndex();
  int i_px  =  jp.npxIndex();
  int i_py  =  jp.npyIndex();
  int i_dpp =  jp.ndpIndex();

  int ret = 0;


  // Check for closed orbit ...
  ClosedOrbitSage clsg( this->_myBeamlinePtr );

  if( forceClosedOrbitCalc ) { clsg.setForcedCalc(); }

  if( this->_verbose ) clsg.set_verbose();
  else                 clsg.unset_verbose();  // Unnecessary line.


  if( ( ret = clsg.findClosedOrbit( jp ) ) == 0 )
  {
    if( this->_verbose ) {
      (*pcout) << "LattFuncSage -- Closed orbit successfully calculated." << endl;
      (*pcout).flush();
    }
  }
  else
  {
    if( this->_verbose ) {
      (*pcout) << "LattFuncSage -- Closed orbit not successfully calculated." << endl;
      (*pcout).flush();
    }
  }


  // :::::::::::::::::::::::::::::::::::::::::::::::::::

  if( ret == 0 )
  {
    // .......... Calculating tunes .........................
    // .......... (lifted from EdwardsTeng) .................
    mtrx = ( jp.State() ).Jacobian();

    if( ( mtrx( i_y,  i_x  ) != 0.0 )  ||
      	( mtrx( i_x,  i_y  ) != 0.0 )  ||
      	( mtrx( i_x,  i_py ) != 0.0 )  ||
      	( mtrx( i_y,  i_px ) != 0.0 )  ||
      	( mtrx( i_py, i_x  ) != 0.0 )  ||
      	( mtrx( i_px, i_y  ) != 0.0 )  ||
      	( mtrx( i_py, i_px ) != 0.0 )  ||
      	( mtrx( i_px, i_py ) != 0.0 )     )
    {
      (*pcerr) << "*** WARNING ***                                     \n"
              "*** WARNING *** LattFuncSage::tuneCalc              \n"
              "*** WARNING *** Coupling detected. Calculation is   \n"
              "*** WARNING *** proceeding but results are suspect. \n"
              "*** WARNING ***                                     \n"
              "*** WARNING *** Suggest you use EdwardsTeng         \n"
              "*** WARNING *** instead.                            \n"
              "*** WARNING ***                                     \n"
           << endl;
    }


    // Calculation in horizontal plane
    M( 0, 0 ) = mtrx( i_x,  i_x  );
    M( 0, 1 ) = mtrx( i_x,  i_px );
    M( 1, 0 ) = mtrx( i_px, i_x  );
    M( 1, 1 ) = mtrx( i_px, i_px );

    lambda = M.eigenValues();
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
      ret = 10;
    }
  }

  // :::::::::::::::::::::::::::::::::::::::::::::::::::

  if( ret == 0 ) {
    if( ( abs( lambda(0) - conj( lambda(1) ) ) > 1.0e-4 ) )
    {
  	(*pcout) << "\n"
  	     << "*** ERROR *** LattFuncSage::TuneCalc               \n"
  	     << "*** ERROR *** Conjugacy condition has been violated\n"
  	     << "*** ERROR *** The lattice may be linearly unstable.\n"
  	     << "*** ERROR *** Eigenvalues =                        \n"
  	     << "*** ERROR *** " << lambda << endl;
  	ret = 11;
    }
  }

  // :::::::::::::::::::::::::::::::::::::::::::::::::::

  if( ret == 0 ) {
    csH = real( lambda(0) );
    snH = sqrt( 1.0 - csH*csH );
    if( M(0,1) < 0.0 ) snH = - snH;
    

    // Calculation in vertical plane
    M( 0, 0 ) = mtrx( i_y,  i_y  );
    M( 0, 1 ) = mtrx( i_y,  i_py );
    M( 1, 0 ) = mtrx( i_py, i_y  );
    M( 1, 1 ) = mtrx( i_py, i_py );

    lambda = M.eigenValues();
    if( fabs( abs(lambda(0)) - 1.0 ) > 1.0e-4 ) {
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
      ret = 12;
    }
  }

  // :::::::::::::::::::::::::::::::::::::::::::::::::::

  if( ret == 0 ) {
    if( ( abs( lambda(0) - conj( lambda(1) ) ) > 1.0e-4 ) )
    {
  	(*pcout) << "\n"
  	     << "*** ERROR *** LattFuncSage::TuneCalc               \n"
  	     << "*** ERROR *** Conjugacy condition has been violated\n"
  	     << "*** ERROR *** The lattice may be linearly unstable.\n"
  	     << "*** ERROR *** Eigenvalues =                        \n"
  	     << "*** ERROR *** " << lambda << endl;
  	ret = 13;
    }
  }

  // :::::::::::::::::::::::::::::::::::::::::::::::::::

  if( ret == 0 ) {
    csV = real( lambda(0) );
    snV = sqrt( 1.0 - csV*csV );
    if( M(0,1) < 0.0 ) snV = - snV;
    
    // Attach data to the beamline

    LattFuncSage::tunes  lftunes;

    double t = atan2( snH, csH );
    if( t < 0.0 )   t += M_TWOPI;
    lftunes.hor = ( t / M_TWOPI );
    t = atan2( snV, csV );
    if( t < 0.0 )   t += M_TWOPI;
    lftunes.ver = ( t / M_TWOPI );

    _myBeamlinePtr->dataHook.eraseAll( "Tunes" );
    _myBeamlinePtr->dataHook.append( Barnacle( "Tunes", lftunes ) );
  }

  // :::::::::::::::::::::::::::::::::::::::::::::::::::

  // Final operations ....................................
  if( this->_verbose ) {
    (*pcout) << "LattFuncSage -- Leaving LattFuncSage::TuneCalc" << endl;
    (*pcout).flush();
  }

  return ret;
}



//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int LattFuncSage::Disp_Calc( JetParticle const& jparg, Sage::CRITFUNC  Crit )
{

  if( _verbose ) {
    (*pcout) << "LattFuncSage -- Entering LattFuncSage::Disp_Calc" << endl;
    (*pcout).flush();
  }

  JetParticle jp(jparg);

  int ret = 0;

  int i_x   =  jp.xIndex();
  int i_y   =  jp.yIndex();
  int i_z   =  jp.cdtIndex();
  int i_px  =  jp.npxIndex();
  int i_py  =  jp.npyIndex();
  int i_dpp =  jp.ndpIndex();

  double lng = 0.0;


  // Preliminary steps ...
  beamline* bml = _myBeamlinePtr;

  ClosedOrbitSage clsg( _myBeamlinePtr );
  if( this->_verbose ) {
    clsg.set_verbose();
  }

  clsg.setForcedCalc();
  ret = clsg.findClosedOrbit( jp );
  clsg.unsetForcedCalc();

  if( ret == 0 ) {
    if( this->_verbose ) {
      (*pcout) << "LattFuncSage -- Closed orbit successfully calculated." << endl;
      (*pcout).flush();
    }
  }
  else {
    if( this->_verbose ) {
      (*pcout) << "LattFuncSage -- Closed orbit not successfully calculated." << endl;
      (*pcout) << "LattFuncSage -- Leaving LattFuncSage::Disp_Calc" << endl;
      (*pcout).flush();
    }

    return ret;
  }

  Particle firstParticle(jp);
  MatrixD firstJacobian  = jp.State().Jacobian();


  // Calculate the closed orbit for an off-momentum particle ...
  if( this->_verbose ) {
    (*pcout) << "LattFuncSage --- Starting calculation of offset closed orbit." << endl;
    (*pcout).flush();
  }


  double dpp      = get_dpp();
  double energy   = firstParticle.ReferenceEnergy();
  double mass     = firstParticle.Mass();
  double momentum = sqrt( energy*energy - mass*mass )*( 1.0 + dpp );
         energy   = sqrt( momentum*momentum + mass*mass );

  jp.SetReferenceEnergy( energy );

  clsg.setForcedCalc();
  ret = clsg.findClosedOrbit( jp );
  clsg.unsetForcedCalc();

  if( ret == 0 ) {
    if( this->_verbose ) {
      (*pcout) << "LattFuncSage -- Offset closed orbit successfully calculated." << endl;
      (*pcout).flush();
    }
  }
  else {
    if( this->_verbose ) {
      (*pcout) << "LattFuncSage -- Off-momentum closed orbit not successfully calculated." << endl;
      (*pcout) << "LattFuncSage -- Leaving LattFuncSage::Disp_Calc" << endl;
      (*pcout).flush();
    }
    return ret;
  }

  Particle secondParticle(jp);
  MatrixD secondJacobian  = jp.State().Jacobian();


  // Attach initial dispersion data to the beamline ...

  Vector d = ( secondParticle.State()  -  firstParticle.State() ) / dpp;

  LattFuncSage::lattFunc lf;
 
  lf.dispersion.hor = d( i_x  );
  lf.dPrime.hor     = d( i_px );
  lf.dispersion.ver = d( i_y  );
  lf.dPrime.ver     = d( i_py );
  lf.arcLength      = lng;
  
  bml->dataHook.eraseAll( "Dispersion" );
  bml->dataHook.insert( Barnacle( "Dispersion", lf ) );


  // Attach dispersion data wherever desired ...

  if( this->_verbose ) {
    (*pcout) << "LattFuncSage --- Attaching dispersion data to the elements." << endl;
    (*pcout).flush();
  }


  for( beamline::deep_iterator it = bml->deep_begin();  it != bml->deep_end(); ++it) {

    (*it)->propagate( firstParticle );
    (*it)->propagate( secondParticle );

    lng += (*it)->OrbitLength( firstParticle );

    if( Crit ) { if ( !Crit(*it) )  continue; } 
    
    d = ( secondParticle.State()  -  firstParticle.State() ) / dpp;
  
    lf.dispersion.hor = d( i_x  );
    lf.dPrime.hor     = d( i_px );
    lf.dispersion.ver = d( i_y  );
    lf.dPrime.ver     = d( i_py );
    lf.arcLength      = lng;

    BarnacleList::iterator itb = (*it)->dataHook.find("Twiss");

    if(  itb == (*it)->dataHook.end() ) {
        (*it)->dataHook.eraseAll( "Dispersion" );
        (*it)->dataHook.insert( Barnacle( "Dispersion", lf ) );
    }
    else {
        boost::any_cast<lattFunc&>( itb->info ).dispersion.hor = lf.dispersion.hor;
        boost::any_cast<lattFunc&>( itb->info ).dPrime.hor     = lf.dPrime.hor;
        boost::any_cast<lattFunc&>( itb->info ).dispersion.ver = lf.dispersion.ver;
        boost::any_cast<lattFunc&>( itb->info ).dPrime.ver     = lf.dPrime.ver;
    }
  }  

  // Attach tune and chromaticity to the beamline ........

  LattFuncSage::lattRing latticeRing;

  Vector    firstNu(2), secondNu(2);

  if( ( 0 == filterTransverseTunes( firstJacobian, firstNu   ) ) && 
      ( 0 == filterTransverseTunes( secondJacobian, secondNu ) ) )
  {
    latticeRing.tune.hor = firstNu(0);
    latticeRing.tune.ver = firstNu(1);
    latticeRing.chromaticity.hor = ( secondNu(0) - firstNu(0) ) / dpp;
    latticeRing.chromaticity.ver = ( secondNu(1) - firstNu(1) ) / dpp;

    _myBeamlinePtr->dataHook.eraseAll( "Ring" );
    _myBeamlinePtr->dataHook.insert( Barnacle( "Ring", latticeRing ) );
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


  // Final operations ....................................

  if( this->_verbose ) {
    (*pcout) << "LattFuncSage -- Leaving LattFuncSage::Disp_Calc" << endl;
    (*pcout).flush();
  }

  return ret;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int LattFuncSage::NewDisp_Calc( JetParticle const& arg_jp,  bool onClosedOrbit )
{
  if( this->_verbose ) {
    (*pcout) << "LattFuncSage -- Entering LattFuncSage::Disp_Calc" << endl;
    (*pcout).flush();
  }

  JetParticle jpart(arg_jp);

  int ret = 0;

  MatrixD   firstJacobian;
  MatrixD   secondJacobian;

  int i_x   =  arg_jp.xIndex();
  int i_y   =  arg_jp.yIndex();
  int i_z   =  arg_jp.cdtIndex();
  int i_px  =  arg_jp.npxIndex();
  int i_py  =  arg_jp.npyIndex();
  int i_dpp =  arg_jp.ndpIndex();

  double dpp;
  double energy;
  double mass;
  double momentum;
  double lng = 0.0;


  // Preliminary steps ...
  beamline* bml = this->_myBeamlinePtr;

  ClosedOrbitSage clsg( this->_myBeamlinePtr );
  if( this->_verbose ) {
    clsg.set_verbose();
  }

  if( !onClosedOrbit ) { 
    clsg.setForcedCalc(); 
    ret = clsg.findClosedOrbit( jpart );
    clsg.unsetForcedCalc();
  }

  if( ret == 0 ) {
    if( this->_verbose ) {
      (*pcout) << "LattFuncSage -- Closed orbit successfully calculated." << endl;
      (*pcout).flush();
    }
  }

  else {
    if( this->_verbose ) {
      (*pcout) << "LattFuncSage -- Closed orbit not successfully calculated." << endl;
      (*pcout) << "LattFuncSage -- Leaving LattFuncSage::Disp_Calc" << endl;
      (*pcout).flush();
    }
    return ret;
  }

  Particle firstParticle(jpart);
  firstJacobian  = jpart.State().Jacobian();


  // Calculate the closed orbit for an off-momentum particle ...
  if( this->_verbose ) {
    (*pcout) << "LattFuncSage --- Starting calculation of offset closed orbit." << endl;
    (*pcout).flush();
  }

  dpp = this->get_dpp();
  energy = firstParticle.ReferenceEnergy();
  mass = firstParticle.Mass();
  momentum = sqrt( energy*energy - mass*mass )*( 1.0 + dpp );
  energy = sqrt( momentum*momentum + mass*mass );

  jpart.SetReferenceEnergy( energy );

  clsg.setForcedCalc();
  ret = clsg.findClosedOrbit( jpart );
  clsg.unsetForcedCalc();

  if( ret == 0 ) {
    if( this->_verbose ) {
      (*pcout) << "LattFuncSage -- Offset closed orbit successfully calculated." << endl;
      (*pcout).flush();
    }
  }
  else {
    if( this->_verbose ) {
      (*pcout) << "LattFuncSage -- Off-momentum closed orbit not successfully calculated." << endl;
      (*pcout) << "LattFuncSage -- Leaving LattFuncSage::Disp_Calc" << endl;
      (*pcout).flush();
    }
    return ret;
  }

  Particle secondParticle(jpart);
  secondJacobian  = jpart.State().Jacobian();


  // Attach dispersion data wherever desired ...
  if( this->_verbose ) {
    (*pcout) << "LattFuncSage --- Attaching dispersion data to the elements." << endl;
    (*pcout).flush();
  }

  // Attach initial dispersion data to the beamline ...

  Vector d( firstParticle.State().Dim() );

  bool stand_alone_disp_calc = ( lfvec_.size() == 0 );   

  std::vector<lattFunc>::iterator lf_it =  lfvec_.begin();

  for( beamline::deep_iterator it = _myBeamlinePtr->deep_begin(); it != _myBeamlinePtr->deep_end(); ++it ) {

    (*it)->propagate( firstParticle );
    (*it)->propagate( secondParticle );

    lng += (*it)->OrbitLength( firstParticle );

    d = ( secondParticle.State()  -  firstParticle.State() ) / dpp;

    if ( stand_alone_disp_calc ) { 
       lattFunc lf;
       lf.dispersion.hor = d( i_x  );
       lf.dPrime.hor     = d( i_px );
       lf.dispersion.ver = d( i_y  );
       lf.dPrime.ver     = d( i_py );
       lf.arcLength        = lng;
       lfvec_.push_back(lf);
    } 
    else {
       lf_it->dispersion.hor = d( i_x  );
       lf_it->dPrime.hor     = d( i_px );
       lf_it->dispersion.ver = d( i_y  );
       lf_it->dPrime.ver     = d( i_py );
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




  if( this->_verbose ) {
    (*pcout) << "LattFuncSage -- Leaving LattFuncSage::Disp_Calc" << endl;
    (*pcout).flush();
  }

  return ret;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int LattFuncSage::FAD_Disp_Calc( JetParticle const& arg_jp, Sage::CRITFUNC Crit )
{
  // NOTE: This routine does not calculate chromaticity.
  // 

  if( this->_verbose ) {
    (*pcout) << "LattFuncSage -- Entering LattFuncSage::FAD_Disp_Calc( map ) " 
         << endl;
    (*pcout).flush();
  }

  int ret = 0;

  int i_x   =  arg_jp.xIndex();
  int i_y   =  arg_jp.yIndex();
  int i_z   =  arg_jp.cdtIndex();
  int i_px  =  arg_jp.npxIndex();
  int i_py  =  arg_jp.npyIndex();
  int i_dpp =  arg_jp.ndpIndex();

  int j_x   =  0;
  int j_px  =  1;
  int j_y   =  2;
  int j_py  =  3;
  int j_dpp =  4;

  beamline* bml = this->_myBeamlinePtr;
  MatrixD FADmap( 5, 5 );
  MatrixD firstJacobian;

  // Calculate the 5x5 matrix ...
  JetParticle jp(arg_jp);
  firstJacobian  = jp.State().Jacobian();

  FADmap( j_x,   j_x   ) = firstJacobian( i_x,   i_x   );
  FADmap( j_x,   j_px  ) = firstJacobian( i_x,   i_px  );
  FADmap( j_x,   j_y   ) = firstJacobian( i_x,   i_y   );
  FADmap( j_x,   j_py  ) = firstJacobian( i_x,   i_py  );
  FADmap( j_x,   j_dpp ) = firstJacobian( i_x,   i_dpp );

  FADmap( j_px,  j_x   ) = firstJacobian( i_px,  i_x   );
  FADmap( j_px,  j_px  ) = firstJacobian( i_px,  i_px  );
  FADmap( j_px,  j_y   ) = firstJacobian( i_px,  i_y   );
  FADmap( j_px,  j_py  ) = firstJacobian( i_px,  i_py  );
  FADmap( j_px,  j_dpp ) = firstJacobian( i_px,  i_dpp );

  FADmap( j_y,   j_x   ) = firstJacobian( i_y,   i_x   );
  FADmap( j_y,   j_px  ) = firstJacobian( i_y,   i_px  );
  FADmap( j_y,   j_y   ) = firstJacobian( i_y,   i_y   );
  FADmap( j_y,   j_py  ) = firstJacobian( i_y,   i_py  );
  FADmap( j_y,   j_dpp ) = firstJacobian( i_y,   i_dpp );

  FADmap( j_py,  j_x   ) = firstJacobian( i_py,  i_x   );
  FADmap( j_py,  j_px  ) = firstJacobian( i_py,  i_px  );
  FADmap( j_py,  j_y   ) = firstJacobian( i_py,  i_y   );
  FADmap( j_py,  j_py  ) = firstJacobian( i_py,  i_py  );
  FADmap( j_py,  j_dpp ) = firstJacobian( i_py,  i_dpp );

  FADmap( j_dpp, j_x   ) = 0.0;
  FADmap( j_dpp, j_px  ) = 0.0;
  FADmap( j_dpp, j_y   ) = 0.0;
  FADmap( j_dpp, j_py  ) = 0.0;
  FADmap( j_dpp, j_dpp ) = 1.0;

  // Finds its eigenvectors....
  int i, j;
  MatrixC EV;
  MatrixC lambda;
  EV = FADmap.eigenVectors();
  lambda = FADmap.eigenValues();   // Wildly inefficient. Does the same
                                   // calculation twice. But this is only
                                   // a 5x5, so it shouldn't matter much.
  
  int numberFound = 0;
  const std::complex<double>  c_one( 1.0, 0.0 );
  int theColumn;

  for( i = 0; i < 5; i++ ) {
    if( abs( EV(j_dpp,i) ) > 1.0e-6 ) {
      numberFound++;
      theColumn = i;
      if( abs( EV(j_dpp,i) - c_one ) > 1.0e-6 ) {
        for( j = 0; j < 5; j++ ) {
          EV(j,i) = EV(j,i) / EV(j_dpp,i);
        }
      }
    }
  }

  if( numberFound != 1 ) {
    (*pcerr) << "*** ERROR ***                                            \n"
            "*** ERROR *** LattFuncSage::FAD_Disp_Calc                \n"
            "*** ERROR *** Exactly " << numberFound << " eigenvectors \n"
            "*** ERROR *** were found matching the criterion.         \n"
            "*** ERROR *** Results of this calculation will not be    \n"
            "*** ERROR *** correct.                                   \n"
            "*** ERROR ***                                            \n"
         << endl;
    ret = LattFuncSage::TOO_MANY_VECTORS;
  }

  else { // Continue

     double lng = 0.0;

     // Attach initial dispersion data to the beamline ...
     LattFuncSage::lattFunc lf;

     lf.dispersion.hor = real( EV( j_x,  theColumn ) );
     lf.dPrime.hor     = real( EV( j_px, theColumn ) );
     lf.dispersion.ver = real( EV( j_y,  theColumn ) );
     lf.dPrime.ver     = real( EV( j_py, theColumn ) );
     lf.arcLength      = lng;
     
     bml->dataHook.eraseAll( "Dispersion" );
     bml->dataHook.insert( Barnacle( "Dispersion", lf ) );
   
     // Attach dispersion data wherever desired ...
     if( this->_verbose ) {
       (*pcout) << "LattFuncSage --- Attaching dispersion data to the elements." << endl;
       (*pcout).flush();
     }
   
   
 
     Particle firstParticle(jp);
     double dpp = get_dpp();

     firstParticle.set_x  ( dpp*real( EV( j_x,  theColumn ) ) );
     firstParticle.set_y  ( dpp*real( EV( j_y,  theColumn ) ) );
     firstParticle.set_cdt( 0.0 );
     firstParticle.set_npx( dpp*real( EV( j_px, theColumn ) ) );
     firstParticle.set_npy( dpp*real( EV( j_py, theColumn ) ) );
     firstParticle.set_ndp( 0.0 );

     firstParticle.SetReferenceMomentum( ( 1.0 + dpp )*(firstParticle.ReferenceMomentum()) );

    for( beamline::deep_iterator it = _myBeamlinePtr->deep_begin(); it != _myBeamlinePtr->deep_end(); ++it) 
    {
       (*it)->propagate( firstParticle );
       lng += (*it)->OrbitLength( firstParticle );
   
       if( !Crit || Crit( (*it) ) ) 
       {
          LattFuncSage::lattFunc lf;
         lf.dispersion.hor = firstParticle.get_x()   /dpp;
         lf.dPrime.hor     = firstParticle.get_npx() /dpp;
         lf.dispersion.ver = firstParticle.get_y()   /dpp;
         lf.dPrime.ver     = firstParticle.get_npy() /dpp;
         lf.arcLength      = lng;
     
         (*it)->dataHook.eraseAll( "Dispersion" );
         (*it)->dataHook.insert( Barnacle( "Dispersion", lf ) );
       }    
     }  
   

     // Attach tune and chromaticity to the beamline ........

     LattFuncSage::lattRing latticeRing;

     Vector firstNu(2);
     if( ( 0 == filterTransverseTunes( firstJacobian, firstNu   ) ) )
     {
       latticeRing.tune.hor = firstNu(0);
       latticeRing.tune.ver = firstNu(1);
       latticeRing.chromaticity.hor = 12345.6789;  // Nonsense value.
       latticeRing.chromaticity.ver = 12345.6789;
       _myBeamlinePtr->dataHook.eraseAll( "Ring" );
       _myBeamlinePtr->dataHook.insert( Barnacle( "Ring", latticeRing ) );
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
     }  

  } // else continue

  // Final operations ....................................

  if( this->_verbose ) {
    (*pcout) << "LattFuncSage -- Leaving LattFuncSage::Disp_Calc" << endl;
    (*pcout).flush();
  }

  return ret;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


int LattFuncSage::Twiss_Calc ( JetParticle& p )
{
 // This code is both obsolete and wrong.
 // It encapsulates the old beamline::twiss

 // .... Check to see if this was done already.

 if ( _myBeamlinePtr->twissIsDone() ) return LattFuncSage::DONE;

 double  t, oldpsiH, oldpsiV, lng;

 LattFuncSage::lattFunc     latticeFunctions;
 LattFuncSage::lattRing     latticeRing;


   // .......... Propagate a JetParticle to get transfer matrix

   p.setState( Vector(BMLN_dynDim) ); // set initial state to zero
   _myBeamlinePtr->propagate( p );

   MatrixD mtrx =  p.getState().Jacobian();

   // .......... Calculating tunes .........................

   double csH = ( mtrx(0,0) + mtrx(3,3) ) / 2.0;  // cosine horizontal tune
   double csV = ( mtrx(1,1) + mtrx(4,4) ) / 2.0;  // cosine vertical   tune

   if( fabs(csH) > 1.0 || fabs(csV) > 1.0 ) {
     (*pcerr) << "\n*** WARNING *** " << endl ;
     (*pcerr) << "*** WARNING *** beamline::twiss()  Lattice is unstable."   << endl;
     (*pcerr) << "*** WARNING *** beamline::twiss()  csH = " << csH
          <<                                 "   csV = " << csV          << endl;
     (*pcerr) << "*** WARNING *** beamline::twiss()  did not exit properly." << endl;
     (*pcerr) << "*** WARNING *** " << endl;

     return LattFuncSage::UNSTABLE;
   }

   double snH = sqrt( -1.0* mtrx(0,3)*mtrx(3,0) - 
               (mtrx(0,0) - mtrx(3,3))*
              (mtrx(0,0) - mtrx(3,3))/4.0);
   double snV = sqrt( -1.0*mtrx(1,4) * mtrx(4,1) - 
               (mtrx(1,1) - mtrx(4,4))*
              (mtrx(1,1) - mtrx(4,4))/4.0);

   if( mtrx(0,3) < 0.0 ) snH *= -1.0;       // ?? Is there a better way of
   if( mtrx(1,4) < 0.0 ) snV *= -1.0;       // ?? changing the sign?

   t = asin( snH );
   if( csH < 0.0 ) t  = M_PI - t;              // 0 < t < 2 pi
   if( t < 0.0 )   t += M_TWOPI;
   latticeRing.tune.hor = ( t / M_TWOPI );

   t = asin( snV );
   if( csV < 0.0 ) t  = M_PI - t;              // 0 < t < 2 pi
   if( t < 0.0 )   t += M_TWOPI;
   latticeRing.tune.ver = t / M_TWOPI;

//   t = atan2(snH,csH);
//   latticeRing->tune.hor = ( t / M_TWOPI );

//   t = atan2(snV,csV);
//   latticeRing->tune.ver = ( t / M_TWOPI );


   // .......... Calculating betas and alphas ..............

   latticeFunctions.beta .hor = mtrx(0,3) / snH;
   latticeFunctions.beta .ver = mtrx(1,4) / snV;
   latticeFunctions.alpha.hor = ( mtrx(0,0) - mtrx(3,3) ) / (2.0*snH);
   latticeFunctions.alpha.ver = ( mtrx(1,1) - mtrx(4,4) ) / (2.0 * snV);
   latticeFunctions.psi  .hor = 0.0;
   latticeFunctions.psi  .ver = 0.0;

// calculate dispersion

   MatrixD A(4,4,0.0);
   MatrixD Disp(4,1,0.0);
   MatrixD Long(4,1,0.0);
  
   A(0,0) = mtrx(0,0);
   A(0,1) = mtrx(0,3);
   A(0,2) = mtrx(0,1);
   A(0,3) = mtrx(0,4);
   A(1,0) = mtrx(3,0);
   A(1,1) = mtrx(3,3);
   A(1,2) = mtrx(3,1);
   A(1,3) = mtrx(3,4);
   A(2,0) = mtrx(1,0);
   A(2,1) = mtrx(1,3);
   A(2,2) = mtrx(1,1);
   A(2,3) = mtrx(1,4);
   A(3,0) = mtrx(4,0);
   A(3,1) = mtrx(4,3);
   A(3,2) = mtrx(4,1);
   A(3,3) = mtrx(4,4);
   
   Long(0,0) = -1.0 * mtrx(0,5);
   Long(1,0) = -1.0 * mtrx(3,5);
   Long(2,0) = -1.0 * mtrx(1,5);
   Long(3,0) = -1.0 * mtrx(4,5);
   MatrixD id("I",4);
   Disp = (A - id).inverse() * Long;

   latticeFunctions.dispersion.hor = Disp(0,0);
   latticeFunctions.dPrime.hor     = Disp(1,0);
   latticeFunctions.dispersion.ver = Disp(2,0);
   latticeFunctions.dPrime.ver     = Disp(3,0);

   // .......... Propagate lattice around beamline .........
   // .......... This is according to the MAD physics manual. ....

   // .......... Initialize states..........................

    p.setState( Vector(BMLN_dynDim) ); // set initial state to zero

   double alpha0H = latticeFunctions.alpha.hor;
   double beta0H  = latticeFunctions.beta.hor;

   double alpha0V = latticeFunctions.alpha.ver;
   double beta0V  = latticeFunctions.beta.ver;

   double tb;                   // use temp variables to save calc time

   MatrixD dispFinal(6,1);
   MatrixD dispVector(6,1);

   dispVector(0,0) = Disp(0,0);
   dispVector(1,0) = Disp(2,0);
   dispVector(2,0) = 0.0;
   dispVector(3,0) = Disp(1,0);
   dispVector(4,0) = Disp(3,0);
   dispVector(5,0) = 1.0;
   
   oldpsiH = oldpsiV = 0.0;
   bmlnElmnt* be = 0;

   int count = 0;
   for( beamline::deep_iterator it = _myBeamlinePtr->deep_begin(); it != _myBeamlinePtr->deep_end(); ++it, ++count) 
   {
     be = (*it);
     lng = be -> Length();

     be->propagate( p );

     // .......... While calculating lattice functions .......

     LattFuncSage::lattFunc lf; 
     LattFuncSage::lattFunc lfp;    

     be->dataHook.eraseFirst( "Twiss" );
     be->dataHook.insert( Barnacle( "Twiss", lf ) );

     mtrx = p.getState().Jacobian();
     dispFinal = mtrx * dispVector;

     if( ( 0 != strcmp( be->Type(), "rbend"    ) ) && 
         ( 0 != strcmp( be->Type(), "CF_rbend" ) ) && 
         ( 0 != strcmp( be->Type(), "Slot"     ) ) )
     {
       tb = mtrx(0,0) * beta0H -  mtrx(0,3) * alpha0H;
       lf.beta.hor = ( tb * tb + mtrx(0,3) * mtrx(0,3))/beta0H;
  
       lf.alpha.hor = -1.0*(tb * (mtrx(3,0)*beta0H - mtrx(3,3)*alpha0H) +
     			     mtrx(0,3)*mtrx(3,3))/beta0H;
  
       t = atan2(mtrx(0,3),tb);
       while(t < oldpsiH) t += M_TWOPI;
       lf.psi.hor = oldpsiH = t;
  
       tb = mtrx(1,1) * beta0V -  mtrx(1,4) * alpha0V;
       lf.beta.ver = (tb * tb + mtrx(1,4) * mtrx(1,4))/beta0V;
  
       lf.alpha.ver = -1.0*(tb * (mtrx(4,1)*beta0V - mtrx(4,4)*alpha0V) +
     			     mtrx(1,4)*mtrx(4,4))/beta0V;
  
       t = atan2(mtrx(1,4),tb);
       while(t < oldpsiV) t += M_TWOPI;
       lf.psi.ver = oldpsiV = t;
     }

     else { // ??? This is a kludge.
       tb = mtrx(0,0) * beta0H -  mtrx(0,3) * alpha0H;
       lf.beta.hor = ( tb * tb + mtrx(0,3) * mtrx(0,3))/beta0H;
  
       lf.alpha.hor = -1.0*(tb * (mtrx(3,0)*beta0H - mtrx(3,3)*alpha0H) +
     			     mtrx(0,3)*mtrx(3,3))/beta0H;
  
       lf.psi.hor = oldpsiH;
  
       tb = mtrx(1,1) * beta0V -  mtrx(1,4) * alpha0V;
       lf.beta.ver = (tb * tb + mtrx(1,4) * mtrx(1,4))/beta0V;
  
       lf.alpha.ver = -1.0*(tb * (mtrx(4,1)*beta0V - mtrx(4,4)*alpha0V) +
     			     mtrx(1,4)*mtrx(4,4))/beta0V;
  
       lf.psi.ver = oldpsiV;
     }

     lf.dispersion.hor = dispFinal(0,0);
     lf.dPrime.hor = dispFinal(3,0);

     lf.dispersion.ver = dispFinal(1,0);
     lf.dPrime.ver = dispFinal(4,0);

     if ( count == 0 ) 
       lf.arcLength = lng;
     else 
       lf.arcLength = lng + lfp.arcLength;
     lfp = lf;

   } // end while loop over the beamline elements ..............

   // .......... A little test to keep everyone honest .....

   if ( count != _myBeamlinePtr->howMany() ) {
     (*pcerr) << "*** ERROR: beamline::twiss(JetParticle): "<< endl;
     (*pcerr) << "*** ERROR: A horrible, inexplicable error has occurred!" << endl;
     (*pcerr) << "*** ERROR: num elements seen, " << count << endl;
     (*pcerr) << "*** ERROR:  not equal to num elements expected, " 
          << _myBeamlinePtr->howMany() << endl;
     (*pcerr) << "*** ERROR: Bailing out" << endl;

     return LattFuncSage::WRONG_COUNT;
   }

   // .......... Cleaning up and leaving ...................

   _myBeamlinePtr->setTwissIsDone();

   _myBeamlinePtr->dataHook.eraseFirst( "Twiss" );
   _myBeamlinePtr->dataHook.insert( Barnacle( "Twiss", latticeFunctions ) );
 
   _myBeamlinePtr->dataHook.eraseFirst( "Ring" );
   _myBeamlinePtr->dataHook.insert( Barnacle( "Ring", latticeRing ) );
  
 
   return LattFuncSage::DONE;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int LattFuncSage::Twiss_Calc( const LattFuncSage::lattFunc& W, JetParticle& p, Sage::CRITFUNC Crit )
{
  // This function replaces int beamline::twiss( LattFuncSage::lattFunc& W, JetParticle& p)
  // This code is both obsolete and wrong.


  double          t, oldpsiH, oldpsiV, lng;
  int             elmntPos = 0;

  LattFuncSage::lattFunc  latticeFunctions;

  MatrixD         mtrx(BMLN_dynDim,BMLN_dynDim,0.0);
  Vector zero( BMLN_dynDim );

  // .......... Calculating betas and alphas ..............
  latticeFunctions.beta .hor = W.beta.hor;
  latticeFunctions.beta .ver = W.beta.ver;
  latticeFunctions.alpha.hor = W.alpha.hor;
  latticeFunctions.alpha.ver = W.alpha.ver;
  latticeFunctions.psi  .hor = 0.0;
  latticeFunctions.psi  .ver = 0.0;
  latticeFunctions.dispersion.hor = W.dispersion.hor;
  latticeFunctions.dPrime.hor = W.dPrime.hor;
  latticeFunctions.dispersion.ver = W.dispersion.ver;
  latticeFunctions.dPrime.ver = W.dPrime.ver;

  oldpsiH = 0.0;
  oldpsiV = 0.0;
  p.setState(zero);

  double alpha0H = latticeFunctions.alpha.hor;
  double beta0H = latticeFunctions.beta.hor;
    
  double alpha0V = latticeFunctions.alpha.ver;
  double beta0V = latticeFunctions.beta.ver;

  MatrixD dispFinal(6,1);
  MatrixD dispVector(6,1);

  dispVector(0,0) = latticeFunctions.dispersion.hor;
  dispVector(1,0) = latticeFunctions.dispersion.ver;
  dispVector(2,0) = 0.0;
  dispVector(3,0) = latticeFunctions.dPrime.hor;
  dispVector(4,0) = latticeFunctions.dPrime.ver;
  dispVector(5,0) = 1.0;

  double tb;                  // use temp variables to save calc time

  Particle dummyp(p);

  int count = 0; 
  for( beamline::deep_iterator it = _myBeamlinePtr->deep_begin(); it != _myBeamlinePtr->deep_end(); ++it, ++count) {

    bmlnElmnt* be= (*it);

    lng = be -> OrbitLength( dummyp );
    elmntPos++;

    be -> propagate( p );

    // .......... While calculating lattice functions .......

    LattFuncSage::lattFunc lf;
    LattFuncSage::lattFunc lfp; // previous values
 
    be->dataHook.eraseFirst( "Twiss" );
    be->dataHook.insert( Barnacle( "Twiss", lf ) );

    mtrx      =  p.getState().Jacobian();
    dispFinal = mtrx * dispVector;

    if( ( 0 != strcmp( be->Type(), "rbend"    ) ) && 
        ( 0 != strcmp( be->Type(), "CF_rbend" ) ) && 
        ( 0 != strcmp( be->Type(), "Slot"     ) ) )
    {
      tb = mtrx(0,0) * beta0H -  mtrx(0,3) * alpha0H;
      lf.beta.hor = ( tb * tb + mtrx(0,3) * mtrx(0,3))/beta0H;

      lf.alpha.hor = -1.0*(tb * (mtrx(3,0)*beta0H - mtrx(3,3)*alpha0H) +
                            mtrx(0,3)*mtrx(3,3))/beta0H;

      t = atan2(mtrx(0,3),tb);
      while(t < oldpsiH) t += M_TWOPI;
      lf.psi.hor = oldpsiH = t;

      tb = mtrx(1,1) * beta0V -  mtrx(1,4) * alpha0V;
      lf.beta.ver = (tb * tb + mtrx(1,4) * mtrx(1,4))/beta0V;
 
      lf.alpha.ver = -1.0*(tb * (mtrx(4,1)*beta0V - mtrx(4,4)*alpha0V) +
                            mtrx(1,4)*mtrx(4,4))/beta0V;

      t = atan2(mtrx(1,4),tb);
      while(t < oldpsiV) t += M_TWOPI;
      lf.psi.ver = oldpsiV = t;
    }

    else { // ??? This is a kludge.

      tb = mtrx(0,0) * beta0H -  mtrx(0,3) * alpha0H;
      lf.beta.hor = ( tb * tb + mtrx(0,3) * mtrx(0,3))/beta0H;

      lf.alpha.hor = -1.0*(tb * (mtrx(3,0)*beta0H - mtrx(3,3)*alpha0H) +
                            mtrx(0,3)*mtrx(3,3))/beta0H;

      lf.psi.hor = oldpsiH;

      tb = mtrx(1,1) * beta0V -  mtrx(1,4) * alpha0V;
      lf.beta.ver = (tb * tb + mtrx(1,4) * mtrx(1,4))/beta0V;
 
      lf.alpha.ver = -1.0*(tb * (mtrx(4,1)*beta0V - mtrx(4,4)*alpha0V) +
                            mtrx(1,4)*mtrx(4,4))/beta0V;

      lf.psi.ver = oldpsiV;
    }

    lf.dispersion.hor = dispFinal(0,0);
    lf.dPrime.hor = dispFinal(3,0);

    lf.dispersion.ver = dispFinal(1,0);
    lf.dPrime.ver = dispFinal(4,0);

    lf.arcLength = lng + lfp.arcLength;

    if ( count == 0 ) 
       lf.arcLength = lng;
     else 
       lf.arcLength = lng + lfp.arcLength;
     
    lfp = lf;

  } // end loop over the beamline elements ..............

  // .......... A little test to keep everyone honest .....
  if ( count != _myBeamlinePtr->countHowMany() ) {
    (*pcerr) << "*** ERROR: beamline::twiss(LatticeFunct, JetParticle):" << endl;
    (*pcerr) << "*** ERROR: A horrible, inexplicable error has occurred!" << endl;
    (*pcerr) << "*** ERROR: num elements seen, " << count << endl;
    (*pcerr) << "*** ERROR: is not equal to num elements expected, " 
         << _myBeamlinePtr->countHowMany() << endl;
    (*pcerr) << "*** ERROR: Bailing out" << endl;

    return LattFuncSage::WRONG_COUNT;
  }

  // .......... Cleaning up and leaving ...................


 
   return LattFuncSage::DONE;
}

