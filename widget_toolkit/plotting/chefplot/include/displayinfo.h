#ifndef DISPLAY_INFO_H
#define DISPLAY_INFO_H

#include <qwt/qwt_data.h>
#include  <qstring.h>
#include <string>
#include <vector>



struct OrderedPair {

    double x;
    double y;

    static bool xcomp(const OrderedPair& p1, const OrderedPair& p2) {
        return (p1.x < p2.x);
      }

    static bool ycomp(const OrderedPair& p1, const OrderedPair& p2) {
        return (p1.y < p2.y);
      }

};


class Interpolator {

  public:
  
  virtual OrderedPair operator()( const OrderedPair* data_begin, const OrderedPair* data_end, double x) = 0;

};



class Curve: public QwtData {

    public:
    
    Curve(const Curve& c);
    Curve(std::ifstream& is);
    Curve(const double* x, const double* y, int npts);
    Curve();                  // an empty curve for testing. This should go away.
    
    virtual ~Curve();
    
    void interpolationOn(int npts);
    void interpolationOff();  // default
    const char* name() const;
    double xMin()  const;
    double xMax()  const;
    double yMin()  const;
    double yMax()  const;
    int    axis()  const;
    void   setAxis(int axis);

       // QwtData Interface

    QwtData * 	copy () const;
    size_t 	size () const;
    double 	x (size_t i) const;
    double 	y (size_t i) const;
    QwtDoubleRect boundingRect () const;

    class DefaultLinearInterpolator: public Interpolator {
      OrderedPair operator()( const OrderedPair* data_begin, const OrderedPair* data_end, double x );
    };
 
    private:

    void minMax();
     
    OrderedPair*  _theData;  // an array of ordered data pairs
    int           _n;        // number of ordered data pairs
                             // in the data array

    int           _npts;     // no of points on the curve  
    int           _axis;
    
    double        _dx;        
    double        _xmin;        
    double        _xmax;        
    double        _ymin;        
    double        _ymax;        
    
    Interpolator* _dataIp;         // interpolates between the ordered
                                   // pairs; assumes a functor
    std::string   _name;           // name associated with the curve (used in legend)
    bool          _visible;        // determines whether curve is displayed

    bool          _interpolation;
    QString       _xLabel;
    QString       _yLLabel;
    QString       _yRLabel;

};

class DisplayInfo {

  public:

    DisplayInfo();
    ~DisplayInfo();
    
    void   addCurve(Curve* c, int axis);
    void   setXLabel(const char* label);
    void   setYLabel(const char*, int axis);
    QString   xLabel()  const;
    QString   yLLabel() const;
    QString   yRLabel() const;
    
    int    nCurves() const;
    double xMin() const;
    double xMax() const;
    double yLMin() const;
    double yLMax() const;
    double yRMin() const;
    double yRMax() const;

    const Curve* operator[](int i) const;
    
  private:
    
    std::vector<Curve*> _curves;   // array of Curves

    QString   _xLabel;
    QString   _yLLabel;
    QString   _yRLabel;
        
    double _global_xmin;
    double _global_xmax;
    double _global_ylmin;
    double _global_ylmax;
    double _global_yrmin;
    double _global_yrmax;
    
};


#endif
