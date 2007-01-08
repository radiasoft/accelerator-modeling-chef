#ifndef CHEFPLOTZOOMER_H
#define CHEFPLOTZOOMER_H

#include <qwt/qwt_plot_zoomer.h> 


class CHEFPlotZoomer: public QwtPlotZoomer {

  Q_OBJECT
    
  public:
  
  CHEFPlotZoomer (QwtPlotCanvas *canvas, const char *name=0);
  CHEFPlotZoomer (int xAxis, int yAxis, QwtPlotCanvas *, const char *name=0);
  CHEFPlotZoomer (int xAxis, int yAxis, int selectionFlags, DisplayMode cursorLabelMode, QwtPlotCanvas *, const char *name=0);
  ~CHEFPlotZoomer();

  signals:
  
  void scaleChangedSignal();
  
  protected:

  virtual        void rescale();
  QwtDoubleRect  scaleRect() const;
  
};


#endif
