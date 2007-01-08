////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                            //
// FILE:       CurveData.h                                                                    //
//                                                                                            //
// AUTHOR(S):  Jean-Francois Ostiguy                                                          // 
//             ostiguy@fnal.gov                                                               //
//                                                                                            //
//             Accelerator Division / Accelerator Integration Dept                            //
//             Fermi National Laboratory, Batavia, IL                                         //
//             ostiguy@fnal.gov                                                               //
//                                                                                            //
// DATE:       September 2004                                                                 //
//                                                                                            //
// COPYRIGHT: Universities Research Association                                               //
//                                                                                            //
//                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CURVEDATA_H
#define CURVEDATA_H

#include <qwt/qwt_plot_classes.h>
#include <boost/shared_array.hpp>

class CurveData : public QwtData {

 public:

  CurveData(boost::shared_array<double> x, boost::shared_array<double> y, int size); 

  CurveData(double const* x, double const* y, int size);      // data is copied
  virtual ~CurveData();  


  virtual QwtData *       copy ()         const;
  virtual size_t          size ()         const;
  virtual double          x (size_t i)    const;
  virtual double          y (size_t i)    const;
  virtual QwtDoubleRect   boundingRect () const;

 private:

  boost::shared_array<double>   xarray_;
  boost::shared_array<double>   yarray_;

  int                       size_;
  double                    xmin_;
  double                    xmax_;
  double                    ymin_;
  double                    ymax_;

 };

#endif // CURVEDATA_H
