#ifndef FILTERS_H
#define FILTERS_H

#include <qobject.h>

class WindowCloseEventEater: public QObject
{

Q_OBJECT

protected:
  bool eventFilter( QObject *o, QEvent *e);

signals:

 void toggled();
};
 
#endif
  
