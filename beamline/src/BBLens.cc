/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      BBLens.cc
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
*****
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
****** REVISION HISTORY
******
****** Mar 2007           ostiguy@fnal.gov
****** - support for reference counted elements
****** - reduced src file coupling due to visitor interface. 
******   visit() takes advantage of (reference) dynamic type.
****** - use std::string for string operations. 
****** - implemented missing operator=()
****** Dec 2007           ostiguy@fnal.gov
****** - new typesafe propagator architecture
******
**************************************************************************
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <basic_toolkit/GenericException.h>
#include <basic_toolkit/utils.h>
#include <beamline/BBLensPropagators.h>
#include <beamline/BBLens.h>
#include <beamline/BmlVisitor.h>
#include <mxyzptlk/Jet.h>
#include <mxyzptlk/JetVector.h>
#include <physics_toolkit/EdwardsTeng.h>  // ??? Doesn't belong here.
#include <algorithm>

using namespace boost;
using namespace std;

static complex<double> complex_i(0.0, 1.0);

extern JetC    w( const JetC& );

double const SIGMA_LIMIT = 64.0;
double const SIGMA_ROUND = 0.1;


// **************************************************
//   class BBLens
// **************************************************

BBLens::BBLens( const char*   nm,
                double const&        l, // IGNORED NOW BUT RETAINED
                                        // FOR POSSIBLE FUTURE USE.
                double const&        s,
                double const&        gmm, 
                const double* emt )
  : bmlnElmnt( nm, 0, s ),num_(s), gamma_(gmm)
{
  if( ( fabs( gamma_ - 1.0 ) < 0.001 ) || 
      ( gamma_ < 1.0  ) 
    ) {
    cerr << "*** ERROR ***                               \n"
            "*** ERROR *** BBLens::BBLens                \n"
            "*** ERROR *** Gamma is too small            \n"
            "*** ERROR ***                               \n"
         << endl;
  }

  if( emt ) for( int i=0; i<3; ++i) 
    emittance_[i] = (emt[i]/6.0)/sqrt( gmm*gmm - 1.0 );
  else      for( int i=0; i<3; ++i) 
    emittance_[i] = 1.0e-6;
  for( int i=0; i<3; ++i) sigma_[i] = 0.0;

  useRound = 1;

  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BBLens::BBLens( BBLens const& x )
: bmlnElmnt( x ), gamma_(x.gamma_), num_(x.num_),  useRound( x.useRound),
  propagator_(x.propagator_->Clone() )
{
  for( int i=0; i < 3; ++i ) emittance_[i] = x.emittance_[i];
  for( int i=0; i < 3; ++i )     sigma_[i] = x.sigma_[i];

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BBLens::~BBLens()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BBLens& BBLens::operator=( BBLens const& rhs) 
{
  if ( &rhs == this) return *this;

  bmlnElmnt::operator=(rhs);

  std::copy( &rhs.emittance_[0], &rhs.emittance_[2],  &emittance_[0]);   // One sigma (noninvariant) emittance_ / pi

  gamma_ = rhs.gamma_;          
  beta_  = rhs.beta_;           
  num_   = rhs.num_;            
  std::copy( &rhs.sigma_[0], &rhs.sigma_[2],  &sigma_[0]);

  propagator_ = PropagatorPtr( rhs.propagator_->Clone() );
  
  return *this;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const&  BBLens::getDistCharge() const
{
  return num_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BBLens::setDistCharge( double const& N )
{
  num_  = N;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BBLens::setSigmas( double const* S )
{
  for( int i=0; i<3; ++i) sigma_[i] = S[i]; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BBLens::setDistParameters( double const& npart, double const& Gamma, double const* sigmas )
{
  num_  =  npart;
  gamma_ = Gamma;
  beta_ = sqrt( 1.0 - 1.0 / ( gamma_*gamma_ ) );
  for( int i=0; i<3; ++i) sigma_[i] = sigmas[i]; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BBLens::AdjustSigma() 
{
  BarnacleList::iterator it = dataHook.find( "EdwardsTeng" );
  
  if( it !=  dataHook.end() ) {
    throw( GenericException( __FILE__, __LINE__, 
           "void BBLens::AdjustSigma()", 
           "Cannot find ETinfo" ) );
  }
   std::cout << " BBLens::AdjustSigma() is BROKEN ! FIXME ! " << std::endl;
   exit (1); 

  // These statement BREAK the library hierarchy !!!
  // FIXME !!!! sigma_[0] = sqrt( any_cast<ETinfo>(it->info).beta.hor*emittance_[0] );
  // FIXME !!!! sigma_[1] = sqrt( any_cast<ETinfo>(it->info).beta.ver*emittance_[1] );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Vector BBLens::NormalizedEField( double const& arg_x, double const& arg_y )
{
  Vector  retvec(3);
  char    normal;
  std::complex<double>  z;

  double  ds, meanSigma;
  std::complex<double>  arg1, arg2;
  double  tmp1,r;
  std::complex<double>  retarg1, retarg2;
  enum    { ur, ul, lr, ll } quadrant;

  double      x = arg_x;  
  double      y = arg_y;
  double sigmaX = sigma_[0];
  double sigmaY = sigma_[1];

  // Asymptotic limit ...
  if( ( sigmaX == 0.0 ) && ( sigmaY == 0.0 ) ) {
    r = x*x + y*y;
    if( r < 1.0e-20 ) {
    throw( GenericException( __FILE__, __LINE__, 
           "Vector BBLens::NormalizedEField( double arg_x, double arg_y )", 
           "Asymptotic limit r seems too small." ) );
    }
    retvec(0) = x/r;
    retvec(1) = y/r;
    retvec(2) = 0.0;
    return retvec;
  }

  // Round beam limit ...
  if( useRound ) {
    if( ( fabs( ( sigmaX - sigmaY ) / ( sigmaX + sigmaY ) ) < SIGMA_ROUND ) ||
        ( ( pow( x/sigmaX, 2.0 ) + pow( y/sigmaY, 2.0 ) ) > SIGMA_LIMIT )
      ) {
      r = x*x + y*y;
      meanSigma = 2.0*sigmaX*sigmaY;
      // Test for small r .....
      if( r > 1.0e-6*meanSigma ) {
  	r = ( 1.0 - exp(-r/ meanSigma ) ) / r;
  	retvec(0) = x*r;
  	retvec(1) = y*r;
  	retvec(2) = 0.0;
  	return retvec;
      }
      else {
  	retvec(0) = x/meanSigma;
  	retvec(1) = y/meanSigma;
  	retvec(2) = 0.0;
  	return retvec;
      }
    } 
  }


  // Elliptic beam ...
  if( arg_x >= 0.0 ) {
    if( arg_y >= 0.0 ) {
      quadrant = ur;
      x = arg_x;  
      y = arg_y;
    }
    else {
      quadrant = lr;
      x = arg_x;  
      y = - arg_y;
    }
  }
  else {
    if( arg_y >= 0.0 ) {
      quadrant = ul;
      x = - arg_x;  
      y = arg_y;
    }
    else {
      quadrant = ll;
      x = - arg_x;  
      y = - arg_y;
    }
  }

  // Check for normal processing ...
  if( !( normal = ( sigmaX > sigmaY ) ) ) {
   tmp1   = sigmaX;
   sigmaX = sigmaY;
   sigmaY = tmp1;
   tmp1   = x;
   x      = y;
   y      = tmp1;
  }

  // The calculation ...
  ds = sqrt(2.0*(sigmaX*sigmaX - sigmaY*sigmaY));
  arg1 = x/ds + complex_i*y/ds;  
  r = sigmaY/sigmaX;
  arg2 = ((x*r)/ds) + complex_i*((y/r)/ds);

  retarg1 = w( arg1 );
  retarg2 = w( arg2 );

  // Normalization ...
  r    = x/sigmaX;
  r    = r*r;
  tmp1 = y/sigmaY;
  r   += tmp1*tmp1;

  z    = retarg1;
  z   -= retarg2 * exp( - r/2.0 );
  z   *= - complex_i * MATH_SQRTPI / ds;

  // And return ...
  retvec(2) = 0.0;
  if( normal ) {
    if( quadrant == ur ) {
      retvec(0) =   real(z);
      retvec(1) = - imag(z);
      return retvec;
    }
    if( quadrant == ul ) {
      retvec(0) = - real(z);
      retvec(1) = - imag(z);
      return retvec;
    }
    if( quadrant == lr ) {
      retvec(0) =   real(z);
      retvec(1) =   imag(z);
      return retvec;
    }
    if( quadrant == ll ) {
      retvec(0) = - real(z);
      retvec(1) =   imag(z);
      return retvec;
    }
  }
  else {
    if( quadrant == ur ) {
      retvec(0) = - imag(z);
      retvec(1) =   real(z);
      return retvec;
    }
    if( quadrant == ul ) {
      retvec(0) =   imag(z);
      retvec(1) =   real(z);
      return retvec;
    }
    if( quadrant == lr ) {
      retvec(0) = - imag(z);
      retvec(1) = - real(z);
      return retvec;
    }
    if( quadrant == ll ) {
      retvec(0) =   imag(z);
      retvec(1) = - real(z);
      return retvec;
    }
    // ??? Just a guess; check this!
  }

  return retvec; // This line should never be reached.
}

JetVector BBLens::NormalizedEField( const Jet& arg_x, const Jet& arg_y )
{
  JetVector  retvec(3);
  char       normal;
  JetC       z;
  Jet        x, y, r;
  double     sigmaX, sigmaY, meanSigma, ds, ratio;

  JetC       arg1;
  JetC       arg2;
  double     tmp1;
  Jet        tmpJ;
  JetC       retarg1;
  JetC       retarg2;

  enum       { ur, ul, lr, ll } quadrant;

  x = arg_x;  
  y = arg_y;
  sigmaX = sigma_[0];
  sigmaY = sigma_[1];

  // Asymptotic limit ...
  if( ( sigmaX == 0.0 ) && ( sigmaY == 0.0 ) ) {
    r = x*x + y*y;
    if( r.standardPart() < 1.0e-20 ) {
    throw(  GenericException( __FILE__, __LINE__, 
           "JetVector BBLens::NormalizedEField( const Jet&, const Jet& )", 
           "Asymptotic limit r seems too small." ) );
    }
    retvec(0) = x/r;
    retvec(1) = y/r;
    retvec(2) = 0.0;
    return retvec;
  }

  // Round beam limit ...
  if( useRound ) {
    if( ( fabs( ( sigmaX - sigmaY ) / ( sigmaX + sigmaY ) ) < SIGMA_ROUND ) ||
        ( ( pow( x.standardPart()/sigmaX, 2.0 ) 
          + pow( y.standardPart()/sigmaY, 2.0 ) ) > SIGMA_LIMIT )
      ) {
      r = x*x + y*y;
      meanSigma = 2.0*sigmaX*sigmaY;
      // Test for small r .....
      if( r.standardPart() > 1.0e-6*meanSigma ) {
  	r = ( 1.0 - exp(-r/ meanSigma ) ) / r;
  	retvec(0) = x*r;
  	retvec(1) = y*r;
  	retvec(2) = 0.0;
  	return retvec;
      }
      else {
  	retvec(0) = x/meanSigma;
  	retvec(1) = y/meanSigma;
  	retvec(2) = 0.0;
  	return retvec;
      }
    } 
  }

  // Elliptic beam ...
  if( arg_x.standardPart() >= 0.0 ) {
    if( arg_y.standardPart() >= 0.0 ) {
      quadrant = ur;
      x = arg_x;  
      y = arg_y;
    }
    else {
      quadrant = lr;
      x = arg_x;  
      y = - arg_y;
    }
  }
  else {
    if( arg_y.standardPart() >= 0.0 ) {
      quadrant = ul;
      x = - arg_x;  
      y = arg_y;
    }
    else {
      quadrant = ll;
      x = - arg_x;  
      y = - arg_y;
    }
  }

  // Check for normal processing ...
  if( !( normal = ( sigmaX > sigmaY ) ) ) {
   tmp1   = sigmaX;
   sigmaX = sigmaY;
   sigmaY = tmp1;
   tmpJ   = x;
   x      = y;
   y      = tmpJ;
  }

  // The calculation ...
  ds = sqrt(2.0*(sigmaX*sigmaX - sigmaY*sigmaY));
  arg1 = ( (x/ds) + complex_i* (y/ds) );
  ratio = sigmaY/sigmaX;
  arg2 = ( (x*ratio)/ds ) + complex_i*( (y/ratio)/ds );

  retarg1 = w( arg1 );
  retarg2 = w( arg2 );

  // Normalization ...
  r    = x/sigmaX;
  r    = r*r;
  tmpJ = y/sigmaY;
  r   += tmpJ*tmpJ;

  z    = retarg1;
  z   -= retarg2 * exp( - r/2.0 );
  z    = - z*( complex_i * MATH_SQRTPI / ds );

  // And return ...
  retvec(2) = 0.0;
  if( normal ) {
    if( quadrant == ur ) {
      retvec(0) =   real(z);
      retvec(1) = - imag(z);
      return retvec;
    }
    if( quadrant == ul ) {
      retvec(0) = - real(z);
      retvec(1) = - imag(z);
      return retvec;
    }
    if( quadrant == lr ) {
      retvec(0) =   real(z);
      retvec(1) =   imag(z);
      return retvec;
    }
    if( quadrant == ll ) {
      retvec(0) = - real(z);
      retvec(1) =   imag(z);
      return retvec;
    }
  }
  else {
    if( quadrant == ur ) {
      retvec(0) = - imag(z);
      retvec(1) =   real(z);
      return retvec;
    }
    if( quadrant == ul ) {
      retvec(0) =   imag(z);
      retvec(1) =   real(z);
      return retvec;
    }
    if( quadrant == lr ) {
      retvec(0) = - imag(z);
      retvec(1) = - real(z);
      return retvec;
    }
    if( quadrant == ll ) {
      retvec(0) =   imag(z);
      retvec(1) = - real(z);
      return retvec;
    }
    // ??? Just a guess; check this!
  }

  return retvec;  // This line should never be reached.
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* BBLens::Type() const 
{ 
  return "BBLens"; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool  BBLens::isMagnet() const
{
  return false;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Vector BBLens::Beta() 
{
  static char firstCall = 1;
  static double answer [] = { 0., 0., -1. };
  if( firstCall ) {
    cout << "\n"
            "*** WARNING ***                               \n"
            "*** WARNING *** BBLens::Beta()                \n"
            "*** WARNING *** Needs to be written properly. \n"
            "*** WARNING ***                               \n"
         << endl;
    firstCall = 0;
  }
  return Vector( 3, answer );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BBLens::GetSigma( double* sgm ) 
{
  for( int i=0; i< 3; ++i) sgm[i] = sigma_[i];
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BBLens::accept( BmlVisitor& v ) 
{ 
  v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BBLens::accept( ConstBmlVisitor& v ) const  
{ 
  v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BBLens::localPropagate(Particle& p)
{  
  (*propagator_)(*this,p);
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BBLens::localPropagate(JetParticle& p)
{  
  (*propagator_)(*this,p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BBLens::localPropagate(ParticleBunch& b)
{  
  (*propagator_)(*this,b);
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BBLens::localPropagate(JetParticleBunch& b)
{  
  (*propagator_)(*this,b);
}
