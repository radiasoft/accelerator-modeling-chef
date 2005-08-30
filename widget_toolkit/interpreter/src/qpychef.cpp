/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF:      A Qt-based Application 
******             Layered on top of of BEAMLINE.
******                                                                
******  File:      qpychef.cpp
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

#include <qpychef.h>

#include <qtextedit.h>

#include "qtmacrosoff.h"
#include <interpreter.h>
#include "qtmacroson.h"
#include <ioredirector.h>

#include <queue>
#include <iostream>
#include <stdio.h>


#include <cstdio>       /* standard I/O functions                         */
#include <unistd.h>     /* standard unix functions, like getpid()         */
#include <sys/types.h>  /* various type definitions, like pid_t           */
#include <signal.h>     /* signal name macros, and the signal() prototype */




/* first, here is the signal handler */
void 
catch_int(int sig_num)
{
    /* re-s   the signal handler again to catch_int, for next time */
    signal(SIGINT, catch_int);
    /* and print the message */
    Py_Finalize(); //  top the interpreter and release its resources.
    exit(1);
}


  int noLeftTabs(QString& s) {

    int l = s.length();
    int result = 0;
  
  
    for (int i=0; i<l; i++) {
  
     if ( s[i] == QChar( 9 ) ) result++; 
     if ( s[i].isPrint() ) break;
    
   };
  
   return result;
  }


bool 
PyCHEF_KeyPressFilter::eventFilter( QObject *o, QEvent *e ) 
{

  int index; 
  int para; 

  QTextEdit* tewidget =  dynamic_cast<QTextEdit*>(o);

  if (!tewidget) return false;  //standard event processing 

  tewidget->getCursorPosition ( &para, &index );
   
  int indexmin = strlen( _w->getPythonCurrentPrompt() );
  
        if ( e->type() == QEvent::KeyPress ) {
            // special processing for key press
            QKeyEvent *k = (QKeyEvent *)e;
            switch (k->key()) {
            case Qt::Key_Left  : 
              if ( index == indexmin) 
                 return true;
              else
                 return false;
              break;
          case Qt::Key_Right :
               return false;
               break;
           case Qt::Key_Backspace  :
              if ( index == indexmin) 
                 return true;
              else
                 return false;
               break;
           case Qt::Key_Up    :
               return true;
               break;
           case Qt::Key_Down  :
               return true;
               break;

           default: return false;
           }

        } else {
            // standard event processing
            return false;
        }
}


QPyCHEF::QPyCHEF(QWidget* parent, const char* name) : 
QPyCHEFBase(parent, name), _indented(false)  
{

  _command_buffer = QString::null;
    
   initializePython();
  _key_press_filter =   new  PyCHEF_KeyPressFilter( this );
   py_textEdit->installEventFilter(  _key_press_filter );
   
}

QPyCHEF::~QPyCHEF() 
{
    
  py_textEdit->removeEventFilter( _key_press_filter );
  delete _key_press_filter;
 }


void 
QPyCHEF::initializePython() 
{

// set the INT (Ctrl-C) signal handler to 'catch_int'
  signal(SIGINT, catch_int);

  _interpreter = Interpreter::getInstance();

  py_textEdit->setTextFormat ( Qt::PlainText );
  connect( py_textEdit, SIGNAL( returnPressed(void) ), this, SLOT (sendCommand(void)) );

  _interpreter->runString("import ioredirector");

  _interpreter->runString("sys.ps1 = '>>> '");
  _interpreter->runString("sys.ps2 = '... '");
  _interpreter->runString("ior_stdout = ioredirector.IORedirector()");
  _interpreter->runString("ior_stderr = ioredirector.IORedirector()");
  _interpreter->runString("sys.stdout = ior_stdout");
  _interpreter->runString("sys.stderr = ior_stderr");

  IORedirector* ior_stdout_ptr = 0;
  IORedirector* ior_stderr_ptr = 0;
  
  _interpreter->redirectIO(ior_stderr_ptr, ior_stdout_ptr);

  connect( ior_stdout_ptr,  SIGNAL( textReady(QString) ), this, SLOT (append(QString)) );
  connect( ior_stderr_ptr,  SIGNAL( textReady(QString) ), this, SLOT (append_err(QString)) );

  _interpreter->runString("print 'PyCHEF (Python) %s on %s' % (sys.version, sys.platform)");
  append( _interpreter->getPythonPrompt1() );


}

const 
char* QPyCHEF::getPythonCurrentPrompt() {

  if (_indented ) {
      return _interpreter->getPythonPrompt2();
  } else {
      return _interpreter->getPythonPrompt1();
  }
}



void 
QPyCHEF::append(QString s) {

  py_textEdit->insert( s );  

  _command_paragraph  = py_textEdit->paragraphs()-1;
  _command_index      = py_textEdit->paragraphLength(_command_paragraph);

  py_textEdit->setCursorPosition (_command_paragraph, _command_index);


}

void QPyCHEF::append_err(QString s) {

 py_textEdit->setColor( QColor("red") );
  
 py_textEdit->insert( s );  

 _command_paragraph  = py_textEdit->paragraphs()-1;
 _command_index      = py_textEdit->paragraphLength(_command_paragraph);

 py_textEdit->setCursorPosition (_command_paragraph, _command_index);

 py_textEdit->setColor( QColor("black") );

}


void 
QPyCHEF::sendCommand()
{

   py_textEdit->setSelection ( _command_paragraph, _command_index, _command_paragraph, 
                              py_textEdit->paragraphLength(_command_paragraph), 0);
  
   QString command = py_textEdit->selectedText();
   py_textEdit->removeSelection();

   py_textEdit->append("");
   _command_paragraph  = py_textEdit->paragraphs()-1;
   _command_index      = py_textEdit->paragraphLength(_command_paragraph);
   py_textEdit->setCursorPosition (_command_paragraph, _command_index);


   if ( noLeftTabs(command) == 0 ) {
         _indented = false;
   } else {
         _indented = true;
   };
   

   // first, take care of the case where the line is empty
   if ( command.stripWhiteSpace().length() == 0) 
   { 
         append( _interpreter->getPythonPrompt1() );
         return;
   };


   switch ( testcomplete( const_cast<char*>(( command.stripWhiteSpace() + QString("\n")).ascii() ) ) ) {

      case 0:
        _command_queue.push( command + QString("\n") );
       _indented = true;
        break;
             
      case 1:
        _command_queue.push( command + QString("\n") );
        break;

      case -1:
        _command_queue.push( command + QString("\n") );
        _indented = false;
        break;
    };

   if (_indented) {
        append( _interpreter->getPythonPrompt2() );
        return;
   }
   

   _command_buffer = QString::null;

   while ( !_command_queue.empty() ) {
     
   _command_buffer = _command_buffer + _command_queue.front();
                     _command_queue.pop();
                     
   };


   
  try {
      
    _interpreter->runString(_command_buffer.ascii() );

    
  }
  catch (boost::python::error_already_set) {
   
       PyErr_Print( );
 
      _command_paragraph  = py_textEdit->paragraphs()-1;
      _command_index      = py_textEdit->paragraphLength(_command_paragraph);

      py_textEdit->setCursorPosition (_command_paragraph, _command_index);

     
  }

    _command_buffer = QString::null;
    append( _interpreter->getPythonPrompt1() );


  
}


void 
QPyCHEF::readFile(const char* fname) 
{


  _interpreter->readFile(fname);

}
