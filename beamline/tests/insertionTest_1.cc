////////////////////////////////////////////////////////////
// 
// File:          insertionTest_1.cc
// Author:        Leo Michelotti
// 
// Revision date: Monday. April 7, 2008.  (original version)
//                (NOTE: this is just the first installment.)
// 
////////////////////////////////////////////////////////////
// 
// Tests Split method on individual elements.
// 
// ------------
// COMMAND LINE
// ------------
// insertionTest_1 [options]
// 
// -------
// OPTIONS
// -------
// Note: NNN represents an integer
//       DDD            a  real
// 
// -pct DDD   fractional distance from upstream end of element
//            at which split is done.
//            : valid range    = [ 0, 1 ]
//            : default  value = 0.5
// 
////////////////////////////////////////////////////////////

#include <beamline/drift.h>

using namespace std;

struct Options
{
  bool   valid;
  double pct;
  double driftLength;

  Options( int, char**, int=0 );
};


Options::Options( int argc, char** argv, int lastargs )
:   valid(true)
  , pct(0.5)
  , driftLength(1)
{
  valid = true;
  int limit = argc-lastargs;
  string s;
  int i = 1;
  while( valid && (i < limit) ) {
    s.assign( argv[i++] );
    if( '-' == s[0] ) {
      s.assign( s.substr(1) );
      if( s == "pct" ) {
        if( i < limit ) { 
          pct = atof( argv[i++] );
          if( pct < 0 || pct > 1 ) {
            cerr << "\n*** ERROR *** Parameter " << pct << " outside range [0,1]." << endl;
            valid = false;
          }
        }
        else {
          cerr << "\n*** ERROR *** Too few parameters." << endl;
          valid = false;
        }
      }
      else {
        cerr << "\n*** ERROR *** Unrecognized option: " << s << endl;
        valid = false;
      }
    }
    else {
      cerr << "\n*** ERROR *** Invalid command line argument: " << s << endl;
      valid = false;
    }
  }
}


/////////////////////////////////////////////////////////////////////

int main( int argc, char** argv ) 
{
  int ret = 0;

  Options myOptions( argc, argv );
  if( myOptions.valid ) {
    cout << "\nCommand line: ";
    for( int i = 0; i < argc; i++ ) {
      cout << argv[i] << "  ";
    }
    cout << endl;
  }
  else {
    return 1;
  }

  drift ooo( "", myOptions.driftLength );
  ElmPtr oo1, oo2;
  ooo.Split( myOptions.pct, oo1, oo2 );
  cout << oo1->Type() << "  " << oo1->Length() << endl;
  cout << oo2->Type() << "  " << oo2->Length() << endl;

  return ret;
}
