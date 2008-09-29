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
#include <iostream>

using namespace std;


TuneDiagram::TuneDiagram( QWidget* parent, char const* name, WFlags f )
  : TuneDiagramBase(parent, name, f), nu1_(0.0), nu2_(0.0) 
{

  qwtPlot->setCanvasBackground( QColor("white")              );
  qwtPlot->setAxisTitle( QwtPlot::yLeft,   "'Vertical'   Tune"   );
  qwtPlot->setAxisTitle( QwtPlot::xBottom, "'Horizontal' Tune" );

  connect ( spinBoxMaxOrder,     SIGNAL( valueChanged(int) ), this, SLOT( setMaxOrder( int ) ) ); 
  connect ( spinBoxMinOrder,     SIGNAL( valueChanged(int) ), this, SLOT( setMinOrder( int ) ) ); 
  connect ( checkBoxSums,        SIGNAL( clicked()  ),        this, SLOT( showSums()       ) ); 
  connect ( checkBoxDifferences, SIGNAL( clicked()  ),        this, SLOT( showDifferences()) ); 

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

void TuneDiagram::clear()
{
  QwtPlotItemList items = qwtPlot->itemList();

  for ( QwtPlotItemIterator it = items.begin(); it != items.end(); ++it ) {
    QwtPlotCurve* curve = dynamic_cast<QwtPlotCurve*>(*it);
    if (!curve) continue; 
    curve->detach(); delete (curve );  
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TuneDiagram::plotResonances()
{
  
  // All resonance lines recreated and displayed.
  // Tunes remain unchanged.


  int min_order = spinBoxMinOrder->value(); 
  int max_order = spinBoxMaxOrder->value(); 
 
  bool show_diffs =  checkBoxDifferences->isChecked();
  bool show_sums  =  checkBoxSums->isChecked();

  double nu1min;  modf( nu1_, &nu1min); 
  double nu2min;  modf( nu2_, &nu2min);

  for ( int xoff= nu1min; xoff<nu1min+1; ++xoff ) {
  for ( int yoff= nu2min; yoff<nu2min+1;  ++yoff ) {

  for ( int n=0; n<max_order; ++n ) {

    for ( int i=0; i <=max_order; ++i ) {
      for ( int j=-max_order; j<=max_order; ++j ) {

	if ( (i==0) && ( j==0) ) continue; 

	if ( (std::abs(i)+abs(j) < min_order  ) ||  (std::abs(i)+abs(j) > max_order) ) continue; 

        bool is_sum =  (j >= 0);
 
	if ( (!show_diffs) && ( !is_sum ) ) continue; 
	if ( (!show_sums)  && (  is_sum ) ) continue; 

         QwtPlotCurve* curve = new QwtPlotCurve();
         ResonanceLine rl( i, j, n, xoff, yoff );
         if (!rl.isVisible() ) continue; 

	 curve->setData( rl );
	 is_sum ? curve->setPen( QPen(QColor("gray"), 2, Qt::SolidLine )) : curve->setPen( QPen( QColor("gray"), 1, Qt::DotLine) );
         curve->setStyle(QwtPlotCurve::Lines);
         curve->attach(qwtPlot);
         
      }
    }
  }

  }}

  qwtPlot->replot();

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TuneDiagram::replot()
{
  clear();

  double nu1min;  modf( nu1_, &nu1min); 
  double nu2min;  modf( nu2_, &nu2min);

  qwtPlot->setAxisScale( QwtPlot::yLeft,   nu1min,  nu1min+1.0,  0     );	 
  qwtPlot->setAxisScale( QwtPlot::xBottom, nu2min,  nu2min+1.0,  0     );	 

  plotResonances(); 
  
  QwtPlotCurve* tunescrv = new QwtPlotCurve();
  tunescrv->setTitle("Tunes");
  tunescrv->setStyle(QwtPlotCurve::NoCurve);
  tunescrv->setSymbol(QwtSymbol(QwtSymbol::Cross, QBrush(), QPen("red",2), QSize(10,10)) );
  tunescrv->setZ(1.0); // always displayed on top 
  tunescrv->attach( qwtPlot) ;
  tunescrv->setData(&nu1_, &nu2_, 1);

  qwtPlot->replot();
   
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TuneDiagram::showSums()
{
  replot();
}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TuneDiagram::showDifferences()
{
  replot();
}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TuneDiagram::setMaxOrder( int )
{
  replot();
}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TuneDiagram::setMinOrder( int ) 
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

  nu1_ = nu1;
  nu2_ = nu2;

  lineEditNu1->setText(QString("%1").arg(nu1_, 7, 'f', 4 ));
  lineEditNu2->setText(QString("%1").arg(nu2_, 7, 'f', 4 ));
  lineEditNu1->setEnabled(true);
  lineEditNu2->setEnabled(true);
  
  lineEditNu1->update();
  lineEditNu2->update();
 
  replot();
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
