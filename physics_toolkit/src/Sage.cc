/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      Sage.cc
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
******  Jan 2007 - Mar 2007  ostiguy@fnal
******    
******  - use new style STL-compatible beamline iterators
******  - added support for reference counted elements/beamlines
******
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

/*
 *  File: Sage.cc
 *  Base class for all sages.
 *  
 *  Leo Michelotti
 *  November 23, 1998
 */


#ifndef NO_RTTI
#include <typeinfo>
#endif

#include <basic_toolkit/GenericException.h>
#include <physics_toolkit/Sage.h>
#include <beamline/FramePusher.h>  // Used by Sage::isRing functions
#include <beamline/Particle.h>

double Sage::defGapTol_   = 0.005;  // = 5 mm
double Sage::defAngleTol_ = 0.001;  // = 1 mrad

using namespace std;

namespace {

 Particle::PhaseSpaceIndex const& i_x     = Particle::i_x;
 Particle::PhaseSpaceIndex const& i_npx   = Particle::i_npx;
 Particle::PhaseSpaceIndex const& i_y     = Particle::i_y;
 Particle::PhaseSpaceIndex const& i_npy   = Particle::i_npy;
 Particle::PhaseSpaceIndex const& i_cdt   = Particle::i_cdt;
 Particle::PhaseSpaceIndex const& i_ndp   = Particle::i_ndp;

} // anonymous namespace



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Sage::Sage( BmlPtr x , sqlite::connection& db)
  :     bml_(x), nelms_(0), verbose_(false), isRing_(Sage::isRing(*x)),
        ringGapTolerance_(  defGapTol_ ), ringAngleTolerance_( defAngleTol_), 
        db_(db)
{

  if( !x ) {
    throw( GenericException( __FILE__, __LINE__, 
           "Sage::Sage( BmlPtr x, bool doClone )", 
           "Constructor invoked with null pointer." ) );
  }

   if( isRing_ ) {
      bml_->setLineMode( beamline::ring );
   }

  nelms_ = bml_->countHowManyDeeply();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Sage::Sage(beamline const& bml, sqlite::connection& db )
  :  bml_(), nelms_(0), verbose_(false), isRing_(Sage::isRing( bml ) ),
     ringGapTolerance_(  defGapTol_ ), ringAngleTolerance_( defAngleTol_), db_(db)
{
   
  bml_ = BmlPtr( bml.Clone() ); 

  if( isRing_ ) {
      bml_->setLineMode( beamline::ring );
  }

  nelms_ = bml_->countHowManyDeeply();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Sage::~Sage()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Sage::set_verbose()
{
  verbose_ = true;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Sage::unset_verbose()
{
   verbose_ = false;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
void Sage::treatAsRing( bool x )
{
  if(x) {
    bml_->setLineMode( beamline::ring );
  }
  else {
    bml_->setLineMode( beamline::line );
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool Sage::isTreatedAsRing() const
{
  return ( beamline::ring == bml_->getLineMode() );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Sage::setGapTolerance( double x )
{
  ringGapTolerance_ = std::abs(x);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Sage::getGapTolerance() const
{
  return ringGapTolerance_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void Sage::setAngleTolerance( double x )
{
  ringAngleTolerance_ = std::abs(x);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


inline double Sage::getAngleTolerance() const
{
  return ringAngleTolerance_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


bool Sage::no ( ConstElmPtr )
{
  return false;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


bool Sage::yes( ConstElmPtr )
{
  return true;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


bool Sage::isRing( beamline const& bml, double const& gap_tol, double const& angle_tol )
{
  // This test will return true for pathologically short lines
  //   like a beamline consisting of a single 1 mm drift.

  const double gtol  = std::abs(gap_tol); 
  const double atol  = std::abs(angle_tol); 

  // Paranoia in case of excessively stupid user.

  FramePusher fp;
  bml.accept( fp );

  // Check the point of return

  Vector r = fp.getFrame().getOrigin();

  for( int i = 0; i < 3; ++i) {
    if( gtol < std::abs(r[i]) ) { return false; }
  }

  // Check the angle of return
  //   Tolerance is hardwired to 1 milliradian
  MatrixD fv = fp.getFrame().getAxes();

  for( int i = 0; i < 3; ++i ) {
    for( int j = 0; j < 3; ++j ) {
      if( i != j ) {
        if( atol < std::abs(fv[i][j]) ) { return false; }
      }
    }
  }
  
  return true;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


bool Sage::isRing( BmlPtr bmlPtr, double const& gt, double const& at )
{
  return Sage::isRing( *bmlPtr, gt, at );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> Sage::compute_tunes( Matrix const& oneturn )
{

  std::vector<double> nu(3, 0.0); 

  VectorC lambda = oneturn.eigenValues();

  std::pair<std::complex<double>, std::complex<double> > lambda_x ( lambda[i_x  ], lambda[i_npx] );
  std::pair<std::complex<double>, std::complex<double> > lambda_y ( lambda[i_y  ], lambda[i_npy] );
  std::pair<std::complex<double>, std::complex<double> > lambda_s ( lambda[i_cdt], lambda[i_ndp] );

 
  if(   ( (abs( lambda_x.first) - 1.0) > 1.0e-4 ) ||
	( (abs( lambda_y.first) - 1.0) > 1.0e-4 )    )
  {       
    (*FNAL::pcerr) << "\n"
         << "*** ERROR *** \n"
         << "*** ERROR *** \n"
         << "*** ERROR *** The lattice is linearly unstable transversly. \n"
		   << "*** ERROR *** |lambda_x | = " << abs(lambda_x.first)
                               << "  |lambda_y | = " << abs(lambda_y.first) 
         << "\n"
         << "*** ERROR *** \n"
         << std::endl;
   }

  if( ( abs( lambda_x.first - conj( lambda_x.second ) ) > 1.0e-4 )  ||
      ( abs( lambda_y.first - conj( lambda_y.second ) ) > 1.0e-4 )
    ) {
    (*FNAL::pcerr) << "\n"
         << "*** ERROR *** EdwardsTengSage()                        \n"
         << "*** ERROR *** Conjugacy condition has been violated\n"
         << "*** ERROR *** The lattice may be linearly unstable.\n"
         << "*** ERROR *** Eigenvalues =                        \n"
         << "*** ERROR *** " << lambda << std::endl;
  }

  double ht = 0.0;
  ht = ( (ht=arg(lambda_x.first)) > 0 ) ? (ht/ M_TWOPI) : (ht += M_TWOPI)/ M_TWOPI; 

  double vt = 0.0;
  vt = ( (vt=arg(lambda_y.first)) > 0 ) ? (vt/ M_TWOPI) : (vt += M_TWOPI)/ M_TWOPI; 

  double lt = 0.0;
  lt = ( (lt=arg(lambda_s.first)) > 0 ) ? (lt/ M_TWOPI) : (lt += M_TWOPI)/ M_TWOPI; 

  nu[0] = ht; nu[1] = vt; nu[2] = lt;

  return nu; 
}
