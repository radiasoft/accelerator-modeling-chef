#include <iostream>
#include <integrator/Splinterpoland.h>

using namespace std;

Splinterpoland::Splinterpoland()
: _M(0), _d(0), _A(0), _n(0), _spl(0.0), _spr(0.0)
{
}


Splinterpoland::~Splinterpoland()
{
  _wipeClean();
}


void Splinterpoland::_build()
{
  int i;
  double hl, hr, yl, yr, y, ml, mr;
  double h, dy;

  if( 0 != _M ) {
    cerr << "\n*** WARNING *** "
         << "\n*** WARNING ***  File: " << __FILE__
                          << "  Line: " << __LINE__
         << "\n*** WARNING ***  double Splinterpoland::_build()"
         << "\n*** WARNING ***  Attempt to construct spline parameters"
         << "\n*** WARNING ***  after they've already been calculated."
         << "\n*** WARNING ***  "
         << "\n*** WARNING ***  Why are you doing this??"
         << "\n*** WARNING ***  "
         << endl;
    return;
  }


  // This follows E.K.Blum, Numerical Analysis and Computation
  // Theory and Practice, Addison-Wesley.
  // 
  // I have added setting boundary conditions on the derivative.

  _sort();

  _n = _nodes.size();
  _M = new MatrixD( _n, 1 );
  _d = new MatrixD( _n, 1 );
  _A = new MatrixD( _n, _n );

  for( i = 0; i < _n; i++ ) {
    (*_A)(i,i) = 2.0;
  }

  (*_A)(0,1)       = 1.0;   // arbitrary; should be < 2
  (*_A)(_n-1,_n-2) = 1.0;   // arbitrary; should be < 2

  dy = ( _nodes[1]._y - _nodes[0]._y );
  h  = ( _nodes[1]._x - _nodes[0]._x );
  if( h <= 0.0 ) {
    cerr << "\n*** ERROR *** "
         << "\n*** ERROR ***  File: " << __FILE__
                        << "  Line: " << __LINE__
         << "\n*** ERROR ***  double Splinterpoland::_build()"
         << "\n*** ERROR ***  Mesh error encountered."
         << "\n*** ERROR *** "
         << endl;
    _wipeClean();
    exit(-12);
  }
  (*_d)(0,0) =   6.0*( (dy/h) - _spl )/h;

  dy = ( _nodes[_n-1]._y - _nodes[_n-2]._y );
  h  = ( _nodes[_n-1]._x - _nodes[_n-2]._x );
  if( h <= 0.0 ) {
    cerr << "\n*** ERROR *** "
         << "\n*** ERROR ***  File: " << __FILE__
                        << "  Line: " << __LINE__
         << "\n*** ERROR ***  double Splinterpoland::_build()"
         << "\n*** ERROR ***  Mesh error encountered."
         << "\n*** ERROR *** "
         << endl;
    _wipeClean();
    exit(-13);
  }
  (*_d)(_n-1,0) = - 6.0*( (dy/h) - _spr )/h;

  for( i = 1; i <= _n-2; i++ ) {
    hl = _nodes[i]._x   - _nodes[i-1]._x;
    hr = _nodes[i+1]._x - _nodes[i]._x;

    if( (hl <= 0.0) || (hr <= 0.0) ) {
      cerr << "\n*** ERROR *** "
           << "\n*** ERROR ***  File: " << __FILE__
  	                  << "  Line: " << __LINE__
           << "\n*** ERROR ***  double Splinterpoland::_build()"
           << "\n*** ERROR ***  Mesh error encountered."
           << "\n*** ERROR *** "
           << endl;
      _wipeClean();
      exit(-11);
    }

    yl = _nodes[i-1]._y;
    y  = _nodes[i  ]._y;
    yr = _nodes[i+1]._y;
     
    (*_A)(i,i-1) = hl/( hl + hr );
    (*_A)(i,i+1) = hr/( hl + hr );

    (*_d)(i,0) = 6.0*( (yr - y)/hr - (y - yl)/hl ) / ( hl + hr );
  }
  

  (*_M) = _A->inverse() * (*_d);
}


double Splinterpoland::operator()( const double& x )
{
  static int i;
  static double hl, hr, yl, yr, y, ml, mr;
  static double ret;
 
  // If necessary, contruct parameters before evaluation.
  if( 0 == _M ) {
    _build();
  }

  // Evaluate function
  // if( _n < 3  ||  x < _nodes[0]._x  ||  x > _nodes[_n-1]._x  ) 
  // { return 0; }
  // if( x == _nodes[0]._x    ) { return _nodes[0]._y;    }
  // if( x == _nodes[_n-1]._x ) { return _nodes[_n-1]._y; }

  if( _n < 3                ) { return 0; }
  if( x <= _nodes[0]._x     ) { return _nodes[0]._y; }
  if( x >= _nodes[_n-1]._x  ) { return _nodes[_n-1]._y; }

  i = 0;
  int im;
  while( x > _nodes[i]._x ) { im = i; i++; }

  double h = _nodes[i]._x - _nodes[im]._x;
  double p = ( x - _nodes[im]._x ) / h;
  double q = 1.0 - p;

  yl = _nodes[im]._y;
  yr = _nodes[i]._y;

  ml = (*_M)(im,0)*h*h/6.0;
  mr = (*_M)(i,0)*h*h/6.0;

  // double ret =   ml*q*q*q 
  //              + mr*p*p*p 
  //              + (yr - mr)*p
  //              + (yl - ml)*q;

  ret =   p*( yr - mr*( 1.0 - p*p ) ) 
        + q*( yl - ml*( 1.0 - q*q ) );

  return ret;
}


double Splinterpoland::derivative( const double& x )
{
  // This follows E.K.Blum, Numerical Analysis and Computation
  // Theory and Practice, Addison-Wesley.
  static int i, im;
  static double h, p, q, yl, yr, ml, mr;
  static double ret;
  
  ret = 0.0;

  // If necessary, contruct parameters before evaluation.
  if( 0 == _M ) {
    _build();
  }

  // Evaluate function
  if( _n < 3  ||  x < _nodes[0]._x  ||  x > _nodes[_n-1]._x  ) 
  { return 0; }
  if( x == _nodes[0]._x    ) { return _spl; }
  if( x == _nodes[_n-1]._x ) { return _spr; }

  i = 0; 
  im = 0; // unnecessary
  while( x > _nodes[i]._x ) { im = i; i++; }

  h = _nodes[i]._x - _nodes[im]._x;
  p = ( x - _nodes[im]._x );
  q = h - p;

  yl = _nodes[im]._y;
  yr = _nodes[i]._y;

  ml = (*_M)(im,0);
  mr = (*_M)(i,0);

  ret  = mr*p*p - ml*q*q + 2.0*( yr - yl );
  ret /= (2.0*h);
  ret += ( ml - mr )*h/6.0;

  return ret;
}


int Splinterpoland::acceptNode( double x, double y )
{
  if( 0 != _M ) { _wipeClean(); }

  Node w;
  w._x = x;
  w._y = y;

  _nodes.push_back(w);
  
  return 0;
}



bool Splinterpoland::Node::operator<( const Node& n ) const
{
  return ( _x < n._x );
}


bool Splinterpoland::Node::cmp::operator()( const Node& x, const Node& y ) const
{
  return ( x < y );
}


void Splinterpoland::_sort()
{
  std::vector<Node>::iterator first = _nodes.begin();
  std::vector<Node>::iterator last  = _nodes.end();

  Node::cmp lessThan;

  std::sort( first, last, lessThan );
}


void Splinterpoland::_wipeClean()
{
  if( 0 != _M ) {
    delete _M;  _M = 0;
    delete _A;  _A = 0;
    delete _d;  _d = 0;
  }
  _n = 0;
}


void Splinterpoland::setLeftDerivative( double x )
{
  _spl = x;
}


void Splinterpoland::setRightDerivative( double x )
{
  _spr = x;
}


void Splinterpoland::setBoundaryDerivatives( double xl, double xr )
{
  _spl = xl;
  _spr = xr;
}

