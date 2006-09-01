////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                            //
// FILE:       chefplotmain.h                                                                 //
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


#ifndef CHEFPLOTMAIN_H
#define CHEFPLOTMAIN_H

#include "chefplotmainbase.h" 
#include "chefplotdata.h" 
#include <qcanvas.h>
#include <qwmatrix.h>

class CHEFPlot;
class beamline;
class bmlnElmnt;
class LegoPlot;
class TuneDiagram;

class CHEFPlotMain: public CHEFPlotMainBase {

  Q_OBJECT
    
  public:
  
  CHEFPlotMain(QWidget * parent = 0, const char* name = 0, WFlags f=0);

  virtual ~CHEFPlotMain();

  void displayLattice(beamline* bml);
  void addData(CHEFPlotData& cpdata);
  void showDataPoints(bool set);
  void enableLegend(bool set);
  void enableInterpolation(bool set);
  void enableCoordinatesDisplay(bool set);
  void clear(); 
  void setAutoClear(bool set); 
    
  public slots:

  virtual void filePrint();
  virtual void enableGrid(bool set);
  virtual void enableThumbWheels(bool set);
  virtual void enableLegoPlot(bool set);
  virtual void zoomUseRightAxis();
  virtual void zoomUseLeftAxis();
  virtual void exit();
  virtual void optionsVerLogScaleLeft(bool set);
  virtual void optionsVerLogScaleRight(bool set);
  virtual void optionsHorLogScale(bool set);
  virtual void _showdata();
  virtual void showTunes();
  virtual void optionsPersistent(bool set);
  
  private:

  CHEFPlot*      _chefplot;
  TuneDiagram*   _tunediagram;
  bool           _auto_clear;


};

#endif
