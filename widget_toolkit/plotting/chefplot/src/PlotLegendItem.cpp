/*******************************************************************************
********************************************************************************
********************************************************************************
********************************************************************************
****                                                                        ****
**** FILE:       PlotLegendItem.cpp                                         ****
****                                                                        ****
**** AUTHOR(S):  Jean-Francois Ostiguy                                      **** 
****             ostiguy@fnal.gov                                           ****
****                                                                        ****
****             Accelerator Division / Accelerator Integration Dept        ****
****             Fermi National Laboratory, Batavia, IL                     ****
****             ostiguy@fnal.gov                                           ****
****                                                                        ****
**** DATE:       September 2004                                             ****
****                                                                        ****
**** COPYRIGHT: Universities Research Association                           ****
****                                                                        ****
****                                                                        ****
********************************************************************************
********************************************************************************
********************************************************************************
*******************************************************************************/

#include <Plot.h>
#include <PlotLegendItem.h>

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


PlotLegendItem::PlotLegendItem(QWidget* parent):
  QwtLegendItem(parent) 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

PlotLegendItem::PlotLegendItem(const QwtSymbol& symbol, const QPen& curvePen,
		 const QString & text, QWidget * parent):
  QwtLegendItem(symbol, curvePen, text, parent) 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void   PlotLegendItem::toggleCurve(bool set) 
{
  #if 0
  Plot* plot = dynamic_cast<Plot*>( parent()->parent()->parent()->parent() );
  // FIXME !! plot->toggleCurve( curve_key_, set );
  #endif

  QwtPlot* plot = dynamic_cast<QwtPlot*>( parent()->parent()->parent()->parent() );
  plot->replot();
}

