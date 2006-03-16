/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF:      A Qt-based Application 
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

#include <plot.h>
#include <chefplotdata.h>
#include <chefplotzoomer.h>

#include <qapplication.h>
#include <qlayout.h>
#include <qcanvas.h>
#include <qwhatsthis.h>
#include <qwt/qwt_scale.h>
#include <qwt/qwt_wheel.h>
#include <qwt/qwt_plot_canvas.h>
#include <qwt/qwt_plot_layout.h>
#include <qwt/qwt_symbol.h>
#include <qwt/qwt_legend.h>
#include <qwt/qwt_scldiv.h>
#include <iostream>


Plot::Plot(QWidget * parent, const char * name, Qt::WFlags f ): 
QwtPlot(parent, name), _bottom_wheel(0), _right_wheel(0), _left_wheel(0), 
                                                                _xmin(0.0), _xmax(0.0) {
 

   enableXBottomAxis(true);
   enableYLeftAxis(true);
   enableYRightAxis(true);
   
   QwtPlotLayout* layout = plotLayout();
   layout->setMargin(15);
   layout->setSpacing(10); 


   //setAxisFont( QwtPlot::xBottom, QFont( "Courier", 9, QFont::Bold ) );
   setAxisLabelFormat(QwtPlot::xBottom, 'g',  5, 8);               //  (int axis, char f, int prec, int fieldwidth=0)
   setAxisLabelAlignment(QwtPlot::xBottom, QwtPlot::AlignCenter);  // (int axis, int alignment)
   setAxisLabelRotation(QwtPlot::xBottom, 0.0);                    // (int axis, double rotation)
   //setAxisTitleFont( QwtPlot::xBottom, QFont( "Courier", 8, QFont::Bold   ) );
   setAxisTitle(QwtPlot::xBottom, "X- Bottom Axis");               //(int axis, const QString &t)

   //setAxisFont( QwtPlot::yLeft, QFont( "Helvetica", 9, QFont::Bold ) );
   setAxisLabelFormat(QwtPlot::yLeft, 'g',  5,  8);        //  (int axis, char f, int prec, int fieldwidth=0)
   //setAxisLabelAlignment(QwtPlot::yLeft, QwtPlot::AlignCenter);  // (int axis, int alignment)
   setAxisLabelRotation(QwtPlot::yLeft, 0.0);           // (int axis, double rotation)
   //setAxisTitleFont( QwtPlot::yLeft, QFont( "Courier", 8, QFont::Bold ) );
   setAxisTitle(QwtPlot::yLeft, "Y- Left Axis");      //(int axis, const QString &t)

   //setAxisFont( QwtPlot::yRight, QFont( "Helvetica", 9, QFont::Bold ) );
   setAxisLabelFormat(QwtPlot::yRight, 'g',  5, 8);        //  (int axis, char f, int prec, int fieldwidth=0)
   //setAxisLabelAlignment(QwtPlot::yRight, QwtPlot::AlignCenter);  // (int axis, int alignment)
   setAxisLabelRotation(QwtPlot::yRight, 0.0);           // (int axis, double rotation)
   //setAxisTitleFont( QwtPlot::yRight, QFont( "Courier", 8, QFont::Bold   ) );
   setAxisTitle(QwtPlot::yRight, "Y- Right Axis");      //(int axis, const QString &t)

 
  //setPaletteBackgroundColor( QColor("white") );
  
  setCanvasBackground(QColor("white"));
  _xrange = _xmax-_xmin;

  QwtScale*scale =  (QwtScale*) axis(QwtPlot::xBottom);
  _bottom_wheel =  new QwtWheel( scale );

  scale =           (QwtScale*) axis(QwtPlot::yLeft);
  _left_wheel   =  new QwtWheel( scale );

  scale =           (QwtScale*) axis(QwtPlot::yRight);
  _right_wheel  =  new QwtWheel( scale );


  _left_zoomer     = new CHEFPlotZoomer(QwtPlot::xBottom, yLeft,  canvas(), 0);
  _right_zoomer    = new CHEFPlotZoomer(QwtPlot::xBottom, yRight,  canvas(), 0);


  connect( _left_zoomer,  SIGNAL( scaleChangedSignal() ), this, SLOT( scaleChanged()  ));
  connect( _right_zoomer, SIGNAL( scaleChangedSignal() ), this, SLOT( scaleChanged()  ));

  setZoomer(true, QwtPlot::yLeft);

  connect (_bottom_wheel, SIGNAL( valueChanged(double) ), this,  SLOT( bottomWheelValueChanged(double) ));
  connect (_left_wheel,   SIGNAL( valueChanged(double) ), this,  SLOT( leftWheelValueChanged(double)  )  ); 
  connect (_right_wheel,  SIGNAL( valueChanged(double) ), this,  SLOT( rightWheelValueChanged(double) )  );

   
  enableThumbWheel(false,QwtPlot::xBottom);
  enableThumbWheel(false,QwtPlot::yLeft);
  enableThumbWheel(false,QwtPlot::yRight);


  setAutoLegend(false);
  enableLegend(true);
    
  //connect( this,  SIGNAL( legendClicked(long) ), this, SLOT( legendClicked( long ) ) );
  
  return;

}


//..................................................................................................

Plot::~Plot() {

  // Qt Widgets are automatically destroyed when their parent is destroyed.
  // QwtCurve objects are also destroyed.


}

//..................................................................................................

bool 
Plot::eventFilter(QObject *object, QEvent *e)
{

    if ( e->type() == QEvent::Resize )
    {
      
        const QSize &size = dynamic_cast<QResizeEvent *>(e)->size();
        int x,y,w,h;

       const int margin = 2;

       if ( object == (QObject *) axis(yLeft) ) { 
    
            const QwtScale*scale = axis(yLeft);

            // adjust the wheel to the scale backbone
            
            x  = size.width() - scale->baseLineDist();
            w  = scale->baseLineDist();

            y  = 0;

            h = (int) (size.height() - 
                scale->startBorderDist() - scale->endBorderDist() ) / 5.0 ;
            
            h = 100;
            _left_wheel->setGeometry(x, y, w, h);
            
       } else if ( object == (QObject *) axis(yRight) ) { 
    
            const QwtScale *scale = axis(yRight);

            // adjust the wheel to the scale backbone
            
            x  = 0; // size.width() - scale->baseLineDist();
            w  = scale->baseLineDist();

            y  = 0;

            h = (int) (size.height() - 
                scale->startBorderDist() - scale->endBorderDist() ) / 5.0 ;
            
            h = 100;
            _right_wheel->setGeometry(x, y, w, h);
            
       } else if ( object == (QObject *) axis(xBottom) ) {

            const QwtScale *scale = axis(QwtPlot::xBottom);
            // adjust the wheel to the scale backbone
            w = (int) (size.width() - 
                 scale->startBorderDist() - scale->endBorderDist()) /  5.0;
 
            w = 100;
            x = size.width() - scale->endBorderDist() - w - margin;
            y = 0;
            h = scale->baseLineDist();

            _bottom_wheel->setGeometry(x, y, w, h);
        
       } ;

    }
       return QwtPlot::eventFilter(object,e);

}

//..................................................................................................

void 
Plot::addData(const CHEFPlotData& cpdata) {

  long int curve_key;

  CHEFCurve* c = 0;

  // Get the data global min/max values

  _xmin  = cpdata.xMin();
  _xmax  = cpdata.xMax();
  
  _ylmin = cpdata.yLMin();
  _ylmax = cpdata.yLMax();

  _yrmin = cpdata.yRMin();
  _yrmax = cpdata.yRMax();

  setAxisScale(QwtPlot::xBottom, _xmin,   _xmax,  0);

  // turn on the appropriate y-axis(es)
  
  setAxisAutoScale(QwtPlot::yRight);  
  setAxisAutoScale(QwtPlot::yLeft);
  
  bool yright_enabled = false;
  bool yleft_enabled  = false;

  for (int i=0; i < cpdata.nCurves(); i++) {
   
    if ( cpdata[i].get()->yAxis() == QwtPlot::yLeft )
    {
        yleft_enabled = true;
    }

    if ( cpdata[i].get()->yAxis() == QwtPlot::yRight ) 
    {  
       yright_enabled = true;
    };
  };


  // add the curves to the plot ...

  PlotLegendItem* litem;
  
  for (int i=0; i < cpdata.nCurves(); i++) {
    
     c  = new CHEFCurve(0,0,0,"undefined"); // an empty curve

     c->copy( *cpdata[i].get());            // copy the curve since the widget destructor 
                                            // destroys the curves
     c->reparent(this);

     curve_key = insertCurve( c );
 
     litem =  new PlotLegendItem( curve_key, curveSymbol(curve_key), curvePen(curve_key), curveTitle(curve_key), this, 0 );
     legend()->insertItem( litem, i );
     litem->setToggleButton(true);
     litem->setOn(true);
     connect( litem, SIGNAL( toggled(bool) ), litem, SLOT( toggleCurve(bool) ) );
   };

  QwtPlotLayout* layout = plotLayout();

   if ( !yright_enabled ) 
   {
      enableYRightAxis(false);
   };
   
   if ( !yleft_enabled )
   {  
      enableYLeftAxis(false);
   };
 

  // set the zoom bases ... 

  double lmag = cpdata.scaleMagLeft();
  double rmag = cpdata.scaleMagRight();

  setAxisScale(QwtPlot::xBottom, _xmin,   _xmax,  0);
  setAxisScale(QwtPlot::yRight, _yrmin,   _yrmin+rmag*(_yrmax-_yrmin),  0);
  setAxisScale(QwtPlot::yLeft,  _ylmin,   _ylmin+lmag*(_ylmax-_ylmin),  0);

  _left_zoomer->setZoomBase();
  _right_zoomer->setZoomBase();
  
  setAxisTitle (QwtPlot::xBottom, cpdata.xLabel() );
  
  setAxisTitle (QwtPlot::yLeft,   cpdata.yLLabel());
  
  setAxisTitle (QwtPlot::yRight,  cpdata.yRLabel());
  
  replot();

  QApplication::postEvent( this, new QResizeEvent( QSize(width(), height()), QSize( width(), height() ) ) );

}


//..................................................................................................

void 
Plot::leftWheelValueChanged(double value) {
  double range = axisScale(QwtPlot::yLeft)->hBound() - axisScale(QwtPlot::yLeft)->lBound();
  setAxisScale(QwtPlot::yLeft, value, value+range, 0);
  emit scaleChangedSignal();
  replot();
}

//..................................................................................................

void 
Plot::rightWheelValueChanged(double value){
  double range = axisScale(QwtPlot::yRight)->hBound() - axisScale(QwtPlot::yRight)->lBound();
  setAxisScale(QwtPlot::yRight, value, value+range, 0);
  emit scaleChangedSignal();
  replot();
}

//..................................................................................................

void 
Plot::bottomWheelValueChanged(double value){

   setAxisScale(QwtPlot::xBottom, value, value+_xrange, 0);
   emit scaleChangedSignal(); // to force a rescaleof the beamline lego display
   replot();

}


//..................................................................................................

double 
Plot::getCurrentXmin() {

  return axisScale(QwtPlot::xBottom)->lBound();

}

//..................................................................................................

double
Plot::getCurrentXmax() {
  
  return axisScale(QwtPlot::xBottom)->hBound();
}

//..................................................................................................

double 
Plot::getCurrentYmin() {

     // test active axis

  
 return axisScale(QwtPlot::yLeft)->lBound();
}

//..................................................................................................

double 
Plot::getCurrentYmax() {

     // test active axis


 return axisScale(QwtPlot::yLeft)->hBound();
 
}

//..................................................................................................
 
void 
Plot::scaleChanged( ){

  double xmin  = axisScale(QwtPlot::xBottom)->lBound();
  double xmax  = axisScale(QwtPlot::xBottom)->hBound();

   _xrange = xmax-xmin;

   int nturns = ceil((_xmax-_xmin)/_xrange) * 10;

  if (_bottom_wheel) {
    _bottom_wheel->setRange( _xmin, _xmax,  0.01);
    _bottom_wheel->setTotalAngle( nturns*360.0 );
    _bottom_wheel->setValue(xmin);
  };
  
  setAxisScale(QwtPlot::xBottom, xmin, xmin+_xrange, 0);

  emit scaleChangedSignal(); // to force a rescale of the lego plot
  
}

//..................................................................................................

void 
Plot::resizeEvent(QResizeEvent *e) {
  
  QwtPlot::resizeEvent(e);
  emit plotResizedSignal();
  
}

//..................................................................................................

void 
Plot::enableThumbWheels(bool set) 
{
  enableThumbWheel(set, QwtPlot::xBottom);
  enableThumbWheel(set, QwtPlot::yLeft);
  enableThumbWheel(set, QwtPlot::yRight);

  replot();
}

//..................................................................................................

void 
Plot::enableThumbWheel(bool set, int axiscode) 
{


QwtScale*   scale =  (QwtScale*) axis(axiscode);

if (scale == 0) return; // the axis is disabled.

Qt::Orientation orientation;
QwtWheel*  wheel;
 

switch (axiscode) {

   case QwtPlot::xBottom:
         wheel = _bottom_wheel;
         orientation = Qt::Horizontal;
         break;
   case QwtPlot::yLeft:
         wheel = _left_wheel;
         orientation = Qt::Vertical;
        break;
   case QwtPlot::yRight:
         wheel = _right_wheel;
         orientation = Qt::Vertical;
        break;
  };


if (set) {
 
  wheel->setOrientation(orientation);
  wheel->setMass(0.5);
  scale->setBaselineDist(20);
  scale->installEventFilter(this);
  wheel->show();

} else {

  removeEventFilter(scale);
  scale->setBaselineDist(2);
  if (wheel) wheel->hide();

}

 replot();
 
}

//..................................................................................................

void Plot::setZoomer(bool set, int axis) {

  if (!set) {
    _left_zoomer->setEnabled(false);
    _right_zoomer->setEnabled(false);
    return;
  };
  

  QwtPlotZoomer* newzoomer = 0;
  QwtPlotZoomer* oldzoomer = 0;
  
  switch( axis) {
  
   case QwtPlot::yLeft: 
     newzoomer = _left_zoomer;
     oldzoomer = _right_zoomer;
     break;
     
   case QwtPlot::yRight: 
     newzoomer = _right_zoomer;
     oldzoomer = _left_zoomer;
     break;
  };
  
   oldzoomer->setRubberBand(QwtPicker::NoRubberBand);
   oldzoomer->setCursorLabelMode(QwtPicker::AlwaysOff);
   oldzoomer->setEnabled(false);
  
   newzoomer->setRubberBand(QwtPicker::RectRubberBand);
   newzoomer->setCursorLabelMode(QwtPicker::ActiveOnly);
   newzoomer->setEnabled(true);

   // attach the Y grid lines to the axis that has the focus

   setGridYAxis(axis);
   replot();
   
}

//..................................................................................................
  
void   Plot::enableDataPointSymbols(bool set) {

  QwtPlotCurveIterator  itc = curveIterator();

  for (QwtPlotCurve *c = itc.toFirst(); c != 0; c = ++itc ) {
    
    if (set)    
      c->setSymbol( QwtSymbol(QwtSymbol::Ellipse, QBrush("red"), QPen("blue"), QSize(4,4) ) );
    else
      c->setSymbol( QwtSymbol(QwtSymbol::None, QBrush("red"), QPen("blue"), QSize(4,4) ) ) ;
  };

  replot();
  
}

//..................................................................................................

void   Plot::enableInterpolation(bool set) {

 QwtPlotCurveIterator  itc = curveIterator();

  for (QwtPlotCurve *c = itc.toFirst(); c != 0; c = ++itc ) {
    
    if (set)    
      c->setStyle( QwtCurve::Lines);
    else
      c->setStyle(QwtCurve::Spline);
  };

  replot();
  

}

//..................................................................................................

void   Plot::enableCoordinatesDisplay(bool set) {


if (set) {

  _left_zoomer->setCursorLabelMode(QwtPicker::AlwaysOn);
  _right_zoomer->setCursorLabelMode(QwtPicker::AlwaysOn);
  
} else {
  
  _left_zoomer->setCursorLabelMode(QwtPicker::ActiveOnly);
  _right_zoomer->setCursorLabelMode(QwtPicker::ActiveOnly);
}
 

}

//..................................................................................................

void   Plot::toggleCurve(long key, bool setting) {
  
  curve(key)->setEnabled(setting);

}


//..................................................................................................



PlotLegendItem::PlotLegendItem(long curve_key, QWidget* parent, const char* name):
  QwtLegendButton(parent,name), _curve_key(curve_key) {

}


PlotLegendItem::PlotLegendItem(long curve_key, const QwtSymbol& symbol, const QPen& curvePen,
		 const QString & text, QWidget * parent, const char* name):
  QwtLegendButton(symbol, curvePen, text, parent, name), _curve_key(curve_key) {
  
}

//..................................................................................................

void   PlotLegendItem::toggleCurve(bool set) {

   Plot* plot = dynamic_cast<Plot*>( parent()->parent()->parent()->parent() );
   plot->toggleCurve( _curve_key, set );
   plot->replot();

}

//..................................................................................................


void 
Plot::setLogScale( int axis) 
{

 setAxisOptions(axis, QwtAutoScale::Logarithmic);
 _left_zoomer->setZoomBase();
 replot();

 
}

//..................................................................................................

void 
Plot::setLinScale( int axis) 
{

 setAxisOptions(axis, QwtAutoScale::None);
 _left_zoomer->setZoomBase();
 replot();

}


