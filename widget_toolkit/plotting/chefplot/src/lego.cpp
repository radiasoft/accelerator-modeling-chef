#include <beamline.h>
#include "lego.h"
#include <qpainter.h>
#include <qwmatrix.h>
#include <qpixmap.h>
#include <iostream>


LegoPlot::LegoPlot(QWidget * parent, const char * name, WFlags f ): QWidget(parent,name,f), _beamline(0) {
 
  setBackgroundMode( Qt::PaletteBase );
   _pixmap  = 0;
   _painter = 0;
   setWFlags(Qt::WNoAutoErase);
  
}

LegoPlot::~LegoPlot() {
   
  if (!_pixmap)    delete _pixmap;
  if (!_painter)   delete _painter;

}


void LegoPlot::setBeamline(beamline* bml) {
  
  _beamline = bml;


}

void LegoPlot::setBeamlineDisplayLimits(double x, double w) {
  
  _x0  = x;
  _w   = w;

}


void LegoPlot::resizeEvent ( QResizeEvent *event ){

     // this needs to be cleaned up. the pixmap should  be resized dynamically

  QRect rect(QPoint(0,0), QSize(width(), height()));

  if ( !_pixmap) 
    _pixmap = new QPixmap(2048, rect.height()); // the width cannot be changed during a paint event
  
  return QWidget::resizeEvent(event);
  
}



void LegoPlot::paintEvent ( QPaintEvent *event ){

  if( !_beamline ) return;

  QRect legorect  = rect();

   _pixmap->fill( ); // should get background color here instead of assuming default

   if ( !_painter)  
       _painter = new QPainter(_pixmap, this);
  
   _painter->setViewport(0, 0, legorect.width(), legorect.height() );
   _painter->setWindow(_painter->viewport());
   _scale = double(legorect.width())/ _w ;

   DeepBeamlineIterator it(_beamline);
     
      double s   = 0;
      double len = 0;
      bmlnElmnt* beptr = 0;
      
     
     while ( (beptr= it++) ) {

       len = beptr->Length();

       if ( std::string(beptr->Type()) == std::string("sbend") ) {

            if ( (s+len) >= _x0) drawBend(len, s);
  
       } else if ( std::string(beptr->Type()) == std::string("quadrupole") ) {

            if ( (s+len) >= _x0) drawQuad(len, s, (beptr->Strength()<0)  );

       } else if ( std::string(beptr->Type()) == std::string("thinQuad") ) {

            if ( (s+len) >= _x0) drawQuad(len, s,  (beptr->Strength()<0) );

       } else if ( std::string(beptr->Type()) == std::string("sextupole") ) {

            if ( (s+len) >= _x0) drawSext(len, s, (beptr->Strength()<0)  );

       } else if ( std::string(beptr->Type()) == std::string("thinSextupole") ) {

            if ( (s+len) >= _x0) drawSext(len, s,  (beptr->Strength()<0) );

       } else {
         
            if ( (s+len) >= _x0) drawDrift(len, s);
       };
 
       s += len;
       
       if ( s > (_x0 + _w ) ) break;

     }

        bitBlt(this, 0, 0, _pixmap, 0, 0, legorect.width(), legorect.height());
     

}

void LegoPlot::drawDrift( double l, double  s ) {
  
  int is, il, joff ;


  is    =   int( ceil( (s - _x0)* _scale ) );
  il    =   int( ceil( l* _scale )         );
  joff  =   _painter->viewport().height()/2;

 
  
 _painter->setPen(QPen(black, 1,  SolidLine));
 _painter->setBrush(QBrush(black, SolidPattern));
 _painter->drawRect(is, -1+joff,  il,  2);
 
}

void LegoPlot::drawBend( double l, double  s ) {

  int is, il, joff;
  is =   int( ceil( (s - _x0)* _scale ) );
  il =   int( ceil( l* _scale )         );
  joff  =   _painter->viewport().height()/2;

 _painter->setPen(QPen(black, 1,  SolidLine));
 _painter->setBrush(QBrush(green, SolidPattern));
 _painter->drawRect(is, -4+joff,  il,  8);

}


void LegoPlot::drawQuad( double l, double  s, bool focusing  ) {

  int is, il, joff;
  is =   int( ceil( (s - _x0)* _scale ) );
  il =   int( ceil( l* _scale )         );
  il =   std::max(3,il);
  joff  =   _painter->viewport().height()/2;
  
 _painter->setPen(QPen(black, 1,  SolidLine));
 _painter->setBrush(QBrush(red, SolidPattern));

 if (focusing) {
    _painter->drawRect(is, -12+joff, il,  12);
 } else {
   _painter->drawRect(is,  joff,   il,  12); 
 };

}


void LegoPlot::drawSext( double l, double  s,  bool focusing ) {

  int is, il, joff;
  is =   int( ceil( (s - _x0)* _scale ) );
  il =   int( ceil( l* _scale )         );
  il =   std::max(3,il);
  joff  =   _painter->viewport().height()/2;

 _painter->setPen(QPen(black, 1,  SolidLine));
 _painter->setBrush(QBrush(blue, SolidPattern));
 if (focusing) {
    _painter->drawRect(is, -10+joff, il,  10);
 } else {
   _painter->drawRect(is,  0+joff,  il,  10); 
 };
 
}

