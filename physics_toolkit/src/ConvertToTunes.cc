/*
** int ConvertNtoTunes( MappingC& nu, const CLieOperator& N )
**
** Converts the data in the normal form "Hamiltonian," N, into
** a MappingC, nu, that maps normal form coordinates into tunes.
**
** Original author: Leo Michelotti
** Date:            September 5, 1996
** 
*/

#include "CLieOperator.h"
#include "MappingC.h"
#include "MathConstants.h"

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


