/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Chef:      A Qt-based Application 
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

#include <CHEFPlotMain.h> 
#include <CHEFPlot.h>
#include <Plot.h>
#include <PlotData.h> 
#include <Lego.h>
#include <TuneDiagram.h>
#include <beamline/beamline.h>
#include <qapplication.h>
#include <qtoolbar.h>
#include <qmenubar.h>
#include <qevent.h>
#include <qprinter.h>
#include <qpainter.h>
#include <qtable.h>

#include <qwt_painter.h>
#include <qwt_plot_printfilter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_widget.h>

#include <DataDisplay.h>

#include <iostream>
#include <string>
#include <list>
#include <string>

const int canvas_height = 30;


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CHEFPlotMain::CHEFPlotMain(QWidget * parent, const char* name, Qt::WFlags f)
: CHEFPlotMainBase(parent,name,f), auto_clear_(true) 
{

  chefplot_    = new CHEFPlot(this);
  setCentralWidget(chefplot_);

  tunediagram_ = new TuneDiagram( parent );
  tunediagram_->hide();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


CHEFPlotMain::~CHEFPlotMain() {

 // Qt widgets are automatically destroyed when their parent is destroyed

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void CHEFPlotMain::displayLattice(BmlPtr bml)
{
  chefplot_->displayLattice(bml);
}
  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void CHEFPlotMain::addData(PlotData& pltdata)
{

 
 if (auto_clear_) chefplot_->clear(); 

 tunediagram_->setTunes( pltdata.getTunes().htune, pltdata.getTunes().vtune );
 tunediagram_->draw();

 chefplot_->setData(pltdata);

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void CHEFPlotMain::enableGrid(bool set) {
  
 chefplot_->enableGrid(set);

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void CHEFPlotMain::enableThumbWheels(bool set) {

 chefplot_->enableThumbWheels(set);

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void CHEFPlotMain::enableLegoPlot(bool set) {

 chefplot_->enableLegoPlot(set);

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void CHEFPlotMain::enableZoomRightYAxis() {

  std::cout << " CHEFPlotMain::enableZoomRightYAxis()" << std::endl;
 chefplot_->enableZoomRightYAxis(); 

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFPlotMain::enableZoomLeftYAxis() {
  
  std::cout << " CHEFPlotMain::enableZoomZoomLeftYAxis()" << std::endl;
 chefplot_->enableZoomLeftYAxis(); 

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFPlotMain::enableZoomIn() {

  std::cout << " CHEFPlotMain::enableZoomIn()" << std::endl;
  chefplot_->enableZoomIn();  

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFPlotMain::enableZoomOut() 
{
  

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void CHEFPlotMain::zoomReset()
{

  std::cout << " CHEFPlotMain::zoomReset()" << std::endl;
  chefplot_->zoomReset();  
  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void CHEFPlotMain::showDataPoints( bool set ){

  chefplot_->plot_->enableDataPointSymbols(set);
  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void CHEFPlotMain::enableLegend( bool set ){

  chefplot_->plot_->enableLegend(set);
  chefplot_->plot_->replot();
  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void CHEFPlotMain::enableInterpolation( bool set ){

  chefplot_->plot_->enableInterpolation(set);
  chefplot_->plot_->replot();
  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void CHEFPlotMain::enableCoordinatesDisplay( bool set ){

  chefplot_->plot_->enableCoordinatesDisplay(set);
  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void CHEFPlotMain::exit( ){
  
  // Do *not* use "delete this" as deleting the raw ptr interferes with situations where a widget* is 
  // wrapped with a smart (shared) pointer. 

  // If the widget has the WDestructiveClose widget flag enabled , the widget will be deleted.

  close(false);  

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void CHEFPlotMain::clear( ){
  
     chefplot_->clear();  //  clear the plot

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void CHEFPlotMain::setAutoClear( bool set){
  
     auto_clear_ = set;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFPlotMain::showTunes() 
{
  tunediagram_->setCaption( this->caption() );

  int x = ( qApp->mainWidget()->width()  - tunediagram_->width()  ) / 2;
  int y = ( qApp->mainWidget()->height() - tunediagram_->height() ) / 2;

  tunediagram_->parentWidget()->move( x, y );

  // NOTE: Must be done this way rather than
  // NOTE: 
  // NOTE:    tunediagram_->move( x, y );
  // NOTE:  
  // NOTE: because of MDI interface.
  // NOTE: See TrollTech's documentation on class QWorkspace.

  tunediagram_->show();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void CHEFPlotMain::showdata() 
{

  QWidget* parent = ( !parentWidget()) ? 0 : parentWidget()->parentWidget();
  
  DataDisplay* dd =  new DataDisplay(parent, "DataDisplay", Qt::WDestructiveClose, ! (chefplot_->getBeamline()->empty()) ); 

  dd->setCaption( this->caption() );
  dd->resize(width(), height());


  unsigned int rowsize    =  0;
  unsigned int colsize    =  0;  
  unsigned int bmlcolsize =  0;
  unsigned int nelmnts    =  0;              // no of elements in the beamline (if it exists)

  // determine maximum no of rows in the table 

  QwtPlotCurve *  cv = 0;
  QwtPlotItemList const&  itemlist = chefplot_->plot_->itemList();

  for (QwtPlotItemList::const_iterator it = itemlist.begin(); it != itemlist.end() ; ++it)
  {
    if ( (*it)->rtti() != QwtPlotItem::Rtti_PlotCurve)  continue;

    cv          = dynamic_cast<QwtPlotCurve*>(*it);
    rowsize     = std::max( (int) rowsize, cv->dataSize() );
    colsize    += 2; 
  };
 
  // adjust row and col size to accomodate beamline element names and azimuth info if applicable  

  QString data("");
  if ( !( chefplot_->getBeamline()->empty() )  ) 
  {  
     nelmnts    = chefplot_->getBeamline()->countHowManyDeeply();  
     bmlcolsize = 4;
     rowsize    = std::max( rowsize, nelmnts );

     dd->DataTable->setNumRows( rowsize );
     dd->DataTable->setNumCols( colsize + bmlcolsize);
  
     dd->DataTable->horizontalHeader()->setLabel( 0,    "Element Name");
     dd->DataTable->horizontalHeader()->setLabel( 1,    "Element Type");
     dd->DataTable->horizontalHeader()->setLabel( 2,    "Element Length [m]");
     dd->DataTable->horizontalHeader()->setLabel( 3,    "End Azimuth [m]");
  
     int idx =0;
     double azimuth = 0.0;
   
     ElmPtr beptr;

     for (beamline::deep_iterator it  = boost::const_pointer_cast<beamline>(chefplot_->getBeamline())->deep_begin();
                                  it != boost::const_pointer_cast<beamline>(chefplot_->getBeamline())->deep_end(); ++it, ++idx )
     {
       beptr = (*it);

       azimuth += beptr->Length();
       dd->DataTable->setText(idx, 0,       beptr->Name()  );
       dd->DataTable->setText(idx, 1,       beptr->Type()  );
       dd->DataTable->setText(idx, 2,       data.setNum( beptr->Length()) );
       dd->DataTable->setText(idx, 3,       data.setNum( azimuth) );

     }
  }

  // set datatable rowsize/colsize

  int idx   = -1; // curve index
  int col1  =  0;
  int col2  =  0;

  for (QwtPlotItemList::const_iterator it = itemlist.begin(); it != itemlist.end() ; ++it ) {

    if ( (*it)->rtti() != QwtPlotItem::Rtti_PlotCurve)  continue;

    cv          = dynamic_cast<QwtPlotCurve*>(*it); ++idx;

    col1 = bmlcolsize+2*idx; 
    col2 = col1+1; 

    dd->DataTable->horizontalHeader()->setLabel( col1,    chefplot_->plot_->axisWidget( cv->xAxis() )->title().text() );
    dd->DataTable->horizontalHeader()->setLabel( col2,    chefplot_->plot_->axisWidget( cv->yAxis() )->title().text()
                                                                         + QString("\n")+ cv->title().text() );
    for (int row=0; row < cv->dataSize(); ++row) { 
   
      dd->DataTable->setText(row, col1,     data.setNum( cv->x( row) ) ); // a rather inefficient way of setting row values. FIX ME !
      dd->DataTable->setText(row, col2,     data.setNum( cv->y( row) ) );

    }
  };


  dd->show();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void CHEFPlotMain::optionsVerLogScaleLeft(bool set) 
{

  if (set)

    chefplot_->setLogScale( QwtPlot::yLeft);

  else

    chefplot_->setLinScale( QwtPlot::yLeft);


}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void CHEFPlotMain::optionsVerLogScaleRight(bool set) 
{

  if (set)

    chefplot_->setLogScale( QwtPlot::yRight);

  else

    chefplot_->setLinScale( QwtPlot::yRight);


}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFPlotMain::optionsHorLogScale(bool set) 
{

  if (set)

    chefplot_->setLogScale( QwtPlot::xBottom);

  else

    chefplot_->setLinScale( QwtPlot::xBottom);


}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFPlotMain::optionsPersistent(bool set) 
{
    
    setAutoClear(!set);     

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFPlotMain::filePrint() 
{

  // not implemented.

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
