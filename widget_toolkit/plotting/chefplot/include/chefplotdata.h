////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                            //
// FILE:       chefplotdata.h                                                                 //
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


#ifndef CHEFPLOTDATA_H
#define CHEFPLOTDATA_H

#include <qwt/qwt_plot_classes.h>
#include <qwt/qwt_array.h>
#include <qwt/qwt_double_rect.h>
#include <qwt/qwt_data.h>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

class beamline;

class CHEFInterpolator {

  public:
  
  virtual QwtDoublePoint operator()( const QwtDoublePoint* data_begin, const QwtDoublePoint* data_end, double x) = 0;

};


class CurveData : public QwtData {

 public:

  CurveData(boost::shared_array<double> x, boost::shared_array<double> y, int size); 

  CurveData(const double* x, const double* y, int size);      // data is copied
  virtual ~CurveData();  


  virtual QwtData *       copy () const;
  virtual size_t          size () const;
  virtual double          x (size_t i) const;
  virtual double          y (size_t i) const;
  virtual QwtDoubleRect   boundingRect () const;

 private:

  boost::shared_array<double> _x;
  boost::shared_array<double> _y;

  int                       _size;
  double                    _xmin;
  double                    _xmax;
  double                    _ymin;
  double                    _ymax;

 };


class CHEFCurve: public QwtPlotCurve {
  public:
  
      class DefaultLinearInterpolator: public CHEFInterpolator {
      QwtDoublePoint operator()( const QwtDoublePoint* data_begin, const QwtDoublePoint* data_end, double x );
    };
 
  public:
    
    CHEFCurve(const CHEFCurve& c, const char* name);
    CHEFCurve(std::ifstream& is,  const char* name);

    CHEFCurve(const CurveData& data, const char* name);
    CHEFCurve(const double* x, const double* y, int npts, const char* name); // should go away
 
    virtual ~CHEFCurve();

    double scalemag() const;
          
    void copy(const CHEFCurve &c);
  

    private:


    CHEFInterpolator*              _dataIp;                    // currently unused 
    bool                           _visible;                   // currently unused 
    
   
};



class CHEFPlotData {

  public:

    CHEFPlotData();
   virtual ~CHEFPlotData();
    
    void      addCurve(boost::shared_ptr<CHEFCurve> c);

    void      setBeamline(const beamline* bml, bool clone=true);
    const beamline* getBeamline();
    
    void      setXLabel(const char* label);
    void      setYLabel(const char*, int axis);

    void      setScaleMagLeft(double lmag);
    void      setScaleMagRight(double rmag);
    double    scaleMagLeft()  const;
    double    scaleMagRight() const;

    QString   xLabel()  const;
    QString   yLLabel() const;
    QString   yRLabel() const;
    
    int       nCurves() const;
    double    xMin()    const;
    double    xMax()    const;
    double    yLMin()   const;
    double    yLMax()   const;
    double    yRMin()   const;
    double    yRMax()   const;

    virtual double      getHorTune();
    virtual double      getVerTune();

    boost::shared_ptr<CHEFCurve> operator[](int i) const;
    
  private:
    
    std::vector< boost::shared_ptr<CHEFCurve> > _curves;   // array of Curves

    QString   _xLabel;
    QString   _yLLabel;
    QString   _yRLabel;
        
    double _global_xmin;
    double _global_xmax;
    double _global_ylmin;
    double _global_ylmax;
    double _global_yrmin;
    double _global_yrmax;

    const beamline* _bml;

    double   _scalemagleft;
    double   _scalemagright;
    
    bool     _cloned;


};


#endif
