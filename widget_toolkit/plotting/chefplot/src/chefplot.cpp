#include "chefplot.h" 
#include "plot.h"
#include "lego.h"
#include <qwt/qwt_plot_canvas.h>
#include <qtoolbar.h>
#include <qmenubar.h>
#include <qevent.h>
#include <qsize.h>
#include <qapplication.h>

#include <iostream>
#include <string>



CHEFPlot::CHEFPlot(QWidget * parent, const char* name, Qt::WFlags f): QWidget(parent,name,f) {

   _lego_height = 30;

   _plot   = new Plot(this, 0, 0);
   _lego   = new LegoPlot(this);

    connect(_plot, SIGNAL( scaleChangedSignal() ), this, SLOT( updateLatticeDisplay() ) );
    connect(_plot, SIGNAL( plotResizedSignal() ),        this, SLOT( resizeLego() ) );

}



CHEFPlot::~CHEFPlot() {
    disconnect(_plot, SIGNAL( scaleChangedSignal() ), this, SLOT( updateLatticeDisplay() ) );
    disconnect(_plot, SIGNAL( plotResizedSignal() ),  this, SLOT( resizeLego() ) );

     // Qt widgets are automatically destroyed when their parent is destroyed 

}




void CHEFPlot::resizeEvent (QResizeEvent* event) {
  
  _plot->setGeometry(0, _lego_height,  width(), height()-_lego_height);

  return;

}

void CHEFPlot::resizeLego () {

  _lego->setGeometry(_plot->canvas()->x(),  0,  _plot->canvas()->width(),  _lego_height );
}




void CHEFPlot::displayLattice(beamline* bml) {

  _plot->setGeometry(0, _lego_height,  width(), height()- _lego_height);
  _lego->setGeometry(_plot->canvas()->x(), 0,  _plot->canvas()->width(),  _lego_height);
  _lego->setBeamline(bml);
  
}

  

void CHEFPlot::updateLatticeDisplay() {

   double xmin =  _plot->getCurrentXmin();
   double xmax =  _plot->getCurrentXmax();
  
  _lego->setBeamlineDisplayLimits(xmin, std::abs(xmax-xmin) );
  _lego->update();

}

void CHEFPlot::enableGrid(bool set) {

  _plot->enableGridX(set);
  _plot->enableGridY(set);
  _plot->replot();
  
}


void CHEFPlot::enableThumbWheels(bool set) {

  _plot->enableThumbWheels(set);
  QApplication::postEvent( _plot, new QResizeEvent( QSize(_plot->width(), _plot->height()), QSize( _plot->width(), _plot->height() ) ) );
  QApplication::postEvent( this,  new QResizeEvent( QSize(width(), height()), QSize( width(), height() ) ) );
  _lego->repaint();

}

void CHEFPlot::enableLegoPlot(bool set) {

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


void CHEFPlot::zoomUseRightAxis() {

  _plot->setZoomer(true, QwtPlot::yRight);
  

}


void CHEFPlot::zoomUseLeftAxis(){
  
  _plot->setZoomer(true, QwtPlot::yLeft);
  
}




void CHEFPlot::addData(CHEFPlotData& cpdata) {


  beamline* bml = cpdata.getBeamline(); // beamline is owned by the CHEFPlotData object which is responsible to delete it;
   
   if (bml) {
      displayLattice(bml);
      enableLegoPlot(true);

   } else {
     

     displayLattice(0);
     enableLegoPlot(false);
   
    }

  _plot->addData(cpdata);
  
}
