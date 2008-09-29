/*************************************************************************
**************************************************************************
**************************************************************************
******                                    
******  File:   ResonanceLine.cc   
******                                                                
******  Copyright (c) Fermi Research Alliance LLC
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-07CH11359.
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******
******                                                                
******  Author:    Jean-Francois Ostiguy                                     
******             ostiguy@fnal.gov                         
****** 
**************************************************************************
**************************************************************************
*************************************************************************/

#include <TuneDiagram.h>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <algorithm>


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TuneDiagram::ResonanceLine::ResonanceLine( int nx, int ny, int n ,  double xoffset, double yoffset )
  : QwtData(), nx_( double(nx) ), ny_( double(ny) ), n_( double(n) ), xoffset_( xoffset), yoffset_(yoffset)  
{

  std::vector<Point> points;

  if ( nx == 0 ) { 
    if (  (n_/ny_ >= 0 ) && ( n_/ny_ <= 1.0) ) { 
     points.push_back( Point (0.0, n_/ny_)); 
     points.push_back( Point (1.0, n_/ny_)); 
    }
  }
  else if ( ny == 0 ) {
   if (  (n_/nx_ >= 0 ) && ( n_/nx_ <= 1.0) ) { 
    points.push_back( Point( n_/nx_, 0.0) );
    points.push_back( Point( n_/nx_, 1.0) );
   }
  }
  else { 

     // case  x = 0 ...
     Point p1(0.0, n_/ny_);              if ( (p1.y() >= 0.0 ) && (p1.y() <= 1.0 ) ) { points.push_back(p1); }  

     // case  y = 0 ... 
     Point p2( n_/nx_, 0.0);             if ( (p2.x() >= 0.0 ) && (p2.x() <= 1.0 ) ) { points.push_back(p2); }   

     // case  x = 1 ...
     Point p3( 1.0, (n_ - nx_)/ ny_ );   if ( (p3.y() >= 0.0 ) && (p3.y() <= 1.0 ) ) { points.push_back(p3); } 

     // case  y = 1 ...
     Point p4( (n_-ny_)/nx_,  1.0 );     if ( (p4.x() >= 0.0 ) && (p4.x() <= 1.0 ) ) { points.push_back(p4); } 
  }

  std::sort( points.begin(), points.end() ); // sort according to increasing  x

  points.erase( std::unique( points.begin(), points.end() ), points.end() ); // erase duplicates

  if (points.size() != 2 ) { 
    point1_ = Point(0.0, 0.0);    // not visible
    point2_ = Point(0.0, 0.0);  
  }
  else {
    point1_ = points[0];
    point2_ = points[1];
  }
   
  } 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TuneDiagram::ResonanceLine::ResonanceLine( ResonanceLine const& o)
  : QwtData(o), nx_(o.nx_), ny_(o.ny_), n_(o.n_), 
    point1_(o.point1_),  point2_(o.point2_),
    xoffset_( o.xoffset_), yoffset_(o.yoffset_)  
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TuneDiagram::ResonanceLine::~ResonanceLine()
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TuneDiagram::ResonanceLine* TuneDiagram::ResonanceLine::copy () const
{
  return new ResonanceLine(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

size_t  TuneDiagram::ResonanceLine::size () const
{ 
  return 2; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double TuneDiagram::ResonanceLine::x(size_t i) const
{
  return  ( xoffset_ + (( i%2 == 0) ? point1_.x() : point2_.x() ) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double TuneDiagram::ResonanceLine::y(size_t i)    const
{
  return  ( yoffset_ + (( i%2 == 0) ? point1_.y() : point2_.y() ) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool TuneDiagram::ResonanceLine::isVisible() const
{ 
  return !( point1_ == point2_ ); 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QwtDoubleRect   TuneDiagram::ResonanceLine::boundingRect () const
{
  return QwtDoubleRect(xoffset_, yoffset_, 1.0, 1.0);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

