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

    typedef std::vector<CurveData>::const_iterator iterator; 

    struct Tunes {
      Tunes():                                       htune(0.0), vtune(0.0), set(false) {}
      Tunes(double const& hor, double const& ver) :  htune(hor), vtune(ver), set(true)  {}
      double htune;
      double vtune;
      bool   set;
    };

    PlotData( sqlite::connection& db );
    PlotData( );

    virtual ~PlotData();
    
    sqlite::connection& getDb() const;

    ConstBmlPtr         getBeamline() const;
    void                setBeamline( BmlPtr bml);

    void                addCurve( CurveData& cv);


    void                setTitle(  std::string const& label               );
    std::string const&  getTitle() const;

    void                setXLabel( std::string const& label               );
    void                setYLabel( CurveData::Axis id, std::string const& );
    std::string const&  getLabel(CurveData::Axis id)  const;

    void                setScaleMag(CurveData::Axis id, double lmag); 
    double              getScaleMag(CurveData::Axis id) const;

    
    int                 nCurves() const;

    double              xMin(CurveData::Axis id = CurveData::xBottom)    const;
    double              xMax(CurveData::Axis id = CurveData::xBottom)    const;
    double              yMin(CurveData::Axis id = CurveData::yLeft  )    const;
    double              yMax(CurveData::Axis id = CurveData::yLeft  )    const;

    Tunes const&        getTunes() const;
    void                setTunes(double const& nu1, double const& nu2);

    CurveData const& operator[](int i) const;
    
  private:
    
    std::vector<CurveData>         curves_;   

    std::string                    label_top_;
    std::string                    label_bottom_;
    std::string                    label_left_;
    std::string                    label_right_;
        
    double                         scalemag_top_;
    double                         scalemag_bottom_;
    double                         scalemag_left_;
    double                         scalemag_right_;
    
    Tunes                          tunes_;

    mutable sqlite::connection*    db_;
    BmlPtr                         bml_; 

 protected:

   void init( sqlite::connection& db, int npltattribs, char const* plot_attributes[],  
	      int ncrvs, CurveData::curve_attribute curve_attributes[],  char const* sql[] );


};


#endif // PLOTDATA_H
