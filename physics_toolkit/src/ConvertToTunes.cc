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
#include <iomanip>

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;


int filterTransverseTunes( /* const */ MatrixD& mtrx, Vector& nu )
{
  if( mtrx.rows() != 6  ||  mtrx.cols() != 6  ||  nu.Dim() != 2 ) {
    (*pcerr) << "*** ERROR ***                                    \n"
            "*** ERROR *** ConvertToTunes                     \n"
            "*** ERROR *** Dimensions are not correct.        \n"
            "*** ERROR ***                                    \n"
         << endl;
    return 1;
  }

  int i;
  double snH, snV, csH, csV;
  double sn, cs;
 
  // Check for coupling ...
  int i_x   =  0;
  int i_y   =  1;
  int i_z   =  2;
  int i_px  =  3;
  int i_py  =  4;
  int i_dpp =  5;

  if( ( mtrx( i_y,  i_x  ) != 0.0 )  ||
      ( mtrx( i_x,  i_y  ) != 0.0 )  ||
      ( mtrx( i_x,  i_py ) != 0.0 )  ||
      ( mtrx( i_y,  i_px ) != 0.0 )  ||
      ( mtrx( i_py, i_x  ) != 0.0 )  ||
      ( mtrx( i_px, i_y  ) != 0.0 )  ||
      ( mtrx( i_py, i_px ) != 0.0 )  ||
      ( mtrx( i_px, i_py ) != 0.0 )     )
  { 
    // Coupled calculation ....
    // (Lifted from EdwardsTeng) ...
    MatrixC lambda;
    lambda = mtrx.eigenValues();
   
    for( i = 0; i < 6; i++ )
     if( fabs( abs(lambda(i)) - 1.0 ) > 1.0e-4 ) {
      (*pcout) << "\n"
 	   << "*** ERROR ***                                     \n"
 	   << "*** ERROR ***                                     \n"
 	   << "*** ERROR *** filterTransverseTunes               \n"
 	   << "*** ERROR *** The lattice is linearly unstable.   \n"
 	   << "*** ERROR *** lambda( " << i << " ) has magnitude = " 
 	   << abs(lambda(i))
 	   << "\n"
 	   << "*** ERROR ***                                     \n"
 	   << endl;
      return 10;
     }
   
    if( ( abs( lambda(0) - conj( lambda(3) ) ) > 1.0e-4 )  ||
 	( abs( lambda(1) - conj( lambda(4) ) ) > 1.0e-4 )
      ) {
      (*pcout) << "\n"
 	   << "*** ERROR *** filterTransverseTunes                \n"
 	   << "*** ERROR *** Conjugacy condition has been violated\n"
 	   << "*** ERROR *** The lattice may be linearly unstable.\n"
 	   << "*** ERROR *** Eigenvalues =                        \n"
 	   << "*** ERROR *** " << lambda << endl;
      return 11;
    }
   
    double csH = real( lambda(0) );    
    double csV = real( lambda(1) );

    double  dcos, cos2phi, sin2phi, tanphi;
    MatrixD M( 2, 2 ), N( 2, 2 ), m( 2, 2 ), n( 2, 2 ),
            D( 2, 2 ), S( "J", 2 ), A( 2, 2 ), B( 2, 2 ),
            U( "I", 2 );
    MatrixC EV( 6, 6 );

    EV = mtrx.eigenVectors(); 
   
    M( 0, 0 ) = mtrx( 0, 0 );     n( 0, 0 ) = mtrx( 0, 1 );
    M( 0, 1 ) = mtrx( 0, 3 );     n( 0, 1 ) = mtrx( 0, 4 );
    M( 1, 0 ) = mtrx( 3, 0 );     n( 1, 0 ) = mtrx( 3, 1 );
    M( 1, 1 ) = mtrx( 3, 3 );     n( 1, 1 ) = mtrx( 3, 4 );
   
    m( 0, 0 ) = mtrx( 1, 0 );     N( 0, 0 ) = mtrx( 1, 1 );
    m( 0, 1 ) = mtrx( 1, 3 );     N( 0, 1 ) = mtrx( 1, 4 );
    m( 1, 0 ) = mtrx( 4, 0 );     N( 1, 0 ) = mtrx( 4, 1 );
    m( 1, 1 ) = mtrx( 4, 3 );     N( 1, 1 ) = mtrx( 4, 4 );
   
   
    if( fabs( csH - csV ) < 1.0e-4 ) {
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
    
    dcos    = csH - csV;
    cos2phi = ( M - N ).trace() / ( 2.0 *( dcos ) );
    if( fabs( cos2phi - 1.0 ) < 1.0e-4 ) cos2phi =   1.0;  // ??? Rather coarse,
    if( fabs( cos2phi + 1.0 ) < 1.0e-4 ) cos2phi = - 1.0;  // ??? isn't it?
    
    if( fabs( cos2phi ) > 1.0 ) {
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
     sin2phi = csH;  // Variable used as dummy register.
     csH = csV;
     csV = sin2phi;
     dcos    = - dcos;
     cos2phi = - cos2phi;
    }
    sin2phi = sqrt( 1.0 - cos2phi*cos2phi );
    tanphi  = sin2phi / ( 1.0 + cos2phi );
    
    if( fabs( sin2phi ) > 1.0e-8 ) 
      D = -( m + S*n.transpose()*S.transpose() ) * ( 1.0 / ( dcos*sin2phi ));
    else {
      D(0,0) = 1.0;  D(0,1) = 0.0;
      D(1,0) = 0.0;  D(1,1) = 1.0;
    }
    
    if( fabs( D.determinant() - 1.0 ) > 1.0e-4 ) {
      (*pcout) << "\n"
    	    << "*** ERROR *** filterTransverseTunes                \n"
    	    << "*** ERROR *** The matrix D is non-symplectic.      \n"
    	    << "*** ERROR *** |D| = " << D.determinant() << "      \n"
    	    << "*** ERROR ***                                        " << endl;
      return 4;
    }
    
    // ...... Edwards-Teng sign convention.
    if( D.trace() < 0.0 ) {
     D = -D;
     sin2phi = -sin2phi;
     tanphi  = -tanphi;
    }
    A = M - D.inverse()*m*tanphi;
    B = N + D*n*tanphi;

   
    // ......  First the "horizontal" ......
    MatrixD JH;
    JH = A - csH*U;
    if( JH( 0, 1 ) > 0.0 )
     snH =   sqrt( 1.0 - csH*csH );
    else {
     snH = - sqrt( 1.0 - csH*csH );
    }
   
    // .......... A little test to keep everyone honest .....
    if( JH( 0, 0 ) != 0.0 )
     if( fabs( ( JH(0,0) + JH(1,1) ) / ( JH(0,0) - JH(1,1) ) ) > 1.0e-4 ) {
      (*pcout) << endl;
      (*pcout) << "*** WARNING ***                                " << endl;
      (*pcout) << "*** WARNING *** filterTransverseTunes          " << endl;
      (*pcout) << "*** WARNING *** \"Horizontal\" matrix does not " << endl;
      (*pcout) << "*** WARNING *** pass symplecticity test.       " << endl;
      (*pcout) << "*** WARNING *** JH( 0, 0 ) = " << JH( 0, 0 )     << endl;
      (*pcout) << "*** WARNING *** JH( 1, 1 ) = " << JH( 1, 1 )     << endl;
      (*pcout) << "*** WARNING ***                                " << endl;
      (*pcout) << "*** WARNING *** The ratio is " 
 	   << fabs( ( JH(0,0) + JH(1,1) ) / ( JH(0,0) - JH(1,1) ) )
 	   << endl;
      (*pcout) << "*** WARNING ***                                " << endl;
     }
   
   
    // ......  Then  the "vertical" ......
    MatrixD JV;
    JV = B - csV*U;
    if( JV( 0, 1 ) > 0.0 )
     snV =   sqrt( 1.0 - csV*csV );
    else {
     snV = - sqrt( 1.0 - csV*csV );
    }
   
    // .......... A little test to keep everyone honest .....
    if( JV( 0, 0 ) != 0.0 )
     if( fabs( ( JV(0,0) + JV(1,1) ) / ( JV(0,0) - JV(1,1) ) ) > 1.0e-4 ) {
      (*pcout) << endl;
      (*pcout) << "*** WARNING ***                                " << endl;
      (*pcout) << "*** WARNING *** filterTransverseTunes          " << endl;
      (*pcout) << "*** WARNING *** \"Vertical\" matrix does not   " << endl;
      (*pcout) << "*** WARNING *** pass symplecticity test.       " << endl;
      (*pcout) << "*** WARNING *** JV( 0, 0 ) = " << JV( 0, 0 )     << endl;
      (*pcout) << "*** WARNING *** JV( 1, 1 ) = " << JV( 1, 1 )     << endl;
      (*pcout) << "*** WARNING ***                                " << endl;
      (*pcout) << "*** WARNING *** The ratio is " 
 	   << fabs( ( JV(0,0) + JV(1,1) ) / ( JV(0,0) - JV(1,1) ) )
 	   << endl;
      (*pcout) << "*** WARNING ***                                " << endl;
    }
   
   
    double theta = atan2( snH, csH );
    if( theta < 0.0 )   theta += M_TWOPI;
    nu(0) = ( theta / M_TWOPI );
    theta = atan2( snV, csV );
    if( theta < 0.0 )   theta += M_TWOPI;
    nu(1) = ( theta / M_TWOPI );
  }


  else 
  {
    // Uncoupled calculation .....
    // (Lifted from LattFuncSage) ...
    // ... first horizontal
    cs = ( mtrx( i_x, i_x ) + mtrx( i_px, i_px ) )/2.0;
    if( fabs( cs ) <= 1.0 ) {
      if( mtrx( i_x, i_px ) > 0.0 )  sn =   sqrt( 1.0 - cs*cs );
      else                           sn = - sqrt( 1.0 - cs*cs );
    }
    else {
      (*pcerr) << "*** ERROR ***                                     \n"
  	      "*** ERROR *** filterTransverseTunes               \n"
  	      "*** ERROR *** cos( psi_H ) = "
  	   << cs
  	   << "\n"
  	      "*** ERROR *** Cannot continue with calculation.   \n"
  	      "*** ERROR ***                                     \n"
  	   << endl;
      return 2;
    }

    double theta = atan2( sn, cs );
    if( theta < 0.0 )   theta += M_TWOPI;
    nu(0) = ( theta / M_TWOPI );
  

    // ... then vertical.
    double alpha_y, beta_y;
    cs = ( mtrx( i_y, i_y ) + mtrx( i_py, i_py ) )/2.0;
    if( fabs( cs ) <= 1.0 ) {
      if( mtrx( i_y, i_py ) > 0.0 )  sn =   sqrt( 1.0 - cs*cs );
      else                           sn = - sqrt( 1.0 - cs*cs );
    }
    else {
      (*pcerr) << "*** ERROR ***                                     \n"
  	      "*** ERROR *** filterTransverseTunes               \n"
  	      "*** ERROR *** cos( psi_V ) = "
  	   << cs
  	   << "\n"
  	      "*** ERROR *** Cannot continue with calculation.   \n"
  	      "*** ERROR ***                                     \n"
  	   << endl;
      return 3;
    }
  
    theta = atan2( sn, cs );
    if( theta < 0.0 )   theta += M_TWOPI;
    nu(1) = ( theta / M_TWOPI );
  }

  return 0;
}


int ConvertNtoTunes( MappingC& nu, /* const */ CLieOperator& N )
{

  const std::complex<double> complex_0(0.0, 0.0);
  const std::complex<double> c_zero = std::complex<double>( 0.0, 0.0 );
  const std::complex<double> c_i    = std::complex<double>( 0.0, 1.0 );

  int returnValue = 0;

  if( nu.Env() != N.Env() )        return 137;
  if( nu.Dim() != N.Dim() )        return 138;

  int sd = N.Env()->spaceDim();
  if( sd%2 != 0 )                  return 139;
  int sd2 = sd/2;

  // Environment is set ........................
  JetC__environment_ptr thisEnv = N.Env();

  IntArray  ndx( N.Env()->numVar() );

  std::complex<double>   v;

  // Construct the Mapping .......................

  JetC y( thisEnv );    // Specifying environment is not 
                        // really necessary here.
  for( int i=0; i < N.Dim(); ++i) {
    y = N( i );
    nu(i) = complex_0;

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
        ndx(i) -= 1;
        for( int j=0; j < sd2; j++ ) {
          if( ndx(j) != ndx(j+sd2) ) {
            return 141;
	  }
          if( ndx(j) < 0 ) {
            return 142;
	  }
        }
	nu(i).addTerm( JLCterm( ndx, c_i*v/MATH_TWOPI, thisEnv) );
      }
    }
  }

  return returnValue;
}






