/********************************************************************************
*********************************************************************************
*********************************************************************************
*********************************************************************************
****                                                                         ****
**** FILE:       PlotData.h                                                  ****
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
*********************************************************************************
********************************************************************************/

#ifndef PLOTDATA_H
#define PLOTDATA_H

#include <CurveData.h>
#include <boost/shared_ptr.hpp>
#include <beamline/beamline.h>
#include <qstring.h>


class PlotData {

  public:

    typedef std::vector<CurveData>::const_iterator CurveIterator; 


    struct Tunes {
      double htune;
      double vtune;
    };

    PlotData();

    virtual ~PlotData();
    
    void             addCurve( CurveData& cv);

    void             setBeamline(ConstBmlPtr bml );
    ConstBmlPtr      getBeamline() const;
    
    void             setXLabel( std::string  label             );
    void             setYLabel( CurveData::Axis id, std::string);

    void             setScaleMag(CurveData::Axis id, double lmag); 
    double           getScaleMag(CurveData::Axis id) const;

    std::string      getLabel(CurveData::Axis id)  const;
    
    int              nCurves() const;

    double           xMin(CurveData::Axis id = CurveData::xBottom)    const;
    double           xMax(CurveData::Axis id = CurveData::xBottom)    const;
    double           yMin(CurveData::Axis id = CurveData::yLeft  )    const;
    double           yMax(CurveData::Axis id = CurveData::yLeft  )    const;

    Tunes            getTunes() const;
    void             setTunes( double nu1, double nu2);

    CurveData const& operator[](int i) const;
    
  private:
    
    std::vector<CurveData> curves_;   

    std::string   label_top;
    std::string   label_bottom_;
    std::string   label_left_;
    std::string   label_right_;
        
    double   scalemag_top_;
    double   scalemag_bottom_;
    double   scalemag_left_;
    double   scalemag_right_;
    
    Tunes       tunes_;

    ConstBmlPtr bml_;

};


#endif // PLOTDATA_H
