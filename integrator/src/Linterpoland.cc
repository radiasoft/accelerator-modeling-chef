#include "Linterpoland.h"

Linterpoland::Linterpoland()
{
}


Linterpoland::~Linterpoland()
{
}


double Linterpoland::operator()( const double& x )
{
  static int n;
  static double p;

  n = _nodes.size();

  if( n < 2  )               { return 0.0;            }
  if( x <= _nodes[0]._x )    { return _nodes[0]._y;   }
  if( x >= _nodes[n-1]._x  ) { return _nodes[n-1]._y; }

  int i = 0;
  int im;
  while( x > _nodes[i]._x ) { im = i; i++; }

  
  p = ( x - _nodes[im]._x ) / ( _nodes[i]._x - _nodes[im]._x );

  return (  (1.0-p) * _nodes[im]._y  + p * _nodes[i]._y  );
}


double Linterpoland::derivative( const double& x )
{
  static int n;
  static double p;

  n = _nodes.size();

  if( n < 2  ||  x <= _nodes[0]._x  ||  x >= _nodes[n-1]._x  ) 
  { return 0; }

  int i = 0;
  int im;
  while( x > _nodes[i]._x ) { im = i; i++; }

  return ( ( _nodes[i]._y - _nodes[im]._y ) / 
           ( _nodes[i]._x - _nodes[im]._x )   );
}


int Linterpoland::acceptNode( double x, double y )
{
  Node w;
  w._x = x;
  w._y = y;

  _nodes.push_back(w);
  _sort();
  
  return 0;
}



bool Linterpoland::Node::operator<( const Node& n ) const
{
  return ( _x < n._x );
}


bool Linterpoland::Node::cmp::operator()( const Node& x, const Node& y ) const
{
  return ( x < y );
}


void Linterpoland::_sort()
{
  std::vector<Node>::iterator first = _nodes.begin();
  std::vector<Node>::iterator last  = _nodes.end();

  Node::cmp lessThan;

  std::sort( first, last, lessThan );
}


Linterpoland& Linterpoland::operator=( const Linterpoland& x )
{
  if( this != &x ) {
    _nodes = x._nodes;
  }

  return (*this);
}


void Linterpoland::removeDomain( double lower, double upper )
{
  std::vector<Node>::iterator first = _nodes.begin();
  std::vector<Node>::iterator last  = _nodes.end();

  std::vector<Node>::iterator pos = first;

  double x;
  while( pos != last ) {
    x = (*pos)._x;
    if( (lower < x ) || (x < upper) ) {
      _nodes.erase(pos);
    }
    pos++;
  }
}


void Linterpoland::clear()
{
  _nodes.clear();
}
