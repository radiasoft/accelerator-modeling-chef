
/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF:      A Qt-based Application 
******             Layered on top of of BEAMLINE.
******                                                                
******  File:      qpychef.h
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


