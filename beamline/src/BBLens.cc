/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.1
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
******                                                                
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <beamline/BBLens.h>
#include <basic_toolkit/utils.h>
#include <mxyzptlk/Jet.h>
#include <mxyzptlk/JetVector.h>
#include <physics_toolkit/EdwardsTeng.h>  // ??? Doesn't belong here.

using namespace std;

static complex<double> complex_i(0.0, 1.0);

extern JetC    w( const JetC& );

double const SIGMA_LIMIT = 64.0;
double const SIGMA_ROUND = 0.1;


// **************************************************
//   class BBLens
// **************************************************

BBLens::BBLens( const char*   nm,
                double        l, 
                double        s,
                double        gmm, 
                const double* emt )
: bmlnElmnt( nm, l, s )
{
  int i;
  num = s;

  gamma = gmm;
  if( ( fabs( gamma - 1.0 ) < 0.001 ) || 
      ( gamma < 1.0  ) 
    ) {
    cerr << "*** ERROR ***                               \n"
            "*** ERROR *** BBLens::BBLens                \n"
            "*** ERROR *** Gamma is too small            \n"
            "*** ERROR ***                               \n"
         << endl;
  }

  if( emt ) for( i = 0; i < 3; i++ ) 
    emittance[i] = (emt[i]/6.0)/sqrt( gmm*gmm - 1.0 );
  else      for( i = 0; i < 3; i++ ) 
    emittance[i] = 1.0e-6;
  for( i = 0; i < 3; i++ ) sigma[i] = 0.0;

  useRound = 1;
}


BBLens::BBLens( BBLens const& x )
: bmlnElmnt( x ), gamma(x.gamma), num(x.num),  useRound( x.useRound)
{
  for( int i = 0; i < 3; ++i ) emittance[i] = x.emittance[i];
  for( int i = 0; i < 3; ++i )     sigma[i] = x.sigma[i];

}


BBLens::~BBLens()
{
}


void BBLens::KludgeNum( double N )
{
  num  = N;
}


void BBLens::KludgeSigma( const double* S )
{
  for( int i = 0; i < 3; i++ ) sigma[i] = S[i]; 
}


void BBLens::Kludge( double N, double G, const double* S )
{
  num  = N;
  gamma = G;
  beta = sqrt( 1.0 - 1.0 / ( gamma*gamma ) );
  for( int i = 0; i < 3; i++ ) sigma[i] = S[i]; 
}


void BBLens::AdjustSigma() 
{
  ETinfo* infoPtr = (ETinfo*) dataHook.find( "EdwardsTeng" );
  
  if( !infoPtr ) {
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "void BBLens::AdjustSigma()", 
           "Cannot find ETinfo" ) );
  }
  
  sigma[0] = sqrt( infoPtr->beta.hor*emittance[0] );
  sigma[1] = sqrt( infoPtr->beta.ver*emittance[1] );
}


Vector BBLens::NormalizedEField( double arg_x, double arg_y )
{
  Vector  retvec(3);
  char    normal;
  std::complex<double>  z;
  double  x, y;
  double  sigmaX, sigmaY, ds, meanSigma;
  std::complex<double>  arg1, arg2;
  double  tmp1,r;
  std::complex<double>  retarg1, retarg2;
  enum    { ur, ul, lr, ll } quadrant;

  x = arg_x;  
  y = arg_y;
  sigmaX = sigma[0];
  sigmaY = sigma[1];

  // Asymptotic limit ...
  if( ( sigmaX == 0.0 ) && ( sigmaY == 0.0 ) ) {
    r = x*x + y*y;
    if( r < 1.0e-20 ) {
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
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
  sigmaX = sigma[0];
  sigmaY = sigma[1];

  // Asymptotic limit ...
  if( ( sigmaX == 0.0 ) && ( sigmaY == 0.0 ) ) {
    r = x*x + y*y;
    if( r.standardPart() < 1.0e-20 ) {
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
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


const char* BBLens::Type() const 
{ 
  return "BBLens"; 
}


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

void BBLens::GetSigma( double* sgm ) {
  for( int i = 0; i < 3; i++ ) sgm[i] = sigma[i];
}

