////////////////////////////////////////////////////////////
//
// File:          test_6and8.cc
// 
// Author:        Eric Stern
//                (edited by Leo Michelotti)
//
// Revision date: Friday.  July 1, 2011.  (original version)
//
////////////////////////////////////////////////////////////
//
// Tests of class ThinPole.
// : instantiates a thin sextupole, a thin octupole, and a
//   second order thinpole object
// : propagates a Proton through them and compares the results
// : propagates a JetProton through them and writes the corresponding
//   Mappings for visual comparison.
// : optional failure mode, using Jet::iterator
//
////////////////////////////////////////////////////////////
//
// ------------
// COMMAND LINE
// ------------
// ./test_6and8 [options]
//
// -------
// OPTIONS
// -------
// -seed NNN  seed to be used by drand48 to generate random initial states
//            : default = 11111
// -n    NNN  number of times a Proton will be propagated through elements
//            : default = 100
// -deg  NNN  maximum degree of jet polynomial representation
//            : default = 5
//            : bounds  = [1,8]  (just for practicality)
// -tol  XXX  tolerance level for comparison tests
//            : default 10^{-11}
// -fail      triggers failure mode induced by using Jet::iterators
//
// Note: NNN represents an integer
//       XXX            a  real
//
////////////////////////////////////////////////////////////

#include <beamline/JetParticle.h>
#include <beamline/sextupole.h>
#include <beamline/octupole.h>
#include <beamline/thinpoles.h>

#include <stdlib.h>

using namespace std;

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

struct Options
{
  bool valid;
  long seed;

  double pr_energy;
  double sex_strength;
  double oct_strength;
  double max_transverse;
  double max_long;
  double max_pt;
  double max_ndp;
  double tolerance;
  int    nloops;
  int    degree;
  bool   use_iterators;

  Options( int, char**, int=0 );
};


Options::Options( int argc, char** argv, int lastargs )
:   valid          (true)
  , seed           (11111)
  , pr_energy      (10.0)    // 10 GeV
  , sex_strength   (1.0)     // sextupole strength
  , oct_strength   (1.0/3.0) // octupole strength
  , max_transverse (0.05)    // max 5 cm transverse displacement
  , max_long       (2.0)     // max 2m longitudinal displacement
  , max_pt         (0.01)    // maximum transverse momentum
  , max_ndp        (0.005)   // maximum deltap/p
  , nloops         (100)
  , tolerance      (1.0e-11)
  , degree         (5)
  , use_iterators  (false)
{
  int limit = argc-lastargs;
  string s;
  int i = 1;

  while( valid && (i < limit) ) {
    s.assign( argv[i++] );
    if( '-' == s[0] ) {
      s.assign( s.substr(1) );

      if( s == "seed" ) {
        if( i < limit ) {
          seed = long( atof( argv[i++] ) );
          if( seed <  0 ) { seed = -seed; }
          if( seed == 0 ) { seed = 11111; }
        }
        else {
          cerr << "\n*** ERROR *** Too few parameters." << endl;
          valid = false;
        }
      }

      else if( s == "n" ) {
        if( i < limit ) {
          nloops = int( atof( argv[i++] ) );
          if( nloops <  0 ) { nloops = -nloops; }
          if( nloops == 0 ) { nloops = 100;     }
        }
        else {
          cerr << "\n*** ERROR *** Too few parameters." << endl;
          valid = false;
        }
      }

      else if( s == "deg" ) {
        if( i < limit ) {
          degree = atoi( argv[i++] );
          if( degree < 1 ) { degree = 1; }
          if( degree > 8 ) { degree = 8; }
        }
        else {
          cerr << "\n*** ERROR *** Too few parameters." << endl;
          valid = false;
        }
      }

      else if( s == "tol" ) {
        if( i < limit ) {
          tolerance = std::abs( atof( argv[i++] ) );
        }
        else {
          cerr << "\n*** ERROR *** Too few parameters." << endl;
          valid = false;
        }
      }

      else if( s == "fail" ) {
        use_iterators = true;
      }
    }

    else {
      cerr << "\n*** ERROR *** Invalid command line argument: " << s << endl;
      valid = false;
    }
  }
}


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


bool cmpvalues( double const x1, double const x2, Options const& myOptions )
{
  // if values are small, compare absolute difference
  // if values are larger, compare relative difference
  return (abs(x1)         <  myOptions.tolerance) ? 
         (abs(x1-x2)      >= myOptions.tolerance) : 
         (abs((x1-x2)/x1) >= myOptions.tolerance) ;
}


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


int main(int argc, char *argv[])
{
  // Preliminaries
  // -------------
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
    return -1;
  }

  srand48( myOptions.seed );

  typedef std::complex<double> cmplx;
  const cmplx zero(0.0,0.0);
  const cmplx one(1.0,0.0);

  int dimen   = Proton( myOptions.pr_energy ).State().Dim();
  // REMOVE: cout << "DGN: dimension = " << dimen << endl;
  JetParticle::createStandardEnvironments( myOptions.degree );
  int result  = 0;
  int badflag = 0;


  // Instantiate thinSextupole and thinOctupole
  // ------------------------------------------
  thinSextupole my_sextupole("my_sextupole", myOptions.sex_strength);
  thinOctupole  my_octupole ("my_octupole" , myOptions.oct_strength);


  // Instantiate compound ThinPole object with components
  // ----------------------------------------------------
  const cmplx components[] = { zero,
                               zero, 
                               one*myOptions.sex_strength,
                               one*myOptions.oct_strength,
                               zero };
  std::vector<cmplx> poles( components,
                            (components+sizeof(components)/sizeof(cmplx))  );
  
  ThinPole my_poles( "my_poles", 1, poles);


  // Propagate JetProton through sextupole and octupole
  // --------------------------------------------------
  JetProton jpr1( myOptions.pr_energy );
  my_sextupole.propagate(jpr1);
  my_octupole.propagate(jpr1);
  
  // Propagate JetProton through poles object
  // ----------------------------------------
  JetProton jpr2( myOptions.pr_energy );
  my_poles.propagate(jpr2);


  // Compare mappings
  // ----------------
  Mapping m1 = jpr1.State();
  cout << "sextupole+octupole coefficients: " << endl;
  m1.printCoeffs();

  Mapping m2 = jpr2.State();
  cout << "my_poles coefficients: " << endl;
  m2.printCoeffs();

  cout << "map difference sextupole+octopole-multipole coefficients" << endl;
  (m1 - m2).printCoeffs();


  // Attempt to use Jet::iterator for comparison.
  // This (currently) fails.
  // It must be fixed at the mxyzptlk level
  // -----------------------------------------------------
  if( myOptions.use_iterators ) 
  {
    for( int i = 0; i < dimen; ++i ) 
    {
      badflag = 0;
      Jet::iterator j1_iter = m1[i].begin();
      Jet::iterator j2_iter = m2[i].begin();

      while( ( j1_iter != m1[i].end() ) && ( j2_iter != m2[i].end() ) ) 
      {
        cout << "DGN: " << __FILE__ << "," << __LINE__
             << ": if( cmpvalues( "
             << (*j1_iter).value_
             << ", "
             << (*j2_iter).value_
             << ", myOptions ) )"
             << endl;
        if( cmpvalues( (*j1_iter).value_, (*j2_iter).value_, myOptions ) ) 
        { ++badflag; }

        ++j1_iter;
        ++j2_iter;
      }
 
      if( ( j1_iter != m1[i].end() ) || ( j2_iter != m2[i].end() ) ) {
        cout <<   "*** ERROR *** "
                "\n*** ERROR *** " << __FILE__ << "," << __LINE__
             << ": At index " << i 
             << ", maps have different number of coefficients!"
             << "\n*** ERROR *** "
             << endl;
        result = result | 1;
      }
  
      if( 0 != badflag ) {
        cout <<   "*** ERROR *** "
                "\n*** ERROR *** " << __FILE__ << "," << __LINE__
             << ": " << badflag << " components of maps do not match"
                                   " at index " << i
             << "\n*** ERROR *** "
             << endl;
        result = result | 2;
      }
    }
  }

                   #if 0  // Development scratch area
                   int counter = 0;
                   for( Mapping::iterator iter = m1.begin(); iter != m1.end(); ++iter ) {
                     ++counter;
                     // cout << counter << endl;
                     for( Jet::iterator jit = (*iter).begin(); jit != (*iter).end(); ++jit ) {
                       cout << *jit << endl;
                       cout << "Test value = " << (*jit).value_ << endl;
                     }
                   }
                   #endif


  // Propagate protons with random states
  // ------------------------------------
  Proton my_proton1(myOptions.pr_energy);
  Proton my_proton2(myOptions.pr_energy);

  Vector prstate0(dimen), prstate1(dimen), prstate2(dimen);
  Vector prstate1a(dimen), prstate2a(dimen);

  for (int i=0; i < myOptions.nloops; ++i) 
  {
    // Generate random particle states
    // -------------------------------
    prstate0[0] = myOptions.max_transverse * ( 2.0*drand48() - 1.0 );
    prstate0[1] = myOptions.max_transverse * ( 2.0*drand48() - 1.0 );
    prstate0[2] = myOptions.max_long       * ( 2.0*drand48() - 1.0 );
    prstate0[3] = myOptions.max_pt         * ( 2.0*drand48() - 1.0 );
    prstate0[4] = myOptions.max_pt         * ( 2.0*drand48() - 1.0 );
    prstate0[5] = myOptions.max_ndp        * ( 2.0*drand48() - 1.0 );


    // Instantiate two protons, propagate both
    // ---------------------------------------
    my_proton1.State() = prstate0;
    my_sextupole.propagate(my_proton1);
    my_octupole.propagate(my_proton1);
    prstate1 = my_proton1.State();

    my_proton2.State() = prstate0;
    my_poles.propagate(my_proton2);
    prstate2 = my_proton2.State();


    // Compare final states of Proton propagated
    // through compound multipole with Proton propagated
    // through thin sextupole and octupole
    // -------------------------------------------------
    badflag = 0;
    for (int coord=0; coord<dimen; ++coord) {
      if ( cmpvalues(prstate1[coord], prstate2[coord], myOptions) ) {
        ++badflag;
      }
    }

    if ( 0 != badflag ) {
      cout << "Propagation mismatch in << badflag << components."
              "\nInitial state: " << prstate0 << endl;
      cout << "       state1: "   << prstate1 << endl;
      cout << "       state2: "   << prstate2 << '\n' << endl;
      result = result | 4;
    }
      

    // Propagate particles with maps
    // -----------------------------
    prstate1a = m1(prstate0);
    prstate2a = m2(prstate0);


    // Compare coordinates of final states propagated by two Mappings
    // --------------------------------------------------------------
    badflag = 0;
    for (int coord=0; coord<dimen; ++coord) {
      if ( cmpvalues(prstate1a[coord], prstate2a[coord], myOptions) ) {
        ++badflag;
      }
    }

    if ( 0 != badflag ) {
      cout << "Poles map mismatch in << badflag << components."
              "\nInitial state: " << prstate0 << endl;
      cout << "    state1a: " << prstate1a << endl;
      cout << "    state2a: " << prstate2a << endl;
      result = result | 8;
    }

    // Compare coordinates of final states propagated by Mapping
    // with direct tracking
    // ---------------------------------------------------------
    badflag = 0;
    for (int coord=0; coord<dimen; ++coord) {
      if ( cmpvalues(prstate2[coord], prstate2a[coord], myOptions) ) {
        ++badflag;
      }
    }

    if ( 0 != badflag ) {
      cout << "Propagate vs. map mismatch in << badflag << components."
              "\nInitial state: " << prstate0 << endl;
      cout << "    state2: " << prstate2 << endl;
      cout << "    state2a: " << prstate2a << endl;
      result = result | 16;
    }

  } // end loop over random initial states

  return result;
}
