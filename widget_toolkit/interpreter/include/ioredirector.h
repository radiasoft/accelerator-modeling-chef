#ifndef IOREDIRECTOR_H
#define IOREDIRECTOR_H


#include <qstring.h>
#include <qobject.h>

class IORedirector: public QObject {
    
  Q_OBJECT
  
  public:

  IORedirector();
    
  void     read(  const char* s);
  void     write( const char* s);
  int     _softspace;

  signals:

  void textReady( QString s );
  
  
  
};

#endif // IOREDIRECTOR_H
