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



//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

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


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool 
PyCHEF_KeyPressFilter::eventFilter( QObject *o, QEvent *e ) 
{

  int index; 
  int para; 

  QTextEdit* tewidget =  dynamic_cast<QTextEdit*>(o);

  if (!tewidget) return false;  //standard event processing 

  tewidget->getCursorPosition ( &para, &index );
   
  int indexmin = strlen( w_->getPythonCurrentPrompt() );
  
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


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QPyCHEF::QPyCHEF(QWidget* parent, const char* name) : 
QPyCHEFBase(parent, name), indented_(false)  
{

  command_buffer_ = QString::null;
    
   initializePython();
   key_press_filter_ =   new  PyCHEF_KeyPressFilter( this );
   py_textEdit->installEventFilter(  key_press_filter_ );
   
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QPyCHEF::~QPyCHEF() 
{
    
  py_textEdit->removeEventFilter( key_press_filter_ );
  delete key_press_filter_;
 }


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void 
QPyCHEF::initializePython() 
{

// set the INT (Ctrl-C) signal handler to 'catch_int'
  signal(SIGINT, catch_int);

  interpreter_ = Interpreter::getInstance();

  py_textEdit->setTextFormat ( Qt::PlainText );
  connect( py_textEdit, SIGNAL( returnPressed(void) ), this, SLOT (sendCommand(void)) );

  interpreter_->runString("import ioredirector");

  interpreter_->runString("sys.ps1 = '>>> '");
  interpreter_->runString("sys.ps2 = '... '");
  interpreter_->runString("ior_stdout = ioredirector.IORedirector()");
  interpreter_->runString("ior_stderr = ioredirector.IORedirector()");
  interpreter_->runString("sys.stdout = ior_stdout");
  interpreter_->runString("sys.stderr = ior_stderr");

  IORedirector* ior_stdout_ptr = 0;
  IORedirector* ior_stderr_ptr = 0;
  
  interpreter_->redirectIO(ior_stderr_ptr, ior_stdout_ptr);

  connect( ior_stdout_ptr,  SIGNAL( textReady(QString) ), this, SLOT (append(QString)) );
  connect( ior_stderr_ptr,  SIGNAL( textReady(QString) ), this, SLOT (append_err(QString)) );

  interpreter_->runString("print 'PyCHEF (Python) %s on %s' % (sys.version, sys.platform)");
  append( interpreter_->getPythonPrompt1() );


}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const 
char* QPyCHEF::getPythonCurrentPrompt() {

  if (indented_ ) {
      return interpreter_->getPythonPrompt2();
  } else {
      return interpreter_->getPythonPrompt1();
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



void 
QPyCHEF::append(QString s) {

  py_textEdit->insert( s );  

  command_paragraph_  = py_textEdit->paragraphs()-1;
  command_index_      = py_textEdit->paragraphLength( command_paragraph_ );

  py_textEdit->setCursorPosition (command_paragraph_, command_index_ );


}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void QPyCHEF::append_err(QString s) {

 py_textEdit->setColor( QColor("red") );
  
 py_textEdit->insert( s );  

 command_paragraph_  = py_textEdit->paragraphs()-1;
 command_index_      = py_textEdit->paragraphLength(command_paragraph_);

 py_textEdit->setCursorPosition (command_paragraph_, command_index_);

 py_textEdit->setColor( QColor("black") );

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void QPyCHEF::sendCommand()
{

   py_textEdit->setSelection ( command_paragraph_, command_index_, command_paragraph_, 
                              py_textEdit->paragraphLength( command_paragraph_), 0);
  
   QString command = py_textEdit->selectedText();
   py_textEdit->removeSelection();

   py_textEdit->append("");
   command_paragraph_  = py_textEdit->paragraphs()-1;
   command_index_      = py_textEdit->paragraphLength( command_paragraph_);
   py_textEdit->setCursorPosition (command_paragraph_, command_index_);


   if ( noLeftTabs(command) == 0 ) {
         indented_ = false;
   } else {
         indented_ = true;
   };
   

   // first, take care of the case where the line is empty
   if ( command.stripWhiteSpace().length() == 0) 
   { 
         append( interpreter_->getPythonPrompt1() );
         return;
   };


   switch ( testcomplete( const_cast<char*>(( command.stripWhiteSpace() + QString("\n")).ascii() ) ) ) {

      case 0:
        command_queue_.push( command + QString("\n") );
        indented_ = true;
        break;
             
      case 1:
        command_queue_.push( command + QString("\n") );
        break;

      case -1:
        command_queue_.push( command + QString("\n") );
        indented_ = false;
        break;
    };

   if (indented_) {
        append( interpreter_->getPythonPrompt2() );
        return;
   }
   

   command_buffer_ = QString::null;

   while ( !command_queue_.empty() ) {
     
   command_buffer_ = command_buffer_ + command_queue_.front();
                     command_queue_.pop();
                     
   };

   
  try {
    interpreter_->runString(command_buffer_.ascii() );
  }

  catch (boost::python::error_already_set) {
   
       PyErr_Print( );
 
      command_paragraph_  = py_textEdit->paragraphs()-1;
      command_index_      = py_textEdit->paragraphLength( command_paragraph_);

      py_textEdit->setCursorPosition (command_paragraph_, command_index_);

     
  }

    command_buffer_ = QString::null;
    append( interpreter_->getPythonPrompt1() );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void QPyCHEF::readFile(const char* fname) 
{
  interpreter_->readFile(fname);
}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

