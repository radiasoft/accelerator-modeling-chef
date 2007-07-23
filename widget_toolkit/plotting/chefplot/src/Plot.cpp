/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Chef:      A Qt-based Application 
******             Layered on top of of BEAMLINE.
******                                                                
******  File:      plot.cpp
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

#include <iostream>
#include <Plot.h>
#include <PlotLegendItem.h>
#include <PlotData.h>

#include <qapplication.h>
#include <qlayout.h>
#include <qcanvas.h>
#include <qwhatsthis.h>

#include <qwt_wheel.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_item.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_engine.h>
#include <qwt_scale_div.h>
#include <qwt_plot_grid.h>


#include <iostream>



//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Plot::Plot(QWidget* parent, const char* name, Qt::WFlags f ): 
QwtPlot(parent, name),    grid_(0),     legend_(0), 
bottom_wheel_(0),  left_wheel_(0), right_wheel_(0), 
xmin_(0.0), xmax_(0.0) 
{

   grid_ = new QwtPlotGrid;
   grid_->enableXMin(true);
   grid_->setMajPen(QPen(Qt::blue,  0, Qt::DotLine));
   grid_->setMinPen(QPen(Qt::blue,  0 ,Qt::DotLine));
   grid_->attach(this);

   legend_ = new QwtLegend(); 
   legend_->setDisplayPolicy(QwtLegend::AutoIdentifier, 
                             QwtLegendItem::ShowLine | QwtLegendItem::ShowSymbol |QwtLegendItem::ShowText );
   legend_->setFrameStyle(QFrame::Box|QFrame::Sunken);
   legend_->setItemMode(QwtLegend::ClickableItem );
 
   insertLegend(legend_,       QwtPlot::BottomLegend);

   enableAxis(xBottom, true);
   enableAxis(yLeft,   true);
   enableAxis(yRight,  true);
   
   QwtPlotLayout* layout = plotLayout();
   layout->setMargin(5);
   //layout->setSpacing(10); 
   //layout->setCanvasMargin(50); 


   setAxisFont( QwtPlot::xBottom, QFont( "Courier", 10, QFont::Bold ) ); // the font of the tick labels.
   setAxisFont( QwtPlot::yLeft,   QFont( "Courier", 10, QFont::Bold ) ); // the font of the tick labels.
   setAxisFont( QwtPlot::yRight,  QFont( "Courier", 10, QFont::Bold ) ); // the font of the tick labels.

   setAxisLabelAlignment(QwtPlot::xBottom, QwtPlot::AlignCenter);   // (int axis, int alignment)
   setAxisLabelRotation( QwtPlot::xBottom, 0.0);                    // (int axis, double rotation)



   setAxisTitle(QwtPlot::xBottom, "X- Bottom Axis");     
   setAxisTitle(QwtPlot::yLeft,   "Y- Left Axis"  );     
   setAxisTitle(QwtPlot::yRight,  "Y- Right Axis" ); 
 

   //setAxisMaxMajor(QwtPlot::xBottom, 6);
   //setAxisMaxMinor(QwtPlot::xBottom, 10);
   //setAxisScaleEngine(QwtPlot::xBottom, new QwtLog10ScaleEngine);

  
   setCanvasBackground(QColor("white"));

   xrange_ = xmax_-xmin_;

   //bottom_wheel_           =   new QwtWheel( axisWidget(QwtPlot::xBottom) );  
   //bottom_wheel_->setOrientation(Qt::Horizontal);

   //left_wheel_             =   new QwtWheel( axisWidget(QwtPlot::yLeft)   );  
   //left_wheel_->setOrientation(Qt::Vertical); 

   //right_wheel_            =   new QwtWheel( axisWidget(QwtPlot::yRight)  );  
   //right_wheel_->setOrientation(Qt::Vertical);


   //connect (bottom_wheel_, SIGNAL( valueChanged(double) ), this,  SLOT( bottomWheelValueChanged(double) ));
   //connect (left_wheel_,   SIGNAL( valueChanged(double) ), this,  SLOT( leftWheelValueChanged(double)  )  ); 
   //connect (right_wheel_,  SIGNAL( valueChanged(double) ), this,  SLOT( rightWheelValueChanged(double) )  );

   
   //enableThumbWheel(false,QwtPlot::xBottom);
   //enableThumbWheel(false,QwtPlot::yLeft);
   //enableThumbWheel(false,QwtPlot::yRight);

   connect ( this, SIGNAL(legendClicked(QwtPlotItem*)) , this, SLOT(toggleCurve(QwtPlotItem*)) ) ;  

  return;

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Plot::~Plot() 
{

  // Qt Widgets are automatically destroyed when their parent is destroyed.
  // QwtPlotCurve objects are also destroyed.

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool 
Plot::eventFilter(QObject *object, QEvent *e)
{
#if 0
    if ( e->type() == QEvent::Resize )
    {
      
       QSize const& size = dynamic_cast<QResizeEvent *>(e)->size();
       
       int x,y,w,h;

       const int margin = 2;

       if ( object == axisWidget(yLeft) ) { 
    
            QwtScaleWidget const* scale = axisWidget(yLeft);

            // adjust the wheel to the scale backbone
            
            x  = size.width(); // - scale->baseLineDist();
            w  = 10; //scale->baseLineDist();

            y  = 0;

            h = (int) (size.height() - 
                scale->startBorderDist() - scale->endBorderDist() ) / 5.0 ;
            
            h = 100;
            left_wheel_->setGeometry(x, y, w, h);
          
       } else if ( object == axisWidget(yRight) ) { 
    
            QwtScaleWidget const* scale = axisWidget(yRight);

            // adjust the wheel to the scale backbone
            
            x  = 0; // size.width() - scale->baseLineDist();
            w  = 10; //scale->baseLineDist();

            y  = 0;

            h = (int) (size.height() - 
                scale->startBorderDist() - scale->endBorderDist() ) / 5.0 ;
            
            h = 100;
            right_wheel_->setGeometry(x, y, w, h);
            
       } else if ( object == axisWidget(xBottom) ) {

            QwtScaleWidget const *scale = axisWidget(QwtPlot::xBottom);
            // adjust the wheel to the scale backbone
            w = (int) (size.width() - 
                 scale->startBorderDist() - scale->endBorderDist()) /  5.0;
 
            w = 100;
            x = size.width() - scale->endBorderDist() - w - margin;
            y = 0;
            //h = scale->baseLineDist();

            bottom_wheel_->setGeometry(x, y, w, h);
        
       } ;

    }

#endif

  return QwtPlot::eventFilter(object,e);

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  Plot::setData( PlotData const& pltdata) {

  // Get the data global min/max values

  xmin_  = pltdata.xMin( CurveData::xBottom );
  xmax_  = pltdata.xMax( CurveData::xBottom );
  
  ylmin_ = pltdata.yMin( CurveData::yLeft );
  ylmax_ = pltdata.yMax( CurveData::yLeft );

  yrmin_ = pltdata.yMin( CurveData::yRight );
  yrmax_ = pltdata.yMax( CurveData::yRight );

  
  setAxisScale(QwtPlot::xBottom, xmin_,   xmax_,  0);

  setAxisAutoScale(QwtPlot::yRight);  
  setAxisAutoScale(QwtPlot::yLeft);

  enableAxis( xBottom, true  );
  enableAxis( xTop,    false );
  enableAxis( yRight,  false );
  enableAxis( yLeft,   false );

  // add the curves to the plot ...

  QwtPlotCurve*   crv     = 0;

  for (unsigned int i=0; i < pltdata.nCurves(); ++i) {
     
     CurveData const& curve = pltdata[i];
 
     crv = new QwtPlotCurve( pltdata[i].getLabel() );
     crv->setData( curve );

     if ( pltdata[i].getYAxis() == CurveData::yLeft  ) enableAxis(yLeft,  true);
     if ( pltdata[i].getYAxis() == CurveData::yRight ) enableAxis(yRight, true);

     crv->setYAxis( curve.getYAxis() );
     crv->setPen(  QColor( curve.getColor().r, curve.getColor().g, curve.getColor().b ) );

     crv->attach(this);
   
     curves_[ crv->title().text() ] = crv;

   };


  // set the zoom bases ... 

  double lmag = pltdata.getScaleMag( CurveData::yLeft  );
  double rmag = pltdata.getScaleMag( CurveData::yRight  );

  setAxisScale(QwtPlot::xBottom, xmin_,   xmax_,                        0 );
  setAxisScale(QwtPlot::yRight,  yrmin_,  yrmin_+rmag*(yrmax_-yrmin_),  0 );
  setAxisScale(QwtPlot::yLeft,   ylmin_,  ylmin_+lmag*(ylmax_-ylmin_),  0 );

  setAxisTitle (QwtPlot::xBottom, pltdata.getLabel( CurveData::xBottom) );
  setAxisTitle (QwtPlot::yLeft,   pltdata.getLabel( CurveData::yLeft  ) );
  setAxisTitle (QwtPlot::yRight,  pltdata.getLabel( CurveData::yRight ) );
 
  replot();

  QApplication::postEvent( this, new QResizeEvent( QSize(width(), height()), QSize( width(), height() ) ) );

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Plot::leftWheelValueChanged(double value) 
{

  double range = axisScaleDiv(QwtPlot::yLeft)->hBound() -axisScaleDiv(QwtPlot::yLeft)->lBound();

  setAxisScale(QwtPlot::yLeft, value, value+range, 0);

  emit scaleChangedSignal();
  replot();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Plot::rightWheelValueChanged(double value)
{
  double range =axisScaleDiv(QwtPlot::yRight)->hBound() -axisScaleDiv(QwtPlot::yRight)->lBound();

  setAxisScale(QwtPlot::yRight, value, value+range, 0);
  emit scaleChangedSignal();
  replot();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void 
Plot::bottomWheelValueChanged(double value)
{
   setAxisScale(QwtPlot::xBottom, value, value+xrange_, 0);
   emit scaleChangedSignal(); // to force a rescale of the beamline lego display
   replot();
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Plot::getCurrentXmin() 
{
  return axisScaleDiv(QwtPlot::xBottom)->lBound();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Plot::getCurrentXmax() 
{
  return axisScaleDiv(QwtPlot::xBottom)->hBound();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Plot::getCurrentYmin() {

     // test active axis
  
 return axisScaleDiv(QwtPlot::yLeft)->lBound();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Plot::getCurrentYmax() 
{

     // test active axis
 return axisScaleDiv(QwtPlot::yLeft)->hBound();
 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
 
void Plot::scaleChanged( )
{

  double xmin  = axisScaleDiv(QwtPlot::xBottom)->lBound();
  double xmax  = axisScaleDiv(QwtPlot::xBottom)->hBound();

   xrange_ = xmax-xmin;

   int nturns = ceil((xmax_-xmin_)/xrange_) * 10;

  if (bottom_wheel_) {
    bottom_wheel_->setRange( xmin_, xmax_,  0.01);
    bottom_wheel_->setTotalAngle( nturns*360.0 );
    bottom_wheel_->setValue(xmin);
  };
  
  setAxisScale(QwtPlot::xBottom, xmin, xmin+xrange_, 0);

  emit scaleChangedSignal(); // to force a rescale of the lego plot
  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void 
Plot::resizeEvent(QResizeEvent *e) {
  
  QwtPlot::resizeEvent(e);
  emit plotResizedSignal();
  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Plot::enableThumbWheels(bool set) 
{
  enableThumbWheel(set, QwtPlot::xBottom);
  enableThumbWheel(set, QwtPlot::yLeft);
  enableThumbWheel(set, QwtPlot::yRight);

  replot();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Plot::enableThumbWheel(bool set, int axiscode) 
{

#if 0
 QwtScaleWidget* scale =  axisWidget(axiscode);

 if (!scale) return; // the axis is disabled.

Qt::Orientation orientation;
QwtWheel*       wheel =0;
 
switch (axiscode) {

   case QwtPlot::xBottom:
         wheel       = bottom_wheel_;
         orientation = Qt::Horizontal;
         break;
   case QwtPlot::yLeft:
         wheel       = left_wheel_;
         orientation = Qt::Vertical;
        break;
   case QwtPlot::yRight:
         wheel       = right_wheel_;
         orientation = Qt::Vertical;
        break;
  };


if (set) {
 
  wheel->setOrientation(orientation);
  wheel->setMass(0.5);
  //scale->setBaselineDist(20);
  scale->installEventFilter(this);
  wheel->show();

} else {

  removeEventFilter(scale);
  //scale->setBaselineDist(2);
  if (wheel) wheel->hide();

}

 replot();

#endif 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
  
void   Plot::enableDataPointSymbols(bool set) {

  QwtPlotItemList const& items = itemList();

  for ( QwtPlotItemList::const_iterator it  = items.begin(); 
	it != items.end(); ++it ) {

    if ( (*it)->rtti() != QwtPlotItem::Rtti_PlotCurve ) continue;

    QwtPlotCurve* cv = dynamic_cast<QwtPlotCurve*>(*it);     

    if (set)    
      cv->setSymbol( QwtSymbol(QwtSymbol::Ellipse,     QBrush("red"), QPen("blue"), QSize(4,4) ) );
    else
      cv->setSymbol( QwtSymbol(QwtSymbol::NoSymbol,    QBrush("red"), QPen("blue"), QSize(4,4) ) ) ;
  };

  replot();
  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void   Plot::enableInterpolation(bool set) {

  QwtPlotItemList const& items = itemList();

  for ( QwtPlotItemList::const_iterator it  = items.begin(); 
	it != items.end(); ++it ) {

    if ( (*it)->rtti() != QwtPlotItem::Rtti_PlotCurve ) continue;

    QwtPlotCurve* cv = dynamic_cast<QwtPlotCurve*>(*it);     
    
    if (set)    
      cv->setStyle( QwtPlotCurve::Lines);
    else {
      //cv->setStyle( QwtPlotCurve::Spline);
      cv->setStyle( QwtPlotCurve::Lines);
    }
  };

  replot();
  

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void   Plot::enableCoordinatesDisplay(bool set) 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Plot::setLogScale( int axis) 
{
  setAxisScaleEngine(axis, new QwtLog10ScaleEngine() ); // qwt auto-deletes old ScaleEngine
   replot();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Plot::setLinScale( int axis) 
{
 setAxisScaleEngine(axis, new QwtLinearScaleEngine()); // qwt auto-deletes old ScaleEngine
 replot();
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void   Plot::enableLegend(bool set) 
{
  if (set) 
    legend_->contentsWidget()->show();
  else 
    legend_->contentsWidget()->hide();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Plot::toggleCurve(QwtPlotItem* item) 
{ 
  QwtPlotCurve* crv =  curves_[ item->title().text() ]; 
  crv->setVisible( !item->isVisible() );
  replot();
}

  
  
       

