/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   2.0
******                                    
******  File:      BmlUtil.cc
******             Source code for Beamline utility methods.
******                                                                
******  Copyright (c) 2003  Universities Research Association, Inc.   
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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "GenericException.h"
#include "BmlUtil.h"
#include "TMatrix.h"

using FNAL::Complex;
using namespace std;

const double BmlUtil::mlt1 = 1.0e-6;

void BmlUtil::normalize( MatrixC& B )
{
  // This code is lifted from emittanceDilution.cc,
  // which lifted it from normalForm.cc

  int i, j;

  // Normalizing the linear normal form coordinates
  Complex  mi( 0., -1. );
  MatrixD  J( "J", 6 );
  MatrixC  Nx;
  Nx = ( B.transpose() * J * B * J ) * mi;

  for( i = 0; i < 6; i++ ) {
   Nx( i, i ) = 1.0 / sqrt( abs( Nx(i,i) ) );  
   if( abs( ( (Complex) 1.0 ) - Nx(i,i) ) < 1.0e-10 ) Nx(i,i) = 1.0;

       /* CAUTION */   for( j = 0; j < 6; j++ ) {
       /* CAUTION */    if( j == i ) continue;
       /* CAUTION */    else if( abs( Nx(i,j) ) > BmlUtil::mlt1 ) {
       /* CAUTION */          ostringstream uic;
       /* CAUTION */          uic << "Nondiagonal element in BJB^TJ: abs( Nx( " 
                                  << i << ", " << j << " ) ) = " 
                                  << std::abs(Nx(i,j));
       /* CAUTION */          throw( GenericException( __FILE__, __LINE__, 
       /* CAUTION */                 "void BmlUtil::normalize( MatrixC& B )",
       /* CAUTION */                 uic.str().c_str() ) );
       /* CAUTION */    }
       /* CAUTION */    else Nx(i,j) = complex_0;
       /* CAUTION */   }
  }

  B = B*Nx;

  // Try to get the phase correct ...
  Complex m0, cm0, m1, cm1;
  m0  = B(0,0)/abs(B(0,0));
  cm0 = conj(m0);
  m1  = B(1,1)/abs(B(1,1));
  cm1 = conj(m1);
  for( i = 0; i < 6; i++ ) {
    B(i,0) *= cm0;
    B(i,3) *= m0;
    B(i,1) *= cm1;
    B(i,4) *= m1;
  }
  if( imag(B(3,0)) > 0.0 ) {
    for( i = 0; i < 6; i++ ) {
      m0 = B(i,0);
      B(i,0) = B(i,3);
      B(i,3) = m0;
    }
  }
  if( imag(B(4,1)) > 0.0 ) {
    for( i = 0; i < 6; i++ ) {
      m0 = B(i,1);
      B(i,1) = B(i,4);
      B(i,4) = m0;
    }
  }
}
