/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF:      A Qt-based Application 
******             Layered on top of of BEAMLINE.
******                                                                
******  File:      chefplotzoomer.cpp
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

#include <CHEFPlotZoomer.h>
#include <qwt/qwt_double_rect.h>
#include <qwt/qwt_plot_canvas.h>
#include <qwt/qwt_plot.h>
#include <iostream>

CHEFPlotZoomer::CHEFPlotZoomer(QwtPlotCanvas* canvas, const char *name):
  QwtPlotZoomer(canvas,name) {
  
}

//...........................................................................

CHEFPlotZoomer::CHEFPlotZoomer(int xAxis, int yAxis, QwtPlotCanvas * canvas, const char *name):
  QwtPlotZoomer(xAxis,yAxis,canvas,name) {
  
}

//...........................................................................

CHEFPlotZoomer::CHEFPlotZoomer(int xAxis, int yAxis, int selectionFlags, 
                               DisplayMode cursorLabelMode, QwtPlotCanvas* canvas, const char *name):
  QwtPlotZoomer(xAxis,yAxis,selectionFlags, cursorLabelMode, canvas,name) {
  
}

//...........................................................................

CHEFPlotZoomer::~CHEFPlotZoomer() {
}

//...........................................................................


void  CHEFPlotZoomer::rescale() {

  QwtPlotZoomer::rescale();
  emit scaleChangedSignal(); 

}

//...........................................................................

QwtDoubleRect CHEFPlotZoomer::scaleRect() const
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


