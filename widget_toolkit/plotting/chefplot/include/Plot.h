/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF:      A Qt-based Application 
******             Layered on top of of BEAMLINE.
******                                                                
******  File:      Plot.h
******                                                                
******  Copyright (c) Universities Research Association, Inc.   
******                All Rights Reserved                             
******                                                                
******  Author:    Jean-Francois Ostiguy
******                                                                
******             Fermilab                                           
******             Batavia, IL  60510                                
******             ostiguy@fnal.gov                         
******                                                                
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******* U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******* The U.S. Government retains a world-wide non-exclusive, 
******* royalty-free license to publish or reproduce documentation 
******* and software for U.S. Government purposes. This software 
******* is protected under the U.S. and Foreign Copyright Laws. 
******* URA/FNAL reserves all rights.
*******                                                                
**************************************************************************
**************************************************************************
*************************************************************************/

#ifndef PLOT_H
#define PLOT_H

#include <qwt_plot.h> 
#include <qwt_legend.h> 
#include <list>

class QwtWheel;
class QwtPlotItem;
class QwtPlotCurve;
class QwtPlotGrid;
class PlotData;
class PlotPropertiesDialog;

class Plot: public QwtPlot {

  friend class CHEFPlotMain;
  friend class CHEFPlot;
  
  Q_OBJECT
    
  public:
  
           Plot(QWidget * parent = 0, const char* name = 0, Qt::WFlags f = 0);
  virtual ~Plot();

  void     setData( PlotData const& cpdata);
  double   getCurrentXmin();
  double   getCurrentXmax();
  double   getCurrentYmin();
  double   getCurrentYmax();
  void     enableDataPointSymbols(bool set);
  void     enableCoordinatesDisplay(bool set);
  void     enableInterpolation(bool set);
  void     enableLegend(bool set);
  void     setAxis(int xAxis, int yAxis);
  
  bool     eventFilter(QObject *object, QEvent *e);

  signals:

      void scaleChangedSignal();
      void plotResizedSignal();

     public slots:

      void scaleChanged();
      void leftWheelValueChanged(   double value);
      void rightWheelValueChanged(  double value);
      void bottomWheelValueChanged( double value);
      void enableThumbWheel(bool set, int axiscode);
      void enableThumbWheels(bool set);
      void setLogScale(int axis);
      void setLinScale(int axis); 
      void toggleCurve(QwtPlotItem* ); 
     
  protected:

     virtual void resizeEvent(QResizeEvent *e);
       
  private:

     void formatPlot();

     QMap<QString,QwtPlotCurve*> curves_; 

     QwtPlotGrid*                grid_; 
     QwtLegend*                  legend_; 

     QwtWheel*                   bottom_wheel_;
     QwtWheel*                   left_wheel_;
     QwtWheel*                   right_wheel_;
 

   // data set bounds  

     double                     xmin_; 
     double                     xmax_;
     double                     ylmin_;
     double                     ylmax_;
     double                     yrmin_;
     double                     yrmax_;

     double                     xrange_;
     double                     ylrange_;
     double                     yrrange_;

};

#endif // PLOT_H
