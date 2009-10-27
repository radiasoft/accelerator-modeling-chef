/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      ConvertToTunes.cc
******  Version:   1.2
******                                                                
******  Copyright  Universities Research Association, Inc.   
******             All Rights Reserved                             
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
****** Mar 2007       ostiguy@fnal.gov
****** - use new-style STL-compatible Jet iterators
******
****** Feb 2008      ostiguy@fnal.gov
******
****** - multiple optimizations
******
**************************************************************************
*************************************************************************/
/*
** int ConvertNtoTunes( MappingC& nu, const CLieOperator& N )
**
** Converts the data in the normal form "Hamiltonian," N, into
** a MappingC, nu, that maps normal form coordinates into tunes.
**
** Original author: Leo Michelotti
** Date:            September 5, 1996
** 
** 
** Version 2.0      
** Added second function which takes a matrix argument and
** finds linear tunes.  This routine was lifted from EdwardsTeng
** and LattFuncSage.
** 
**                  Leo Michelotti
**                  December 1, 1998
*/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <basic_toolkit/iosetup.h>
#include <mxyzptlk/LieOperator.h>
#include <mxyzptlk/Mapping.h>
#include <basic_toolkit/Matrix.h>
#include <basic_toolkit/MathConstants.h>
#include <beamline/Particle.h>
#include <iomanip>

using namespace MathConstants;
using namespace std;
using FNAL::pcerr;
using FNAL::pcout;

namespace {

 typedef PhaseSpaceIndexing::index index;

index const i_x     = Particle::i_x;
index const i_npx   = Particle::i_npx;
index const i_y     = Particle::i_y;
index const i_npy   = Particle::i_npy;
index const i_cdt   = Particle::i_cdt;
index const i_ndp   = Particle::i_ndp;

} // namespace


#if 0 
//////////////////////////////  OBSOLETE /////////////////////////////////////////////////
enum tune_status { OK=0, TUNES_UNSTABLE, TUNES_NOTCONJUGATE, TUNES_EQUAL, TUNES_NOTSYMPLECTIC };

int filterTransverseTunes(  MatrixD const& mtrx,  Vector& nu )
{

    VectorC lambda = mtrx.eigenValues();
   
    for( int i=0; i<2; ++i) {
     if( abs( abs(lambda[i]) - 1.0 ) > 1.0e-4 ) {
      (*pcout) << "\n"
 	   << "*** ERROR ***                                     \n"
 	   << "*** ERROR ***                                     \n"
 	   << "*** ERROR *** filterTransverseTunes               \n"
 	   << "*** ERROR *** The lattice is linearly unstable.   \n"
 	   << "*** ERROR *** lambda[ " << i << " ] has magnitude = " 
 	   << abs(lambda[i])
 	   << "\n"
 	   << "*** ERROR ***                                     \n"
 	   << endl;
      return  TUNES_UNSTABLE;
     }
    }

    if( ( abs( lambda[i_x] - conj( lambda[i_npx] ) ) > 1.0e-4 )  ||
 	( abs( lambda[i_y] - conj( lambda[i_npy] ) ) > 1.0e-4 )
      ) {
      (*pcout) << "\n"
 	   << "*** ERROR *** filterTransverseTunes                \n"
 	   << "*** ERROR *** Conjugacy condition has been violated\n"
 	   << "*** ERROR *** The lattice may be linearly unstable.\n"
 	   << "*** ERROR *** Eigenvalues =                        \n"
 	   << "*** ERROR *** " << lambda << endl;
      return  TUNES_NOTCONJUGATE;
    }
   
    double csH = real( lambda[i_x] );    
    double csV = real( lambda[i_y] );

    MatrixD M( 2, 2 ), N( 2, 2 ), m( 2, 2 ), n( 2, 2 );
    MatrixD U =  MatrixD::Imatrix(2 ), S = MatrixD::Jmatrix(2); 
    MatrixC EV = mtrx.eigenVectors(); 
   
    M[0][0] = mtrx[0][0];     n[0][0] = mtrx[0][1];
    M[0][1] = mtrx[0][3];     n[0][1] = mtrx[0][4];
    M[1][0] = mtrx[3][0];     n[1][0] = mtrx[3][1];
    M[1][1] = mtrx[3][3];     n[1][1] = mtrx[3][4];
   
    m[0][0] = mtrx[1][0];     N[0][0] = mtrx[1][1];
    m[0][1] = mtrx[1][3];     N[0][1] = mtrx[1][4];
    m[1][0] = mtrx[4][0];     N[1][0] = mtrx[4][1];
    m[1][1] = mtrx[4][3];     N[1][1] = mtrx[4][4];
   
   
    if( abs( csH - csV ) < 1.0e-4 ) {
      (*pcout) << "\n"
    	    << "*** ERROR *** filterTransverseTunes                \n"
    	    << "*** ERROR *** \"Horizontal\" and \"vertical\" tunes\n"
    	    << "*** ERROR *** are too near each other for          \n"
    	    << "*** ERROR *** reasonable results.                  \n"
    	    << "*** ERROR ***                                      \n"
    	    << "*** ERROR *** The calculation is meaningless.      \n"
    	    << "*** ERROR ***                                        " << endl;
      return 2;
    }
    
    double dcos    = csH - csV;
    double cos2phi = ( M - N ).trace() / ( 2.0 *( dcos ) );
 
    if( abs( cos2phi - 1.0 ) < 1.0e-4 ) cos2phi =   1.0;  // ??? Rather coarse,
    if( abs( cos2phi + 1.0 ) < 1.0e-4 ) cos2phi = - 1.0;  // ??? isn't it?
    
    if( abs( cos2phi ) > 1.0 ) {
     (*pcout) << "\n"
    	   << "*** ERROR: filterTransverseTunes                \n"
    	   << "*** ERROR: cos( 2 phi ) = " 
    	   <<           setprecision(10) << cos2phi 
    	   <<                       "; has magnitude larger    \n"
    	   << "*** ERROR: than one.                            \n"
    	   << "*** ERROR:                                      \n"
    	   << "*** ERROR: Cannot continue calculation.         \n"
    	   << "*** ERROR:                                        " << endl;
     return 3;
    }
    
    if( cos2phi < 0.0 ) {
     std::swap( csH, csV);
     dcos    = - dcos;
     cos2phi = - cos2phi;
    }
  
    double sin2phi = sqrt( 1.0 - cos2phi*cos2phi );
    double tanphi  = sin2phi / ( 1.0 + cos2phi );
    
    Matrix D = ( abs(sin2phi) > 1.0e-8 ) ? -( m + S*n.transpose()*S.transpose() ) * ( 1.0 / ( dcos*sin2phi )) : Matrix::Imatrix(2);

    if( abs( D.determinant() - 1.0 ) > 1.0e-4 ) {
      (*pcout) << "\n"
    	    << "*** ERROR *** filterTransverseTunes                \n"
    	    << "*** ERROR *** The matrix D is non-symplectic.      \n"
    	    << "*** ERROR *** |D| = " << D.determinant() << "      \n"
    	    << "*** ERROR ***                                        " << endl;
      return TUNES_NOTSYMPLECTIC;
    }
        // ...... Edwards-Teng sign convention.

    if( D.trace() < 0.0 ) {
     D = -D;
     sin2phi = -sin2phi;
     tanphi  = -tanphi;
    }
    
    Matrix A = M - D.inverse()*m*tanphi;
    Matrix B = N + D*n*tanphi;

   
    // ......  First the "horizontal" ......
    MatrixD JH = A - csH*U;

    double snH = ( JH[0][1]  > 0.0 ) ? sqrt( 1.0 - csH*csH ) : - sqrt( 1.0 - csH*csH );
   
    // .......... A little test to keep everyone honest .....

    if( JH [0][0] != 0.0 )
     if( abs( ( JH[0][0] + JH[1][1] ) / ( JH[0][0] - JH[1][1] ) ) > 1.0e-4 ) {
      (*pcout) << endl;
      (*pcout) << "*** WARNING ***                                " << endl;
      (*pcout) << "*** WARNING *** filterTransverseTunes          " << endl;
      (*pcout) << "*** WARNING *** \"Horizontal\" matrix does not " << endl;
      (*pcout) << "*** WARNING *** pass symplecticity test.       " << endl;
      (*pcout) << "*** WARNING *** JH( 0, 0 ) = " << JH[0][0]       << endl;
      (*pcout) << "*** WARNING *** JH( 1, 1 ) = " << JH[1][1]       << endl;
      (*pcout) << "*** WARNING ***                                " << endl;
      (*pcout) << "*** WARNING *** The ratio is " 
 	   << fabs( ( JH[0][0] + JH[1][1] ) / ( JH[0][0] - JH[1][1] ) )
 	   << endl;
      (*pcout) << "*** WARNING ***                                " << endl;
     }
   
   
    // ......  Then  the "vertical" ......
    MatrixD JV = B - csV*U;

    double snV = ( JV[0][1]  > 0.0 ) ? sqrt( 1.0 - csV*csV ) : - sqrt( 1.0 - csV*csV );
  
    // .......... A little test to keep everyone honest .....
    if( JV[0][0] != 0.0 )
     if( abs( ( JV[0][0] + JV[1][1] ) / ( JV[0][0] - JV[1][1] ) ) > 1.0e-4 ) {
      (*pcout) << endl;
      (*pcout) << "*** WARNING ***                                " << endl;
      (*pcout) << "*** WARNING *** filterTransverseTunes          " << endl;
      (*pcout) << "*** WARNING *** \"Vertical\" matrix does not   " << endl;
      (*pcout) << "*** WARNING *** pass symplecticity test.       " << endl;
      (*pcout) << "*** WARNING *** JV( 0, 0 ) = " << JV[0][0]       << endl;
      (*pcout) << "*** WARNING *** JV( 1, 1 ) = " << JV[1][1]       << endl;
      (*pcout) << "*** WARNING ***                                " << endl;
      (*pcout) << "*** WARNING *** The ratio is " 
 	   << abs( ( JV[0][0] + JV[1][1] ) / ( JV[0][0] - JV[1][1] ) )
 	   << endl;
      (*pcout) << "*** WARNING ***                                " << endl;
    }
   
   
    nu = Vector(2);  

    double theta = atan2( snH, csH );
    if( theta < 0.0 )   theta += M_TWOPI;
    nu[0] = ( theta / M_TWOPI );

    theta = atan2( snV, csV );
    if( theta < 0.0 )   theta += M_TWOPI;
    nu[1] = ( theta / M_TWOPI );
 
  return 0;
}

#endif

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int ConvertNtoTunes( MappingC&  nu,  CLieOperator& N )
{

  static std::complex<double> const c_zero = std::complex<double>( 0.0, 0.0 );
  static std::complex<double> const c_i    = std::complex<double>( 0.0, 1.0 );

  int returnValue = 0;

  if( nu.Env() != N.Env() )        return 137;
  if( nu.Dim() != N.Dim() )        return 138;

  int sd = N.Env()->spaceDim();
  if( sd%2 != 0 )                  return 139;
  int sd2 = sd/2;

  // Environment is set ........................

  JetC__environment_ptr thisEnv = N.Env();

  IntArray  ndx( N.Env()->numVar() );

  std::complex<double> v;

  // Construct the Mapping .......................

  JetC y( thisEnv ); 

  for( int i=0; i < N.Dim(); ++i) {
    y = N[i];
    nu[i] = c_zero;

    for ( JetC::iterator it= y.begin(); it != y.end(); ++it ) { 
      v = it->coefficient();
      if((  v != c_zero  )) {

        // Reality check ...
        if( fabs( real(v) ) > 1.0e-10 * fabs( imag(v) ) ) {
          (*pcerr) << "WARNING:                              " << endl;
          (*pcerr) << "WARNING: " << __FILE__ << ", Line " << __LINE__ << endl;
          (*pcerr) << "WARNING: ConvertNToTunes              " << endl;
          (*pcerr) << "WARNING: Reality check was violated for exponent "
                   << it->exponents( y.Env())
                   << ".  " << endl;
          (*pcerr) << "WARNING: Offending ratio is "
                   << "| " << real(v) << " / " << imag(v) << " |  = " ;
          if( fabs( imag(v) ) == 0.0 ) {
            (*pcerr) << "infinite." << endl;
          }
          else {
            (*pcerr) << fabs( real(v) )/fabs( imag(v) ) << endl;
          }
          (*pcerr) << "WARNING:                              " << endl;
          returnValue = 140;
        }

        ndx = it->exponents(y.Env());
        ndx[i] -= 1;
        for( int j=0; j<sd2; ++j ) {
          if( ndx[j] != ndx[j+sd2] ) {
            return 141;
	  }
          if( ndx[j] < 0 ) {
            return 142;
	  }
        }
	nu[i].addTerm( JLCterm( ndx, c_i*v/Math_TWOPI, thisEnv) );
      }
    }
  }

  return returnValue;
}







