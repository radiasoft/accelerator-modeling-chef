#include <basic_toolkit/Frame.h>

using namespace std;

const double twopi = 6.2831853072;
const double deg_to_rad = twopi/360.0;

void pitch( Frame& f, double degrees )
{
  // Locally pitches the frame about its origin
  //   through an angle given in degrees.
  // A "pitch" is a rotation about the frame's x-axis.
  f.rotate( degrees*deg_to_rad, f.getxAxis(), false );
}


void yaw( Frame& f, double degrees )
{
  // Locally yaws the frame about its origin 
  //   through an angle given in degrees.
  // A "yaw" is a rotation about the frame's y-axis.
  f.rotate( degrees*deg_to_rad, f.getyAxis(), false );
}


void roll( Frame& f, double degrees )
{
  // Locally rolls the frame about its origin 
  //   through an angle given in degrees.
  // A "roll" is a rotation about the frame's z-axis.
  f.rotate( degrees*deg_to_rad, f.getzAxis(), false );
}


void rotateFrame( Frame& f, const Vector& p, const Vector& v, double theta )
{
  // Rotates the entire frame, including the origin.
  // p           = (fixed) point of rotation
  // v           = axis of rotation
  // theta [deg] = rotation angle
  Frame g;
  g.translate( p );
  Frame fp( f.relativeTo(g) );
  fp.rotate( theta*deg_to_rad, v, true );
  f = fp.patchedOnto( g );
}


void showResults( const Frame& f, Vector p, Vector v)
{
  // Prints p and v as seen within Frame f.
  f.convertInPlace( p, v );
  cout << "p = "   << p
       << ";   v = " << v 
       << endl;
}


int main() 
{
  enum { x = 0, y, z };

  // Vector v is "attached" to point p.
  //   fyi: the pair <p,v> is called a "tangent vector."
  Vector p(3), v(3);
  v(x) = 1.0;
  v(y) = 2.0;
  v(z) = 3.0;

  cout << "Initial tangent vector.\n"
       << "p = "   << p
       << ";   v = " << v 
       << endl;


  // Instantiate a frame. By default, it is the identity.
  cout << "\nAs viewed from the default frame.\n";
  Frame f;
  showResults( f, p, v );


  // Rotate the frame by 180 degrees about its y-axis
  //   and translate it by 100 units in the "global" +z direction,
  //   which would then be the frame's "local" -z direction.
  cout << "\nAfter 180 degree rotation about y-axis:\n";
  Vector r(3);
  r(z) = 100.0;
  f.rotate( 180.0*deg_to_rad, f.getyAxis(), false );
  f.translate( r );
  showResults( f, p, v );


  // In sequence, pitch, yaw and roll the frame by 10 degrees.
  cout << "\nPitch:\n";
  pitch(f,10.0);
  showResults( f, p, v );

  cout << "Yaw:\n";
  yaw(f,10.0);
  showResults( f, p, v );

  cout << "Roll:\n";
  roll(f,10.0);
  showResults( f, p, v );


  // Now rotate the entire frame about the point p
  //   along the axis v through 10 degrees.
  cout << "\nGlobal rotation about <p,v>:\n";
  rotateFrame( f, p, v, 10.0 );
  showResults( f, p, v );
  cout << "After p <- p+v\n";
  showResults( f, p+v, v );


  // v(z) = v(z) + 1.0;
  // showResults( f, p, v );


  // Move the frame 20 units along its local +z direction.
  cout << "\nLocally move +20 along z.\n";
  f.translate( 20.0*f.getzAxis() );
  showResults( f, p, v );


  // Move the frame 20 units toward p.
  cout << "\nMove +20 toward p.\n";
  f.translate( 20.0*( ( p - f.getOrigin() ).Unit() ) );
  showResults( f, p, v );


  // Move 20 units in the "global" +x direction
  cout << "\nMove +20 in global x.\n";
  r(z) = 0.0;
  r(x) = 20.0;
  f.translate( r );
  showResults( f, p, v );


  // Locally rotate three times by 120 degrees 
  //   about an axis symmetrically positioned
  //   in the frame's first octant.
  cout << "\nC_3 subgroup rotations.\n";

  Vector w(3);
  w = f.getxAxis() + f.getyAxis() + f.getzAxis();

  cout << "Zero:\n";
  showResults( f, p, v );

  cout << "One:\n";
  f.rotate( 120.0*deg_to_rad, w, false);
  showResults( f, p, v );

  cout << "Two:\n";
  f.rotate( 120.0*deg_to_rad, w, false);
  showResults( f, p, v );

  cout << "Three:\n";
  f.rotate( 120.0*deg_to_rad, w, false);
  showResults( f, p, v );


  // Tween operation connecting two frames.
  cout << "\nA tween operation\n";
  Frame g(f);
  g.rotate( 120.0*deg_to_rad, w, false);
  g.setOrigin( 2.0*g.getOrigin() );
  
  cout << "First frame:\n";
  showResults( f, p, v );
  cout << "In between frames:\n";
  for( double pct = 0.0; pct < 1.05; pct += 0.1 ) {
    showResults( Frame::tween(f,g,pct), p, v );
  }
  cout << "Second frame:\n";
  showResults( g, p, v );

  return 0;
}
