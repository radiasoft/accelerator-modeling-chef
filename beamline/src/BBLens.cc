#if HAVE_CONFIG_H
#include <config.h>
#endif
/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      BBLens.cc
******                                                                
******  Copyright (c) 1991 Universities Research Association, Inc.    
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
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


#include "JetC.h"
#include "BBLens.h"
#include "EdwardsTeng.h"

using namespace std;
using FNAL::Complex;

extern Complex w( Complex );
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


BBLens::BBLens( const BBLens& x )
: bmlnElmnt( (bmlnElmnt&) x )
{
  int i;
  for( i = 0; i < 3; i++ ) emittance[i] = x.emittance[i];
  for( i = 0; i < 3; i++ ) sigma[i] = x.sigma[i];
  gamma = x.gamma;
  num = x.num;
  useRound = x.useRound;
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
    cout << "*** ERROR ***                                    \n"
            "*** ERROR *** BBLens::AdjustSigma                \n"
            "*** ERROR *** Cannot find ETinfo                 \n"
            "*** ERROR ***                                    \n"
         << endl;
    exit(1);
  }
  
  sigma[0] = sqrt( infoPtr->beta.hor*emittance[0] );
  sigma[1] = sqrt( infoPtr->beta.ver*emittance[1] );
}


Vector BBLens::NormalizedEField( double arg_x, double arg_y )
{
  Vector  retvec(3);
  char    normal;
  Complex z;
  double  x, y;
  double  sigmaX, sigmaY, ds, meanSigma;
  Complex arg1, arg2;
  double  tmp1,r;
  Complex retarg1, retarg2;
  enum    { ur, ul, lr, ll } quadrant;

  x = arg_x;  
  y = arg_y;
  sigmaX = sigma[0];
  sigmaY = sigma[1];

  // Asymptotic limit ...
  if( ( sigmaX == 0.0 ) && ( sigmaY == 0.0 ) ) {
    r = x*x + y*y;
    if( r < 1.0e-20 ) {
      cerr << "\n";
      cerr << "*** ERROR ***                                 \n";
      cerr << "*** ERROR *** BBLens::NormalizedEField        \n";
      cerr << "*** ERROR *** Asymptotic limit                \n";
      cerr << "*** ERROR *** r seems too small.              \n";
      cerr << "*** ERROR ***                                 \n";
      exit(1);
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
      cerr << "\n";
      cerr << "*** ERROR ***                                 \n";
      cerr << "*** ERROR *** BBLens::NormalizedEField        \n";
      cerr << "*** ERROR *** Asymptotic limit                \n";
      cerr << "*** ERROR *** r seems too small.              \n";
      cerr << "*** ERROR ***                                 \n";
      exit(1);
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
  arg1 = ((JetC) (x/ds)) + complex_i*((JetC) (y/ds) );
  ratio = sigmaY/sigmaX;
  arg2 = ((JetC) ((x*ratio)/ds)) + complex_i*((JetC) ((y/ratio)/ds));

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

