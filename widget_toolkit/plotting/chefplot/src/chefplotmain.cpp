/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF:      A Qt-based Application 
******             Layered on top of of BEAMLINE.
******                                                                
******  File:      chefplotmain.cpp
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

#include <chefplotmain.h> 
#include <chefplot.h>
#include <plot.h>
#include <lego.h>
#include <TuneDiagram.h>
#include <beamline.h>
#include <qwt/qwt_plot_canvas.h>
#include <qwt/qwt_scale.h>
#include <qapplication.h>
#include <qtoolbar.h>
#include <qmenubar.h>
#include <qevent.h>
#include <qprinter.h>
#include <qpainter.h>
#include <qtable.h>

#include <qwt/qwt_painter.h>
#include <qwt/qwt_plot_printfilter.h>
#include <datadisplay.h>

#include <iostream>
#include <string>
#include <list>
#include <string>

const int canvas_height = 30;


CHEFPlotMain::CHEFPlotMain(QWidget * parent, const char* name, Qt::WFlags f)
: CHEFPlotMainBase(parent,name,f), _auto_clear(true) 
{
  _chefplot = new CHEFPlot(this);
  setCentralWidget(_chefplot);
  // REMOVE: _tunediagram = new TuneDiagram( parent, 0, Qt::WDestructiveClose );
  _tunediagram = new TuneDiagram( parent );
  _tunediagram->hide();
}

//.................................................................................

CHEFPlotMain::~CHEFPlotMain() {

 // Qt widgets are automatically destroyed when their parent is destroyed


}

//.................................................................................

void CHEFPlotMain::displayLattice(beamline* bml){

  _chefplot->displayLattice(bml);
  
  }

  
//.................................................................................

void CHEFPlotMain::addData(CHEFPlotData& cpdata)
{
 if (_auto_clear) _chefplot->clear(); 
 _tunediagram->setTunes( cpdata.getHorTune(), cpdata.getVerTune());
 _tunediagram->draw();
 _chefplot->addData(cpdata);
}

//.................................................................................

void CHEFPlotMain::enableGrid(bool set) {
  
 _chefplot->enableGrid(set);

}


//.................................................................................

void CHEFPlotMain::enableThumbWheels(bool set) {

 _chefplot->enableThumbWheels(set);

}

//.................................................................................

void CHEFPlotMain::enableLegoPlot(bool set) {

 _chefplot->enableLegoPlot(set);
}


//.................................................................................

void CHEFPlotMain::zoomUseRightAxis() {
  
 _chefplot->zoomUseRightAxis();


}

//.................................................................................

void CHEFPlotMain::zoomUseLeftAxis(){

  _chefplot->zoomUseLeftAxis();
  
}

//.................................................................................

void CHEFPlotMain::showDataPoints( bool set ){

  _chefplot->_plot->enableDataPointSymbols(set);
  
}

//.................................................................................

void CHEFPlotMain::enableLegend( bool set ){

  _chefplot->_plot->enableLegend(set);
  _chefplot->_plot->replot();
  
}

//.................................................................................

void CHEFPlotMain::enableInterpolation( bool set ){

  _chefplot->_plot->enableInterpolation(set);
  _chefplot->_plot->replot();
  
}

//.................................................................................

void CHEFPlotMain::enableCoordinatesDisplay( bool set ){

  _chefplot->_plot->enableCoordinatesDisplay(set);
  
}

//.................................................................................

void CHEFPlotMain::exit( ){
  
  // Do *not* use "delete this" as deleting the raw ptr interferes with situations where a widget* is 
  // wrapped with a smart (shared) pointer. 

  // If the widget has the WDestructiveClose widget flag enabled , the widget will be deleted.

  close(false);  

}

//.................................................................................

void CHEFPlotMain::clear( ){
  
     _chefplot->clear();  //  clear the plot

}

//.................................................................................

void CHEFPlotMain::setAutoClear( bool set){
  
     _auto_clear = set;

}

//.................................................................................



void CHEFPlotMain::filePrint() {

  QPrinter printer;
  printer.setOrientation(QPrinter::Landscape);
     //printer.setResolution(600);
  printer.setMinMax(1,1);
  printer.setPageSize(QPrinter::Letter);
  printer.setColorMode(QPrinter::Color);
  if ( !printer.setup() ) return;

  if ( printer.printerName() != "" )
  {
    _chefplot->_plot->print(printer);

  }

#if 0 

   //////////////////////////////////////////
  
  QPainter  painter(&printer);
     //QRect     plotRect           = painter.viewport();
     //QRect     xBottomScaleRect   = painter;
     //QRect     yLeftScaleRect     = ;
     //QRect     yRightScaleRect    = ;
     //QRect     titleRect          = ;
     //QRect     legendRect         = ;
  


  //QRect               canvasRect;
  //QwtArray            map; // mapping between plot and paint device coordinates


     //QwtPlotLayout* plotlayout  = _chefplot->_plot->layout();
  

    QwtPlotPrintFilter  pfilter =   QwtPlotPrintFilter();

    int axisCnt  = 3;
    int sDist    = 0;
    int eDist    = 0;
    const QwtScaleDiv* scaleDiv;

    QwtPainter::setMetricsMap(_chefplot->_plot, painter.device());
    const QwtMetricsMap& metricsMap = QwtPainter::metricsMap();


    QwtArray<QwtDiMap> map(axisCnt);

    for (int axis_index = 0; axis_index < axisCnt; axis_index++)
    {
        scaleDiv = _chefplot->_plot->axisScale(axis_index);
        
        map[axis_index].setDblRange(scaleDiv->lBound(), scaleDiv->hBound(), scaleDiv->logScale());

        std::cout << "scaleDiv->lBound(), scaleDiv->hBound()" << scaleDiv->lBound() << "  " << scaleDiv->hBound() << std::endl;

        sDist = _chefplot->_plot->axis(axis_index)->startBorderDist();
        eDist = _chefplot->_plot->axis(axis_index)->endBorderDist();

        const QRect scaleRect = _chefplot->_plot->axis(axis_index)->rect();

        double from, to;

        if ( axis_index == QwtPlot::xTop || axis_index == QwtPlot::xBottom )
        {
            from = metricsMap.layoutToDeviceX(scaleRect.left()  + sDist);
            to   = metricsMap.layoutToDeviceX(scaleRect.right() - eDist);
        }
        else
        {
            from = metricsMap.layoutToDeviceY(scaleRect.bottom() - sDist);
            to   = metricsMap.layoutToDeviceY(scaleRect.top()    + eDist);
        }

        map[axis_index].setIntRange(qwtInt(from), qwtInt(to));
        std::cout << "from, to: " << from << "  " << to << std::endl;
    }

     const QRect canvasRect = metricsMap.layoutToDevice(_chefplot->_plot->canvas()->rect());
     _chefplot->_plot->printCanvas(&painter, canvasRect,  map,  pfilter);

     QwtPainter::resetMetricsMap();


  
    //_chefplot->_plot->printScale(painter, axis, startDist, endDist, baseDist, rect);
   //_chefplot->_plot->printCanvas(painter, canvasRect,  map,  pfilter);
   //  _chefplot->_plot->printLegend(painter, rect);
     //_chefplot->_plot->printTitle(painter, rect);

     //painter.setViewport(plotRect.width()*0.20, plotRect.height()*0.20,  plotRect.width()*0.60, plotRect.height()*0.60);

#endif


}

//.................................................................................

void 
CHEFPlotMain::showTunes() 
{
  _tunediagram->setCaption( this->caption() );

  int x = ( qApp->mainWidget()->width()  - _tunediagram->width()  ) / 2;
  int y = ( qApp->mainWidget()->height() - _tunediagram->height() ) / 2;
  _tunediagram->parentWidget()->move( x, y );
  // NOTE: Must be done this way rather than
  // NOTE: 
  // NOTE:    _tunediagram->move( x, y );
  // NOTE:  
  // NOTE: because of MDI interface.
  // NOTE: See TrollTech's documentation on class QWorkspace.

  _tunediagram->show();
}


//.................................................................................

void
CHEFPlotMain::_showdata() 
{

  // as much of possible of this function should be moved to Datadisplay.

  QWidget* parent = 0;
  
  if (this->parentWidget() == 0) 
    parent = 0;
  else
    parent = this->parentWidget()->parentWidget();


  DataDisplay* dd =  new DataDisplay(parent, "DataDisplay", Qt::WDestructiveClose, (bool) _chefplot->getBeamline() ); 
  dd->setCaption( this->caption() );
  dd->resize(width(), height());

   
  const QwtPlotCurve* c = 0;
  QwtArray<long> keys = _chefplot->_plot->curveKeys(); 

  unsigned int rowsize    =  1;
  unsigned int colsize    =  2*keys.size();  
  unsigned int bmlcolsize =  0;

  unsigned int nelmnts =  0;              // no of elements in the beamline (if it exists)

  // determine maximum no of rows in the table 

  for (unsigned int i=0; i< keys.size(); i++)
  {
    c = _chefplot->_plot->curve( keys[i] );
    rowsize = std::max( (int) rowsize, c->dataSize() );
  };
 
  // adjust row and col size to accomodate beamline element names and azimuth info if applicable  

  QString data("");
  if ( _chefplot->getBeamline() ) 
  {  
     nelmnts    = _chefplot->getBeamline()->countHowManyDeeply();  
     bmlcolsize = 4;
     rowsize    = std::max( rowsize, nelmnts );

     dd->DataTable->setNumRows( rowsize );
     dd->DataTable->setNumCols( colsize + bmlcolsize);
  
     dd->DataTable->horizontalHeader()->setLabel( 0,    "Element Name");
     dd->DataTable->horizontalHeader()->setLabel( 1,    "Element Type");
     dd->DataTable->horizontalHeader()->setLabel( 2,    "Element Length [m]");
     dd->DataTable->horizontalHeader()->setLabel( 3,    "End Azimuth [m]");
  
     DeepBeamlineIterator it( *_chefplot->getBeamline() );
     bmlnElmnt* beptr = 0;

     int idx =0;
     double azimuth = 0.0;

     while ( (beptr = it++) ) 
     {

       azimuth += beptr->Length();
       dd->DataTable->setText(idx, 0,       beptr->Name()  );
       dd->DataTable->setText(idx, 1,       beptr->Type()  );
       dd->DataTable->setText(idx, 2,       data.setNum( beptr->Length()) );
       dd->DataTable->setText(idx, 3,       data.setNum( azimuth) );
      idx++;
     }
  }

  // set datatable rowsize/colsize

  for (unsigned int i=0; i < keys.size() ; i++) 
  {
    c = _chefplot->_plot->curve( keys[i] );
    dd->DataTable->horizontalHeader()->setLabel( bmlcolsize+2*i,      _chefplot->_plot->axis( c->xAxis() )->title() );
    dd->DataTable->horizontalHeader()->setLabel( bmlcolsize+2*i+1,    _chefplot->_plot->axis( c->yAxis() )->title() + QString("\n")+ c->title() );

  };


  for (unsigned int j=0; j < keys.size() ; j++)
  {
    c = _chefplot->_plot->curve( keys[j] );

    for (int i=0; i < c->dataSize(); i++)    
    { 
   
      dd->DataTable->setText(i, bmlcolsize+2*j,       data.setNum( c->x( i ) ));
      dd->DataTable->setText(i, bmlcolsize+2*j+1,     data.setNum( c->y( i ) ));

    }

  };

  dd->show();
}

//.................................................................................

void CHEFPlotMain::optionsVerLogScaleLeft(bool set) 
{

  if (set)

    _chefplot->setLogScale( QwtPlot::yLeft);

  else

    _chefplot->setLinScale( QwtPlot::yLeft);


}


//.................................................................................

void CHEFPlotMain::optionsVerLogScaleRight(bool set) 
{

  if (set)

    _chefplot->setLogScale( QwtPlot::yRight);

  else

    _chefplot->setLinScale( QwtPlot::yRight);


}

//.................................................................................

void CHEFPlotMain::optionsHorLogScale(bool set) 
{

  if (set)

    _chefplot->setLogScale( QwtPlot::xBottom);

  else

    _chefplot->setLinScale( QwtPlot::xBottom);


}

//.................................................................................

void CHEFPlotMain::optionsPersistent(bool set) 
{
    
    setAutoClear(!set);     

}
