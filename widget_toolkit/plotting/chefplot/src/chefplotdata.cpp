#include "chefplotdata.h"
#include <beamline.h>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_classes.h>

CHEFPlotData::CHEFPlotData(): _bml(0), _scalemagright(1.0), _scalemagleft(1.0) {

 
 _global_xmin  = 0.0;
 _global_xmax  = 0.0;
 _global_ylmin = 0.0;
 _global_ylmax = 0.0;
 _global_yrmin = 0.0;
 _global_yrmax = 0.0;

}


CHEFPlotData::~CHEFPlotData(){

   // deletes the _curves vector elements.
   // Note that the curves generally have been reparented by the 
   // plot widget. In that case, the plot widget destroys them.
   // Only curves that have *not* been reparented (i.e. have parent 0)
   // are destroyed here.  

   std::vector<CHEFCurve*>::iterator it;
   
   for (it = _curves.begin(); it != _curves.end(); it++) {
        //( !( (*it)->parentPlot() ) ) delete (*it);
     delete (*it);
     
   };
   
     // delete the beamline elements using zap
     // then delete the beamline itself

   if (_bml) {

     _bml->zap();
     delete _bml;

  };

}


beamline* CHEFPlotData::getBeamline() {
 
  return _bml;
  
}



void CHEFPlotData::setBeamline(const beamline* bml){

  if (bml == 0) return;

  _bml = dynamic_cast<beamline*>( bml->clone() );
  
}

void CHEFPlotData::addCurve(CHEFCurve* curve){

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


CHEFCurve* CHEFPlotData::operator[](int i) const {
  
  return _curves[i];
  
}

    

void CHEFPlotData::setXLabel(const char* label) {

  _xLabel = QString(label);

}
 

void CHEFPlotData::setYLabel(const char* label, int axis){

switch (axis) {
  case QwtPlot::yRight:  
     _yRLabel = QString(label);
     break;
  case QwtPlot::yLeft:
     _yLLabel = QString(label);
     break;
  }

}



QString CHEFPlotData::yLLabel() const {
  return _yLLabel;
}

QString CHEFPlotData::yRLabel() const {
  return _yRLabel;
}

QString CHEFPlotData::xLabel() const {
  return _xLabel;
}




double CHEFPlotData::xMin() const {
  return _global_xmin;
}

double CHEFPlotData::xMax()const {
  return  _global_xmax;
}


double CHEFPlotData::yLMin() const {
  return _global_ylmin;
}

double CHEFPlotData::yLMax() const {
  return _global_ylmax; 
}


double CHEFPlotData::yRMin() const {
  return _global_yrmin;
}

double CHEFPlotData::yRMax() const  {
  return _global_yrmax;
  
}

int CHEFPlotData::nCurves() const  {
  return _curves.size();
  
}

void CHEFPlotData::setScaleMagRight(double rmag)  {
  _scalemagright = rmag;
  
}


void CHEFPlotData::setScaleMagLeft(double lmag)  {
  _scalemagleft = lmag;
  
}

double CHEFPlotData::scaleMagLeft() const  {
  return _scalemagleft;
  
}

double CHEFPlotData::scaleMagRight() const  {
  return _scalemagright;
  
}


