#include <iostream>
#include <cstdlib>

extern char nexcom( int, int, int* );

using namespace std;

int main( int argc, char** argv )
{
  if( argc != 3 ) {
    cout << "Usage: " << argv[0]
         << " <order> <number of variables>"
         << endl;
    return -1;
  }
  int order = atoi( argv[1] );
  int n     = atoi( argv[2] );

  int e[n];
  int w, i;
  int k = 0;
  for( w = 1; w <= order; w++ ) {
    int m = 0;
    while( nexcom( w, n, e  ) ) {
      cout << ++k << ": " << ++m << ": ( " << e[0];
      for( i = 1; i < n; i++ ) {
        cout << ", " << e[i];
      }
      cout << " )" << endl;
    }
    cout << "-----------------------------" << endl;
  }
  return 0;
}
