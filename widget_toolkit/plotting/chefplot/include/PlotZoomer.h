/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF:      A Qt-based Application 
******             Layered on top of of BEAMLINE.
******                                                                
******  File:      PlotZoomer.h
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

#ifndef PLOTZOOMER_H
#define PLOTZOOMER_H

#include <qwt_plot_zoomer.h> 

class QwtPlotCanvas;

class PlotZoomer: public QwtPlotZoomer {

  Q_OBJECT
    
  public:
  
    PlotZoomer (int xaxis, int yaxis, QwtPlotCanvas* canvas, const char *name=0);
    PlotZoomer (QwtPlotCanvas* canvas, const char *name=0);
 
 ~PlotZoomer();

  signals:
  
    void scaleChangedSignal();
  
  protected:

    void rescale();

};


#endif
