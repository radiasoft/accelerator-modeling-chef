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

#include <chefplotdata.h>
#include <beamline.h>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_classes.h>


CurveData::CurveData( boost::shared_array<double> x , boost::shared_array<double> y, int size):
  _x(x), _y(y), _size(size) 

{

  const double* xp = _x.get();
  const double* yp = _y.get();

  _xmin = *std::min_element( xp, xp+_size);
  _xmax = *std::max_element( xp, xp+_size);

  _ymin = *std::min_element( yp, yp+_size);
  _ymax = *std::max_element( yp, yp+_size);


}

//..........................................................................................

CurveData::CurveData( const double* x , const double* y, int size):
  _x( new double[size] ), _y( new double[size] ), _size(size) 

{

  // copy the data 

  double* xdum = _x.get();
  double* ydum = _y.get();

  for ( int i=0; i<size ; i++ ) 
  {

    *xdum++ = *x++; 
    *ydum++ = *y++; 

   };

 // find and store min/max values

  _xmin = *std::min_element( _x.get(), _x.get()+_size);
  _xmax = *std::max_element( _x.get(), _x.get()+_size);

  _ymin = *std::min_element( _y.get(), _y.get()+_size);
  _ymax = *std::max_element( _y.get(), _y.get()+_size);


}

//..........................................................................................

CurveData::~CurveData()
{

  // data is automatically destroyed when smart_array[s] go out of scope   

  // std::cout << "CurveData::~CurveData" << std::endl;
  // std::cout << "_x.use_count() = " << _x.use_count() << endl;
  // std::cout << "_y.use_count() = " << _y.use_count() << endl;

}

//..........................................................................................

QwtData*   
CurveData::copy () const
{

  return new CurveData(_x, _y, _size);

}

//..........................................................................................

size_t
CurveData::size () const 
{

  return _size;

}

//..........................................................................................

double
CurveData::x (size_t i) const
{
  return _x.get()[i];

}

//..........................................................................................

double
CurveData::y (size_t i) const
{

  return _y.get()[i];
}


//..........................................................................................

QwtDoubleRect   
CurveData::boundingRect () const
{

  return QwtDoubleRect(_xmin, _xmax, _ymin, _ymax);

}

//..........................................................................................


CHEFPlotData::CHEFPlotData(): _bml(0), _scalemagright(1.0), _scalemagleft(1.0), _cloned(true) {
 
 _global_xmin  = 0.0;
 _global_xmax  = 0.0;
 _global_ylmin = 0.0;
 _global_ylmax = 0.0;
 _global_yrmin = 0.0;
 _global_yrmax = 0.0;

}

//..........................................................................................

CHEFPlotData::~CHEFPlotData(){

   // plot widget destroys Curves and beamline.
   // both have been cloned, so this is safe  

  if (_bml && _cloned) 
  { 
    const_cast<beamline*>(_bml)->zap();
    delete const_cast<beamline*>(_bml);
    _bml = 0;
  }


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

const beamline* 
CHEFPlotData::getBeamline() 
{
 
  return _bml;
  
}


//..........................................................................................

void 
CHEFPlotData::setBeamline(const beamline* bml, bool clone)
{

  if (bml == 0) return;

  if (clone) 
  {
    _cloned = true;  
    _bml = dynamic_cast<const beamline*>( bml->Clone() );
  }  
  else 
  {
    _cloned = false;
    _bml = dynamic_cast<const beamline*>( bml );  
  }
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
  
  
  _global_xmin     =  std::min(xmin, _global_xmin);
  _global_xmax     =  std::max(xmax, _global_xmax);

  switch (axis) {

    case QwtPlot::yLeft:
      
        _global_ylmin    =  std::min(ymin, _global_ylmin);
        _global_ylmax    =  std::max(ymax, _global_ylmax);

        break;
        

    case QwtPlot::yRight:
      
       _global_yrmin    =  std::min(ymin, _global_yrmin);
       _global_yrmax    =  std::max(ymax, _global_yrmax);

       break;
       
  };
   
  _curves.push_back(curve);
  

}

//..........................................................................................

boost::shared_ptr<CHEFCurve> 
CHEFPlotData::operator[](int i) const 
{
  
  return _curves[i];
  
}

    
//..........................................................................................

void 
CHEFPlotData::setXLabel(const char* label) 
{

  _xLabel = QString(label);

}
 
//..........................................................................................

void 
CHEFPlotData::setYLabel(const char* label, int axis)
{

switch (axis) {
  case QwtPlot::yRight:  
     _yRLabel = QString(label);
     break;
  case QwtPlot::yLeft:
     _yLLabel = QString(label);
     break;
  }

}

//..........................................................................................

QString 
CHEFPlotData::yLLabel() const 
{
  return _yLLabel;
}

//..........................................................................................

QString 
CHEFPlotData::yRLabel() const 
{
  return _yRLabel;
}

//..........................................................................................

QString CHEFPlotData::xLabel() const 
{
  return _xLabel;
}


//..........................................................................................

double 
CHEFPlotData::xMin() const 
{
  return _global_xmin;
}

//..........................................................................................

double CHEFPlotData::xMax()const 
{
  return  _global_xmax;
}


//..........................................................................................

double CHEFPlotData::yLMin() const 
{
  return _global_ylmin;
}

//..........................................................................................

double CHEFPlotData::yLMax() const 
{
  return _global_ylmax; 
}


//..........................................................................................

double CHEFPlotData::yRMin() const 
{
  return _global_yrmin;
}

//..........................................................................................

double CHEFPlotData::yRMax() const  
{
  return _global_yrmax;
  
}

//..........................................................................................

int CHEFPlotData::nCurves() const  
{
  return _curves.size();
  
}

//..........................................................................................

void CHEFPlotData::setScaleMagRight(double rmag)  
{
  _scalemagright = rmag;
  
}


//..........................................................................................

void CHEFPlotData::setScaleMagLeft(double lmag)  
{
  _scalemagleft = lmag;
  
}

//..........................................................................................

double CHEFPlotData::scaleMagLeft() const  
{
  return _scalemagleft;
  
}

//..........................................................................................

double CHEFPlotData::scaleMagRight() const  
{
  return _scalemagright;
  
}


