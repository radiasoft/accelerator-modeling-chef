////////////////////////////////////////////////////////////
// 
// File:          normalForm.cc
//                                                                
// -------------------------------------------------------------
// 
// Copyright Universities Research Association, Inc./ Fermilab    
//           All Rights Reserved                             
//
// Usage, modification, and redistribution are subject to terms          
// of the License supplied with this software.
// 
// Software and documentation created under 
// U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
// The U.S. Government retains a world-wide non-exclusive, 
// royalty-free license to publish or reproduce documentation 
// and software for U.S. Government purposes. This software 
// is protected under the U.S. and Foreign Copyright Laws.
// 
// -------------------------------------------------------------
// 
// Author:        Leo Michelotti
// 
// REVISION HISTORY
// 
// Thursday. November 4, 1993.    (original version)
// 
// Thursday. October 25, 2001.
// * Specialized to 2D phase space.
//   - see introductory comments in file lpsDemo.cc.
// 
// Friday. May 30, 2008.          (this file)
// * Minor tweaks of implementation because of changes
//   in lower level libraries.
// 
////////////////////////////////////////////////////////////
// 
// Ancient and honorable utility function to calculate
// the nonresonant normal form of a one-turn map.
// 
////////////////////////////////////////////////////////////

#include <basic_toolkit/Matrix.h>
#include <mxyzptlk/mxyzptlk.h>

#define MLT1  1.0e-6

using namespace std;

bool sh0( const IntArray& index, const std::complex<double>& /* value */ ) {
  if( index(0) != index(1) + 1 ) return false;
  return true;
}

bool sh1( const IntArray& index, const std::complex<double>& /* value */ ) {
  if( index(0) != index(1) - 1 ) return false;
  return true;
}

// ===============================================================

void twod_normalForm( const Mapping& theMapping, /* input */
                      int            maxOrder,   /* input */
                      MatrixC*       Bptr,
                      CLieOperator*  N,
                      CLieOperator*  T ) 
{
  const std::complex<double> complex_0( 0.0, 0.0 );
  const std::complex<double> complex_1( 1.0, 0.0 );
  const std::complex<double> mi( 0., -1. );

  bool (*shear[])(   const IntArray&
                   , const std::complex<double>& ) 
    = { sh0, sh1 };

  int i, j;
 
       /* CAUTION */  // A little test
       /* CAUTION */  if( !(theMapping.IsNilpotent()) ) {
       /* CAUTION */  cout  << "\n"
       /* CAUTION */        << "***                                        \n"
       /* CAUTION */        << "*** SORRY:                                 \n"
       /* CAUTION */        << "*** SORRY: void normalForm()               \n"
       /* CAUTION */        << "*** SORRY: This version only supports      \n"
       /* CAUTION */        << "*** SORRY: nilpotent maps.                 \n"
       /* CAUTION */        << "*** SORRY:                                 \n"
       /* CAUTION */        << endl;
       /* CAUTION */   exit(0);
       /* CAUTION */  }
 
  // Establishing linear normal form coordinates
  MatrixD  A;
  A = theMapping.Jacobian();
  MatrixC B;
  B = A.eigenVectors();
 
  // Normalizing the linear normal form coordinates
  MatrixD  J( "J", 2 );
  MatrixC  Nx;
  Nx = ( B.transpose() * J * B * J ) * mi;
 
  for( i = 0; i < 2; i++ ) {
   Nx( i, i ) = 1.0 / sqrt( abs( Nx(i,i) ) );  
 					// ??? In principle, could get divide
 					// ??? by zero here.
 
   if( abs( ( (std::complex<double>) 1.0 ) - Nx(i,i) ) < 1.0e-10 ) Nx(i,i) = 1.0;
 
       /* CAUTION */   for( j = 0; j < 2; j++ ) {
       /* CAUTION */    if( j == i ) continue;
       /* CAUTION */    else if( abs( Nx(i,j) ) > MLT1) {
       /* CAUTION */          cout << "\n"
       /* CAUTION */               << "***                                        \n"
       /* CAUTION */               << "*** ERROR:                                 \n"
       /* CAUTION */               << "*** ERROR: void normalForm()               \n"
       /* CAUTION */               << "*** ERROR: Nondiagonal element in BJB^TJ   \n"
       /* CAUTION */               << "*** ERROR: abs( Nx( " << i << ", " << j 
 				   << " ) ) = " << abs(Nx(i,j)) << "\n"
       /* CAUTION */               << "*** ERROR:                                 \n"
       /* CAUTION */               << endl;
       /* CAUTION */          exit(0);
       /* CAUTION */         }
       /* CAUTION */        else Nx(i,j) = complex_0;
       /* CAUTION */   }
  }
 
  B = B*Nx;
 
 
  // // Try to get the phase correct ...
  // std::complex<double> m0, cm0, m1, cm1;
  // m0  = B(0,0)/abs(B(0,0));
  // cm0 = conj(m0);
  // m1  = B(1,1)/abs(B(1,1));
  // cm1 = conj(m1);
  // for( i = 0; i < 6; i++ ) {
  //   B(i,0) *= cm0;
  //   B(i,3) *= m0;
  //   B(i,1) *= cm1;
  //   B(i,4) *= m1;
  // }
  // if( imag(B(3,0)) > 0.0 ) {
  //   for( i = 0; i < 6; i++ ) {
  //     m0 = B(i,0);
  //     B(i,0) = B(i,3);
  //     B(i,3) = m0;
  //   }
  // }
  // if( imag(B(4,1)) > 0.0 ) {
  //   for( i = 0; i < 6; i++ ) {
  //     m0 = B(i,1);
  //     B(i,1) = B(i,4);
  //     B(i,4) = m0;
  //   }
  // }
 
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
  for( i = 0; i < D.cols(); i++ ) {
    lambda(i) = D(i,i);
  }
 
       /* CAUTION */  for( i = 0; i < 2; i++ ) {
       /* CAUTION */   if( fabs( abs(lambda(i)) - 1.0 ) > MLT1 ) {
       /* CAUTION */    cout << "\n"
       /* CAUTION */         << "***                                            \n"
       /* CAUTION */         << "*** SORRY:                                     \n"
       /* CAUTION */         << "*** SORRY: void normalForm()                   \n"
       /* CAUTION */         << "*** SORRY: For now, I am only allowing         \n"
       /* CAUTION */         << "*** SORRY: maps with an elliptic fixed point   \n"
       /* CAUTION */         << "*** SORRY: at the origin.  This will be        \n"
       /* CAUTION */         << "*** SORRY: loosened as soon as I'm sure        \n"
       /* CAUTION */         << "*** SORRY: everything is working properly.     \n"
       /* CAUTION */         << "*** SORRY:                                     \n"
       /* CAUTION */         << "*** SORRY: lambda( " << i 
 			     <<                          " ) has magnitude = " 
 			     << abs(lambda(i)) << "    \n"
       /* CAUTION */         << "*** SORRY: lambda( 0 ) has magnitude = " 
 			     << abs(lambda(0)) 
 			     << "  Distance from 1 = "
 			     << fabs( abs(lambda(0)) - 1.0 ) 
 			     << "    \n"
       /* CAUTION */         << "*** SORRY: lambda( 1 ) has magnitude = " 
 			     << abs(lambda(1)) 
 			     << "  Distance from 1 = "
 			     << fabs( abs(lambda(1)) - 1.0 ) 
 			     << "    \n"
       /* CAUTION */         << "*** SORRY: lambda( 2 ) has magnitude = " 
 			     << abs(lambda(2)) 
 			     << "  Distance from 1 = "
 			     << fabs( abs(lambda(2)) - 1.0 ) 
 			     << "    \n"
       /* CAUTION */         << "*** SORRY: lambda( 3 ) has magnitude = " 
 			     << abs(lambda(3)) 
 			     << "  Distance from 1 = "
 			     << fabs( abs(lambda(3)) - 1.0 ) 
 			     << "    \n"
       /* CAUTION */         << "*** SORRY: lambda( 4 ) has magnitude = " 
 			     << abs(lambda(4)) 
 			     << "  Distance from 1 = "
 			     << fabs( abs(lambda(4)) - 1.0 ) 
 			     << "    \n"
       /* CAUTION */         << "*** SORRY: lambda( 5 ) has magnitude = " 
 			     << abs(lambda(5)) 
 			     << "  Distance from 1 = "
 			     << fabs( abs(lambda(5)) - 1.0 ) 
 			     << "    \n"
       /* CAUTION */         << "*** SORRY:                                     \n"
       /* CAUTION */         << endl;
       /* CAUTION */    cout << "The eigenvector matrix follows: " 
       /* CAUTION */         << endl;
       /* CAUTION */    cout << B;
       /* CAUTION */    exit(0);
       /* CAUTION */   }
       /* CAUTION */  }
       /* CAUTION */  
       /* CAUTION */  // A little checking and cleaning.
       /* CAUTION */  for( i = 0; i < 2; i++ ) {
       /* CAUTION */   if( fabs( abs(Dinv(i,i)) - 1.0 ) > MLT1 ) {
       /* CAUTION */    cout << "\n"
       /* CAUTION */         << "***                                            \n"
       /* CAUTION */         << "*** SORRY:                                     \n"
       /* CAUTION */         << "*** SORRY: void normalForm()                   \n"
       /* CAUTION */         << "*** SORRY: For now, I am only allowing         \n"
       /* CAUTION */         << "*** SORRY: maps with an elliptic fixed point   \n"
       /* CAUTION */         << "*** SORRY: at the origin.  This will be        \n"
       /* CAUTION */         << "*** SORRY: loosened as soon as I'm sure        \n"
       /* CAUTION */         << "*** SORRY: everything is working properly.     \n"
       /* CAUTION */         << "*** SORRY:                                     \n"
       /* CAUTION */         << "*** SORRY: Dinv( " << i << ", " << i
 			     << " ) has magnitude = " << abs(Dinv(i,i)) <<    " \n"
       /* CAUTION */         << "*** SORRY:                                     \n"
 			     << endl;
       /* CAUTION */    exit(0);
       /* CAUTION */   }
       /* CAUTION */   for( j = 0; j < 2; j++ ) {
       /* CAUTION */    if( j == i ) continue;
       /* CAUTION */    else if( abs( Dinv(i,j) ) > MLT1) {
       /* CAUTION */          cout << "\n"
       /* CAUTION */               << "***                                        \n"
       /* CAUTION */               << "*** ERROR:                                 \n"
       /* CAUTION */               << "*** ERROR: void normalForm()               \n"
       /* CAUTION */               << "*** ERROR: A horrible, inexplicable error  \n"
       /* CAUTION */               << "*** ERROR: has occurred.                   \n"
       /* CAUTION */               << "*** ERROR:                                 \n"
       /* CAUTION */               << endl;
       /* CAUTION */          exit(0);
       /* CAUTION */         }
       /* CAUTION */        else Dinv(i,j) = complex_0;
       /* CAUTION */   }
       /* CAUTION */  }
       /* CAUTION */ 
       /* CAUTION */  for( i = 0; i < 2; i++ ) {
       /* CAUTION */   if( fabs( abs(D(i,i)) - 1.0 ) > MLT1 ) {
       /* CAUTION */    cout << "\n"
       /* CAUTION */         << "***                                          \n"
       /* CAUTION */         << "*** SORRY:                                   \n"
       /* CAUTION */         << "*** SORRY: void normalForm()                 \n"
       /* CAUTION */         << "*** SORRY: For now, I am only allowing       \n"
       /* CAUTION */         << "*** SORRY: maps with an elliptic fixed point \n"
       /* CAUTION */         << "*** SORRY: at the origin.  This will be      \n"
       /* CAUTION */         << "*** SORRY: loosened as soon as I'm sure      \n"
       /* CAUTION */         << "*** SORRY: everything is working properly.   \n"
       /* CAUTION */         << "*** SORRY:                                   \n"
       /* CAUTION */         << "*** SORRY: D( " << i << ", " << i 
 			     << " ) has magnitude = " << abs(D(i,i))   << "   \n"
       /* CAUTION */         << "*** SORRY:                                   \n"
       /* CAUTION */         << endl;
       /* CAUTION */    exit(0);
       /* CAUTION */   }
       /* CAUTION */ 
       /* CAUTION */   for( j = 0; j < 2; j++ ) {
       /* CAUTION */    if( j == i ) continue;
       /* CAUTION */    else if( abs( D(i,j) ) > MLT1) {
       /* CAUTION */          cout << "\n"
       /* CAUTION */               << "***                                        \n"
       /* CAUTION */               << "*** ERROR:                                 \n"
       /* CAUTION */               << "*** ERROR: void normalForm()               \n"
       /* CAUTION */               << "*** ERROR: A horrible, inexplicable error  \n"
       /* CAUTION */               << "*** ERROR: has occurred.                   \n"
       /* CAUTION */               << "*** ERROR:                                 \n"
       /* CAUTION */               << endl;
       /* CAUTION */          exit(0);
       /* CAUTION */         }
       /* CAUTION */        else D(i,j) = complex_0;
       /* CAUTION */   }
       /* CAUTION */  }
 
 
  // The original near-identity transformation
  MappingC CL1;
  CL1 = theMapping;

  MappingC id( "ident" );
  MappingC calN;
  #if 1
  calN = Binv*CL1( B*(Dinv*id) );
  #endif
  #if 0
  calN = Dinv*id;
  calN = B*calN;
  calN = CL1( calN );
  calN = Binv*calN;
  #endif
  // ??? REMOVE: old error:  calN = Dinv*(Binv*CL1( B*id ));
  MappingC mapT;
 
 
 
  // And the rest ...
  std::complex<double> factor, denom, temp;
  int              l, ll;
  const JLCterm*   q;
  #if 0
  JetC             scr;
  #endif
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
   for( i = 0; i < 2; i++ ) {
    #if 0
    scr.clear();
    #endif


   for ( JetC::iterator it = doc(i).begin() ; it != doc(i).end(); ++it ) {
     factor = 1.0;
     for( int j=0; j < 2; ++j) {
      temp = complex_1 / lambda(j);
      IntArray exponents =  it->exponents( doc(i).Env() ) ;

      for( int l=0;  l < exponents(j); ++l) {
       factor *= temp;
      }
     }
     factor *= lambda(i);
 
     // Either absorption or resonance subtraction ... 
     denom = factor - complex_1;
     if( abs( denom ) <= 1.0e-7 ) {
       N[k](i).addTerm( JLCterm( - it->coefficient(), it->offset_, it->weight_ ) );
     }
     else {
       #if 0
       scr.addTerm( JLCterm( *q ) );
       #endif
       #if 1
       T[k](i).addTerm( JLCterm( it->coefficient()/denom, it->offset_, it->weight_ ) );
       #endif
     }
 
    }
    #if 0
    T[k].SetComponent( i, scr );
    #endif
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
