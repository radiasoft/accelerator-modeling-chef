/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF:      A Qt-based Application 
******             Layered on top of of BEAMLINE.
******                                                                
******  File:      PlotZoomer.cpp
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

#include <PlotZoomer.h>
#include <qwt_double_rect.h>


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


PlotZoomer::PlotZoomer(int xaxis, int yaxis, QwtPlotCanvas* cv, char const* name):
  QwtPlotZoomer(xaxis, yaxis, cv, name) 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

PlotZoomer::PlotZoomer(QwtPlotCanvas* cv, char const* name):
  QwtPlotZoomer(cv,name) 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



PlotZoomer::~PlotZoomer() 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



void  PlotZoomer::rescale() {

  QwtPlotZoomer::rescale();
  emit scaleChangedSignal(); 

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


#if  0

QwtDoubleRect PlotZoomer::scaleRect() const{

 
    QwtPlot const* plt = plot();
    //plt->replot(); // force the adjustment of the plot scales

    //    const QwtDoubleRect rect(
    //    plt->axisWidget(xAxis())->lBound(),
    //    plt->axisWidget(xAxis())->hBound(),
    //    plt->axisWidget(yAxis())->lBound(),
    //    plt->axisWidget(yAxis())->hBound()
    //);

    //   
    //return rect.normalize();

    return  QwtDoubleRect(0, 0, 100, 100); // FIXME !!! 
}

#endif
