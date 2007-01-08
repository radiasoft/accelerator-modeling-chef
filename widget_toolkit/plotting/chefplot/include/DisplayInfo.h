////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                            //
// FILE:       displayinfo.h                                                                  //
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

#ifndef DISPLAY_INFO_H
#define DISPLAY_INFO_H

#include  <qwt/qwt_data.h>
#include  <qstring.h>
#include  <string>
#include  <vector>


class Curve;

class DisplayInfo {

  public:

    DisplayInfo();
   ~DisplayInfo();
    
    void   addCurve( Curve* c, int axis);
    void   setXLabel(char const*  label);
    void   setYLabel(char const*, int axis);

    QString   xLabel()  const;
    QString   yLLabel() const;
    QString   yRLabel() const;
    
    int    nCurves() const;

    double xMin()  const;
    double xMax()  const;
    double yLMin() const;
    double yLMax() const;
    double yRMin() const;
    double yRMax() const;

    Curve const* operator[](int i) const;
    
  private:
    
    std::vector<Curve*> curves_;   // array of Curves

    QString   xLabel_;
    QString   yLLabel_;
    QString   yRLabel_;
        
    double  global_xmin_;
    double  global_xmax_;
    double  global_ylmin_;
    double  global_ylmax_;
    double  global_yrmin_;
    double  global_yrmax_;
    
};


#endif // DISPLAY_INFO_H
 
