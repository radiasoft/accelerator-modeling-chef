////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                            //
// FILE:       CHEFCurve.h                                                                    //
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

#ifndef CHEFCURVE_H
#define CHEFCURVE_H

#include <CHEFInterpolator.h>

#include <qwt/qwt_plot_classes.h>
#include <qwt/qwt_double_rect.h>
#include <fstream>

class CurveData;


class CHEFCurve: public QwtPlotCurve {

  public:
  
      class DefaultLinearInterpolator: public CHEFInterpolator {
      QwtDoublePoint operator()( QwtDoublePoint const* data_begin, QwtDoublePoint const* data_end, double x );
    };
 
  public:
    
    CHEFCurve(CHEFCurve const& c, const char* name);
    CHEFCurve(std::ifstream& is,  const char* name);

    CHEFCurve(CurveData const& data, const char* name);
    CHEFCurve(double    const* x, double const* y, int npts, const char* name); 
 
    virtual ~CHEFCurve();

    double scalemag() const;
          
    void copy(CHEFCurve const& curve);
  

    private:


    CHEFInterpolator*              dataIp_;                    // currently unused 
    bool                           visible_;                   // currently unused 
    
   
};

#endif // CHEFCURVE_H


