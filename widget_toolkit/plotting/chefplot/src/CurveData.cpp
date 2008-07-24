/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Chef:      A Qt-based Application 
******             Layered on top of of BEAMLINE.
******                                                                
******  File:      CurveData.cpp
******                                                                
******  Copyright (c) Universities Research Association, Inc.   
******                All Rights Reserved                             
******                                                                
******  Author:    Jean-Francois Ostiguy
******                                                                
******             Fermilab                                           
******             Batavia, IL  60510                                
******             ostiguy@fnal.gov                         
******                                                                
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******* U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******* The U.S. Government retains a world-wide non-exclusive, 
******* royalty-free license to publish or reproduce documentation 
******* and software for U.S. Government purposes. This software 
******* is protected under the U.S. and Foreign Copyright Laws. 
******* URA/FNAL reserves all rights.
*******                                                                
**************************************************************************
**************************************************************************
*************************************************************************/

#include <CurveData.h>
#include <algorithm>
#include <iostream>
#include <boost/iterator/transform_iterator.hpp>

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CurveData::CurveData( int size, std::string label )
: array_(), size_(size),  
  xaxis_(CurveData::xBottom), yaxis_(CurveData::yLeft), color_( Color(0,0,0) ), 
  connected_(true), label_(label)  
{
  array_ = boost::shared_array<Point> ( new Point[size] ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CurveData::CurveData(CurveData const& cd)           
  :  QwtData(),    array_(cd.array_),  size_(cd.size_),  
                   xaxis_(cd.xaxis_),  yaxis_(cd.yaxis_),                  
                   color_(cd.color_),  connected_(cd.connected_),
                   label_(cd.label_) 
{}
 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CurveData&  CurveData::operator=( CurveData const& cd)
{
  if ( &cd == this ) return *this;
  
  array_  =  cd.array_;
  size_   =  cd.size_;

  xaxis_  =  cd.xaxis_;
  yaxis_  =  cd.yaxis_;

  connected_  =  cd.connected_; 
  label_      =  cd.label_; 

  return *this;
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CurveData* CurveData::copy() const   // this should be called clone(), but Qwt uses copy() 
{
   return new CurveData( *this );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CurveData::~CurveData()
{
  // data destroyed when smart_array[s] go out of scope.   
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

size_t CurveData::size () const 
{
  return  size_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CurveData::iterator CurveData::begin() 
{
  return &array_[0]; 
}  

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CurveData::iterator      CurveData::end()
{
  return  &array_[0]+ size_;
}  

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CurveData::const_iterator  CurveData::begin() const 
{
  return &array_[0]; 
}  

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CurveData::const_iterator      CurveData::end() const
{
  return  &array_[0]+ size_;
}  

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


double CurveData::x (size_t i) const
{
  return array_[i].x;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


double CurveData::y (size_t i) const
{
  return array_[i].y;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CurveData::setAxes( CurveData::Axis xid, CurveData::Axis yid  )
{
  xaxis_ = xid;
  yaxis_ = yid;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
 
CurveData::Axis CurveData::getXAxis() const 
{
  return xaxis_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 
CurveData::Axis CurveData::getYAxis() const
{
  return yaxis_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CurveData::xMin() const 
{
  typedef boost::transform_iterator<CurveData::XValue, const_iterator> x_const_iterator;

  x_const_iterator  ixbeg(begin(), XValue() );
  x_const_iterator  ixend(end(),   XValue() );

  return *std::min_element( ixbeg, ixend);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CurveData::xMax() const
{
  typedef boost::transform_iterator<CurveData::XValue, const_iterator> x_const_iterator;

  x_const_iterator  ixbeg(begin(), XValue() );
  x_const_iterator  ixend(end(),   XValue() );

  return *std::max_element( ixbeg, ixend);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CurveData::yMin() const
{
  typedef boost::transform_iterator<CurveData::YValue, const_iterator> y_const_iterator;

  y_const_iterator  iybeg(begin(), YValue() );
  y_const_iterator  iyend(end(),   YValue());

  return *std::min_element( iybeg, iyend);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CurveData::yMax() const
{
  typedef boost::transform_iterator<CurveData::YValue, const_iterator> y_const_iterator;

  y_const_iterator  iybeg(begin(), YValue() );
  y_const_iterator  iyend(end(),   YValue() );

  return *std::max_element( iybeg, iyend);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


std::string const& CurveData::getLabel() const
{
  return label_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void CurveData::setLabel( std::string const& label ) 
{
  label_ = label;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CurveData::setColor( CurveData::Color const& color)
{
  color_ = color;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CurveData::Color const&  CurveData::getColor() const
{
  return color_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CurveData::setConnected(bool set)
{
  connected_ = set;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool CurveData::isConnected() const
{
  return connected_;
}
