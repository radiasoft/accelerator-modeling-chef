#include "chefplotzoomer.h"
#include <qwt/qwt_double_rect.h>
#include <qwt/qwt_plot_canvas.h>
#include <qwt/qwt_plot.h>
#include <iostream>

CHEFPlotZoomer::CHEFPlotZoomer(QwtPlotCanvas* canvas, const char *name):
  QwtPlotZoomer(canvas,name) {
  
}

CHEFPlotZoomer::CHEFPlotZoomer(int xAxis, int yAxis, QwtPlotCanvas * canvas, const char *name):
  QwtPlotZoomer(xAxis,yAxis,canvas,name) {
  
}

CHEFPlotZoomer::CHEFPlotZoomer(int xAxis, int yAxis, int selectionFlags, 
                               DisplayMode cursorLabelMode, QwtPlotCanvas* canvas, const char *name):
  QwtPlotZoomer(xAxis,yAxis,selectionFlags, cursorLabelMode, canvas,name) {
  
}

CHEFPlotZoomer::~CHEFPlotZoomer() {
}



void  CHEFPlotZoomer::rescale() {

  QwtPlotZoomer::rescale();
  emit scaleChangedSignal(); 

}


QwtDoubleRect QwtPlotPicker::scaleRect() const
{

 
    QwtPlot *plt = (QwtPlot*) plot();
    plt->replot(); // force the adjustment of the plot scales

    const QwtDoubleRect rect(
        plt->axisScale(xAxis())->lBound(),
        plt->axisScale(xAxis())->hBound(),
        plt->axisScale(yAxis())->lBound(),
        plt->axisScale(yAxis())->hBound()
    );


    return rect.normalize();
}


