////////////////////////////////////////////////////////////
// 
// File:          SVDFitTest.cc
// Author:        Leo Michelotti
// 
// Revision date: October 31, 2006   (original)
// 
////////////////////////////////////////////////////////////
// 
// Tests solution to linear measurement problem generated by SVDFit.
// 
// Command line: SVDFitTest rows cols
// 
// where (int) rows    = number of rows    in response matrix
//       (int) cols    = number of columns in response matrix
// 
// Note: if rows < cols, the system is underdetermined
//       : i.e. fewer "measurements" than "state variables."
// 
////////////////////////////////////////////////////////////

#include <iostream>

#include <basic_toolkit/GenericException.h>
#include <basic_toolkit/Matrix.h>
#include <basic_toolkit/RandomOrthogonal.h>
#include <basic_toolkit/SVDFit.h>

using namespace std;

// For uncorrelated errors, omit the following definition
#define CORRELATED_ERRORS

int main( int argc, char** argv )
{
  if( argc < 3 ) {
    cout << "Usage: " << argv[0]
         << " <rows> <columns> "
         << endl;
    return -1;
  }
  cout << "Command line: ";
  for( int i = 0; i < argc; i++ ) { cout << argv[i] << "  "; }
  cout << "\n--------------------------------\n" << endl;

  int r = atoi( argv[1] ); if( r <= 0 ) { r = 5; }
  int c = atoi( argv[2] ); if( c <= 0 ) { c = 3; }


  // Construct a random response matrix
  // ----------------------------------
  Matrix linearModel(r,c);
  for( int i = 0; i < r; i++ ) {
    for( int j = 0; j < c; j++ ) {
      linearModel(i,j) = -1.0 + 2.0*drand48();
    }
  }
  cout << "Your model matrix is:\n" << linearModel << endl;


  // Construct a random state column matrix
  // (i.e. one column, multiple rows)
  // -----------------------------------
  Matrix state(c);
  for( int j = 0; j < c; j++ ) {
    state(j) = -10.0 + 20.0*drand48();
  }
  cout << "\nYour system's state is:\n" << state << endl;
  cout << "\nError free data would be:\n";
  cout << linearModel*state << endl;
  

  const double max_epsilon  = 0.01;
  double epsilon[r], sigmaSquared[r];
  for( int i = 0; i < r; i++ ) { 
    epsilon[i]      = max_epsilon*drand48(); 
    sigmaSquared[i] = epsilon[i]*epsilon[i]/3.0;
  }



  // Construct a random error correlation matrix
  // -------------------------------------------
  #ifdef CORRELATED_ERRORS
  Matrix C(r,r);
  RandomOrthogonal orthoFactory(r);
  for( int i = 0; i < r; i++ ) { C(i,i) = sigmaSquared[i]; }  // Modify this as desired.
  Matrix R = orthoFactory.build();
  C = R*C*(R.transpose());

  #else
  Matrix C(r);
  for( int i = 0; i < r; i++ ) { C(i) = sigmaSquared[i]; }    // Modify this as desired.
  #endif


  // Construct a data matrix with errors
  // -----------------------------------
  Matrix data(r), error(r);
  for( int i = 0; i < r; i++ ) { error(i) = -epsilon[i] + 2.0*epsilon[i]*drand48(); }
  #ifdef CORRELATED_ERRORS
  error = R*error;
  #endif
  data = linearModel*state + error;
  cout << "\nThe simulated data are:\n" << data << endl;
  

  // Find the solution
  // -----------------
  SVDFit wageSlave;
  wageSlave.setLinearResponse( linearModel );
  wageSlave.setErrorCovariance( C );   // NOTE: This line can be omitted
                                       // if all standard deviations are 
                                       // identical.

  Matrix stateEstimate = wageSlave.solve(data);

  cout << "\nThe estimated state is:\n" 
       << wageSlave.solve(data) 
       << endl;


  // Testing solution
  // ----------------
  Matrix diff = stateEstimate - state;
  double chiSquared = (   diff.transpose()
                        * wageSlave.stateCovariance().inverse()
                        * diff
                      )(0,0);

  cout << "\nState chi-squared / # state coordinates      = "
       << ( chiSquared / state.rows() )
       << endl;
  if( 9.0 < chiSquared ) { return 1; }


  chiSquared = ( error.transpose() * C.inverse() * error )(0,0);
  cout << "Data chi-squared / # measurements            = "
       << ( chiSquared / error.rows() )
       << endl;
  if( 9.0 < chiSquared ) { return 2; }

  diff = data - linearModel*stateEstimate;
  chiSquared = ( diff.transpose() * C.inverse() * diff )(0,0);
  cout << "Estimated data chi-squared / # measurements  = "
       << ( chiSquared / diff.rows() )
       << endl;
  if( 9.0 < chiSquared ) { return 3; }

  return 0;
}
