/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   1.0                    
******                                    
******  File:      MatrixCalcVisitor.cc
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
****** REVSION HISTORY
****** 
****** Mar 2007     ostiguy@fnal.gov
******
****** - use STL container for linear model matrices
****** - visitor interface takes advantage of (reference) dynamic type
****** - initialization optimizations
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <basic_toolkit/GenericException.h>
#include <beamline/beamline.h>
#include <beamline/beamline_elements.h>
#include <physics_toolkit/MatrixCalcVisitor.h>
#include <basic_toolkit/iosetup.h>

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;


int MatrixCalcVisitor::DONE         =  0;
int MatrixCalcVisitor::UNSTABLE     = -1;
int MatrixCalcVisitor::INTEGER_TUNE = -2;
int MatrixCalcVisitor::PHASE_ERROR  = -3;


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

MatrixCalcVisitor::MatrixCalcVisitor( Particle const& x )
: numberOfElements_(0), 
  firstTime_(true),
  calcDone_(false),
  map_h_(2,2), 
  map_v_(2,2),
  counter_(0), 
  particle_(x),
  myBeamlinePtr_(0),
  linearModel_h_(0),
  linearModel_v_(0),
  beta_h_(),
  beta_v_(),
  alpha_h_(),
  alpha_v_(),
  psi_h_(),
  psi_v_()
{
  map_h_(0,0) = 1.0;    map_h_(0,1) = 0.0;
  map_h_(1,0) = 0.0;    map_h_(1,1) = 1.0;
  map_v_(0,0) = 1.0;    map_v_(0,1) = 0.0;
  map_v_(1,0) = 0.0;    map_v_(1,1) = 1.0;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

MatrixCalcVisitor::~MatrixCalcVisitor()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> const&  MatrixCalcVisitor::s() {
  return  arcLength_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int MatrixCalcVisitor::NumberOfElements() const {
  return numberOfElements_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void MatrixCalcVisitor::visit( beamline& x )
{
  if( firstTime_ ) 
  {
     myBeamlinePtr_    = &x;
     numberOfElements_ = x.countHowManyDeeply();
     firstTime_        = false;
  }


  if( myBeamlinePtr_ = &x ) 
  {
    calcDone_ = 0;

    map_h_(0,0) = 1.0;    map_h_(0,1) = 0.0;
    map_h_(1,0) = 0.0;    map_h_(1,1) = 1.0;
    map_v_(0,0) = 1.0;    map_v_(0,1) = 0.0;
    map_v_(1,0) = 0.0;    map_v_(1,1) = 1.0;


    double s = 0.0;
    counter_ = 0;

    ElmPtr q;
    for (beamline::deep_iterator it  = x.deep_begin();  
	 it !=  x.deep_end(); ++it ) { 
      
      ++counter_;

      q = (*it);

      if( counter_ <= numberOfElements_ ) {
  	q->accept( *this );
  	s += q->OrbitLength( particle_ );
  	arcLength_.push_back(s);
      }
      else {
        ostringstream uic;
        uic  << "The counter has overrun the number of elements: "
  	     << counter_ << " > " << numberOfElements_;
        throw( GenericException( __FILE__, __LINE__, 
               "void MatrixCalcVisitor::visitBeamline( beamline* x )", 
               uic.str().c_str() ) );
      }
    }
  
    if( counter_  != numberOfElements_ ) {
      ostringstream uic;
      uic  << "The count is not correct: "
  	   << counter_ << "  != " << numberOfElements_;
      throw( GenericException( __FILE__, __LINE__, 
             "void MatrixCalcVisitor::visit( beamline& x )", 
             uic.str().c_str() ) );
    }
  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void MatrixCalcVisitor::visit( bmlnElmnt& x )
{
  // The arbitrary elements will act like a drift.

  double lng = x.Length();

  MatrixD mtrx_h(2,2);

  mtrx_h(0,0) = 1.0;    
  mtrx_h(0,1) = lng;
  mtrx_h(1,1) = 1.0;

  linearModel_h_.push_back(mtrx_h);
 

  MatrixD mtrx_v(2,2);

  mtrx_v(0,0) = 1.0;
  mtrx_v(0,1) = lng;
  mtrx_v(1,1) = 1.0;

  linearModel_v_.push_back(mtrx_v);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void MatrixCalcVisitor::visit( marker& x )
{
  // The matrices are unit matrices; 
  // i.e., nothing happens.


   MatrixD mtrx_h(2,2);

   mtrx_h(0,0) = 1.0;    
   mtrx_h(0,1) = 0.0;
   mtrx_h(1,0) = 0.0;
   mtrx_h(1,1) = 1.0;

   linearModel_h_.push_back(mtrx_h);

   MatrixD mtrx_v(2,2);

   mtrx_v(0,0) = 1.0;
   mtrx_v(0,1) = 0.0;
   mtrx_v(1,0) = 0.0;
   mtrx_v(1,1) = 1.0;

   linearModel_v_.push_back(mtrx_v);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void MatrixCalcVisitor::visit( drift& x )
{
  double lng = x.Length();

  MatrixD mtrx_h(2,2);

  mtrx_h(0,0) = 1.0;    
  mtrx_h(0,1) = lng;
  mtrx_h(1,1) = 1.0;

  linearModel_h_.push_back(mtrx_h);

  MatrixD mtrx_v(2,2);

  mtrx_v(0,0) = 1.0;
  mtrx_v(0,1) = lng;
  mtrx_v(1,1) = 1.0;
 
  linearModel_v_.push_back(mtrx_v);

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void MatrixCalcVisitor::visit( sbend& x )
{
  
  double lng   = x.Length();
  double rho   = particle_.ReferenceBRho() / x.Strength();
  double theta = lng / rho;
  double cs    = cos( theta );
  double sn    = sin( theta );


  MatrixD mtrx_h(2,2);

  mtrx_h(0,0) = cs;
  mtrx_h(0,1) = rho*sn;
  mtrx_h(1,0) = - sn/rho;
  mtrx_h(1,1) = cs;

  linearModel_h_.push_back(mtrx_h);

  MatrixD mtrx_v(2,2);

  mtrx_v(0,0) = 1.0;
  mtrx_v(0,1) = lng;
  mtrx_v(1,1) = 1.0;

  linearModel_v_.push_back(mtrx_v);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void MatrixCalcVisitor::visit( rbend& x )
{
  throw( GenericException( __FILE__, __LINE__, 
         "void MatrixCalcVisitor::visitRbend( rbend* x )", 
         "Function not written." ) );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void MatrixCalcVisitor::visit( sector& x )
{
  throw( GenericException( __FILE__, __LINE__, 
         "void MatrixCalcVisitor::visitSector( sector* x )", 
         "Sorry. Sectors not allowed." ) );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void MatrixCalcVisitor::visit( quadrupole& x )
{
  
  double kappa = sqrt( fabs( x.Strength() / particle_.ReferenceBRho() ) );
  double theta = x.Length() * kappa;
  double cs    = cos ( theta );
  double sn    = sin ( theta );
  double csh   = cosh( theta );
  double snh   = sinh( theta );

  MatrixD mtrx_h(2,2);
  MatrixD mtrx_v(2,2);

  if( x.Strength() > 0.0 ) {
    mtrx_h(0,0) = cs;
    mtrx_h(0,1) = sn/kappa;
    mtrx_h(1,0) = - kappa*sn;
    mtrx_h(1,1) = cs;

    mtrx_v(0,0) = csh;
    mtrx_v(0,1) = snh/kappa;
    mtrx_v(1,0) = - kappa*snh;
    mtrx_v(1,1) = csh;
  }
  else {
    mtrx_h(0,0) = cs;
    mtrx_h(0,1) = sn/kappa;
    mtrx_h(1,0) = - kappa*sn;
    mtrx_h(1,1) = cs;

    mtrx_v(0,0) = csh;
    mtrx_v(0,1) = snh/kappa;
    mtrx_v(1,0) = - kappa*snh;
    mtrx_v(1,1) = csh;
  }

  linearModel_h_.push_back(mtrx_h);
  linearModel_v_.push_back(mtrx_v);


}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void MatrixCalcVisitor::visit( thinQuad& x )
{
  double kappa = x.Strength() / particle_.ReferenceBRho(); // 1/f
  
  MatrixD  mtrx_h(2,2);
  MatrixD  mtrx_v(2,2);

  mtrx_h(0,0) = 1.0;    
  mtrx_h(1,0) = - kappa;
  mtrx_h(1,1) = 1.0;

  linearModel_h_.push_back(mtrx_h);

  mtrx_v(0,0) = 1.0;
  mtrx_v(1,0) = kappa;
  mtrx_v(1,1) = 1.0;

  linearModel_v_.push_back(mtrx_v);

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void MatrixCalcVisitor::visit( Slot& x )
{
  throw( GenericException( __FILE__, __LINE__, 
         "void MatrixCalcVisitor::visitSlot( Slot* x )", 
         "Function not written." ) );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void MatrixCalcVisitor::visit( CF_rbend& x )
{
  throw( GenericException( __FILE__, __LINE__, 
         "void MatrixCalcVisitor::visitCF_rbend( CF_rbend* x )", 
         "Function not written." ) );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void visit( combinedFunction& x )
{
  throw( GenericException( __FILE__, __LINE__, 
         "void visitCombinedFunction( combinedFunction* x )", 
         "Function not written." ) );
}



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Vector& MatrixCalcVisitor::getState()
{
  return  particle_.getState();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Vector MatrixCalcVisitor::State()
{
  return particle_.State();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int MatrixCalcVisitor::doCalc()
{
  
  std::vector<MatrixD>::iterator ith;
  std::vector<MatrixD>::iterator itv;

  for( int i=0;  i < numberOfElements_; ++i, ++ith, ++itv) {

     map_h_ = map_h_ * (*ith);
     map_v_ = map_v_ * (*itv);

  }

  // ==========================================================
  // This is more or less cloned from LattFuncSage::Fast_CS_Calc
  // 

        int i_x  = 0,
            i_px = 1,
            i_y  = 0,
            i_py = 1;

  // Calculate initial lattice functions ...
  // ... first horizontal
  double alpha_x, beta_x;

  double sn;
  double cs = ( map_h_( i_x, i_x ) + map_h_( i_px, i_px ) )/2.0;
  if( fabs( cs ) <= 1.0 ) {
    if( map_h_( i_x, i_px ) > 0.0 )  sn =   sqrt( 1.0 - cs*cs );
    else                             sn = - sqrt( 1.0 - cs*cs );
  }
  else {
    (*pcerr) << "*** ERROR ***                                     \n"
            "*** ERROR *** MatrixCalcVisitor::_doCalc          \n"
            "*** ERROR *** cos( psi_H ) = "
         << cs
         << "\n"
            "*** ERROR *** Cannot continue with calculation.   \n"
            "*** ERROR ***                                     \n"
         << endl;

    return MatrixCalcVisitor::UNSTABLE;
  }

  if( sn == 0.0 ) {
    (*pcerr) << "*** ERROR ***                                     \n"
            "*** ERROR *** MatrixCalcVisitor::_doCalc          \n"
            "*** ERROR *** Integer horizontal tune.            \n"
            "*** ERROR ***                                     \n"
         << endl;

    return MatrixCalcVisitor::INTEGER_TUNE;
  }

  beta_x  = map_h_( i_x, i_px ) / sn;
  alpha_x = ( map_h_( i_x, i_x ) - map_h_( i_px, i_px ) ) / ( 2.0*sn );


  // ... then vertical.
  double alpha_y, beta_y;
  cs = ( map_v_( i_y, i_y ) + map_v_( i_py, i_py ) )/2.0;
  if( fabs( cs ) <= 1.0 ) {
    if( map_v_( i_y, i_py ) > 0.0 )  sn =   sqrt( 1.0 - cs*cs );
    else                           sn = - sqrt( 1.0 - cs*cs );
  }
  else {
    (*pcerr) << "*** ERROR ***                                     \n"
            "*** ERROR *** MatrixCalcVisitor::_doCalc          \n"
            "*** ERROR *** cos( psi_V ) = "
         << cs
         << "\n"
            "*** ERROR *** Cannot continue with calculation.   \n"
            "*** ERROR ***                                     \n"
         << endl;

    return MatrixCalcVisitor::UNSTABLE;
  }

  if( sn == 0.0 ) {
    (*pcerr) << "*** ERROR ***                                     \n"
            "*** ERROR *** MatrixCalcVisitor::_doCalc          \n"
            "*** ERROR *** Integer vertical tune.              \n"
            "*** ERROR ***                                     \n"
         << endl;

    return MatrixCalcVisitor::INTEGER_TUNE;
  }

  beta_y  = map_v_( i_y, i_py ) / sn;
  alpha_y = ( map_v_( i_y, i_y ) - map_v_( i_py, i_py ) ) / ( 2.0*sn );


  // Set up the "proton" states ...

  MatrixC w_x(2,1);
  MatrixC w_y(2,1);
  double  dum;
  std::complex<double> ci( 0., 1.0 );

  w_x( i_x,  0 ) = dum = sqrt( beta_x );
  w_x( i_px, 0 ) = ( ci - alpha_x ) / dum;
  w_y( i_y,  0 ) = dum = sqrt( beta_y );
  w_y( i_py, 0 ) = ( ci - alpha_y ) / dum;

  MatrixC outState_x(2,1), outState_y(2,1);
  std::complex<double> phase;
 
  double psi_x = 0.0;
  double psi_y = 0.0;
  double dpsi_x, dpsi_y;

  // cout << "DGN> number of elements = " << numberOfElements_ << endl;
  // cout << "DGN> w_x = \n" << w_x << endl;

  for( int jc = 0; jc < numberOfElements_; ++jc)
  {

    MatrixC w_z = w_x;

    w_x  = w_x * linearModel_h_[jc];
    w_y  = w_y * linearModel_v_[jc];

    outState_x = w_x;
    outState_y = w_y;

    phase = w_x(i_x,0) / abs( w_x(i_x,0) );
    outState_x = outState_x/phase;
    if(   fabs( imag( outState_x(i_px,0) )*real( outState_x(i_x,0) ) - 1.0 )
        > 0.01 )
    {
  	(*pcerr) << "*** ERROR *** Phase error in horizontal plane at index " 
             << jc
             << endl;
  	(*pcerr) << "*** ERROR *** imag( outState_x(i_px,0) ) = " 
             << imag( outState_x(i_px,0) ) 
             << endl;
  	(*pcerr) << "*** ERROR *** outState_x(i_px,0) = " 
             << outState_x(i_px,0)
             << endl;

  	return MatrixCalcVisitor::PHASE_ERROR;
    }
    dpsi_x = atan2( imag(phase), real(phase) );
    while( dpsi_x < psi_x ) dpsi_x += M_TWOPI;
  
    phase = outState_y(i_y,0) / abs( outState_y(i_y,0) );
    outState_y = outState_y/phase;
    if(   fabs( imag( outState_y(i_py,0) )*real( outState_y(i_y,0) ) - 1.0 )
        > 0.01 )
    {
  	(*pcerr) << "*** ERROR *** Phase error in vertical plane at index " 
             << jc
             << endl;
  	(*pcerr) << "*** ERROR *** imag( outState_y(i_py,0) ) = " 
             << imag( outState_y(i_py,0) ) 
             << endl;
  	(*pcerr) << "*** ERROR *** outState_y(i_py,0) = " 
             << outState_y(i_py,0)
             << endl;

 	return MatrixCalcVisitor::PHASE_ERROR;
    }
    dpsi_y = atan2( imag(phase), real(phase) );
    while( dpsi_y < psi_y ) dpsi_y += M_TWOPI;
  

    // Calculate lattice functions ...
    beta_x   =   real( outState_x(i_x,0)  );
    alpha_x  = - real( outState_x(i_px,0) );
    alpha_x *= beta_x;
    beta_x  *= beta_x;
  
    beta_y  =   real( outState_y(i_y,0) );
    alpha_y = - real( outState_y(i_py,0) );

    alpha_y *= beta_y;
    beta_y  *= beta_y;

      beta_h_[jc]   = beta_x;
      beta_v_[jc]   = beta_y;
     alpha_h_[jc]   = alpha_x;
     alpha_v_[jc]   = alpha_y;
       psi_h_[jc]   = psi_x;
       psi_v_[jc]   = psi_y;

  }  // End loop on jc ...


  // 
  // ==========================================================  

  calcDone_ = true;
  return MatrixCalcVisitor::DONE;
}



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> const& MatrixCalcVisitor::Beta_h()  {

  if( !calcDone_ ) doCalc();
  return beta_h_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> const& MatrixCalcVisitor::Beta_v()  {
  if( !calcDone_ ) doCalc();
  return beta_v_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> const& MatrixCalcVisitor::Alpha_h()  {
  if( !calcDone_ ) doCalc();
  return alpha_h_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> const& MatrixCalcVisitor::Alpha_v()  {

  if( !calcDone_ ) doCalc();
  return alpha_v_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> const& MatrixCalcVisitor::Psi_h()  {

  if( !calcDone_ ) doCalc();
  return psi_h_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> const& MatrixCalcVisitor::Psi_v() {

  if( !calcDone_ ) doCalc();
  return psi_v_;
}

