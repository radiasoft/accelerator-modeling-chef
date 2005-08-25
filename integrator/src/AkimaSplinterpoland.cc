#ifdef HAS_GSL

#include <iostream>

#include "AkimaSplinterpoland.h"

using namespace std;

AkimaSplinterpoland::AkimaSplinterpoland()
  : _spline(0), _accelerator(0), _n(0),
    _firstX(0.0), _firstY(0.0), _lastX(0.0), _lastY(0.0)
{
}


AkimaSplinterpoland::~AkimaSplinterpoland()
{
  _wipeClean();
}


void AkimaSplinterpoland::_build()
{
  if( 0 != _spline ) {
    cerr << "\n*** WARNING *** "
         << "\n*** WARNING ***  File: " << __FILE__
                          << "  Line: " << __LINE__
         << "\n*** WARNING ***  double AkimaSplinterpoland::_build()"
         << "\n*** WARNING ***  Attempt to construct spline parameters"
         << "\n*** WARNING ***  after they've already been calculated."
         << "\n*** WARNING ***  "
         << "\n*** WARNING ***  Why are you doing this??"
         << "\n*** WARNING ***  "
         << endl;
    return;
  }

  _sort();

  _n = _nodes.size();
  double x[_n];
  double y[_n];
  int i;
  for( i = 0; i < _n; i++ ) {
    x[i] = _nodes[i]._x;
    y[i] = _nodes[i]._y;
  }

  _firstX = x[0];
  _firstY = y[0];
  _lastX  = x[_n-1];
  _lastY  = y[_n-1];

  _spline = gsl_spline_alloc( gsl_interp_akima, _n );
  gsl_spline_init( _spline, x, y, _n );

  _accelerator = gsl_interp_accel_alloc();
}


double AkimaSplinterpoland::operator()( const double& x )
{
  // If necessary, contruct parameters before evaluation.
  if( 0 == _spline ) {
    _build();
  }

  if( x <= _firstX ) { return _firstY; }
  if( _lastX <= x  ) { return _lastY; }
  return gsl_spline_eval( _spline, x, _accelerator );
}


double AkimaSplinterpoland::derivative( const double& x )
{
  // If necessary, contruct parameters before evaluation.
  if( 0 == _spline ) {
    _build();
  }

  if( x <= _firstX ) { return 0.0; }
  if( _lastX <= x  ) { return 0.0; }
  return gsl_spline_eval_deriv( _spline, x, _accelerator );
}


int AkimaSplinterpoland::acceptNode( double x, double y )
{
  if( 0 != _spline ) { _wipeClean(); }

  Node w;
  w._x = x;
  w._y = y;

  _nodes.push_back(w);
  
  return 0;
}



bool AkimaSplinterpoland::Node::operator<( const Node& n ) const
{
  return ( _x < n._x );
}


bool AkimaSplinterpoland::Node::cmp::operator()( const Node& x, const Node& y ) const
{
  return ( x < y );
}


void AkimaSplinterpoland::_sort()
{
  std::vector<Node>::iterator first = _nodes.begin();
  std::vector<Node>::iterator last  = _nodes.end();

  Node::cmp lessThan;

  std::sort( first, last, lessThan );
}


void AkimaSplinterpoland::_wipeClean()
{
  if( 0 != _spline ) {
    gsl_spline_free( _spline );
    _spline = 0;
    gsl_interp_accel_free( _accelerator ); 
    _accelerator = 0;
  }
  _n = 0;
  _firstX = 0.0;
  _firstY = 0.0;
  _lastX  = 0.0;
  _lastY  = 0.0;
}

#endif
