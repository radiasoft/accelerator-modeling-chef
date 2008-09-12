/*************************************************************************
**************************************************************************
**************************************************************************
******                                    
******  File:      TuneDiagram.cc
******                                                                
******  Copyright (c) Fermi Research Alliance LLC
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-07CH11359.
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

#include <TuneDiagram.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qwt_plot_curve.h>
#include <qwt_plot.h>
#include <qwt_symbol.h>
#include <cstdlib>
#include <iostream>

using namespace std;


TuneDiagram::TuneDiagram( QWidget* parent, char const* name, WFlags f )
: TuneDiagramBase(parent, name, f), tunescrv_(0) 
{

  qwtPlot->setCanvasBackground( QColor("white")              );
  qwtPlot->setAxisTitle( QwtPlot::yLeft,   "Vertical Tune"   );
  qwtPlot->setAxisTitle( QwtPlot::xBottom, "Horizontal Tune" );
  qwtPlot->setAxisScale( QwtPlot::yLeft,   0.0,  1.0,  0     );	 
  qwtPlot->setAxisScale( QwtPlot::xBottom, 0.0,  1.0,  0     );	 

  connect ( spinBoxMaxOrder,     SIGNAL( valueChanged(int) ), this, SLOT( maxOrderValueChanged( int ) ) ); 
  connect ( spinBoxMinOrder,     SIGNAL( valueChanged(int) ), this, SLOT( minOrderValueChanged( int ) ) ); 
  connect ( checkBoxSums,        SIGNAL( clicked()  ),        this, SLOT( checkBoxSumsClicked()       ) ); 
  connect ( checkBoxDifferences, SIGNAL( clicked()  ),        this, SLOT( checkBoxDifferencesClicked()) ); 

  lineEditNu1->setEnabled(false);
  lineEditNu2->setEnabled(false);
  lineEditChroma1->setEnabled(false);
  lineEditChroma2->setEnabled(false);

  lineEditNu1->setReadOnly(true);
  lineEditNu2->setReadOnly(true);
  lineEditChroma1->setReadOnly(true);
  lineEditChroma2->setReadOnly(true);

  replot();     	
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TuneDiagram::~TuneDiagram()
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TuneDiagram::replot()
{
  
  // All resonance lines recreated and displayed.
  // Tunes remain unchanged.

  QwtPlotItemList items = qwtPlot->itemList();

  for ( QwtPlotItemIterator it = items.begin(); it != items.end(); ++it ) {

    QwtPlotCurve* curve = dynamic_cast<QwtPlotCurve*>(*it);

    if (!curve) continue; 

    if ( dynamic_cast<ResonanceLine*>( &(curve->data()) )) { 
        curve->attach(0); delete (curve );  
    }
  }

  int min_order = spinBoxMinOrder->value(); 
  int max_order = spinBoxMaxOrder->value(); 
 
  bool show_diffs =  checkBoxDifferences->isChecked();
  bool show_sums  =  checkBoxSums->isChecked();

  for ( int n=0; n<max_order; ++n ) {

    for ( int i=0; i <=max_order; ++i ) {
      for ( int j=-max_order; j<=max_order; ++j ) {

	if ( (i==0) && ( j==0) ) continue; 

	if ( (std::abs(i)+abs(j) < min_order  ) ||  (std::abs(i)+abs(j) > max_order) ) continue; 

        bool is_sum =  (j >= 0);
 
	if ( (!show_diffs) && ( !is_sum ) ) continue; 
	if ( (!show_sums)  && (  is_sum ) ) continue; 

         QwtPlotCurve* curve = new QwtPlotCurve();
	 curve->setData( ( ResonanceLine( i, j, n )) );
	 is_sum ? curve->setPen( QPen(QColor("gray"), 2, Qt::SolidLine )) : curve->setPen( QPen( QColor("gray"), 1, Qt::DotLine) );
         curve->setStyle(QwtPlotCurve::Lines);
         curve->attach(qwtPlot);
         
      }
    }
  }

  qwtPlot->replot();

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TuneDiagram::checkBoxSumsClicked()
{
  replot();
}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TuneDiagram::checkBoxDifferencesClicked()
{
  replot();
}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TuneDiagram::maxOrderValueChanged( int )
{
  replot();
}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TuneDiagram::minOrderValueChanged( int ) 
{
  replot();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  TuneDiagram::setRange( int min, int max, bool showdiffs ) 
{
  spinBoxMinOrder->setValue(min); 
  spinBoxMaxOrder->setValue(max); 
  checkBoxDifferences->setChecked( showdiffs);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TuneDiagram::setTunes( double nu1, double nu2)
{

  if (!tunescrv_) { 
    tunescrv_ = new QwtPlotCurve();
    tunescrv_->setTitle("Tunes");
    tunescrv_->setStyle(QwtPlotCurve::NoCurve);
    tunescrv_->setSymbol(QwtSymbol(QwtSymbol::Cross, QBrush(), QPen("red",2), QSize(10,10)) );
    tunescrv_->setZ(1.0); // always displayed on top 
    tunescrv_->attach( qwtPlot) ;
  }
  
  tunescrv_->setData(&nu1, &nu2, 1);
  lineEditNu1->setText(QString("%1").arg(nu1, 7, 'f', 4 ));
  lineEditNu2->setText(QString("%1").arg(nu2, 7, 'f', 4 ));
  lineEditNu1->setEnabled(true);
  lineEditNu2->setEnabled(true);
  
  lineEditNu1->update();
  lineEditNu2->update();

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TuneDiagram::setChromas( double xi1, double xi2)
{
  lineEditChroma1->setText(QString("%1").arg(xi1, 7, 'f', 4 ));
  lineEditChroma2->setText(QString("%1").arg(xi2, 7, 'f', 4 ));
  lineEditChroma1->setEnabled(true);
  lineEditChroma2->setEnabled(true);

  lineEditChroma1->update();
  lineEditChroma2->update();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
