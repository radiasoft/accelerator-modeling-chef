#include "chefplotdata.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>


QwtDoublePoint CHEFCurve::DefaultLinearInterpolator::operator()( const QwtDoublePoint* data_begin, 
                                                                 const QwtDoublePoint* data_end, double x ) {
  
return QwtDoublePoint(0.0,0.0);

};
 


CHEFCurve::CHEFCurve(std::ifstream& is, const char* name): QwtPlotCurve(0,name)  {

#if 0 

char linebuf[256];
int n = 0;
    
while ( !(is.getline(linebuf, 256)).eof() ){
   n++;
};

 n--; // the first line is a comment; _n is the no of data points
 

is.clear();
is.seekg (0, std::ios::beg);

is.getline(linebuf, 256);
 
   //_name = std::string(linebuf);
 
 QwtDoublePoint point;
 

 for (int i = 0; i < n; i++) {
   is >> point.x();
   is >> point.y();
   dataptr++;
 };

 _dataIp = new CHEFCurve::DefaultLinearInterpolator();
 _visible      = true;

 interpolationOff();
#endif

 return;

}


CHEFCurve::CHEFCurve(const CHEFCurve& c, const char* name): QwtPlotCurve(0,name) {


#if 0 
   for (int i=0; i<c._n; i++) {
    _theData[i] = c._theData[i];

  }; 

 _dataIp        = c._dataIp;     
 _visible       = c._visible;        
 _interpolation = c._interpolation;

#endif
  
}


CHEFCurve::CHEFCurve(const double* x, const double* y, int npts, const char* name): QwtPlotCurve(0,name) {
  

  QwtDoublePoint point;

  const double* xptr = x;
  const double* yptr = y;
    
  _theData.resize(npts);


  // save a copy of the data. This may be useful to interpolate lattice functions "smartly" 

  for (int i=0; i< npts; i++) {
    point.setX(*xptr);   
    point.setY(*yptr);
    _theData[i] = point;
     xptr++; yptr++;
 };

  setData(x, y, npts);

  _dataIp = new CHEFCurve::DefaultLinearInterpolator();  // UNUSED FOR THE MOMENT

  _visible      = true;                                  // UNUSED FOR THE MOMENT

   
}


CHEFCurve::CHEFCurve(const QwtData &data, const char* name): QwtPlotCurve(0,name) {
  

  setData(data);


}



CHEFCurve::~CHEFCurve() {
     // handled by the QwtPlotCurve destructor.
}






