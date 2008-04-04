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

#include <sqlite/connection.hpp>
#include <CurveData.h>
#include <boost/shared_ptr.hpp>
#include <beamline/beamline.h>
#include <qstring.h>


class PlotData {

  public:

    typedef std::vector<CurveData>::const_iterator CurveIterator; 


    struct Tunes {
      Tunes():                                       htune(0.0), vtune(0.0), set(false) {}
      Tunes(double const& hor, double const& ver) :  htune(hor), vtune(ver), set(true)  {}
      double htune;
      double vtune;
      bool   set;
    };

    PlotData();

    virtual ~PlotData();
    
    void             addCurve( CurveData& cv);

    void                setDb( sqlite::connection& db );
    sqlite::connection* getDb() const;

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

    Tunes const&     getTunes() const;
    void             setTunes(double const& nu1, double const& nu2);

    CurveData const& operator[](int i) const;
    
  private:
    
    std::vector<CurveData> curves_;   

    std::string            label_top_;
    std::string            label_bottom_;
    std::string            label_left_;
    std::string            label_right_;
        
    double                 scalemag_top_;
    double                 scalemag_bottom_;
    double                 scalemag_left_;
    double                 scalemag_right_;
    
    Tunes                          tunes_;
    ConstBmlPtr                    bml_;
    mutable sqlite::connection*    db_;

};


#endif // PLOTDATA_H
