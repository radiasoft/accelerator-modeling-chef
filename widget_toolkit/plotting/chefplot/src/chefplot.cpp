/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF:      A Qt-based Application 
******             Layered on top of of BEAMLINE.
******                                                                
******  File:      chefplot.cpp
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

#include <CHEFPlot.h> 
#include <CHEFPlotData.h>

#include <Plot.h>
#include <Lego.h>
#include <qwt/qwt_plot_canvas.h>
#include <qtoolbar.h>
#include <qmenubar.h>
#include <qevent.h>
#include <qsize.h>
#include <qapplication.h>

#include <iostream>
#include <string>
#include <cmath>



CHEFPlot::CHEFPlot(QWidget * parent, const char* name, Qt::WFlags f): 
QWidget(parent,name,f) 
{

   _lego_height = 30;

   _plot   = new Plot(this);
   _lego   = new LegoPlot(this);

    connect(_plot, SIGNAL( scaleChangedSignal() ), this, SLOT( updateLatticeDisplay() ) );
    connect(_plot, SIGNAL( plotResizedSignal() ),        this, SLOT( resizeLego() ) );

}


//..........................................................................................

CHEFPlot::~CHEFPlot() {


     // Qt widgets are automatically destroyed when their parent is destroyed 

}


//............................................................................................

void 
CHEFPlot::resizeEvent (QResizeEvent* event) {
  
  _plot->setGeometry(0, _lego_height,  width(), height()-_lego_height);

  return;

}

//............................................................................................

void 
CHEFPlot::resizeLego () {

  _lego->setGeometry(_plot->canvas()->x(),  0,  _plot->canvas()->width(),  _lego_height );

  updateLatticeDisplay();

}

//............................................................................................

void 
CHEFPlot::displayLattice(beamline const& bml) {

  _plot->setGeometry(0, _lego_height,  width(), height()- _lego_height);
  _lego->setGeometry(_plot->canvas()->x(), 0,  _plot->canvas()->width(),  _lego_height);
  _lego->setBeamline(bml);
  
}

//............................................................................................

const beamline&
CHEFPlot::getBeamline()
{


  return _lego->getBeamline();

}


//............................................................................................

void 
CHEFPlot::updateLatticeDisplay() {

  double xmin =  _plot->getCurrentXmin();
  double xmax =  _plot->getCurrentXmax();

  int loffset = _plot->transform( QwtPlot::xBottom, xmin);
  int roffset = _plot->transform( QwtPlot::xBottom, xmax);
  
    _lego->setBeamlineDisplayLimits(xmin, std::abs(xmax-xmin), loffset, roffset );
  _lego->update();

}

//............................................................................................

void 
CHEFPlot::enableGrid(bool set) {

  _plot->enableGridX(set);
  _plot->enableGridY(set);
  _plot->replot();
  
}

//............................................................................................

void 
CHEFPlot::enableThumbWheels(bool set) {

  _plot->enableThumbWheels(set);
  QApplication::postEvent( _plot, new QResizeEvent( QSize(_plot->width(), _plot->height()), QSize( _plot->width(), _plot->height() ) ) );
  QApplication::postEvent( this,  new QResizeEvent( QSize(width(), height()), QSize( width(), height() ) ) );
  _lego->repaint();

}

//............................................................................................

void 
CHEFPlot::enableLegoPlot(bool set) {

  if (set) {
    
    _lego_height = 30;
    _lego->show();

  } else {
    
    _lego_height = 0;
    _lego->hide();
  };

     // resize does not send a resize event when the size is unchanged.
     // The following is needed to force a resize of the lego display
 
  QApplication::postEvent( this, new QResizeEvent( QSize(width(), height()), QSize( width(), height() ) ) );
  
  _plot->replot();
  _lego->repaint();

}


//............................................................................................

void 
CHEFPlot::zoomUseRightAxis() {

  _plot->setZoomer(true, QwtPlot::yRight);
  

}

//............................................................................................

void 
CHEFPlot::zoomUseLeftAxis(){
  
  _plot->setZoomer(true, QwtPlot::yLeft);
  
}


//............................................................................................


void 
CHEFPlot::addData(CHEFPlotData& cpdata) {


  beamline const& bml = cpdata.getBeamline(); 
   
   if ( bml.empty() ) {
      displayLattice( bml );
      enableLegoPlot( true);

   } else {
     
     enableLegoPlot(false);
   
    }

  _plot->addData(cpdata);
  
}

//............................................................................................
void 
CHEFPlot::setLogScale( int axis) 
{

  _plot->setLogScale(axis); 

}

//............................................................................................

void 
CHEFPlot::setLinScale( int axis) 
{

  _plot->setLinScale(axis); 

}

//............................................................................................

void
CHEFPlot::clear() 
{

  _plot->clear(); 

}
