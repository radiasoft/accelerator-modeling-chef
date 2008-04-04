
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


  char const* getPythonCurrentPrompt();

  public slots:

     virtual void sendCommand( );
     virtual void append(QString s);
     virtual void append_err(QString s);
     virtual void readFile(const char* fname);     
   
  private:

   
   void          initializePython();

   Interpreter*           interpreter_;
   int                    command_paragraph_;
   int                    command_index_;
   bool                   indented_;
  
   QString                command_buffer_;
   std::queue<QString>    command_queue_;

   PyCHEF_KeyPressFilter* key_press_filter_;
  

};




class PyCHEF_KeyPressFilter : public QObject
{

  public:
 
    PyCHEF_KeyPressFilter(QPyCHEF* w): w_(w) { }
  
   
  protected:

     bool eventFilter( QObject *o, QEvent *e );

     QPyCHEF *w_;
        
};

  
#endif


