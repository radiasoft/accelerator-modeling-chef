/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Chef:      A Qt-based Application 
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
#include <Plot.h>
#include <PlotData.h>


#include <qwt_plot_picker.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_grid.h>


#include <Plot.h>
#include <PlotZoomer.h>
#include <Lego.h>

#include <qtoolbar.h>
#include <qmenubar.h>
#include <qevent.h>
#include <qsize.h>
#include <qapplication.h>

#include <iostream>
#include <string>
#include <cmath>

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


CHEFPlot::CHEFPlot(QWidget * parent, const char* name, Qt::WFlags f): 
  QWidget(parent,name,f), 
  plot_(0), lego_(0), lego_height_(30),
  zoomer_left_(0),  picker_left_(0),
  zoomer_right_(0), picker_right_(0)
{
   plot_   = new Plot(this);
   lego_   = new LegoPlot(this);

   panner_ = new  QwtPlotPanner( plot_->canvas() );
   panner_->setAxisEnabled(QwtPlot::xBottom , true );
   panner_->setAxisEnabled(QwtPlot::yRight,   false);
   panner_->setAxisEnabled(QwtPlot::yLeft,    true );


   connect(plot_, SIGNAL( scaleChangedSignal() ), this, SLOT( updateLatticeDisplay() ) );
   connect(plot_, SIGNAL( plotResizedSignal() ),  this, SLOT( resizeLego() ) );

   return;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CHEFPlot::~CHEFPlot() 
{
  // Qt widgets are automatically destroyed when their parent is destroyed 
}



//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFPlot::resizeEvent (QResizeEvent* event) {
  
  if (plot_) plot_->setGeometry(0, lego_height_,  width(), height()-lego_height_);

  QWidget::resizeEvent(event);

  return;

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFPlot::resizeLego() 
{

  lego_->setGeometry(plot_->canvas()->x(),  0,  plot_->canvas()->width(),  lego_height_ );

  updateLatticeDisplay();

}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void CHEFPlot::displayLattice( ConstBmlPtr bml) 
{

  plot_->setGeometry(0, lego_height_,  width(), height()- lego_height_);
  lego_->setGeometry(plot_->canvas()->x(), 0,  plot_->canvas()->width(),  lego_height_);
  lego_->setBeamline(bml);
  
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ConstBmlPtr CHEFPlot::getBeamline()
{
  return lego_->getBeamline();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void  CHEFPlot::updateLatticeDisplay() 
{

  double xmin =  plot_->getCurrentXmin();
  double xmax =  plot_->getCurrentXmax();

  int loffset = plot_->transform( QwtPlot::xBottom, xmin);
  int roffset = plot_->transform( QwtPlot::xBottom, xmax);
  
  lego_->setBeamlineDisplayLimits(xmin, std::abs(xmax-xmin), loffset, roffset );
  lego_->update();

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFPlot::enableGrid(bool set) 
{

  plot_->grid_->enableX(set);
  plot_->grid_->enableY(set);
  plot_->replot();
  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  CHEFPlot::enableThumbWheels(bool set) 
{

  plot_->enableThumbWheels(set);
  QApplication::postEvent( plot_, new QResizeEvent( QSize(plot_->width(), plot_->height()), 
                                                    QSize( plot_->width(), plot_->height() ) ) );
  QApplication::postEvent( this,  new QResizeEvent( QSize(width(), height()), 
                                                    QSize( width(), height() ) ) );
  lego_->repaint();

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  CHEFPlot::enableLegoPlot(bool set) 
{

  if (set) {
    
    lego_height_ = 30;
    lego_->show();

  } else {
    
    lego_height_ = 0;
    lego_->hide();
  };

     // resize does not send a resize event when the size is unchanged.
     // The following is needed to force a resize of the lego display
 
  QApplication::postEvent( this, new QResizeEvent( QSize(width(), height()), QSize( width(), height() ) ) );
  
  plot_->replot();
  lego_->repaint();

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFPlot::initZoom() 
{

   panner_->setEnabled(false);

   if  (!picker_right_) {
     picker_right_ =  new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yRight, plot_->canvas() );  
   }

   if  (!zoomer_right_){ 
       zoomer_right_ =  new PlotZoomer(QwtPlot::xBottom, QwtPlot::yRight, plot_->canvas() );
       connect(zoomer_right_, SIGNAL( zoomed( const QwtDoubleRect& ) ),  this, SLOT( zoomed( const QwtDoubleRect&) ) );
   }


   if  (!picker_left_) {
     picker_left_  =  new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft,  plot_->canvas() );  
   } 
   if  (!zoomer_left_) {
       zoomer_left_  =  new PlotZoomer(QwtPlot::xBottom, QwtPlot::yLeft,  plot_->canvas() );
       connect(zoomer_left_, SIGNAL( zoomed( const QwtDoubleRect& ) ),  this, SLOT( zoomed( const QwtDoubleRect&) ) );
   }

   
   picker_right_->setEnabled(false);
   zoomer_right_->setEnabled(false);
   picker_left_->setEnabled(false);
   zoomer_left_->setEnabled(false);


}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFPlot::enableZoomYAxisLeft() 
{ 
   
   initZoom();

   picker_right_->setEnabled(false);
   zoomer_right_->setEnabled(false);

   zoomer_left_->setTrackerMode( QwtPicker::AlwaysOn ); 
   zoomer_left_->setTrackerPen(QColor(Qt::red));
   zoomer_left_->setRubberBandPen(QColor(Qt::red));
 
   picker_left_->setSelectionFlags(QwtPicker::PointSelection || QwtPicker::ClickSelection );  
   picker_left_->setTrackerMode(QwtPicker::ActiveOnly);

   picker_left_->setEnabled(true);
   zoomer_left_->setEnabled(true);

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFPlot::enableZoomYAxisRight() 
{ 

   initZoom();

   picker_left_->setEnabled(false);
   zoomer_left_->setEnabled(false);

   zoomer_right_->setTrackerMode( QwtPicker::AlwaysOn ); 
   zoomer_right_->setTrackerPen(QColor(Qt::red));
   zoomer_right_->setRubberBandPen(QColor(Qt::red));

   picker_right_->setSelectionFlags(QwtPicker::PointSelection || QwtPicker::ClickSelection );  
   picker_right_->setTrackerMode(QwtPicker::ActiveOnly);

   picker_right_->setEnabled(true);
   zoomer_right_->setEnabled(true);
  

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFPlot::zoomReset() 
{ 

     zoomer_left_ ->setSelectionFlags(QwtEventPattern::KeyHome); 
     zoomer_right_ ->setSelectionFlags(QwtEventPattern::KeyHome); 
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFPlot::zoomOff() 
{ 

     if (zoomer_left_)  zoomer_left_->setEnabled(false);
     if (picker_left_)  picker_left_->setEnabled(false); 

     if (zoomer_right_) zoomer_right_->setEnabled(false); 
     if (picker_right_) picker_right_->setEnabled(false); 

     panner_->setEnabled(true);

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFPlot::setData(PlotData const& pltdata) {

   ConstBmlPtr bml = pltdata.getBeamline(); 
   
   if ( bml ) {
      displayLattice( bml );
      enableLegoPlot( true);

   } else {
     
     enableLegoPlot(false);
   
   }

  plot_->setData(pltdata);
  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFPlot::setLogScale( int axis) 
{

  plot_->setLogScale(axis); 

}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void CHEFPlot::setLinScale( int axis) 
{

  plot_->setLinScale(axis); 

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFPlot::clear() 
{

  plot_->clear(); 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void CHEFPlot::zoomed(QwtDoubleRect const& pg ) 
{
  updateLatticeDisplay();
} 

