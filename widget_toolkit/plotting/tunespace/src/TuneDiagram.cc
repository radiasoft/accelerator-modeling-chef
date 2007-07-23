/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF:      A Qt-based Application 
******             Layered on top of of BEAMLINE and MXYZPTLK
******
******  File:      TuneDiagram.cc
******  Original:  August   24, 2006
******  Revision:  September 4, 2006
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

#include <basic_toolkit/GenericException.h>

#include <qapplication.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qpixmap.h>

#include <TuneDiagram.h>

using namespace std;


/////////////////////////////////////////////////////////////////


TuneDiagram::TuneDiagram( QWidget* parent, const char* nnn, WFlags fff )
: QVBox( parent, nnn, fff )
, tunes_(2)
, order_(5)
, border_(60)
, tuneLabelPtr_(0)
, chalkBoardPtr_(0)
, imagoPtr_(0)
{
  tunes_(0) = 0.5; 
  tunes_(1) = 0.5; 

  finishConstructor();
}


/////////////////////////////////////////////////////////////////


TuneDiagram::TuneDiagram(   Vector const& nu, int ord
                          , QWidget* parent, const char* nnn, WFlags fff )
: QVBox( parent, nnn, fff )
, tunes_(nu)
, order_(ord)
, border_(60)
, tuneLabelPtr_(0)
, chalkBoardPtr_(0)
, imagoPtr_(0)
{
  if( order_ < 0  ) { order_ = - order_; }
  if( 0 == order_ ) { order_ = 5;        }

  if( 2 == tunes_.Dim() ) {
    finishConstructor();

  }    
  else {
    ostringstream uic;
    uic  << "Current version handles allows only two-dimensional tunespace."
            "\nYours has dimension "
         << tunes_.Dim()
         << '.';
    throw GenericException( __FILE__, __LINE__
          , "TuneDiagram::TuneDiagram( const Vector&, double )"
          , uic.str().c_str() );
  }

  draw();
  show();
}


/////////////////////////////////////////////////////////////////


void TuneDiagram::finishConstructor()
{
  tuneLabelPtr_  = new QLabel( this );
  chalkBoardPtr_ = new ChalkBoard( this );
  imagoPtr_      = new QPixmap;

  tunes_(0) = fabs(tunes_(0));
  tunes_(1) = fabs(tunes_(1));

  nx_lo_    = int(tunes_(0));
  ny_lo_    = int(tunes_(1));
  nx_hi_    = nx_lo_ + 1;
  ny_hi_    = ny_lo_ + 1;

  ostringstream uic;
  uic << "    "
      << "\nHorizontal tune = " 
      << (double(nearestInteger(1000.0*tunes_(0)))/1000.0)
      << "    Vertical tune   = "
      << (double(nearestInteger(1000.0*tunes_(1)))/1000.0)
      << "\n    ";
  tuneLabelPtr_->setTextFormat( Qt::PlainText );
  tuneLabelPtr_->setAlignment( Qt::AlignHCenter );
  tuneLabelPtr_->setFrameStyle( QFrame::Panel|QFrame::Raised );
  tuneLabelPtr_->setText( QString(uic.str().c_str()) );

  setCaption( QString("CHEF: Tune Diagram") );
}


/////////////////////////////////////////////////////////////////


void TuneDiagram::draw()
{
  
  imagoPtr_->fill();
  chalkBoardPtr_->erase();

  ostringstream uic;
  uic << "    "
      << "\nHorizontal tune = " 
      << (double(nearestInteger(1000.0*tunes_(0)))/1000.0)
      << "    Vertical tune   = "
      << (double(nearestInteger(1000.0*tunes_(1)))/1000.0)
      << "\n    ";
  tuneLabelPtr_->setTextFormat( Qt::PlainText );
  tuneLabelPtr_->setAlignment( Qt::AlignHCenter );
  tuneLabelPtr_->setFrameStyle( QFrame::Panel|QFrame::Raised );
  tuneLabelPtr_->setText( QString(uic.str().c_str()) );


  // ----------------------------------------------
  // Font specification taken from 
  // TrollTech's "Painting on the Desktop" example.
  // ----------------------------------------------
  // REMOVE: QFont f( "charter", 96, QFont::Black );
  // REMOVE: f.setPixelSize(border_/3);
  // REMOVE: f.setStyleHint( QFont::Times );
  // REMOVE: QApplication::setFont( f );
  
  QFont oldFont( QApplication::font() );
  QFont f( "charter", 96, QFont::Black );
  f.setPixelSize(border_/3);
  f.setStyleHint( QFont::Times );
  QApplication::setFont( f );

  const int w = 400 + border_;
  const int h = 400 + border_;

  chalkBoardPtr_->setBackgroundMode( Qt::FixedColor );
  chalkBoardPtr_->setBackgroundColor( Qt::white );

  this->setSize(w,h);

  QPen blackPen;
  blackPen.setColor( Qt::black );
  blackPen.setStyle ( Qt::SolidLine );
  blackPen.setWidth ( 1 );

  if( order_ <= 0 ) { order_ = 5; }

  int m_1, m_2;  
  Pair a, b;
  Pair trial[4];
  double n, n_lo, n_hi;

  for( int ord = 1; ord <= order_; ord++ ) {
    // Draw the line m_1 = ord, m_2 = 0
    n_lo = - nearestInteger( ord*nx_lo_ );
    n_hi = - nearestInteger( ord*nx_hi_ );
    for( n = n_hi; n < n_lo + 0.01; n++ ) {
      a.x = (-n/ord); a.y = ny_lo_;
      b.x = (-n/ord); b.y = ny_hi_;
      drawLine( a, b, &blackPen );
    }
       
    // Draw the line m_1 = 0, m_2 = ord
    n_lo = - nearestInteger( ord*ny_lo_ );
    n_hi = - nearestInteger( ord*ny_hi_ );
    for( n = n_hi; n < n_lo + 0.01; n++ ) {
      a.x = nx_lo_; a.y = (-n/ord);
      b.x = nx_hi_; b.y = (-n/ord);
      drawLine( a, b, &blackPen );
    }

    // Draw the sum resonances
    for( m_1 = 1; m_1 < ord; m_1++ ) 
    {
      m_2 = ord - m_1;
      n_lo = - nearestInteger( m_1*nx_lo_ + m_2*ny_lo_ );
      n_hi = - nearestInteger( m_1*nx_hi_ + m_2*ny_hi_ );

      for( n = n_hi; n < n_lo + 0.01; n++ ) 
      {
        trial[0].x = nx_lo_;
        trial[0].y = - (m_1*nx_lo_ + n )/m_2;

        trial[1].x = nx_hi_;
        trial[1].y = - (m_1*nx_hi_ + n )/m_2;

        trial[2].y = ny_lo_;
        trial[2].x = - (m_2*ny_lo_ + n )/m_1;

        trial[3].y = ny_hi_;
        trial[3].x = - (m_2*ny_hi_ + n )/m_1;

        int jj = -1;
        int j = 0;
        int count = 0;
        while( j < 4  &&  count < 2 ) 
        {
          if( nx_lo_ <= trial[j].x   &&   trial[j].x <= nx_hi_  &&
              ny_lo_ <= trial[j].y   &&   trial[j].y <= ny_hi_ ) 
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
          drawLine( a, b, &blackPen );
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
  mark( tunes_(0), tunes_(1), &redBrush );


  // -----------------
  // Write tune values
  // -----------------
  QPainter localPainter(chalkBoardPtr_);
  {
  ostringstream uic;
  uic << int(tunes_(0));

  blackPen.setColor(Qt::black);
  blackPen.setWidth(1);
  localPainter.setPen( blackPen );

  localPainter.drawText( border_, h-1-border_, 100, border_
                         , Qt::AlignLeft|Qt::AlignTop, QString(uic.str().c_str()) );
  localPainter.end();

  localPainter.begin( imagoPtr_ );
  localPainter.setPen( blackPen );
  localPainter.drawText( border_, h-1-border_, 100, border_
                         , Qt::AlignLeft|Qt::AlignTop, QString(uic.str().c_str()) );
  localPainter.end();
  }

  {
  ostringstream uic;
  uic << int(tunes_(1));

  localPainter.begin(chalkBoardPtr_);
  blackPen.setColor(Qt::black);
  blackPen.setWidth(1);

  localPainter.setPen( blackPen );
  localPainter.drawText( 0, h-1-2*border_, border_, border_
                         , Qt::AlignRight|Qt::AlignBottom, QString(uic.str().c_str()) );
  localPainter.end();

  localPainter.begin( imagoPtr_ );
  localPainter.setPen( blackPen );
  localPainter.drawText( 0, h-1-2*border_, border_, border_
                         , Qt::AlignRight|Qt::AlignBottom, QString(uic.str().c_str()) );
  localPainter.end();
  }

  {
  ostringstream uic;
  uic << (int(tunes_(0)) + 1);

  localPainter.begin(chalkBoardPtr_);
  localPainter.setPen( blackPen );
  localPainter.drawText( w - 1 - border_ - 100, h-1-border_, 100, border_
                         , Qt::AlignRight|Qt::AlignTop, QString(uic.str().c_str()) );
  localPainter.end();

  localPainter.begin( imagoPtr_ );
  localPainter.setPen( blackPen );
  localPainter.drawText( w - 1 - border_ - 100, h-1-border_, 100, border_
                         , Qt::AlignRight|Qt::AlignTop, QString(uic.str().c_str()) );
  localPainter.end();
  }

  {
  ostringstream uic;
  uic << (int(tunes_(1)) + 1);

  localPainter.begin(chalkBoardPtr_);
  localPainter.setPen( blackPen );
  localPainter.drawText( 0, border_, border_, border_
                         , Qt::AlignRight|Qt::AlignTop, QString(uic.str().c_str()) );
  localPainter.end();

  localPainter.begin( imagoPtr_ );
  localPainter.setPen( blackPen );
  localPainter.drawText( 0, border_, border_, border_
                         , Qt::AlignRight|Qt::AlignTop, QString(uic.str().c_str()) );
  localPainter.end();
  }

  QApplication::setFont( oldFont );
}


/////////////////////////////////////////////////////////////////


TuneDiagram::~TuneDiagram()
{
  if( imagoPtr_ ) { 
    delete imagoPtr_; 
    imagoPtr_ = 0; 
  }
}


/////////////////////////////////////////////////////////////////


void TuneDiagram::setSize( int w, int h )
{
  imagoPtr_->resize(w,h);
  imagoPtr_->fill();
  chalkBoardPtr_->setFixedSize(w,h);
  chalkBoardPtr_->erase();
  this->adjustSize();
}


/////////////////////////////////////////////////////////////////

void TuneDiagram::mark( double x, double y, QBrush* z )
{
  static QPainter localPainter;

  x = border_ + (chalkBoardPtr_->width()  - 1 - 2*border_)*(( x - nx_lo_ )/( nx_hi_ - nx_lo_ ));
  y = border_ + (chalkBoardPtr_->height() - 1 - 2*border_)*(( ny_hi_ - y )/( ny_hi_ - ny_lo_ ));

  localPainter.begin( chalkBoardPtr_ );
  localPainter.setBrush( *z );
  localPainter.setPen( Qt::NoPen );
  localPainter.drawEllipse( ((int) x)-5, ((int) y)-5, 10, 10 );
  localPainter.flush();
  localPainter.end();

  localPainter.begin( imagoPtr_ );
  localPainter.setBrush( *z );
  localPainter.setPen( Qt::NoPen );
  localPainter.drawEllipse( ((int) x)-5, ((int) y)-5, 10, 10 );
  localPainter.flush();
  localPainter.end();
}


/////////////////////////////////////////////////////////////////


void TuneDiagram::drawLine( const Pair& a, const Pair& b, QPen* z )
{
  static QPainter localPainter;

  localPainter.begin( chalkBoardPtr_ );
  localPainter.setPen( *z );
  localPainter.drawLine( border_ + int((chalkBoardPtr_->width()  - 1 - 2*border_) * double( a.x - nx_lo_ )/double( nx_hi_ - nx_lo_ )),
                         border_ + int((chalkBoardPtr_->height() - 1 - 2*border_) * double( ny_hi_ - a.y )/double( ny_hi_ - ny_lo_ )),
                         border_ + int((chalkBoardPtr_->width()  - 1 - 2*border_) * double( b.x - nx_lo_ )/double( nx_hi_ - nx_lo_ )),
                         border_ + int((chalkBoardPtr_->height() - 1 - 2*border_) * double( ny_hi_ - b.y )/double( ny_hi_ - ny_lo_ ))  );
  localPainter.end();

  localPainter.begin( imagoPtr_ );
  localPainter.setPen( *z );
  localPainter.drawLine( border_ + int((imagoPtr_->width()  - 1 - 2*border_) * double( a.x - nx_lo_ )/double( nx_hi_ - nx_lo_ )),
                         border_ + int((imagoPtr_->height() - 1 - 2*border_) * double( ny_hi_ - a.y )/double( ny_hi_ - ny_lo_ )),
                         border_ + int((imagoPtr_->width()  - 1 - 2*border_) * double( b.x - nx_lo_ )/double( nx_hi_ - nx_lo_ )),
                         border_ + int((imagoPtr_->height() - 1 - 2*border_) * double( ny_hi_ - b.y )/double( ny_hi_ - ny_lo_ ))  );
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
  bitBlt (   this
           ,   x->rect().topLeft()
           , dynamic_cast<TuneDiagram*>(this->parentWidget())->copyPtr()
           ,   x->rect(), CopyROP );
  this->QWidget::paintEvent(x);
}


/////////////////////////////////////////////////////////////////


void TuneDiagram::setTunes( double nu_x, double nu_y )
{
  tunes_(0) = nu_x;
  tunes_(1) = nu_y;
}
