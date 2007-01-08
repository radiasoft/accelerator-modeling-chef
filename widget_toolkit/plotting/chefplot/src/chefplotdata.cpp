/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF:      A Qt-based Application 
******             Layered on top of of BEAMLINE.
******                                                                
******  File:      chefplotdata.cpp
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

#include <CHEFPlotData.h>
#include <CHEFCurve.h>
#include <Curve.h>
#include <beamline.h>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_classes.h>
#include <qstring.h>


CurveData::CurveData( boost::shared_array<double> x , boost::shared_array<double> y, int size):
  xarray_(x), yarray_(y), size_(size) 

{

  const double* xp = xarray_.get();
  const double* yp = yarray_.get();

  xmin_ = *std::min_element( xp, xp+ size_);
  xmax_ = *std::max_element( xp, xp+ size_);

  ymin_ = *std::min_element( yp, yp+ size_);
  ymax_ = *std::max_element( yp, yp+ size_);


}

//..........................................................................................

CurveData::CurveData( const double* x , const double* y, int size):
  xarray_( new double[size] ), yarray_( new double[size] ), size_(size) 

{

  // copy the data 

  double* xdum = xarray_.get();
  double* ydum = yarray_.get();

  for ( int i=0; i<size_ ; ++i) 
  {

    *xdum = *x; ++xdum; ++x; 
    *ydum = *y; ++ydum; ++y; 

   };

 // find and store min/max values

   xmin_ = *std::min_element( x, x + size);
   xmax_ = *std::max_element( x, x + size);

   ymin_ = *std::min_element( y, y + size);
   ymax_ = *std::max_element( y, y + size);


}

//..........................................................................................

CurveData::~CurveData()
{

  // data is automatically destroyed when smart_array[s] go out of scope   

}

//..........................................................................................

QwtData*   
CurveData::copy () const
{

  return new CurveData(xarray_, yarray_, size_);

}

//..........................................................................................

size_t
CurveData::size () const 
{

  return  size_;

}

//..........................................................................................

double
CurveData::x (size_t i) const
{
  return xarray_.get()[i];

}

//..........................................................................................

double
CurveData::y (size_t i) const
{

  return yarray_.get()[i];
}


//..........................................................................................

QwtDoubleRect   
CurveData::boundingRect () const
{

  return QwtDoubleRect(xmin_, xmax_, ymin_, ymax_);

}

//..........................................................................................


CHEFPlotData::CHEFPlotData(): 

  global_xmin_(0.0),
  global_xmax_(0.0),
  global_ylmin_(0.0),
  global_ylmax_(0.0),
  global_yrmin_(0.0),
  global_yrmax_(0.0), 
  scalemagleft_(1.0),
  scalemagright_(1.0),
  bml_()
{}


//..........................................................................................

CHEFPlotData::~CHEFPlotData(){

   // Qwt plot widget destroys Curves 

    bml_.zap(); 

}

//..........................................................................................

double CHEFPlotData::getHorTune()
{
  // this is a virtual function. Default implementation just returns -1.0 

  return -1.0;

}

//..........................................................................................

double CHEFPlotData::getVerTune()
{
  // this is a virtual function. Default implementation just returns -1.0 

  return -1.0;

}


//..........................................................................................

beamline const& 
CHEFPlotData::getBeamline() 
{
 
  return bml_;
  
}


//..........................................................................................

void 
CHEFPlotData::setBeamline( beamline const& bml )
{

  bml_    = *(bml.Clone());  

}

//..........................................................................................

void 
CHEFPlotData::addCurve(boost::shared_ptr<CHEFCurve> curve)
{

     // add a curve and compute global min/max

  int axis = curve->yAxis();
  

  double xmin = curve->minXValue();
  double xmax = curve->maxXValue();
  double ymin = curve->minYValue();
  double ymax = curve->maxYValue();
  
  
  global_xmin_     =  std::min(xmin, global_xmin_);
  global_xmax_     =  std::max(xmax, global_xmax_);

  switch (axis) {

    case QwtPlot::yLeft:
      
        global_ylmin_    =  std::min(ymin, global_ylmin_);
        global_ylmax_    =  std::max(ymax, global_ylmax_);

        break;
        

    case QwtPlot::yRight:
      
        global_yrmin_    =  std::min(ymin, global_yrmin_);
        global_yrmax_    =  std::max(ymax, global_yrmax_);

       break;
       
  };
   
   curves_.push_back(curve);
  

}

//..........................................................................................

boost::shared_ptr<CHEFCurve> 
CHEFPlotData::operator[](int i) const 
{
  
  return curves_[i];
  
}

    
//..........................................................................................

void 
CHEFPlotData::setXLabel(const char* label) 
{

  xLabel_ = QString(label);

}
 
//..........................................................................................

void 
CHEFPlotData::setYLabel(const char* label, int axis)
{

switch (axis) {
  case QwtPlot::yRight:  
     yRLabel_ = QString(label);
     break;
  case QwtPlot::yLeft:
     yLLabel_ = QString(label);
     break;
  }

}

//..........................................................................................

QString 
CHEFPlotData::yLLabel() const 
{
  return yLLabel_;
}

//..........................................................................................

QString 
CHEFPlotData::yRLabel() const 
{
  return yRLabel_;
}

//..........................................................................................

QString CHEFPlotData::xLabel() const 
{
  return xLabel_;
}


//..........................................................................................

double 
CHEFPlotData::xMin() const 
{
  return global_xmin_;
}

//..........................................................................................

double CHEFPlotData::xMax()const 
{
  return  global_xmax_;
}


//..........................................................................................

double CHEFPlotData::yLMin() const 
{
  return  global_ylmin_;
}

//..........................................................................................

double CHEFPlotData::yLMax() const 
{
  return global_ylmax_; 
}


//..........................................................................................

double CHEFPlotData::yRMin() const 
{
  return global_yrmin_;
}

//..........................................................................................

double CHEFPlotData::yRMax() const  
{
  return  global_yrmax_;
  
}

//..........................................................................................

int CHEFPlotData::nCurves() const  
{
  return curves_.size();
  
}

//..........................................................................................

void CHEFPlotData::setScaleMagRight(double rmag)  
{
   scalemagright_ = rmag;
  
}


//..........................................................................................

void CHEFPlotData::setScaleMagLeft(double lmag)  
{
   scalemagleft_ = lmag;
  
}

//..........................................................................................

double CHEFPlotData::scaleMagLeft() const  
{
  return scalemagleft_;
  
}

//..........................................................................................

double CHEFPlotData::scaleMagRight() const  
{
  return scalemagright_;
  
}


