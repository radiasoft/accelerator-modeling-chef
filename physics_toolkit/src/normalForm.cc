/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   1.0                    
******                                    
******  File:      normalForm.cc
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
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

/*
**
** Utility function.
**
** Calculates the nonresonant normal form
** of a one-turn map.
**
** --- Leo Michelotti
** --- First version: November 4, 1993
** --- Final version: March 3, 1994
**
*/

#include "GenericException.h"
#include "mxyzptlk.h"

#define MLT1  1.0e-6

using namespace std;
using FNAL::Complex;

char sh0( const IntArray& index, const Complex& /* value */ ) {
 if( index(0) != index(3) + 1 ) return 0;
 if( index(1) != index(4) )     return 0;
 if( index(2) != index(5) )     return 0;
 return 1;
}

char sh1( const IntArray& index, const Complex& /* value */ ) {
 if( index(0) != index(3) )     return 0;
 if( index(1) != index(4) + 1 ) return 0;
 if( index(2) != index(5) )     return 0;
 return 1;
}

char sh2( const IntArray& index, const Complex& /* value */ ) {
 if( index(0) != index(3) )     return 0;
 if( index(1) != index(4) )     return 0;
 if( index(2) != index(5) + 1 ) return 0;
 return 1;
}

char sh3( const IntArray& index, const Complex& /* value */ ) {
 if( index(0) != index(3) - 1 ) return 0;
 if( index(1) != index(4) )     return 0;
 if( index(2) != index(5) )     return 0;
 return 1;
}

char sh4( const IntArray& index, const Complex& /* value */ ) {
 if( index(0) != index(3) )     return 0;
 if( index(1) != index(4) - 1 ) return 0;
 if( index(2) != index(5) )     return 0;
 return 1;
}

char sh5( const IntArray& index, const Complex& /* value */ ) {
 if( index(0) != index(3) )     return 0;
 if( index(1) != index(4) )     return 0;
 if( index(2) != index(5) - 1 ) return 0;
 return 1;
}

// ===============================================================

void normalForm( const Mapping& theMapping, /* input */
                 int            maxOrder,   /* input */
                 MatrixC*       Bptr,
                 CLieOperator*  N,
                 CLieOperator*  T ) {

 // typedef char (*MX_R_FUNCCPTR)(const IntArray&, const Complex&);
 // typedef char (*CFUNCPTR)(const IntArray&, const Complex& );
 static MX_R_FUNCCPTR shear[] = { sh0, sh1, sh2, sh3, sh4, sh5 };
 int i, j;

      /* CAUTION */  // A little test
      /* CAUTION */  if( !(theMapping.IsNilpotent()) ) {
      /* CAUTION */   throw( GenericException( __FILE__, __LINE__, 
      /* CAUTION */          "void normalForm( const Mapping& theMapping, /* input */", 
      /* CAUTION */          "This version only supports nilpotent maps." ) );
      /* CAUTION */  }

 // Establishing linear normal form coordinates
 MatrixD  A;
 A = theMapping.Jacobian();
 MatrixC B;
 B = A.eigenVectors();

 // Normalizing the linear normal form coordinates
 Complex  mi( 0., -1. );
 MatrixD  J( "J", 6 );
 MatrixC  Nx;
 Nx = ( B.transpose() * J * B * J ) * mi;

 for( i = 0; i < 6; i++ ) {
  Nx( i, i ) = 1.0 / sqrt( abs( Nx(i,i) ) );  
                                       // ??? "abs" should not be necessary,
                                       // ??? but Holt is finding some
                                       // ??? counterexamples.(!!??)
                                       // ???
                                       // ??? In principle, could get divide
                                       // ??? by zero here.

  if( abs( ( (Complex) 1.0 ) - Nx(i,i) ) < 1.0e-10 ) Nx(i,i) = 1.0;

      /* CAUTION */   for( j = 0; j < 6; j++ ) {
      /* CAUTION */    if( j == i ) continue;
      /* CAUTION */    else if( abs( Nx(i,j) ) > MLT1) {
      /* CAUTION */     ostringstream uic;
      /* CAUTION */     uic  << "Nondiagonal element in BJB^TJ; abs( Nx( " 
                             << i << ", " << j << " ) ) = " 
                             << abs(Nx(i,j));
      /* CAUTION */     throw( GenericException( __FILE__, __LINE__, 
      /* CAUTION */            "void normalForm( const Mapping& theMapping, /* input */", 
      /* CAUTION */            uic.str().c_str() ) );
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

 // Store the result
 *Bptr = B;


 // Some useful matrices
 MatrixC Binv;
 Binv = B.inverse();
 MatrixC D;
 D = Binv * A * B;
 MatrixC Dinv;
 Dinv = D.inverse();

 MatrixC lambda(1,D.cols());
 for( i = 0; i < 6; i++ ) {
   lambda(i) = D(i,i);
 }
      /* CAUTION */  for( i = 0; i < 6; i++ ) {
      /* CAUTION */   if( fabs( abs(lambda(i)) - 1.0 ) > MLT1 ) {
      /* CAUTION */    ostringstream uic;
      /* CAUTION */    uic  << "For now, only elliptic fixed points allowed:"
                               " |lambda( " << i <<  " )| = " 
                            << std::abs(lambda(i));
      /* CAUTION */    throw( GenericException( __FILE__, __LINE__, 
      /* CAUTION */           "void normalForm( const Mapping& theMapping, /* input */", 
      /* CAUTION */           uic.str().c_str() ) );
      /* CAUTION */   }
      /* CAUTION */  }
      /* CAUTION */  
      /* CAUTION */  // A little checking and cleaning.
      /* CAUTION */  for( i = 0; i < 6; i++ ) {
      /* CAUTION */   if( fabs( abs(Dinv(i,i)) - 1.0 ) > MLT1 ) {
      /* CAUTION */    ostringstream uic;
      /* CAUTION */    uic  << "For now, only elliptic maps allowed: | Dinv( " 
                            << i << ", " << i << " ) | = " 
                            << std::abs(Dinv(i,i));
      /* CAUTION */    throw( GenericException( __FILE__, __LINE__, 
      /* CAUTION */           "void normalForm( const Mapping& theMapping, /* input */", 
      /* CAUTION */           uic.str().c_str() ) );
      /* CAUTION */   }
      /* CAUTION */   for( j = 0; j < 6; j++ ) {
      /* CAUTION */    if( j == i ) continue;
      /* CAUTION */    else if( abs( Dinv(i,j) ) > MLT1) {
      /* CAUTION */     throw( GenericException( __FILE__, __LINE__, 
      /* CAUTION */            "void normalForm( const Mapping& theMapping, /* input */", 
      /* CAUTION */            "Impossible error has occurred!" ) );
      /* CAUTION */    }
      /* CAUTION */    else Dinv(i,j) = complex_0;
      /* CAUTION */   }
      /* CAUTION */  }
      /* CAUTION */ 
      /* CAUTION */  for( i = 0; i < 6; i++ ) {
      /* CAUTION */   if( fabs( abs(D(i,i)) - 1.0 ) > MLT1 ) {
      /* CAUTION */    ostringstream uic;
      /* CAUTION */    uic  << "For now, only elliptic maps allowed: | D( " 
                            << i << ", " << i << " ) | = " 
                            << std::abs(D(i,i));
      /* CAUTION */    throw( GenericException( __FILE__, __LINE__, 
      /* CAUTION */           "void normalForm( const Mapping& theMapping, /* input */", 
      /* CAUTION */           uic.str().c_str() ) );
      /* CAUTION */   }
      /* CAUTION */ 
      /* CAUTION */   for( j = 0; j < 6; j++ ) {
      /* CAUTION */    if( j == i ) continue;
      /* CAUTION */    else if( abs( D(i,j) ) > MLT1) {
      /* CAUTION */     throw( GenericException( __FILE__, __LINE__, 
      /* CAUTION */            "void normalForm( const Mapping& theMapping, /* input */", 
      /* CAUTION */            "An impossible error has occurred!" ) );
      /* CAUTION */    }
      /* CAUTION */    else D(i,j) = complex_0;
      /* CAUTION */   }
      /* CAUTION */  }


 // The original near-identity transformation
 MappingC CL1;
 CL1 = theMapping;


 MappingC id( "ident" );
 MappingC calN;
 calN = Binv*CL1( B*(Dinv*id) );
 // ??? REMOVE: old error:  calN = Dinv*(Binv*CL1( B*id ));
 MappingC mapT;



 // And the rest ...
 Complex          factor, denom, temp;
 int              l, ll;
 JLCterm*         q;
 JetC             scr;
 MappingC         reg;
 MappingC         doc;

 for( int k = 0; k <= maxOrder - 2; k++ ) {
  reg = id;
  ll = 0;
  while( ll < k ) reg = N[ll++].expMap( -complex_1, reg );
  reg = calN( reg );
  // -------------
  reg = reg.filter( k+2, k+2 );
  N[k] = reg.filter( shear );

  doc = N[k] - reg;
  for( i = 0; i < 6; i++ ) {
   scr.clear();
   doc(i).resetIterator();
   while((  q = doc(i).stepIterator()  )) {
    factor = 1.0;
    for( j = 0; j < 6; j++ ) {
     temp = complex_1 / lambda(j);
     for( l = 0; l < q->exponents()(j); l++ ) {
      factor *= temp;
     }
     // REMOVE: factor *= pow( complex_1 / lambda(j), q->exponents()(j) );
     // REMOVE: factor *= pow( 1.0 / lambda(j), q->exponents()(j) );
    }
    factor *= lambda(i);

    // Either absorption or resonance subtraction ... 
    denom = factor - complex_1;
    if( abs( denom ) <= 1.0e-7 ) {
      N[k](i).addTerm( new JLCterm( q->exponents(), 
                                    - q->coefficient(), 
                                    CL1.Env() ) );
    }
    else {
      q->coefficient() /= denom;
      scr.addTerm( new JLCterm( *q ) );
    }

   }
   T[k].SetComponent( i, scr );
  }
  // T[k].reset();    // ??? Probably unnecessary


  // Prepare for the next order
  reg = Dinv*id;
  mapT = T[k].expMap( complex_1, id );
  reg = mapT( reg );
  reg = D*reg;
  reg = calN( reg );
  mapT = T[k].expMap( -complex_1, id );
  calN = mapT( reg );
  // In one line:
  // calN = T[k].expMap( -1.0, calN( D*( T[k].expMap( 1.0, Dinv*id ) ) ) );
 }

}
