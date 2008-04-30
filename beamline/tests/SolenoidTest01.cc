////////////////////////////////////////////////////////////
// 
// File:          SolenoidTest01.cc
// Author:        Leo Michelotti
// 
// Revision date: Saturday.  April 12, 2008.  (original version)
//                Wednesday. April 30, 2008.  (current  version)
//                
// 
////////////////////////////////////////////////////////////
// 
// Tests propagation through solenoid against benchmark for
// for solenoids within a range of lengths.
// 
// ------------
// COMMAND LINE
// ------------
// SolenoidTest01 [options]
// 
// -------
// OPTIONS
// -------
// Note: NNN represents an integer
//       XXX             a double
// 
// -x XXX   [m]  proton's initial x
//               : default = 0.002
// -y XXX   [m]  proton's initial y
//               : default = 0
// -xy           output mode: x y 
//               : default
// -lxy          output mode: l x y
// -ls           output mode: l state
// 
////////////////////////////////////////////////////////////

#include <iostream>

#include <beamline/Particle.h>
#include <beamline/Solenoid.h>
#include <beamline/beamline.h>
#include <beamline/RefRegVisitor.h>

using namespace std;

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

struct Options
{
  bool   valid;
  double pc;
  double energy;
  double x;
  double y;
  double lmin;
  double lmax;
  double dl;
  double solenoidField;   // T

  enum   DataOption { xy = 0, lxy, ls } data_option;

  Options( int, char**, int=0 );
};


Options::Options( int argc, char** argv, int lastargs )
:   valid(true)
  , energy(5)
  , x(0.002)
  , y(0)
  , lmin(0.1)
  , lmax(20)
  , dl(0.1)
  , solenoidField(4)
  , data_option(xy)
{
  Proton proton(energy);
  pc = proton.ReferenceMomentum();

  int limit = argc-lastargs;
  string s;
  int i = 1;
  while( valid && (i < limit) ) {
    s.assign( argv[i++] );
    if( '-' == s[0] ) {
      s.assign( s.substr(1) );
      if( s == string("x") ) {
        if( i < limit ) {
          x = atof( argv[i++] );
        }
        else {
          cerr << "\n*** ERROR *** Too few parameters." << endl;
          valid = false;
        }
      }
      else if( s == string("y") ) {
        if( i < limit ) {
          y = atof( argv[i++] );
        }
        else {
          cerr << "\n*** ERROR *** Too few parameters." << endl;
          valid = false;
        }
      }
      else if( s == string("xy") ) {
        data_option = xy;
      }
      else if( s == string("lxy") ) {
        data_option = lxy;
      }
      else if( s == string("ls") ) {
        data_option = ls;
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
/////////////////////////////////////////////////////////////////////


void benchmark_propagate( double l, Particle& p, Options const& myOptions )
{
  // Lifted and modified from beamline/src/SolenoidPropagators.cc
  // ; working revision 1.3
  // : sticky tag StRel20070808-patches (branch: 1.3.2)
  // : non-branch tag Apr_30_2008
  // : file date: June 26, 2007


  double length   = l;
  double strength = myOptions.solenoidField;

  // Entry edge effect
  // -----------------
  const double edgeKick = strength / ( 2.0* p.ReferenceBRho() );

  p.set_npx( p.get_npx() + edgeKick*(p.get_y()) );
  p.set_npy( p.get_npy() - edgeKick*(p.get_x()) );


  // Body field
  // -----------
  double dtheta =   ( strength * length / p.ReferenceBRho() ) 
                  / p.get_npz();
  
  double sn = sin(dtheta);
  double cs = cos(dtheta);

  double x  = p.get_x();
  double y  = p.get_y();
  double px = p.get_npx();
  double py = p.get_npy();

  p.set_npx(    cs*px + sn*py );
  p.set_npy( (-sn)*px + cs*py );
  
  cs = cs - 1.0;

  p.set_x( x + ( (    cs*(-py) + sn*px ) / (2.0*edgeKick) ) );
  p.set_y( y + ( ( (-sn)*(-py) + cs*px ) / (2.0*edgeKick) ) );


  // Exit edge effect
  // ----------------
  p.set_npx( p.get_npx() - edgeKick*(p.get_y()) );
  p.set_npy( p.get_npy() + edgeKick*(p.get_x()) );


  // It's about time
  // ----------------
  double refTime  = l/p.ReferenceBeta();
  double duration =   length 
                    / ( p.get_npz() * p.ReferenceMomentum() / p.Energy() );
  p.set_cdt ( p.get_cdt() + ( duration - refTime ) );
}


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void report( double l, Particle const& p, Options const& myOptions )
{
  switch( myOptions.data_option ) 
  {
    case Options::xy:
      cout << p.get_x() << "  " << p.get_y() << endl;
      break;
    case Options::lxy:
      cout << l << "  " << p.get_x() << "  " << p.get_y() << endl;
      break;
    case Options::ls:
      cout << l << "  " << p.State() << endl;
      break;
    default:
      cout << l << "  " << p.State() << endl;
      break;
  }
}

/////////////////////////////////////////////////////////////////////
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
    return -1;
  }

  int ret = 0;

  Proton p( myOptions.energy );
  p.set_x( myOptions.x );
  p.set_y( myOptions.y );

  report( 0, p, myOptions );

  for(   double l = myOptions.lmin
       ;        l < myOptions.lmax + (myOptions.dl/2.0)
       ;        l += myOptions.dl                     )
  {
    Solenoid s( "", l, myOptions.solenoidField );
    p.setStateToZero();
    RefRegVisitor registrar( p );
    s.accept(registrar);

    p.setStateToZero();
    p.set_x( myOptions.x );
    p.set_y( myOptions.y );

    Proton bp(p);

    s.propagate(p);
    benchmark_propagate( l, bp, myOptions );
    
    report( l, p, myOptions );

    if( 1.0e-12 < ( p.State() - bp.State() ).Norm() ) {
      cout <<   "*** FAILED *** : "
              "\n*** FAILED *** : " << __FILE__ << ", line " << __LINE__
           << "\n*** FAILED *** : Proton state does not match benchmark." 
           << "\n*** FAILED *** : state     = " << p.State()
           << "\n*** FAILED *** : benchmark = " << bp.State()
           << "\n*** FAILED *** : "
           << endl;
      ret += 1;
    }
  }

  return ret;
}
