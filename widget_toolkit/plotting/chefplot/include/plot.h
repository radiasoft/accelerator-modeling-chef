#ifndef PLOT_H
#define PLOT_H

#include <qwt/qwt_plot.h> 
#include <qwt/qwt_plot_classes.h> 
#include "chefplotdata.h" 
#include <list>

class QwtWheel;
class CHEFPlotZoomer;
class PlotPropertiesDialog;

class Plot: public QwtPlot {

  friend class CHEFPlotMain;
  friend class CHEFPlot;
  
  Q_OBJECT
    
  public:
  
  Plot(QWidget * parent = 0, const char* name = 0, Qt::WFlags f = 0);
  virtual ~Plot();

  void   addData(const CHEFPlotData& cpdata);
  double getCurrentXmin();
  double getCurrentXmax();
  double getCurrentYmin();
  double getCurrentYmax();
  void   enableDataPointSymbols(bool set);
  void   enableCoordinatesDisplay(bool set);
  void   enableInterpolation(bool set);
  void   setAxis(int xAxis, int yAxis);
  
  bool eventFilter(QObject *object, QEvent *e);

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
      void setZoomer(bool set, int axis);
      void toggleCurve(long key, bool set);
      void setLogScale(int axis);
      void setLinScale(int axis);
      
  protected:

     virtual void resizeEvent(QResizeEvent *e);
       
  private:

  void formatPlot();
  

    CHEFPlotZoomer*             _left_zoomer;
    CHEFPlotZoomer*             _right_zoomer;
    CHEFPlotZoomer*             _active_zoomer;
  
    QwtWheel*                   _bottom_wheel;
    QwtWheel*                   _left_wheel;
    QwtWheel*                   _right_wheel;
 

    PlotPropertiesDialog *      _context_menu;

       // data set bounds  

   double                     _xmin; 
   double                     _xmax;
   double                     _ylmin;
   double                     _ylmax;
   double                     _yrmin;
   double                     _yrmax;

   double                     _xrange;
   double                     _ylrange;
   double                     _yrrange;

};


class PlotLegendItem: public QwtLegendButton {

  Q_OBJECT
    
  public:
  
  PlotLegendItem(long curveKey, QWidget* parent = 0, const char* name = 0);
  PlotLegendItem(long curveKey, const QwtSymbol& symbol, const QPen& curvePen,
		 const QString & text, QWidget * parent = 0,
		 const char*  	name = 0);



  public slots:
    
  void toggleCurve(bool set);
  
  private:

  long _curve_key;
  
};


#endif
