/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF:      A Qt-based Application 
******             Layered on top of of BEAMLINE.
******                                                                
******  File:      lego.cpp
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

#include <Lego.h>
#include <beamline/beamline.h>
#include <qpainter.h>
#include <qwmatrix.h>
#include <qpixmap.h>
#include <iostream>

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LegoPlot::LegoPlot(QWidget * parent, const char * name, WFlags f ): 
QWidget(parent,name,f) {
 
  setBackgroundMode( Qt::PaletteBase );
   pixmap_  = 0;
   painter_ = 0;
   setWFlags(Qt::WNoAutoErase);
  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



LegoPlot::~LegoPlot() {
   
  if (!pixmap_)    delete pixmap_;
  if (!painter_)   delete painter_;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LegoPlot::setBeamline( ConstBmlPtr bml) 
{
  
     bml_ = bml;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ConstBmlPtr LegoPlot::getBeamline() 
{

  return bml_;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void LegoPlot::setBeamlineDisplayLimits(double x, double w, int loffset, int roffset) {
  
  x0_     = x;
  w_      = w;
  loffset_ = loffset; // loffset is the offset in screen coordinates on the canvas of x =  _xmin 
  roffset_ = roffset; // roffset is the offset in screen coordinates on the canvas of x =  _xmax 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  LegoPlot::resizeEvent ( QResizeEvent *event )
{

  // this needs to be cleaned up. the pixmap should  be resized dynamically

  QRect rect(QPoint(0,0), QSize(width(), height()));

  if ( !pixmap_) 
    pixmap_ = new QPixmap(2048, rect.height()); // the width cannot be changed during a paint event
  
  return QWidget::resizeEvent(event);
  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LegoPlot::paintEvent ( QPaintEvent *event )
{

  if( !bml_ ) return;

  QRect legorect  = rect();

   pixmap_->fill( ); // should get background color here instead of assuming default

   if ( !painter_)  
       painter_ = new QPainter(pixmap_, this);
  
   painter_->setViewport(0, 0, legorect.width(), legorect.height() );
   painter_->setWindow(painter_->viewport());
   scale_ = double( ( roffset_ -loffset_ ) / w_ );

     double s   = 0;
     double len = 0;
     ElmPtr beptr;

     for (beamline::const_deep_iterator it    = bml_->deep_begin();
	                                it   != bml_->deep_end(); ++it) {
       
       beptr = *it;

       len = beptr->Length();

       if ( std::string(beptr->Type()) == std::string("sbend") ) {

            if ( (s+len) >= x0_) drawBend(len, s);
  
       } else  if ( std::string(beptr->Type()) == std::string("rbend") ) {

            if ( (s+len) >= x0_) drawBend(len, s);
  
       } else  if ( std::string(beptr->Type()) == std::string("CF_sbend") ) {

            if ( (s+len) >= x0_) drawBend(len, s);
  
       } else  if ( std::string(beptr->Type()) == std::string("CF_rbend") ) {

            if ( (s+len) >= x0_) drawBend(len, s);
  
       } else if ( std::string(beptr->Type()) == std::string("quadrupole") ) {

            if ( (s+len) >= x0_) drawQuad(len, s, (beptr->Strength()<0)  );

       } else if ( std::string(beptr->Type()) == std::string("thinQuad") ) {

            if ( (s+len) >= x0_) drawQuad(len, s,  (beptr->Strength()<0) );

       } else if ( std::string(beptr->Type()) == std::string("sextupole") ) {

            if ( (s+len) >= x0_) drawSext(len, s, (beptr->Strength()<0)  );

       } else if ( std::string(beptr->Type()) == std::string("thinSextupole") ) {

            if ( (s+len) >= x0_) drawSext(len, s,  (beptr->Strength()<0) );

       } else {
         
            if ( (s+len) >= x0_) drawDrift(len, s);
       };
 
       s += len;
       
       if ( s > (x0_ + w_ ) ) break;

     }

        bitBlt(this, 0, 0, pixmap_, 0, 0, legorect.width(), legorect.height());
     

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void  LegoPlot::drawDrift( double l, double  s ) 
{
  
  int is, il, joff ;


  is    =   int( ceil( (s - x0_)* scale_) ) + loffset_;
  il    =   int( ceil( l* scale_ )         );
  joff  =   painter_->viewport().height()/2;

 
  
 painter_->setPen(QPen(black, 1,  SolidLine));
 painter_->setBrush(QBrush(black, SolidPattern));
 painter_->drawRect(is, -1+joff,  il,  2);
 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LegoPlot::drawBend( double l, double  s ) 
{

  int is, il, joff;
  is =   int( ceil( (s - x0_)* scale_ ) ) + loffset_;
  il =   int( ceil( l* scale_ )         );
  joff  =   painter_->viewport().height()/2;

 painter_->setPen(QPen(black, 1,  SolidLine));
 painter_->setBrush(QBrush(green, SolidPattern));
 painter_->drawRect(is, -4+joff,  il,  8);

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LegoPlot::drawQuad( double l, double  s, bool focusing  ) 
{

  int is, il, joff;
  is =   int( ceil( (s - x0_)* scale_ ) ) + loffset_;
  il =   int( ceil( l* scale_ )         );
  il =   std::max(3,il);
  joff  =   painter_->viewport().height()/2;
  
 painter_->setPen(QPen(black, 1,  SolidLine));
 painter_->setBrush(QBrush(red, SolidPattern));

 if (!focusing) {
    painter_->drawRect(is, -12+joff, il,  12);
 } else {
   painter_->drawRect(is,  joff,   il,  12); 
 };

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void LegoPlot::drawSext( double l, double  s,  bool focusing ) 
{

  int is, il, joff;
  is =   int( ceil( (s - x0_)* scale_ ) ) + loffset_;
  il =   int( ceil( l* scale_ )         );
  il =   std::max(3,il);
  joff  =   painter_->viewport().height()/2;

 painter_->setPen(QPen(black, 1,  SolidLine));
 painter_->setBrush(QBrush(blue, SolidPattern));
 if (!focusing) {
    painter_->drawRect(is, -10+joff, il,  10);
 } else {
   painter_->drawRect(is,  0+joff,  il,  10); 
 };
 
}

