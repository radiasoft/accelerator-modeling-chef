/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF:      A Qt-based Application 
******             Layered on top of of BEAMLINE and MXYZPTLK
******
******  File:      TuneDiagram.cc
******  Revision:  September 1, 2006
******                                                                
******  Copyright (c) Universities Research Association, Inc.   
******                All Rights Reserved                             
******                                                                
******  Author:    Leo Michelotti
******                                                                
******             Fermilab                                           
******             Batavia, IL  60510                                
******             michelotti@fnal.gov                         
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

#include <math.h>
#include <iomanip>

extern "C" { int nearestInteger( double ); }

#include <GenericException.h>

#include <qapplication.h>
#include <qlabel.h>
#include <qpainter.h>

#include <TuneDiagram.h>

using namespace std;


/////////////////////////////////////////////////////////////////

QPixmap* TuneDiagram::_imagoPtr = 0;

/////////////////////////////////////////////////////////////////


TuneDiagram::TuneDiagram( QWidget* parent, const char* nnn, WFlags fff )
: QVBox( parent, nnn, fff )
, _tunes(2)
, _order(5)
, _border(60)
{
  _tunes(0) = 0.5; 
  _tunes(1) = 0.5; 
  _nx_lo    = 0;
  _ny_lo    = 0;
  _nx_hi    = 1;
  _ny_hi    = 1;

  _imagoPtr = new QPixmap;
  _tuneLabelPtr  = new QLabel( this );
  _chalkBoardPtr = new ChalkBoard( this );

  ostringstream uic;
  uic << "    "
      << "\nHorizontal tune = " 
      << (double(nearestInteger(1000.0*_tunes(0)))/1000.0)
      << "    Vertical tune   = "
      << (double(nearestInteger(1000.0*_tunes(1)))/1000.0)
      << "\n    ";
  _tuneLabelPtr->setTextFormat( Qt::PlainText );
  _tuneLabelPtr->setAlignment( Qt::AlignHCenter );
  _tuneLabelPtr->setFrameStyle( QFrame::Panel|QFrame::Raised );
  _tuneLabelPtr->setText( QString(uic.str().c_str()) );

  this->setCaption( QString("CHEF: Tune Diagram") );

  _draw();

}


/////////////////////////////////////////////////////////////////


TuneDiagram::TuneDiagram(   const Vector& nu, int ord
                          , QWidget* parent, const char* nnn, WFlags fff )
: QVBox( parent, nnn, fff )
, _tunes(nu)
, _order(ord)
, _border(60)
{
  if( 2 == _tunes.Dim() ) {
    _imagoPtr = new QPixmap;
    _tuneLabelPtr  = new QLabel( "Hi there", this );
    _chalkBoardPtr = new ChalkBoard( this );

    _nx_lo = int(fabs(nu(0)));
    _ny_lo = int(fabs(nu(1)));
    _nx_hi = _nx_lo + 1;
    _ny_hi = _ny_lo + 1;
  }    
  else {
    ostringstream uic;
    uic  << "Current version handles allows only two-dimensional tunespace."
            "\nYours has dimension "
         << _tunes.Dim()
         << '.';
    throw GenericException( __FILE__, __LINE__
          , "TuneDiagram::TuneDiagram( const Vector&, double )"
          , uic.str().c_str() );
  }

  this->setCaption( QString("CHEF: Tune Diagram") );

  _draw();
}


/////////////////////////////////////////////////////////////////


void TuneDiagram::_draw()
{
  ostringstream uic;
  uic << "    "
      << "\nHorizontal tune = " 
      << (double(nearestInteger(1000.0*_tunes(0)))/1000.0)
      << "    Vertical tune   = "
      << (double(nearestInteger(1000.0*_tunes(1)))/1000.0)
      << "\n    ";
  _tuneLabelPtr->setTextFormat( Qt::PlainText );
  _tuneLabelPtr->setAlignment( Qt::AlignHCenter );
  _tuneLabelPtr->setFrameStyle( QFrame::Panel|QFrame::Raised );
  _tuneLabelPtr->setText( QString(uic.str().c_str()) );


  // ----------------------------------------------
  // Font specification taken from 
  // TrollTech's "Painting on the Desktop" example.
  // ----------------------------------------------
  // REMOVE: QFont f( "charter", 96, QFont::Black );
  // REMOVE: f.setPixelSize(_border/3);
  // REMOVE: f.setStyleHint( QFont::Times );
  // REMOVE: QApplication::setFont( f );
  
  QFont oldFont( QApplication::font() );
  QFont f( "charter", 96, QFont::Black );
  f.setPixelSize(_border/3);
  f.setStyleHint( QFont::Times );
  QApplication::setFont( f );

  const int w = 400 + _border;
  const int h = 400 + _border;

  _chalkBoardPtr->setBackgroundMode( Qt::FixedColor );
  _chalkBoardPtr->setBackgroundColor( Qt::white );

  this->setSize(w,h);

  QPen blackPen;
  blackPen.setColor( Qt::black );
  blackPen.setStyle ( Qt::SolidLine );
  blackPen.setWidth ( 1 );

  if( _order <= 0 ) { _order = 5; }

  int m_1, m_2;  
  Pair a, b;
  Pair trial[4];
  double n, n_lo, n_hi;

  for( int ord = 1; ord <= _order; ord++ ) {
    // Draw the line m_1 = ord, m_2 = 0
    n_lo = - nearestInteger( ord*_nx_lo );
    n_hi = - nearestInteger( ord*_nx_hi );
    for( n = n_hi; n < n_lo + 0.01; n++ ) {
      a.x = (-n/ord); a.y = _ny_lo;
      b.x = (-n/ord); b.y = _ny_hi;
      this->_drawLine( a, b, &blackPen );
    }
       
    // Draw the line m_1 = 0, m_2 = ord
    n_lo = - nearestInteger( ord*_ny_lo );
    n_hi = - nearestInteger( ord*_ny_hi );
    for( n = n_hi; n < n_lo + 0.01; n++ ) {
      a.x = _nx_lo; a.y = (-n/ord);
      b.x = _nx_hi; b.y = (-n/ord);
      this->_drawLine( a, b, &blackPen );
    }

    // Draw the sum resonances
    for( m_1 = 1; m_1 < ord; m_1++ ) 
    {
      m_2 = ord - m_1;
      n_lo = - nearestInteger( m_1*_nx_lo + m_2*_ny_lo );
      n_hi = - nearestInteger( m_1*_nx_hi + m_2*_ny_hi );

      for( n = n_hi; n < n_lo + 0.01; n++ ) 
      {
        trial[0].x = _nx_lo;
        trial[0].y = - (m_1*_nx_lo + n )/m_2;

        trial[1].x = _nx_hi;
        trial[1].y = - (m_1*_nx_hi + n )/m_2;

        trial[2].y = _ny_lo;
        trial[2].x = - (m_2*_ny_lo + n )/m_1;

        trial[3].y = _ny_hi;
        trial[3].x = - (m_2*_ny_hi + n )/m_1;

        int jj = -1;
        int j = 0;
        int count = 0;
        while( j < 4  &&  count < 2 ) 
        {
          if( _nx_lo <= trial[j].x   &&   trial[j].x <= _nx_hi  &&
              _ny_lo <= trial[j].y   &&   trial[j].y <= _ny_hi ) 
          {
            if( jj == -1 ) 
            {
              a = trial[j];
              count++;
              jj = j;
            }
            else 
            {
              if( ( fabs( trial[j].x - trial[jj].x ) > 0.001 )  && 
                  ( fabs( trial[j].y - trial[jj].y ) > 0.001 )      )
              {
                b = trial[j];
                count++;
              }
            }
          }

          j++;
        }

        if( count == 2 ) {
          this->_drawLine( a, b, &blackPen );
        }
      }
    }
  }


  // ------------------------
  // Mark the tunes' position 
  // ------------------------
  QBrush redBrush;
  redBrush.setColor( Qt::red );
  redBrush.setStyle( Qt::SolidPattern );
  this->_mark( _tunes(0), _tunes(1), &redBrush );


  // -----------------
  // Write tune values
  // -----------------
  QPainter localPainter(_chalkBoardPtr);
  {
  ostringstream uic;
  uic << int(_tunes(0));

  blackPen.setColor(Qt::black);
  blackPen.setWidth(1);
  localPainter.setPen( blackPen );

  localPainter.drawText( _border, h-1-_border, 100, _border
                         , Qt::AlignLeft|Qt::AlignTop, QString(uic.str().c_str()) );
  localPainter.end();

  localPainter.begin( _imagoPtr );
  localPainter.setPen( blackPen );
  localPainter.drawText( _border, h-1-_border, 100, _border
                         , Qt::AlignLeft|Qt::AlignTop, QString(uic.str().c_str()) );
  localPainter.end();
  }

  {
  ostringstream uic;
  uic << int(_tunes(1));

  localPainter.begin(_chalkBoardPtr);
  blackPen.setColor(Qt::black);
  blackPen.setWidth(1);

  localPainter.setPen( blackPen );
  localPainter.drawText( 0, h-1-2*_border, _border, _border
                         , Qt::AlignRight|Qt::AlignBottom, QString(uic.str().c_str()) );
  localPainter.end();

  localPainter.begin( _imagoPtr );
  localPainter.setPen( blackPen );
  localPainter.drawText( 0, h-1-2*_border, _border, _border
                         , Qt::AlignRight|Qt::AlignBottom, QString(uic.str().c_str()) );
  localPainter.end();
  }

  {
  ostringstream uic;
  uic << (int(_tunes(0)) + 1);

  localPainter.begin(_chalkBoardPtr);
  localPainter.setPen( blackPen );
  localPainter.drawText( w - 1 - _border - 100, h-1-_border, 100, _border
                         , Qt::AlignRight|Qt::AlignTop, QString(uic.str().c_str()) );
  localPainter.end();

  localPainter.begin( _imagoPtr );
  localPainter.setPen( blackPen );
  localPainter.drawText( w - 1 - _border - 100, h-1-_border, 100, _border
                         , Qt::AlignRight|Qt::AlignTop, QString(uic.str().c_str()) );
  localPainter.end();
  }

  {
  ostringstream uic;
  uic << (int(_tunes(1)) + 1);

  localPainter.begin(_chalkBoardPtr);
  localPainter.setPen( blackPen );
  localPainter.drawText( 0, _border, _border, _border
                         , Qt::AlignRight|Qt::AlignTop, QString(uic.str().c_str()) );
  localPainter.end();

  localPainter.begin( _imagoPtr );
  localPainter.setPen( blackPen );
  localPainter.drawText( 0, _border, _border, _border
                         , Qt::AlignRight|Qt::AlignTop, QString(uic.str().c_str()) );
  localPainter.end();
  }

  QApplication::setFont( oldFont );
}


/////////////////////////////////////////////////////////////////


TuneDiagram::~TuneDiagram()
{
}


/////////////////////////////////////////////////////////////////


void TuneDiagram::setSize( int w, int h )
{
  _imagoPtr->resize(w,h);
  _imagoPtr->fill();
  _chalkBoardPtr->setFixedSize(w,h);
  _chalkBoardPtr->erase();
  this->adjustSize();
}


/////////////////////////////////////////////////////////////////

void TuneDiagram::_mark( double x, double y, QBrush* z )
{
  static QPainter localPainter;

  x = _border + (_chalkBoardPtr->width()  - 1 - 2*_border)*(( x - _nx_lo )/( _nx_hi - _nx_lo ));
  y = _border + (_chalkBoardPtr->height() - 1 - 2*_border)*(( _ny_hi - y )/( _ny_hi - _ny_lo ));

  localPainter.begin( _chalkBoardPtr );
  localPainter.setBrush( *z );
  localPainter.setPen( Qt::NoPen );
  localPainter.drawEllipse( ((int) x)-5, ((int) y)-5, 10, 10 );
  localPainter.flush();
  localPainter.end();

  localPainter.begin( _imagoPtr );
  localPainter.setBrush( *z );
  localPainter.setPen( Qt::NoPen );
  localPainter.drawEllipse( ((int) x)-5, ((int) y)-5, 10, 10 );
  localPainter.flush();
  localPainter.end();
}


/////////////////////////////////////////////////////////////////


void TuneDiagram::_drawLine( const Pair& a, const Pair& b, QPen* z )
{
  static QPainter localPainter;

  localPainter.begin( _chalkBoardPtr );
  localPainter.setPen( *z );
  localPainter.drawLine( _border + int((_chalkBoardPtr->width()  - 1 - 2*_border) * double( a.x - _nx_lo )/double( _nx_hi - _nx_lo )),
                         _border + int((_chalkBoardPtr->height() - 1 - 2*_border) * double( _ny_hi - a.y )/double( _ny_hi - _ny_lo )),
                         _border + int((_chalkBoardPtr->width()  - 1 - 2*_border) * double( b.x - _nx_lo )/double( _nx_hi - _nx_lo )),
                         _border + int((_chalkBoardPtr->height() - 1 - 2*_border) * double( _ny_hi - b.y )/double( _ny_hi - _ny_lo ))  );
  localPainter.end();

  localPainter.begin( _imagoPtr );
  localPainter.setPen( *z );
  localPainter.drawLine( _border + int((_imagoPtr->width()  - 1 - 2*_border) * double( a.x - _nx_lo )/double( _nx_hi - _nx_lo )),
                         _border + int((_imagoPtr->height() - 1 - 2*_border) * double( _ny_hi - a.y )/double( _ny_hi - _ny_lo )),
                         _border + int((_imagoPtr->width()  - 1 - 2*_border) * double( b.x - _nx_lo )/double( _nx_hi - _nx_lo )),
                         _border + int((_imagoPtr->height() - 1 - 2*_border) * double( _ny_hi - b.y )/double( _ny_hi - _ny_lo ))  );
  localPainter.flush();
  localPainter.end();
}


/////////////////////////////////////////////////////////////////


TuneDiagram::ChalkBoard::ChalkBoard( QWidget* parent )
: QWidget( parent )
{
}


/////////////////////////////////////////////////////////////////


TuneDiagram::ChalkBoard::~ChalkBoard()
{
}


/////////////////////////////////////////////////////////////////


void TuneDiagram::ChalkBoard::paintEvent( QPaintEvent* x )
{
  bitBlt ( this, x->rect().topLeft(), TuneDiagram::_imagoPtr, x->rect(), CopyROP );
  this->QWidget::paintEvent(x);
}


/////////////////////////////////////////////////////////////////


void TuneDiagram::setTunes( double nu_x, double nu_y )
{
  _tunes(0) = nu_x;
  _tunes(1) = nu_y;
  this->_draw();
}
