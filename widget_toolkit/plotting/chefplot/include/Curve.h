////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                            //
// FILE:       Curve.h                                                                        //
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
#ifndef CURVE_H
#define CURVE_H


class Interpolator;
class OrderedPair;

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

#endif // CURVE_H
