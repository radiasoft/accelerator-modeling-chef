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

  const QPixmap* copyPtr() { return imagoPtr_; }

 private:

  void finishConstructor();
  void mark( double, double, QBrush* );
  void drawLine( const Pair&, const Pair&, QPen* );

  Vector  tunes_;
  double  nx_lo_;
  double  nx_hi_;
  double  ny_lo_;
  double  ny_hi_;
  int     order_;

  int     border_;

  QLabel*      tuneLabelPtr_;
  ChalkBoard*  chalkBoardPtr_;
  QPixmap*     imagoPtr_;


};

#endif // TUNEDIAGRAM_H
