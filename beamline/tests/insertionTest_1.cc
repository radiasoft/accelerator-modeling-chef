////////////////////////////////////////////////////////////
//
// File:          insertionTest_1.cc
// Author:        Leo Michelotti
//
// Revision date: Monday.    April  7, 2008.  (original version)
//                Monday.    April 21, 2008.  (current  version)
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
#include <beamline/CF_sbend.h>
#include <beamline/CF_rbend.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/RefRegVisitor.h>

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

  std::vector<double> mapTolerance;
  std::vector<double> sigma;

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

  double eps = 128.0*std::numeric_limits<double>::epsilon();
  mapTolerance.push_back(eps);    // x
  mapTolerance.push_back(eps);    // y
  mapTolerance.push_back(eps);    // cdt
  mapTolerance.push_back(eps);    // px/p
  mapTolerance.push_back(eps);    // py/p
  mapTolerance.push_back(eps);    // dp/p

  sigma.push_back(0.001);  // x
  sigma.push_back(0.001);  // y
  sigma.push_back(0.001);  // cdt
  sigma.push_back(0.001);  // px/p
  sigma.push_back(0.001);  // py/p
  sigma.push_back(0.001);  // dp/p

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
      if( s == string("pct") ) {
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


int testMaps(   ElmPtr    const& a
              , ElmPtr    const& b
              , ElmPtr    const& c
              , Options   const& myOptions )
{
  int ret = 0;

  JetProton probeOne( myOptions.energy );
  JetProton probeTwo( myOptions.energy );

  a->propagate( probeOne );
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
             << "\n*** FAILED *** : [" << i << "]:" 
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

int testBendMap(  ElmPtr  const& oo0
                , ElmPtr  const& oo1
                , ElmPtr  const& oo2
                , Options const& myOptions )
{
  int ret = 0;

  bool is_bend =    ( 0 == strcmp( "rbend"   , oo0->Type() ) )
                 || ( 0 == strcmp( "sbend"   , oo0->Type() ) )
                 || ( 0 == strcmp( "CF_rbend", oo0->Type() ) )
                 || ( 0 == strcmp( "CF_sbend", oo0->Type() ) );
  if( !is_bend ) {
    cout <<   "*** FAILED *** "
            "\n*** FAILED *** : " << __FILE__ << ", line " << __LINE__
         << "\n*** FAILED *** : An impossibility has occurred."
         << "\n*** FAILED *** : Magnet of type " << oo0->Type() << " sent to testBendMap."
         << "\n*** FAILED *** "
         << endl;
    return 7;
  }

  bool parallel = oo0->hasParallelFaces();

  cout << "PARTICLE PROPAGATION TEST" << endl;
  Proton proton( myOptions.energy );
  double px = sin(myOptions.bendAngle/2.0);

  if( parallel ) { proton.set_npx(px); }
  cout << "Initial: " << proton.State() << endl;
  oo0->propagate( proton );
  cout << "Final  : " << proton.State() << endl;

  double testValue = proton.get_npx();
  if( parallel ) { testValue += px;   }

  if( 1.0e-14 < std::abs( testValue ) ) {
    cout <<   "*** FAILED *** "
            "\n*** FAILED *** : " << __FILE__ << ", line " << __LINE__
         << "\n*** FAILED *** : Failed bend test: 1.0e-14 < "
         << std::abs( testValue )
         << "\n*** FAILED *** : Proton does not bend through correct angle"
         << "\n*** FAILED *** : in original " << oo0->Type() << ". bendTest_1 should have"
         << "\n*** FAILED *** : failed also!"
         << "\n*** FAILED *** "
         << endl;
    ret = 2;
  }
  proton.setStateToZero();

  if( parallel ) { proton.set_npx(px); }
  cout << "Initial: " << proton.State() << endl;
  oo1->propagate( proton );
  cout << "Mid    : " << proton.State() << endl;
  oo2->propagate( proton );
  cout << "Final  : " << proton.State() << endl;

  testValue = proton.get_npx();
  if( parallel ) { testValue += px;   }

  if( 1.0e-14 < std::abs( testValue ) ) {
    cout <<   "*** FAILED *** "
            "\n*** FAILED *** : " << __FILE__ << ", line " << __LINE__
         << "\n*** FAILED *** : Failed bend test: 1.0e-14 < "
         << std::abs( testValue )
         << "\n*** FAILED *** : Proton does not bend through correct angle."
         << "\n*** FAILED *** : in split " << oo0->Type() << "s."
         << "\n*** FAILED *** "
         << endl;
    ret = 2;
  }
  proton.setStateToZero();


  Mapping firstMap, splitMap;
  {
  cout << "JETPARTICLE PROPAGATION TEST" << endl;
  proton.set_npx( sin(myOptions.bendAngle/2.0) );
  JetProton jpr( proton );
  cout << "BEFORE FULL BEND" << endl;
  jpr.State().printCoeffs();
  oo0->propagate( jpr );
  cout << "AFTER FULL BEND" << endl;
  jpr.State().printCoeffs();
  firstMap = jpr.State();
  }
  proton.setStateToZero();

  {
  proton.set_npx( sin(myOptions.bendAngle/2.0) );
  JetProton jpr( proton );
  cout << "BEFORE SPLIT BENDS" << endl;
  jpr.State().printCoeffs();
  oo1->propagate( jpr );
  oo2->propagate( jpr );
  cout << "AFTER SPLIT BENDS" << endl;
  jpr.State().printCoeffs();
  splitMap = jpr.State();
  }
  proton.setStateToZero();


  Mapping diffMap( splitMap - firstMap );
  cout << "\n\n--- BEGIN DIFFERENCE MAP ----------\n" << endl;
  diffMap.printCoeffs();
  cout << "\n--- END   DIFFERENCE MAP ----------\n" << endl;


  JLterm termPtr;
  IntArray exps( diffMap[0].getEnvNumVar() );

  double scaler = 1;
  double ccc = 0;
  for(   int counter = 0
       ; counter < diffMap.Dim()
       ; ++counter ) {
    Jet zlorfik = diffMap[ counter ];
    bool constTerm = true;
    for(   Jet::iterator iter = zlorfik.begin()
         ; iter != zlorfik.end()
         ; ++iter ) {
      termPtr = *iter;
      exps = termPtr.exponents( zlorfik.Env() );
      ccc  = termPtr.coefficient();

      if( constTerm ) {
        if( 0 != exps.Sum() ) {
          cout <<   "*** FAILED *** "
                  "\n*** FAILED *** : " << __FILE__ << ", line " << __LINE__
               << "\n*** FAILED *** : A serious mxyzptlk error occurred at map component "
               <<                     counter
               << "\n*** FAILED *** : The first term in the map had a non-zero exponent."
               << "\n*** FAILED *** : This test will be aborted."
               << "\n*** FAILED *** : "
               << endl;
          return 999;
        }
        scaler = 1;
        constTerm = false;
      }
      else {
        for( int k = 0; k < exps.Dim(); ++k ) {
          if( 0 != exps[k] ) { scaler *= pow(myOptions.sigma[k],exps[k]); }
        }
        scaler /= myOptions.sigma[counter];
      }

      if( myOptions.mapTolerance[counter] < std::abs(scaler*ccc) ) { 
        cout <<   "*** FAILED *** "
                "\n*** FAILED *** : " << __FILE__ << ", line " << __LINE__
             << "\n*** FAILED *** : Map component "
             <<                     counter
             <<                   " failed difference:  at exponent "
             <<                     exps
             <<                     " the coefficient = "
             <<                     ccc
             << "\n*** FAILED *** : maps are not equivalent at order "
             <<                     exps.Sum()
             << "\n*** FAILED *** "
             << endl;
        ret = 3;
      }
    }
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
  } // drift


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
  } // quadrupole


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
  } // sextupole


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
  } // octupole


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
  ret += testBendMap( oo0, oo1, oo2, myOptions );


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
  ret += testBendMap( oo0, oo1, oo2, myOptions );
  } // sbend


  { // rbend
  cout << "\n\n--- TESTING RBEND ---"
            "\n--- WITH PARALLEL EDGES ---\n"
       << endl;
  oo0 = ElmPtr( new rbend ( "", myOptions.bodyLength
                              , myOptions.bendField
                              , myOptions.bendAngle ) );

  Proton proton( myOptions.energy );
  proton.set_npx( sin(myOptions.bendAngle/2.0) );
  {
  RefRegVisitor rrv( proton );
  oo0->accept(rrv);
  }
  proton.setStateToZero();


  oo0->Split( myOptions.pct, oo1, oo2 );


  proton.set_npx( sin(myOptions.bendAngle/2.0) );
  {
  RefRegVisitor rrv( proton );
  oo1->accept(rrv);
  oo2->accept(rrv);
  }
  proton.setStateToZero();

  ret += testLengths( oo0, oo1, oo2, myOptions.pct );
  ret += testStrengths( oo0, oo1, oo2, myOptions.pct );
  ret += testBendMap( oo0, oo1, oo2, myOptions );
  } // rbend


  { // CF_rbend
  cout << "\n\n--- TESTING CF_RBEND ---"
            "\n--- WITH PARALLEL EDGES ---\n"
       << endl;
  oo0 = ElmPtr( new CF_rbend ( "", myOptions.bodyLength
                                 , myOptions.bendField
                                 , myOptions.bendAngle ) );

  Proton proton( myOptions.energy );
  proton.set_npx( sin(myOptions.bendAngle/2.0) );
  {
  RefRegVisitor rrv( proton );
  oo0->accept(rrv);
  }
  proton.setStateToZero();


  oo0->Split( myOptions.pct, oo1, oo2 );


  proton.set_npx( sin(myOptions.bendAngle/2.0) );
  {
  RefRegVisitor rrv( proton );
  oo1->accept(rrv);
  oo2->accept(rrv);
  }
  proton.setStateToZero();

  ret += testLengths( oo0, oo1, oo2, myOptions.pct );
  ret += testStrengths( oo0, oo1, oo2, myOptions.pct );
  ret += testBendMap( oo0, oo1, oo2, myOptions );
  } // CF_rbend


  { // CF_sbend
  cout << "\n\n--- TESTING CF_SBEND ---"
            "\n--- WITH NORMAL ENTRY EDGES ---\n"
       << endl;
  oo0 = ElmPtr( new CF_sbend ( "", myOptions.arcLength
                                 , myOptions.bendField
                                 , myOptions.bendAngle
                                 , 0
                                 , 0 ) );

  Proton proton( myOptions.energy );
  {
  RefRegVisitor rrv( proton );
  oo0->accept(rrv);
  }
  proton.setStateToZero();


  oo0->Split( myOptions.pct, oo1, oo2 );


  {
  RefRegVisitor rrv( proton );
  oo1->accept(rrv);
  oo2->accept(rrv);
  }
  proton.setStateToZero();

  ret += testLengths( oo0, oo1, oo2, myOptions.pct );
  ret += testStrengths( oo0, oo1, oo2, myOptions.pct );
  ret += testBendMap( oo0, oo1, oo2, myOptions );
  } // CF_sbend


  return ret;
}
