/*******************************************************************************
********************************************************************************
********************************************************************************
********************************************************************************
****                                                                        ****
**** FILE:       PlotLegendItem.h                                           ****
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

#ifndef PLOTLEGENDITEM_H
#define PLOTLEGENDITEM_H

#include <qwt_legend_item.h> 

class PlotLegendItem: public QwtLegendItem {

  Q_OBJECT
    
  public:
  
    PlotLegendItem(QWidget* parent = 0);
    PlotLegendItem(QwtSymbol const& symbol, QPen const& curvePen,
	  	   QString const & text, QWidget * parent = 0);

  public slots:
    
    void toggleCurve(bool set);
  
  private:

    long curve_key_;
  
};


#endif // PLOTLEGENDITEM_H
