/**********************************************************************/
/*                                                                    */
/* File:           TuneDiagram.h                                      */
/*                                                                    */ 
/* Author:         Leo Michelotti                                     */
/*                 michelotti@fnal.gov                                */     
/*                                                                    */ 
/* Creation Date:  August   24, 2006                                  */
/* Current rev:    September 4, 2006                                  */
/*                                                                    */ 
/* Copyright:      (c) URA/Fermilab                                   */
/*                                                                    */
/**********************************************************************/


#ifndef TUNEDIAGRAM_H
#define TUNEDIAGRAM_H

#include <qvbox.h>

#include <VectorD.h>

class QLabel;
class QPixmap;

class TuneDiagram : public QVBox
{
 public:
  struct Pair
  {
    double x;
    double y;

    Pair() {}
    Pair( const Pair& u ) : x(u.x), y(u.y) {}
    ~Pair() {}
  };

  class ChalkBoard : public QWidget
  {
   public:
    ChalkBoard( QWidget* = 0 );
    ~ChalkBoard();
    void paintEvent( QPaintEvent* );
  };

 public:
  TuneDiagram( QWidget* = 0, const char* = 0, WFlags = 0 );
  TuneDiagram( const Vector&, int = 5, QWidget* = 0, const char* = 0, WFlags = 0 );
  TuneDiagram( const TuneDiagram& );
  ~TuneDiagram();

  void setSize(int,int);
  void setTunes( double, double );

  void draw();

 private:
  Vector  _tunes;
  double  _nx_lo;
  double  _nx_hi;
  double  _ny_lo;
  double  _ny_hi;
  int     _order;

  int     _border;

  QLabel*      _tuneLabelPtr;
  ChalkBoard*  _chalkBoardPtr;
  QPixmap*     _imagoPtr;

  void _finishConstructor();

  void _mark( double, double, QBrush* );
  void _drawLine( const Pair&, const Pair&, QPen* );

 public:
  const QPixmap* copyPtr() { return _imagoPtr; }
};

#endif // TUNEDIAGRAM_H
