#include "chefplotmain.h" 
#include "chefplot.h"
#include "plot.h"
#include "lego.h"
#include <qwt/qwt_plot_canvas.h>
#include <qwt/qwt_scale.h>
#include <qtoolbar.h>
#include <qmenubar.h>
#include <qevent.h>
#include <qprinter.h>
#include <qpainter.h>
#include <qwt/qwt_painter.h>
#include <qwt/qwt_plot_printfilter.h>

#include <iostream>
#include <string>

const int canvas_height = 30;


CHEFPlotMain::CHEFPlotMain(QWidget * parent, const char* name, Qt::WFlags f): CHEFPlotMainBase(parent,name,f) {


   _chefplot = new CHEFPlot(this);
   setCentralWidget(_chefplot);

}



CHEFPlotMain::~CHEFPlotMain() {

     // Qt widgets are automatically destroyed when their parent is destroyed

}

void CHEFPlotMain::displayLattice(beamline* bml){

  _chefplot->displayLattice(bml);
  
}

  
void CHEFPlotMain::addData(CHEFPlotData& cpdata){
  
 _chefplot->addData(cpdata);


}

void CHEFPlotMain::enableGrid(bool set) {
  
 _chefplot->enableGrid(set);

}


void CHEFPlotMain::enableThumbWheels(bool set) {

 _chefplot->enableThumbWheels(set);

}

void CHEFPlotMain::enableLegoPlot(bool set) {

 _chefplot->enableLegoPlot(set);
}


void CHEFPlotMain::zoomUseRightAxis() {
  
 _chefplot->zoomUseRightAxis();


}

void CHEFPlotMain::zoomUseLeftAxis(){

  _chefplot->zoomUseLeftAxis();
  
}

void CHEFPlotMain::showDataPoints( bool set ){

  _chefplot->_plot->enableDataPointSymbols(set);
  
}

void CHEFPlotMain::enableLegend( bool set ){

  _chefplot->_plot->enableLegend(set);
  _chefplot->_plot->replot();
  
}

void CHEFPlotMain::enableInterpolation( bool set ){

  _chefplot->_plot->enableInterpolation(set);
  _chefplot->_plot->replot();
  
}

void CHEFPlotMain::enableCoordinatesDisplay( bool set ){

  _chefplot->_plot->enableCoordinatesDisplay(set);
  
}

void CHEFPlotMain::exit( ){
  
  // deletes itself only when the plot is a main widget
  // This behavior should be changed to cover the case
  // when the widget is part of an MDI application 

  if ( ! parent() ) {
    delete this; 
  }
}

void CHEFPlotMain::clear( ){
  
     //  clear the plot

}


void CHEFPlotMain::filePrint() {

  QPrinter printer;
  printer.setOrientation(QPrinter::Landscape);
     //printer.setResolution(600);
  printer.setMinMax(1,1);
  printer.setPageSize(QPrinter::Letter);
  printer.setColorMode(QPrinter::Color);
  printer.setup();


  _chefplot->_plot->print(printer);

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




}


