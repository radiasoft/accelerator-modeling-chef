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


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CurveData::CurveData( boost::shared_array<double> x , boost::shared_array<double> y, int size, std::string label):
  xarray_(x), yarray_(y), size_(size),  xaxis_(CurveData::xBottom), yaxis_(CurveData::yLeft), color_( Color(0,0,0) ), label_(label)  
{

  xmin_ = *std::min_element( &xarray_[0], &xarray_[0] + size_);
  xmax_ = *std::max_element( &xarray_[0], &xarray_[0] + size_);

  ymin_ = *std::min_element( &yarray_[0], &yarray_[0] + size_);
  ymax_ = *std::max_element( &yarray_[0], &yarray_[0] + size_);


}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CurveData::CurveData( double const* x , double const* y, 
                                    int size, std::string label )
: size_(size), xaxis_(CurveData::xBottom), yaxis_(CurveData::yLeft), color_( Color(0,0,0) ), label_(label)  

{

  xarray_ = boost::shared_array<double> ( new double[ size ] ); 
  yarray_ = boost::shared_array<double> ( new double[ size ] ); 
  
  std::copy( x, x+size_, &xarray_[0] );
  std::copy( y, y+size_, &yarray_[0] );

  xmin_ = *std::min_element( &xarray_[0], &xarray_[0] + size_);
  xmax_ = *std::max_element( &xarray_[0], &xarray_[0] + size_);

  ymin_ = *std::min_element( &yarray_[0], &yarray_[0] + size_);
  ymax_ = *std::max_element( &yarray_[0], &yarray_[0] + size_);

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


CurveData::CurveData( std::vector<double> const& x ,  std::vector<double> const& y, std::string label ):
  xarray_(0), yarray_(0), size_(0), xaxis_(CurveData::xBottom), yaxis_(CurveData::yLeft), color_( Color(0,0,0) ), label_(label) 
{

  // copy the data 

  size_ = std::min ( x.size(), y.size() );  

  if ( size_ <= 0 ) return ; // this is an error condition FIXME !
 
  xarray_ = boost::shared_array<double>(  new double[ x.size() ] );
  yarray_ = boost::shared_array<double>(  new double[ y.size() ] );

  std::copy( &x[0], &x[0]+size_, &xarray_[0] );
  std::copy( &y[0], &y[0]+size_, &yarray_[0] );

 // find and store min/max values

   xmin_ = *std::min_element( x.begin(), x.end() );
   xmax_ = *std::max_element( x.begin(), x.end() );

   ymin_ = *std::min_element( y.begin(), y.end() );
   ymax_ = *std::max_element( y.begin(), y.end() );



}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


CurveData::CurveData(CurveData const& cd)           
  :  QwtData(),    xarray_(cd.xarray_),  yarray_(cd.yarray_), size_(cd.size_),  
                   xmin_(cd.xmin_),      xmax_(cd.xmax_), 
                   ymin_(cd.ymin_),      ymax_(cd.ymax_), 
                   xaxis_(cd.xaxis_),    yaxis_(cd.yaxis_),                  
                   color_(cd.color_),    label_(cd.label_) 
{}
 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CurveData&  CurveData::operator=( CurveData const& cd)
{
  if ( &cd == this ) return *this;
  
  xarray_ =  cd.xarray_;
  yarray_ =  cd.yarray_;
  size_   =  cd.size_;


  xmin_   =  cd.xmin_;
  xmax_   =  cd.xmax_;
  ymin_   =  cd.ymin_;
  ymax_   =  cd.ymax_;

  xaxis_  =  cd.xaxis_;
  yaxis_  =  cd.yaxis_;

  label_  =  cd.label_; 

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


double CurveData::x (size_t i) const
{
  return xarray_[i];
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


double CurveData::y (size_t i) const
{
  return yarray_[i];
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
  return xmin_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CurveData::xMax() const
{
  return xmax_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CurveData::yMin() const
{
  return ymin_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double CurveData::yMax() const
{
  return ymax_;
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
