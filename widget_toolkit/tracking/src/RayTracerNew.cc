/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  File:      RayTracerNew.cpp
******                                                                
******  Copyright (c) Fermi Research Alliance LLC 
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-07CH11359 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******
******                                                                
******  Author:    Jean-Francois Ostiguy                                     
******             ostiguy@fnal.gov                         
****** 
**************************************************************************
**************************************************************************
*************************************************************************/

#include <RayTracerNew.h>
#include <qwt_plot_grid.h>
#include <qwt_plot.h>
#include <qpen.h>


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||    
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||    

RayTracerNew::RayTracerNew( QWidget* parent, char const* name, WFlags f)
  : RayTracerBase(parent, name, f)
{
  qwtPlotLeft->setCanvasBackground(QColor(255,255,255)) ;
  qwtPlotRight->setCanvasBackground(QColor(255,255,255)) ;
  QwtPlotGrid* gridX = new QwtPlotGrid(); 
  QwtPlotGrid* gridY = new QwtPlotGrid(); 

  gridX->enableX(true);
  gridX->enableY(true);

  gridY->enableX(true);
  gridY->enableY(true);

  gridX->setPen(QPen(Qt::DotLine));
  gridY->setPen(QPen(Qt::DotLine));
 
  gridX->attach( qwtPlotLeft  );
  gridY->attach( qwtPlotRight );
  

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||    
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||    
