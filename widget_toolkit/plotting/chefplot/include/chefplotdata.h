#ifndef CHEFPLOTDATA_H
#define CHEFPLOTDATA_H

#include <qwt/qwt_plot_classes.h>
#include <qwt/qwt_array.h>
#include <qwt/qwt_double_rect.h>
#include <qwt/qwt_data.h>
#include <vector>

class beamline;

class CHEFInterpolator {

  public:
  
  virtual QwtDoublePoint operator()( const QwtDoublePoint* data_begin, const QwtDoublePoint* data_end, double x) = 0;

};


class CHEFCurve: public QwtPlotCurve {
  public:
  
      class DefaultLinearInterpolator: public CHEFInterpolator {
      QwtDoublePoint operator()( const QwtDoublePoint* data_begin, const QwtDoublePoint* data_end, double x );
    };
 
  public:
    
    CHEFCurve(const CHEFCurve& c, const char* name);
    CHEFCurve(std::ifstream& is,  const char* name);

    CHEFCurve(const double* x, const double* y, int npts, const char* name);
    CHEFCurve(const QwtData& data, const char* name);

    virtual ~CHEFCurve();

    double scalemag() const;
    
    private:


    QwtArray<QwtDoublePoint>       _theData;         
    bool                           _visible;
    CHEFInterpolator*              _dataIp;         // UNUSED at the moment.


};



class CHEFPlotData {

  public:

    CHEFPlotData();
    ~CHEFPlotData();
    
    void      addCurve(CHEFCurve* c);

    void      setBeamline(const beamline* bml);
    beamline* getBeamline();
    
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

    CHEFCurve* operator[](int i) const;
    
  private:
    
    std::vector<CHEFCurve*> _curves;   // array of Curves

    QString   _xLabel;
    QString   _yLLabel;
    QString   _yRLabel;
        
    double _global_xmin;
    double _global_xmax;
    double _global_ylmin;
    double _global_ylmax;
    double _global_yrmin;
    double _global_yrmax;

    beamline* _bml;

    double   _scalemagleft;
    double   _scalemagright;

    
};


#endif
