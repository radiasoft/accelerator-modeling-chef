#ifndef QPYEDIT_H
#define QPYCHEF_H

#include <queue>
#include <qpychefbase.h>

class Interpreter;
class PyCHEF_KeyPressFilter;


class QPyCHEF : public QPyCHEFBase {

  Q_OBJECT
    
  public:

  QPyCHEF(QWidget* p, const char* text);
  ~QPyCHEF();


  const char* getPythonCurrentPrompt();

  public slots:

     virtual void sendCommand( );
     virtual void append(QString s);
     virtual void append_err(QString s);
     virtual void readFile(const char* fname);     
   
  private:

   
   void          initializePython();

   Interpreter*  _interpreter;
   int           _command_paragraph;
   int           _command_index;
   bool          _indented;
  
   QString             _command_buffer;
   std::queue<QString> _command_queue;

   PyCHEF_KeyPressFilter* _key_press_filter;
  

};




class PyCHEF_KeyPressFilter : public QObject
{

  public:
 
  PyCHEF_KeyPressFilter(QPyCHEF* w) :_w(w) { }
  
   
   protected:

     bool eventFilter( QObject *o, QEvent *e );
     QPyCHEF *_w;
        
};

  
#endif


