////////////////////////////////////////////////////////////
//
// File:          insertionTest_2.cc
// Author:        Leo Michelotti
//
// Revision dates: 
// --------------
// Wednesday.  April 23, 2008.  (original version)
// 
// Friday.     November 20, 2009.
// Added handling exceptions.
// - lpjm
//
////////////////////////////////////////////////////////////
//
// Tests usage of insertion lists
// : i.e. usage of beamline::InsertElementsFromList(..)
//
// ------------
// COMMAND LINE
// ------------
// insertionTest_2 [options]
//
// -------
// OPTIONS
// -------
// -seed NNN  seed to be used by drand48 in generating a random beamline
//            : default = 11111
// -N    NNN  number of markers to be inserted into the line
//            : default = 3
//
// Note: NNN represents an integer
//       XXX            a  real
//
////////////////////////////////////////////////////////////

#include <stdlib.h>

#include <basic_toolkit/MathConstants.h>
#include <beamline/beamline.h>
#include <beamline/drift.h>
#include <beamline/sbend.h>
#include <beamline/quadrupole.h>
#include <beamline/Particle.h>
#include <beamline/marker.h>
// #include <beamline/JetParticle.h>
// #include <beamline/RefRegVisitor.h>

extern beamline* DriftsToSlots( beamline const& argbml );

using namespace std;


/////////////////////////////////////////////////////////////////////

struct Options
{
  bool valid;

  int  N;         // number of "cuts"
  long seed;
  bool s_option;

  double pc;
  double energy;
  double brho;
  double rho;
  double bodyLength;
  double arcLength;
  double bendAngle;
  double bendField;

  Options( int, char**, int=0 );
};


Options::Options( int argc, char** argv, int lastargs )
:   valid(true)
  , N(3)
  , seed(11111)
  , s_option(true)
  , pc(1000)
  , bodyLength(2)
{
  Proton proton(100);
  proton.SetReferenceMomentum(pc);

  energy     = proton.ReferenceEnergy();
  bendAngle  = M_TWOPI/774.0;
  brho       = proton.ReferenceBRho();  // Tesla-meters
  rho        = bodyLength / (2.0*sin( bendAngle/2.0 ));
  arcLength  = rho*bendAngle;
  bendField  = brho/rho;

  int limit = argc-lastargs;
  string s;
  int i = 1;
  while( valid && (i < limit) ) {
    s.assign( argv[i++] );
    if( '-' == s[0] ) {
      s.assign( s.substr(1) );
      if( s == string("N") ) {
        if( i < limit ) {
          N = atoi( argv[i++] );
          if( N < 0 ) { N = -N; }
        }
        else {
          cerr << "\n*** ERROR *** Too few parameters." << endl;
          valid = false;
        }
      }
      else if( s == "seed" ) {
        if( i < limit ) {
          seed = atoi( argv[i++] );
          if( seed <  0 ) { seed = -seed; }
          if( seed == 0 ) { seed = 11111; }
        }
        else {
          cerr << "\n*** ERROR *** Too few parameters." << endl;
          valid = false;
        }
      }
      else if( s == "l" ) {
        s_option = false;
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

BmlPtr randomLine()
{
  BmlPtr bmlptr( new beamline("") );
  ElmPtr elmptr;
  
  int num = 32 + ((int) (128.0*drand48()) );
  double chooser = 0.3333;
  for( int i = 0; i < num; ++i ) {
    int x = ((int) (drand48()/chooser));
    switch(x) {
      case 0:
        bmlptr->append( drift("", 1.0) );
        break;
      case 1:
        bmlptr->append( sbend("", 1.0, 4.0, M_TWOPI/774.0 ) );
        break;
      case 2:
        bmlptr->append( quadrupole("", 1.0, 4.0 ) );
        break;
      default:
        cout << __FILE__ << "," << __LINE__
             << ": ignored random value " << x
             << endl;
        --i;
        break;
    }
  }

  return bmlptr;
}

/////////////////////////////////////////////////////////////////////

int main( int argc, char** argv )
{
  Options myOptions( argc, argv, 0 );
  if( myOptions.valid ) {
    cout << "\nCommand line: ";
    for( int i = 0; i < argc; i++ ) {
      cout << argv[i] << "  ";
    }
    cout << "\n-----------------------------------------\n"
         << endl;
  }
  else {
    return 1;
  }

  srand48( myOptions.seed );

  BmlPtr myLine( randomLine() );

  cout << "\n\nBEFORE:\n" << endl;
  for(beamline::deep_iterator it  = myLine->deep_begin(); 
                              it != myLine->deep_end();  
                            ++it) {
    cout         << (*it)->Type() 
         << "  " << (*it)->Length()
         << "  " << (*it)->Strength()
         << endl;
  }

  Proton proton( myOptions.energy );
  proton.set_npy(0.001);
  cout << "inState : " << proton.State() << endl;
  myLine->propagate( proton );
  cout << "outState: " << proton.State() << endl;
  proton.setStateToZero();

  Vector testState( proton.State() );

  std::list<std::pair<ElmPtr,double> > myList;
  double ds = myLine->Length()/((double) (myOptions.N + 1));
  double s  = ds;
  for( int i = 0; i < myOptions.N; ++i ) {
    myList.push_back( std::pair<ElmPtr,double>(ElmPtr(new marker("")),s) );
    s += ds;
  }
  
  s = 0;
  if( myOptions.s_option ) {
    try { 
      myLine->InsertElementsFromList( proton, s, myList ); 
    }
    catch (GenericException& ge){
      cerr << "\n *** ERROR *** "
              "\n *** ERROR *** Exception thrown by beamline::InsertElementsFromList"
              "\n *** ERROR *** ---------------------------------------------\n"
           << ge.what()
           << "\n *** ERROR *** ---------------------------------------------"
              "\n *** ERROR *** "
           << endl;
      return 2;
    }
  }
  else {
    try { 
      myLine->InsertElementsFromList( s, myList );
    }
    catch (GenericException& ge){
      cerr << "\n *** ERROR *** "
              "\n *** ERROR *** Exception thrown by beamline::InsertElementsFromList"
              "\n *** ERROR *** ---------------------------------------------\n"
           << ge.what()
           << "\n *** ERROR *** ---------------------------------------------"
              "\n *** ERROR *** "
           << endl;
      return 2;
    }
  }

  cout << "\n\nAFTER:\n" << endl;
  for(beamline::deep_iterator it  = myLine->deep_begin(); 
                              it != myLine->deep_end();  
                            ++it) {
    cout         << (*it)->Type() 
         << "  " << (*it)->Length()
         << "  " << (*it)->Strength()
         << endl;
  }

  proton.setStateToZero();
  proton.set_npy(0.001);
  cout << "inState : " << proton.State() << endl;
  myLine->propagate( proton );
  cout << "outState: " << proton.State() << endl;
  proton.setStateToZero();

  for( int j = 0; j < testState.Dim(); ++j ) {
    if( 1.0e-12 < std::abs( testState[j] - proton.State()[j] ) ) {
      cerr << "\n *** ERROR *** "
              "\n *** ERROR *** Splitting elements affected propagation."
              "\n *** ERROR *** "
           << endl;
      return 3;
    }
  }

  return 0;
}
