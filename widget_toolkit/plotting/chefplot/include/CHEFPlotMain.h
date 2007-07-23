/********************************************************************************
*********************************************************************************
*********************************************************************************
****
****
**** FILE:       chefplotmain.h                                                                 
****                                                                                            
**** AUTHOR(S):  Jean-Francois Ostiguy                                                           
****             ostiguy@fnal.gov                                                               
****                                                                                            
****             Accelerator Division / Accelerator Integration Dept                            
****             Fermi National Laboratory, Batavia, IL                                         
****             ostiguy@fnal.gov                                                               
****                                                                                            
**** DATE:       September 2004                                                                 
****                                                                                            
**** COPYRIGHT: Universities Research Association                                               
****                                                                                            
****
*********************************************************************************
*********************************************************************************
********************************************************************************/

#ifndef CHEFPLOTMAIN_H
#define CHEFPLOTMAIN_H

#include "chefplotmainbase.h" 
#include <beamline/beamline.h>
#include <qcanvas.h>
#include <qwmatrix.h>

class CHEFPlot;
class PlotData;
class LegoPlot;
class TuneDiagram;

class CHEFPlotMain: public CHEFPlotMainBase {

  Q_OBJECT
    
  public:
  
    CHEFPlotMain(QWidget* parent=0, const char* name=0, WFlags f=0);
   ~CHEFPlotMain();

    void displayLattice(BmlPtr bml);
    void addData(PlotData& cpdata);
    void showDataPoints(bool set);
    void enableLegend(bool set);
    void enableInterpolation(bool set);
    void enableCoordinatesDisplay(bool set);
    void clear(); 
    void setAutoClear(bool set); 
    
  public slots:

    void filePrint();
    void enableGrid(bool set);
    void enableThumbWheels(bool set);
    void enableLegoPlot(bool set);
    void optionsVerLogScaleLeft(bool set);
    void optionsVerLogScaleRight(bool set);
    void optionsHorLogScale(bool set);
    void showdata();
    void showTunes();
    void optionsPersistent(bool set);

    void enableZoomYAxisRight();
    void enableZoomYAxisLeft();
    void zoomOff();
    void zoomReset();

    void exit();
  
  private:

  CHEFPlot*      chefplot_;
  TuneDiagram*   tunediagram_;
  bool           auto_clear_;


};

#endif  //CHEFPLOTMAIN_H
