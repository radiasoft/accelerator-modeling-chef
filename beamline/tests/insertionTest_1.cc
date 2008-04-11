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
#include <beamline/quadrupole.h>
#include <beamline/sextupole.h>
#include <beamline/octupole.h>
#include <beamline/sbend.h>
#include <beamline/rbend.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>

using namespace std;

struct Options
{
  bool   valid;
  double pc;
  double energy;
  double pct;
  double driftLength;
  double quadLength;
  double quadStrength;
  double sextLength;
  double sextStrength;
  double octLength;
  double octStrength;
  double n;
  double brho;
  double rho;
  double bodyLength;
  double arcLength;
  double bendAngle;
  double bendField;
  int    order;

  Options( int, char**, int=0 );
};


Options::Options( int argc, char** argv, int lastargs )
:   valid(true)
  , pc(8)
  , pct(0.5)
  , driftLength(1)
  , quadLength(1)
  , quadStrength(5)
  , sextLength(1)
  , sextStrength(5)
  , octLength(1)
  , octStrength(5)
  , n(128)
  , bodyLength(2)
  , order(3)
{
  Proton proton(100);
  proton.SetReferenceMomentum(pc);

  energy     = proton.ReferenceEnergy();
  bendAngle  = M_TWOPI/n;
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

int testLengths(   ElmPtr const& a
                 , ElmPtr const& b
                 , ElmPtr const& c
                 , double pct )
{
  int ret = 0;
  cout << "*** LENGTH TEST: "
       << a->Length() << "  "
       << b->Length() << "  "
       << c->Length() << "  "
       << endl;
  if( 1.0e-14 < std::abs(a->Length() - b->Length() - c->Length()) ) {
    cout <<   "*** FAILED *** "
            "\n*** FAILED *** : " << __FILE__ << ", line " << __LINE__
         << "\n*** FAILED *** : For " << a->Type() << "  " << a->Name()
         << "\n*** FAILED *** : "
         << a->Length() << " != " << b->Length() << " + " << c->Length()
         << "\n*** FAILED *** "
         << endl;
    ret = 1;
  }
  if( 1.0e-14 < std::abs(b->Length() - pct*a->Length()) ) {
    ret = 2;
  }
  if( 1.0e-14 < std::abs(c->Length() - (1.0-pct)*a->Length()) ) {
    ret = 3;
  }
  return ret;
}


int testStrengths(   ElmPtr const& a
                   , ElmPtr const& b
                   , ElmPtr const& c
                   , double pct )
{
  int ret = 0;
  cout << "*** STRENGTH TEST: "
       << a->Strength() << "  "
       << b->Strength() << "  "
       << c->Strength() << "  "
       << endl;
  return ret;
}


int testMaps(   ElmPtr    const& u
              , ElmPtr    const& v
              , ElmPtr    const& b
              , ElmPtr    const& c
              , Options   const& myOptions )
{
  int ret = 0;

  JetProton probeOne( myOptions.energy );
  JetProton probeTwo( myOptions.energy );

  u->propagate( probeOne );
  v->propagate( probeOne );
  b->propagate( probeTwo );
  c->propagate( probeTwo );

  cout << "*** AFTER " << endl;
  probeOne.State().printCoeffs();
  probeTwo.State().printCoeffs();

  cout << "*** THE DIFFERENCE " << endl;
  Mapping diff( probeOne.State() - probeTwo.State() );
  diff.printCoeffs();

  cout << "*** THE DIFFERENCE COEFFICIENTS " << endl;
  // ??? REWRITE THIS USING JETWRITER in ASCII MODE ???
  int d = diff.Dim();
  IntArray exps(d);
  JLterm termPtr;
  for( int i = 0; i < d; i++ ) {
    cout << "Component " << i << ":" << endl;
    for(   Jet::const_iterator iter = diff[i].begin()
         ; iter != diff[i].end()
         ; ++iter ) {
      termPtr = *iter;
      if( 10.0*std::numeric_limits<double>::epsilon() < std::abs( termPtr.coefficient() ) ) {
    	cout <<   "*** FAILED *** "
    	        "\n*** FAILED *** : " << __FILE__ << ", line " << __LINE__
             << "\n*** FAILED *** : Map[" << i << "]:" 
             <<                     termPtr.exponents( diff[i].Env() )
             <<                     " = " << termPtr.coefficient()
    	     << "\n*** FAILED *** "
    	     << endl;
    	ret = 4;
      }
    }
    cout << endl;
  }

  return ret;
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


  ElmPtr oo0, oo1, oo2;
  ElmPtr u, v;
  JetParticle::createStandardEnvironments( myOptions.order );


  { // drift
  cout << "\n\n--- TESTING DRIFT ---\n" << endl;
  oo0 = ElmPtr( new drift( "", myOptions.driftLength ) );
  oo0->Split( myOptions.pct, oo1, oo2 );
  u = ElmPtr( new drift( "",        myOptions.pct *myOptions.driftLength ) );
  v = ElmPtr( new drift( "", (1.0 - myOptions.pct)*myOptions.driftLength ) );
  ret += testLengths( oo0, oo1, oo2, myOptions.pct );
  ret += testStrengths( oo0, oo1, oo2, myOptions.pct );
  ret += testMaps( u, v, oo1, oo2, myOptions );
  }

  { // quadrupole
  cout << "\n\n--- TESTING QUADRUPOLE ---\n" << endl;
  oo0 = ElmPtr( new quadrupole(   ""
                                , myOptions.quadLength
                                , myOptions.quadStrength ) );
  oo0->Split( myOptions.pct, oo1, oo2 );
  u = ElmPtr( new quadrupole(   ""
                              , myOptions.pct*myOptions.quadLength
                              , myOptions.quadStrength ) );
  v = ElmPtr( new quadrupole(   ""
                              , (1.0 - myOptions.pct)*myOptions.quadLength
                              , myOptions.quadStrength ) );
  ret += testLengths( oo0, oo1, oo2, myOptions.pct );
  ret += testStrengths( oo0, oo1, oo2, myOptions.pct );
  ret += testMaps( u, v, oo1, oo2, myOptions );
  }

  { // sextupole
  cout << "\n\n--- TESTING SEXTUPOLE ---\n" << endl;
  oo0 = ElmPtr( new sextupole(  ""
                              , myOptions.sextLength
                              , myOptions.sextStrength ) );
  oo0->Split( myOptions.pct, oo1, oo2 );
  u = ElmPtr( new sextupole(   ""
                            , myOptions.pct*myOptions.sextLength
                            , myOptions.sextStrength ) );
  v = ElmPtr( new sextupole(   ""
                            , (1.0 - myOptions.pct)*myOptions.sextLength
                            , myOptions.sextStrength ) );
  ret += testLengths( oo0, oo1, oo2, myOptions.pct );
  ret += testStrengths( oo0, oo1, oo2, myOptions.pct );
  ret += testMaps( u, v, oo1, oo2, myOptions );
  }

  { // octupole
  cout << "\n\n--- TESTING OCTUPOLE ---\n" << endl;
  oo0 = ElmPtr( new octupole(   ""
                              , myOptions.octLength
                              , myOptions.octStrength ) );
  oo0->Split( myOptions.pct, oo1, oo2 );
  u = ElmPtr( new octupole(   ""
                            , myOptions.pct*myOptions.octLength
                            , myOptions.octStrength ) );
  v = ElmPtr( new octupole(   ""
                            , (1.0 - myOptions.pct)*myOptions.octLength
                            , myOptions.octStrength ) );
  ret += testLengths( oo0, oo1, oo2, myOptions.pct );
  ret += testStrengths( oo0, oo1, oo2, myOptions.pct );
  ret += testMaps( u, v, oo1, oo2, myOptions );
  }

  { // sbend
  cout << "\n\n--- TESTING SBEND ---"
            "\n--- WITH NORMAL ENTRY EDGES ---\n"
       << endl;
  oo0 = ElmPtr( new sbend ( "", myOptions.arcLength
                              , myOptions.bendField
                              , myOptions.bendAngle ) );
  oo0->Split( myOptions.pct, oo1, oo2 );
  u   = ElmPtr( new sbend ( "", myOptions.pct*myOptions.arcLength
                              , myOptions.bendField
                              , myOptions.pct*myOptions.bendAngle ) );
  v   = ElmPtr( new sbend ( "", (1.0 - myOptions.pct)*myOptions.arcLength
                              , myOptions.bendField
                              , (1.0 - myOptions.pct)*myOptions.bendAngle ) );
  ret += testLengths( oo0, oo1, oo2, myOptions.pct );
  ret += testStrengths( oo0, oo1, oo2, myOptions.pct );
  ret += testMaps( u, v, oo1, oo2, myOptions );


  cout << "\n\n--- TESTING SBEND ---"
            "\n--- WITH PARALLEL EDGES ---\n"
       << endl;
  oo0 = ElmPtr( new sbend ( "", myOptions.arcLength
                              , myOptions.bendField
                              , myOptions.bendAngle
                              , myOptions.bendAngle/2.0
                              , myOptions.bendAngle/2.0 ) );
  oo0->Split( myOptions.pct, oo1, oo2 );
  u   = ElmPtr( new sbend ( "", myOptions.pct*myOptions.arcLength
                              , myOptions.bendField
                              , myOptions.pct*myOptions.bendAngle
                              , myOptions.bendAngle/2.0
                              , 0 ) );
  v   = ElmPtr( new sbend ( "", (1.0 - myOptions.pct)*myOptions.arcLength
                              , myOptions.bendField
                              , (1.0 - myOptions.pct)*myOptions.bendAngle
                              , 0 
                              , myOptions.bendAngle/2.0 ) );
  ret += testLengths( oo0, oo1, oo2, myOptions.pct );
  ret += testStrengths( oo0, oo1, oo2, myOptions.pct );
  ret += testMaps( u, v, oo1, oo2, myOptions );
  }

  { // rbend
  cout << "\n\n--- TESTING RBEND ---"
            "\n--- WITH PARALLEL EDGES ---\n"
       << endl;
  oo0 = ElmPtr( new rbend ( "", myOptions.bodyLength
                              , myOptions.bendField
                              , myOptions.bendAngle ) );
  oo0->Split( myOptions.pct, oo1, oo2 );
  u   = ElmPtr( new rbend ( "", myOptions.pct*myOptions.bodyLength
                              , myOptions.bendField
                              , myOptions.pct*myOptions.bendAngle ) );
  v   = ElmPtr( new rbend ( "", (1.0 - myOptions.pct)*myOptions.bodyLength
                              , myOptions.bendField
                              , (1.0 - myOptions.pct)*myOptions.bendAngle ) );
  ret += testLengths( oo0, oo1, oo2, myOptions.pct );
  ret += testStrengths( oo0, oo1, oo2, myOptions.pct );
  ret += testMaps( u, v, oo1, oo2, myOptions );
  }

  return ret;
}
