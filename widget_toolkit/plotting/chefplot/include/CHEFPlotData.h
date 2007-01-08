////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                            //
// FILE:       CHEFPlotData.h                                                                 //
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

class CHEFCurve;

#include <boost/shared_ptr.hpp>
#include <beamline/beamline.h>
#include <qstring.h>




class CHEFPlotData {

  public:

    CHEFPlotData();

    virtual ~CHEFPlotData();
    
    void      addCurve(boost::shared_ptr<CHEFCurve> c);

    void             setBeamline(beamline const& bml );
    beamline  const& getBeamline();
    
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
    
    std::vector< boost::shared_ptr<CHEFCurve> > curves_;   // array of Curves

    QString   xLabel_;
    QString   yLLabel_;
    QString   yRLabel_;
        
    double  global_xmin_;
    double  global_xmax_;
    double  global_ylmin_;
    double  global_ylmax_;
    double  global_yrmin_;
    double  global_yrmax_;


    double   scalemagleft_;
    double   scalemagright_;
    
    beamline bml_;

};


#endif
