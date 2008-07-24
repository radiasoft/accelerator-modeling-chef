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
#include <functional>

class CurveData : public QwtData {
 
 public:

  enum Axis { 
     yLeft, 
     yRight, 
     xBottom, 
     xTop, 
     axisCnt 
  };


  struct Color {
    Color(int red, int green, int blue): r(red), g(green), b(blue) {}
    Color( Color const& o) : r(o.r), g(o.g), b(o.b) {}
    int r;
    int g;
    int b;
  };


  struct Point {
    Point()                    : x(0.0), y(0.0)  {} 
    Point( double X, double Y ): x(X),   y(Y)    {} 
    Point( Point const& p)     : x(p.x), y(p.y)  {} 
    double x;
    double y;
  };
 
  
  struct curve_attribute {

   char const*      label;
   CurveData::Axis  yaxis;
   CurveData::Color rgb; 
  };


  struct XValue: public std::unary_function< Point const&, double const&> {
    double const& operator()( Point const& p) const { return  p.x; } 
  };

  struct YValue: public std::unary_function< Point const&, double const&> {
    double const& operator()( Point const& p) const{ return p.y; } 
  };
  
  typedef Point*             iterator;
  typedef Point const* const_iterator;

  CurveData( int size, std::string label);      
  CurveData(CurveData const&);                                

 ~CurveData();  

  CurveData&  operator=( CurveData const& cd); 

  CurveData* copy() const;

  size_t             size ()         const;
  double             x (size_t i)    const;
  double             y (size_t i)    const;

  void               setColor( CurveData::Color const& color);
  CurveData::Color const& 
                     getColor() const;

  void               setAxes( Axis xid, Axis yid);

  void               setLabel( std::string const& label);
  std::string const& getLabel()  const;

  Axis               getXAxis() const;
  Axis               getYAxis() const;

  double             xMin()     const;
  double             xMax()     const;
  double             yMin()     const;
  double             yMax()     const;

  void               setConnected(bool set);
  bool               isConnected() const;

  iterator   begin();  
  iterator     end();  

  const_iterator   begin() const;  
  const_iterator     end() const;  

 private:

  boost::shared_array<Point>   array_;  // Copying the curve does not copy the data

  int          size_;

  Axis         xaxis_;
  Axis         yaxis_;

  
  Color        color_;
  bool         connected_;
  std::string  label_;


 };

#endif // CURVEDATA_H
