/********************************************************************************
*********************************************************************************
*********************************************************************************
*********************************************************************************
****                                                                         ****
**** FILE:       chefplot.h                                                  ****
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
*********************************************************************************/

#ifndef CHEFPLOT_H
#define CHEFPLOT_H

#include <qwidget.h>
#include <beamline/BmlPtr.h>
#include <qwmatrix.h>

class QwtDoubleRect;
class Plot;
class PlotZoomer;
class QwtPlotPicker;
class QwtPlotPanner;
class PlotData;
class LegoPlot;


class CHEFPlot: public QWidget {

  friend class CHEFPlotMain;
  
  Q_OBJECT
    
  public:
  
    CHEFPlot(QWidget * parent = 0, const char* name = 0, Qt::WFlags f = 0);
   ~CHEFPlot();

    void              displayLattice(ConstBmlPtr bml);
    ConstBmlPtr       getBeamline();

    void              setData(PlotData const& cpdata);
    void              clear();
  
  protected:

    void resizeEvent (QResizeEvent * e);

  public slots:

    void updateLatticeDisplay();
    void enableGrid(bool set);
    void enableThumbWheels(bool set);
    void enableLegoPlot(bool set);
    void setLogScale(int axis);
    void setLinScale(int axis);
    void enableZoomYAxisLeft();
    void enableZoomYAxisRight();
    void zoomOff();
    void zoomReset();
   

  private slots:
  
    void resizeLego();
    void zoomed(QwtDoubleRect const& pg ); 

  private:

    void initZoom();

    Plot*             plot_;
    LegoPlot*         lego_;
    QWMatrix          canvas_transformation_;
    int               lego_height_;

    QwtPlotPanner*    panner_;
    PlotZoomer*       zoomer_left_;
    QwtPlotPicker*    picker_left_;

    PlotZoomer*       zoomer_right_;
    QwtPlotPicker*    picker_right_;

 
};

#endif
