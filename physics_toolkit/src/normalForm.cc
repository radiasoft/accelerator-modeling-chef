/*************************************************************************
**************************************************************************
**************************************************************************
******
******  PHYSICS TOOLKIT: Library of utilites and Sage classes
******             which facilitate calculations with the
******             BEAMLINE class library.
******
******  File:      normalForm.cc
******
******  Copyright  Universities Research Association, Inc / Fermilab.
******             All Rights Reserved
******
******  Author:    Leo Michelotti
******
******             Fermilab
******             P.O.Box 500
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
****** REVISION HISTORY
****** ----------------
****** Mar 2007       ostiguy@fnal.gov
****** - some efficiency improvements
****** - use new style Jet iterators.
******
****** Aug 2008       ostiguy@fnal
****** - templated version of "shear" function
******
****** Jan 2011       michelotti@fnal.gov
****** - Eric Stern extended the algorithm
******   from four to six dimensions by adopting
******   the normalization and ordering procedures
******   to the "longitudinal" sector.
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

#include <basic_toolkit/GenericException.h>
#include <physics_toolkit/NormalForm.h>
#include <basic_toolkit/Matrix.h>

#define MLT1  1.0e-6

using namespace std;

namespace {

std::complex<double> const complex_0 (0.0, 0.0);
std::complex<double> const complex_1 (1.0, 0.0);  
std::complex<double> const mi( 0., -1. );        // -i

template < unsigned int idx>
bool sh( IntArray const& index, std::complex<double> const& value ) {

 int k = ( idx < 3 ) ? 1 : -1; 

 for (int i=0; i<3; ++i ) {
   if( index[i] !=  index[3+i] + (i == idx) ? k : 0  ) return false;
 }
 return true;
}

} // anonymous namespace


// ===============================================================

void normalForm( Mapping const& theMapping, int maxOrder, MatrixC& B, 
                                std::vector<CLieOperator>&  N, std::vector<CLieOperator>&  T ) 
{

 bool (*shear[])( IntArray const&, std::complex<double> const& value ) 
    = { sh<0>, sh<1>, sh<2>, sh<3>, sh<4>, sh<5> };


      /* CAUTION */  // A little test
      /* CAUTION */  if( !(theMapping.IsNilpotent()) ) {
      /* CAUTION */   throw( GenericException( __FILE__, __LINE__, 
      /* CAUTION */          "void normalForm( const Mapping& theMapping, /* input */", 
      /* CAUTION */          "This version only supports nilpotent maps." ) );
      /* CAUTION */  }

 // Establishing linear normal form coordinates

 MatrixD  A = theMapping.jacobian();
          B = A.eigenVectors();

 // Normalizing the linear normal form coordinates

 static MatrixD const J = Matrix::Jmatrix(6 );

 MatrixC             Nx = ( B.transpose() * J * B * J ) * mi;

 for( int i=0; i < 6; ++i) {
  Nx[i][i]  = 1.0 / sqrt( abs( Nx[i][i] ) );  // ??? "abs" should not be necessary,
                                              // ??? In principle, could get divide
                                              // ??? by zero here.

  if( abs( ( (std::complex<double>) 1.0 ) - Nx[i][i] ) < 1.0e-10 ) Nx[i][i] = 1.0;

      /* CAUTION */   for( int j=0; j<6; ++j) {
      /* CAUTION */    if( j == i ) continue;
      /* CAUTION */    else if( abs( Nx[i][j] ) > MLT1) {
      /* CAUTION */     ostringstream uic;
      /* CAUTION */     uic  << "Nondiagonal element in BJB^TJ; abs( Nx( " 
                             << i << ", " << j << " ) ) = " 
                             << abs(Nx[i][j]);
      /* CAUTION */     throw( GenericException( __FILE__, __LINE__, 
      /* CAUTION */            "void normalForm( const Mapping& theMapping, /* input */", 
      /* CAUTION */            uic.str().c_str() ) );
      /* CAUTION */    }
      /* CAUTION */    else Nx[i][j] = complex_0;
      /* CAUTION */   }
 }

 B = B*Nx;

 // Try to get the phase correct ...

 std::complex<double> m0  = B[0][0]/abs(B[0][0]);
 std::complex<double> cm0 = conj(m0);
 std::complex<double> m1  = B[1][1]/abs(B[1][1]);
 std::complex<double> cm1 = conj(m1);
 std::complex<double> m2  = B[2][2]/abs(B[2][2]);
 std::complex<double> cm2 = conj(m2);

 for( int i=0; i < 6; ++i) {
   B[i][0] *= cm0;
   B[i][3] *= m0;
   B[i][1] *= cm1;
   B[i][4] *= m1;
   B[i][2] *= cm2;
   B[i][5] *= m2;
 }
 if( imag(B[3][0]) > 0.0 ) {
   for( int i=0; i < 6; ++i) {
     m0 = B[i][0];          // NOTE: the variable m0 is reused here and
     B[i][0] = B[i][3];     // below as a dummy variable. This nullifies
     B[i][3] = m0;          // its previous interpretation.
   }
 }
 if( imag(B[4][1]) > 0.0 ) {
   for( int i = 0; i < 6; ++i) {
     m0 = B[i][1];
     B[i][1] = B[i][4];
     B[i][4] = m0;
   }
 }
 if( imag(B[5][2]) > 0.0 ) {
   for( int i = 0; i < 6; ++i) {
     m0 = B[i][2];
     B[i][2] = B[i][5];
     B[i][5] = m0;
   }
 }


 // Some useful matrices

 MatrixC Binv = B.inverse();
 MatrixC D    = Binv * A * B;
 MatrixC Dinv = D.inverse();

 MatrixC lambda(1,D.cols());

 for( int i=0; i< D.cols(); ++i) {
   lambda(i) = D[i][i];
 }
      /* CAUTION */  for( int i=0; i<6; ++i) {
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
      /* CAUTION */  for( int i=0; i<6; ++i) {
      /* CAUTION */   if( fabs( abs(Dinv[i][i]) - 1.0 ) > MLT1 ) {
      /* CAUTION */    ostringstream uic;
      /* CAUTION */    uic  << "For now, only elliptic maps allowed: | Dinv( " 
                            << i << ", " << i << " ) | = " 
                            << std::abs(Dinv[i][i]);
      /* CAUTION */    throw( GenericException( __FILE__, __LINE__, 
      /* CAUTION */           "void normalForm( const Mapping& theMapping, /* input */", 
      /* CAUTION */           uic.str().c_str() ) );
      /* CAUTION */   }
      /* CAUTION */   for( int j=0; j<6; ++j) {
      /* CAUTION */    if( j == i ) continue;
      /* CAUTION */    else if( abs( Dinv[i][j] ) > MLT1) {
      /* CAUTION */     throw( GenericException( __FILE__, __LINE__, 
      /* CAUTION */            "void normalForm( const Mapping& theMapping, /* input */", 
      /* CAUTION */            "Impossible error has occurred!" ) );
      /* CAUTION */    }
      /* CAUTION */    else Dinv[i][j] = complex_0;
      /* CAUTION */   }
      /* CAUTION */  }
      /* CAUTION */ 
      /* CAUTION */  for( int i = 0; i<6; ++i) {
      /* CAUTION */   if(  abs( abs(D[i][i]) - 1.0 ) > MLT1 ) {
      /* CAUTION */    ostringstream uic;
      /* CAUTION */    uic  << "For now, only elliptic maps allowed: | D( " 
                            << i << ", " << i << " ) | = " 
                            << std::abs(D[i][i]);
      /* CAUTION */    throw( GenericException( __FILE__, __LINE__, 
      /* CAUTION */           "void normalForm( const Mapping& theMapping, /* input */", 
      /* CAUTION */           uic.str().c_str() ) );
      /* CAUTION */   }
      /* CAUTION */ 
      /* CAUTION */   for( int j=0; j < 6; ++j) {
      /* CAUTION */    if( j == i ) continue;
      /* CAUTION */    else if( abs( D[i][j] ) > MLT1) {
      /* CAUTION */     throw( GenericException( __FILE__, __LINE__, 
      /* CAUTION */            "void normalForm( const Mapping& theMapping, /* input */", 
      /* CAUTION */            "An impossible error has occurred!" ) );
      /* CAUTION */    }
      /* CAUTION */    else D[i][j] =  complex_0;
      /* CAUTION */   }
      /* CAUTION */  }


 // The original near-identity transformation

 static MappingC const id( "I" );

 MappingC CL1 = theMapping;
 MappingC calN = Binv*CL1( B*(Dinv*id) );
 MappingC mapT(6);

 // And the rest ...
 
 for( int k=0; k <= maxOrder-2; ++k) {

  MappingC reg = id;
  int ll   = 0;
  while( ll < k ) reg = N[ll++].expMap( -complex_1, reg );

  reg = calN( reg );
  // -------------
  reg = reg.filter( k+2, k+2 );
  N[k] = reg.filter( shear );

  MappingC doc = N[k] - reg;

  for( int i=0; i<6; ++i ) { 
 
    T[k][i] = JetC( T[k][i].Env() ); // FIXME ! reference point ambiguity ?
 
    for ( JetC::iterator it = doc[i].begin() ; it != doc[i].end(); ++it ) {
    
      std::complex<double> factor = 1.0;

      for( int j=0; j<6; ++j) {
        IntArray exponents        =  it->exponents( doc[i].Env() ) ;
        std::complex<double> temp = complex_1 / lambda(j);

        for( int l=0;  l < exponents[j]; ++l) {
          factor *= temp;
        }
      }

      factor *= lambda(i);

      // Either absorption or resonance subtraction ... 

      std::complex<double> denom = factor - complex_1;

      if( abs( denom ) <= 1.0e-7 ) {
        N[k][i].addTerm( JLCterm( - it->coefficient(),    it->offset_, it->weight_ ) );
      }
      else {
        T[k][i].addTerm( JLCterm( it->coefficient()/denom, it->offset_, it->weight_ ) );
      }
    }
  }

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
