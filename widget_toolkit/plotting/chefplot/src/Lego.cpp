/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF:      A Qt-based Application 
******             Layered on top of of BEAMLINE.
******                                                                
******  File:      Lego.cpp
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

#include <sqlite/connection.hpp>
#include <sqlite/result.hpp>
#include <sqlite/execute.hpp>
#include <sqlite/query.hpp>

#include <Lego.h>
#include <beamline/beamline.h>
#include <qpainter.h>
#include <qwmatrix.h>
#include <qpixmap.h>
#include <iostream>

#include <sstream>


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LegoPlot::LegoPlot(QWidget * parent, const char * name, WFlags f ): 
  QWidget(parent,name,f), painter_(0),  pixmap_(0)
{
 
   pixmap_  = new QPixmap(2048, height()); // the width cannot be changed during a paint event
   painter_ = new QPainter(pixmap_, this);

   setBackgroundMode( Qt::PaletteBase );
   setWFlags(Qt::WNoAutoErase);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LegoPlot::~LegoPlot() 
{

  if (painter_)   painter_->end(); delete painter_;
  if (pixmap_)    delete pixmap_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LegoPlot::setBeamline( sqlite::connection& db) 
{

  std::ostringstream sql;
  sql << "SELECT type, length, strength FROM ELEMENTS"
         << std::ends;

  sqlite::query q(db, sql.str() );
  sqlite::result_type res = q.emit_result(); 

  if ( !res ) return;
  
  bmldata_.clear();

  do { 
       std::string elmtype =  res->get_string(0); 
       double length       =  res->get_double(1);
       double strength     =  res->get_double(2);

       bmldata_.push_back( ElmData(elmtype, length, (strength > 0.0) ));     
 
   } while ( res->next_row() );
  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LegoPlot::setBeamline( ConstBmlPtr bml) 
{

  if ( !bml  ) return;
  bmldata_.clear();
  
  for (beamline::const_deep_iterator it  = bml->deep_begin();
	                             it != bml->deep_end(); ++it) {
       
       ElmPtr elm = *it;

       std::string  elmtype = elm->Type();
       double        length = elm->Length();
       bool        polarity = ( elm->Strength() > 0.0 );

       bmldata_.push_back( ElmData(elmtype, length, polarity ));
  }
}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LegoPlot::setView(double x, double w, int loffset, int roffset) 
{
  
  x0_      = x;
  w_       = w;
  loffset_ = loffset; // loffset is the offset in screen coordinates on the canvas of x =  xmin_ 
  roffset_ = roffset; // roffset is the offset in screen coordinates on the canvas of x =  xmax_ 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  LegoPlot::resizeEvent ( QResizeEvent *event )
{

  // this needs to be cleaned up. the pixmap should  be resized dynamically

  QRect rect(QPoint(0,0), QSize(width(), height()));

  painter_->end();

  delete painter_; 
  delete pixmap_;  

  pixmap_  = new QPixmap(2048, rect.height()); // the width cannot be changed during a paint event
  painter_ = new QPainter(pixmap_, this);

  return QWidget::resizeEvent(event);
  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LegoPlot::paintEvent ( QPaintEvent *event )
{

  QRect legorect  = rect();

  pixmap_->fill( ); // should get background color here instead of assuming default
  
  painter_->setViewport(0, 0, legorect.width(), legorect.height() );
  painter_->setWindow(painter_->viewport());
  scale_ = double( ( roffset_ -loffset_ ) / w_ );

  double s =0.0;  
  for ( std::vector<ElmData>::iterator it =   bmldata_.begin();
                                       it !=  bmldata_.end(); ++it ) {

       std::string elmtype =  it->type;
       double len          =  it->length;
       bool  polarity      =  it->polarity;
    
       if ( (s+len) < x0_   ) { s += len; continue; }

       if ( s > (x0_ + w_ ) ) break;

       if ( elmtype == "sbend"            )    {
            drawBend(len, s);
       } else  if ( elmtype == "rbend"    )    {
            drawBend(len, s);
       } else  if ( elmtype == "CF_sbend" )    {
            drawBend(len, s);
       } else  if ( elmtype == "CF_rbend" )    {
            drawBend(len, s);
       } else if ( elmtype == "quadrupole")    {
            drawQuad(len, s, polarity  );
       } else if ( elmtype == "thinQuad"     ) {
            drawQuad(len, s,  polarity);
       } else if ( elmtype == "sextupole"    ) {
            drawSext(len, s, polarity );
       } else if ( elmtype == "thinSextupole") {
            drawSext(len, s,  polarity   );
       } else {
            drawDrift(len, s);
       };
 
       s += len;
 
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

  int is =   int( ceil( (s - x0_)* scale_ ) ) + loffset_;
  int il =   int( ceil( l* scale_ )         );
      il =   std::max(3,il);
  int joff  =   painter_->viewport().height()/2;
  
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
  int    is =   int( ceil( (s - x0_)* scale_ ) ) + loffset_;
  int    il =   int( ceil( l* scale_ )         );
         il =   std::max(3,il);
  int joff  =   painter_->viewport().height()/2;

 painter_->setPen(QPen(black, 1,  SolidLine));
 painter_->setBrush(QBrush(blue, SolidPattern));

 if (!focusing) {
    painter_->drawRect(is, -10+joff, il,  10);
 } else {
    painter_->drawRect(is,  0+joff,  il,  10); 
 };
 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
