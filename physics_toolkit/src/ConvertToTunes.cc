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

#include "CLieOperator.h"
#include "MappingC.h"
#include "MathConstants.h"

#include <iomanip.h>

int filterTransverseTunes( /* const */ MatrixD& mtrx, Vector& nu )
{
  if( mtrx.rows() != 6  ||  mtrx.cols() != 6  ||  nu.Dim() != 2 ) {
    cerr << "*** ERROR ***                                    \n"
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
  short int i_x   =  0;
  short int i_y   =  1;
  short int i_z   =  2;
  short int i_px  =  3;
  short int i_py  =  4;
  short int i_dpp =  5;

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
      cout << "\n"
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
      cout << "\n"
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
      cout << "\n"
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
     cout << "\n"
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
      cout << "\n"
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
      cout << endl;
      cout << "*** WARNING ***                                " << endl;
      cout << "*** WARNING *** filterTransverseTunes          " << endl;
      cout << "*** WARNING *** \"Horizontal\" matrix does not " << endl;
      cout << "*** WARNING *** pass symplecticity test.       " << endl;
      cout << "*** WARNING *** JH( 0, 0 ) = " << JH( 0, 0 )     << endl;
      cout << "*** WARNING *** JH( 1, 1 ) = " << JH( 1, 1 )     << endl;
      cout << "*** WARNING ***                                " << endl;
      cout << "*** WARNING *** The ratio is " 
 	   << fabs( ( JH(0,0) + JH(1,1) ) / ( JH(0,0) - JH(1,1) ) )
 	   << endl;
      cout << "*** WARNING ***                                " << endl;
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
      cout << endl;
      cout << "*** WARNING ***                                " << endl;
      cout << "*** WARNING *** filterTransverseTunes          " << endl;
      cout << "*** WARNING *** \"Vertical\" matrix does not   " << endl;
      cout << "*** WARNING *** pass symplecticity test.       " << endl;
      cout << "*** WARNING *** JV( 0, 0 ) = " << JV( 0, 0 )     << endl;
      cout << "*** WARNING *** JV( 1, 1 ) = " << JV( 1, 1 )     << endl;
      cout << "*** WARNING ***                                " << endl;
      cout << "*** WARNING *** The ratio is " 
 	   << fabs( ( JV(0,0) + JV(1,1) ) / ( JV(0,0) - JV(1,1) ) )
 	   << endl;
      cout << "*** WARNING ***                                " << endl;
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
      cerr << "*** ERROR ***                                     \n"
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
      cerr << "*** ERROR ***                                     \n"
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
  int returnValue = 0;
  static const Complex c_zero = Complex( 0.0,   0.0 );
  static const Complex c_mi   = Complex( 0.0, - 1.0 );

  if( nu.Env() != N.Env() )        return 137;
  if( nu.Dim() != N.Dim() )        return 138;

  int sd = N.Env()->SpaceDim;
  if( sd%2 != 0 )                  return 139;
  int sd2 = sd/2;

  // Environment is set ........................
  JetC__environment* thisEnv = N.Env();

  JLCterm* jlctPtr;
  IntArray  ndx( N.Env()->NumVar );
  Complex   v;

  // Construct the Mapping .......................
  int  i, j;
  JetC y( thisEnv );    // Specifying environment is not 
                        // really necessary here.
  for( i = 0; i < N.Dim(); i++ ) {
    y = N( i );
    nu(i) = complex_0;
    while((  jlctPtr = y.get()  )) {
      v = jlctPtr->value;
      if((  v != c_zero  )) {

        // Reality check ...
        if( fabs( real(v) ) > 1.0e-10 * fabs( imag(v) ) ) {
          cerr << "WARNING:                              " << endl;
          cerr << "WARNING: ConvertNToTunes              " << endl;
          cerr << "WARNING: Reality check was violated.  " << endl;
          cerr << "WARNING: Offending ration is ";
          if( fabs( imag(v) ) == 0.0 ) {
            cerr << "infinite." << endl;
          }
          else {
            cerr << fabs( real(v) )/fabs( imag(v) ) << endl;
          }
          cerr << "WARNING:                              " << endl;
          returnValue = 140;
        }

        ndx = jlctPtr->index;
        ndx(i) -= 1;
        for( j = 0; j < sd2; j++ ) {
          if( ndx(j) != ndx(j+sd2) ) return 141;
          if( ndx(j) < 0           ) return 142;
        }
        nu(i).addTerm( new JLCterm( ndx, 
                                    c_mi*v/MATH_TWOPI, 
                                    thisEnv 
                                  ) );
      }
    }
  }

  return returnValue;
}



// int ConvertToTunes( /* const */ LieOperator& N, 
//                     const Jet__environment*  y )
// {
//   // Environment is set ...
//   Jet__environment* thisJetEnv;
//   coord* dummyCoordPtr;
//   JLterm* jltPtr;
//   IntArray  ndx;
// 
//   if( y != 0 ) thisJetEnv = y;
//   else 
//   {
//     // A quick check
//     int sd = N.Env()->SpaceDim;
//     if( ( sd%2 != 0 ) || ( sd != N.Dim() ) ) {
//       cerr << "*** ERROR ***                                 \n"
//            << "*** ERROR ***  ConvertToTunes                 \n"
//            << "*** ERROR ***                                 \n"
//            << "*** ERROR ***  Phase space dimension in N is not correct.\n"
//            << "*** ERROR ***                                 \n"
//            << endl;
//       exit( 137 );
//     }
// 
//     // Create an environment
//     Jet::BeginEnvironment( sd/2 );
//     for( i = 0; i < sd/2; i++ ) dummyCoordPtr = new coord( 0.0 );
//     thisJetEnv = Jet::EndEnvironment();
// 
//     // Check to see if this environment already exists.
//     // This is horribly inefficient, but it is assumed that
//     // ConvertToTunes is not going to be called many times
//     // within a program.
//     slist_iterator g( Jet::environments );
//     Jet__environment* pje;
//     while( pje = (JetC__environment*) g() ) {
//       if( *pje == *thisJetEnv ) {
//         slist_iterator gc( thisJetEnv->myCoords );
//         while((  dummyCoordPtr = (coord*) gc()  )) delete dummyCoordPtr;
//         delete thisJetEnv;
//         thisJetEnv = pje;
//         break;
//       }
//     }
//   }
// 
//   // Construct the Mapping
//   int  i;
//   JetC y( N.Env() );    // Specifying environment is not 
//                         // really necessary here.
//   Mapping nu( sd/2, 0, thisJetEnv );
//   for( i = 0; i < sd/2; i++ ) {
//     y = N( i );
//     while((  jltPtr = y.get()  )) {
//       ndx = jltPtr->index;
//       ndx(i) -= 1;
//       nu(i).addTerm( new JLterm( ndx, jltPtr->value,  ) );
//     }
//   }
// 
//   return nu;
// }


