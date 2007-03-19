/********************************************************************************
*********************************************************************************
*********************************************************************************
****                                                                         ****
**** FILE:       CurveData.h                                                 ****
****                                                                         ****
**** AUTHOR(S):  Jean-Francois Ostiguy                                       **** 
****             ostiguy@fnal.gov                                            ****
****                                                                         ****
****             Accelerator Division / Accelerator Integration Dept         ****
****             Fermi National Laboratory, Batavia, IL                      ****
****             ostiguy@fnal.gov                                            ****
****                                                                         ****
**** DATE:       September 2004                                              ****
****                                                                         ****
**** COPYRIGHT: Universities Research Association                            ****
****                                                                         ****
****                                                                         ****
*********************************************************************************
*********************************************************************************
********************************************************************************/

#ifndef CURVEDATA_H
#define CURVEDATA_H

#include <qwt_data.h>
#include <string>
#include <vector>
#include <boost/shared_array.hpp>

class CurveData : public QwtData {
 
 public:

  enum Axis { 
     yLeft, 
     yRight, 
     xBottom, 
     xTop, 
     axisCnt 
  };


  CurveData(boost::shared_array<double> x, boost::shared_array<double> y, int size, std::string label); 

  CurveData(std::vector<double>  const& x,  std::vector<double> const& y, std::string label);      

  CurveData(  double const*,  double const*,  int size, std::string label);      

  CurveData(CurveData const&);                                

  CurveData* copy()               const;

 ~CurveData();  
  
  CurveData&  operator=( CurveData const& cd); 

  size_t          size ()         const;
  double          x (size_t i)    const;
  double          y (size_t i)    const;
  QwtDoubleRect   boundingRect () const;

  void   setAxes( Axis xid, Axis yid);

  void        setLabel( std::string label);
  std::string getLabel() const;

  Axis   getXAxis() const;
  Axis   getYAxis() const;

  double xMin() const;
  double xMax() const;
  double yMin() const;
  double yMax() const;

 private:

  boost::shared_array<double>   xarray_;
  boost::shared_array<double>   yarray_;

  int          size_;
  double       xmin_;
  double       xmax_;
  double       ymin_;
  double       ymax_;

  Axis         xaxis_;
  Axis         yaxis_;

  std::string  label_;


 };

#endif // CURVEDATA_H
