/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF:      A Qt-based Application 
******             Layered on top of of BEAMLINE.
******                                                                
******  File:      interpreter.h
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

#ifndef INTERPRETER_H
#define INTERPRETER_H


extern "C" { 

int  testcomplete(char*);
}


#include <boost/python.hpp>


class IORedirector;

typedef IORedirector* IORedirectorPtr;

class Interpreter {
  
 public:

  static Interpreter*  getInstance();
  void runString(const char* s);
  void redirectIO( IORedirectorPtr& xstderr, IORedirectorPtr& xstdout);
  const char* getPythonPrompt1();
  const char* getPythonPrompt2();
  void readFile( const char* fname);

 private:

  Interpreter();
 ~Interpreter();
 

 private:
  
  boost::python::handle<>                  _main_module;
  boost::python::handle<>                  _main_dictionary;
  boost::python::handle<>                  _sys_module;
  std::auto_ptr< boost::python::handle<> > _pyrun_handle_ptr;

  static Interpreter* _instance;
    
};

#endif // INTERPRETER_H
