/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF:      A Qt-based Application 
******             Layered on top of of BEAMLINE.
******                                                                
******  File:      interpreter.cpp
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

#include <interpreter.h>
#include <ioredirector.h>
#include <qfileinfo.h>
#include <qdir.h>

#include <iostream>
#include <stdio.h>

#include <Jet.h>
#include <JetC.h>
#include <GenericException.h>

using namespace boost::python;

extern "C" { 
void initioredirector(); 
void initbasic_toolkit();
void initmxyzptlk();
void initbeamline();
void initphysics_toolkit();
void initbmlfactory();
void initchefplot();
void initchef();
int  testcomplete(char*);
  
}

// Start the interpreter and initialize resident extensions 
// Note: inintx() is called automatically for dynamically loaded modules.
// We need to explicitly call initx because the modules are statically linked.


Interpreter* Interpreter::_instance = 0;


Interpreter::Interpreter() 

{

   Py_Initialize();
   initioredirector();
   initbasic_toolkit();
   initmxyzptlk();
   initbeamline();
   initphysics_toolkit();
   initbmlfactory();
   initchefplot();
   // initchef() ***DISABLED ****
// Call Python C API routines to use the interpreter.

   _main_module     = boost::python::handle<> ( boost::python::borrowed( PyImport_AddModule("__main__") ) );
   _main_dictionary = boost::python::handle<> ( boost::python::borrowed( PyModule_GetDict(_main_module.get()) ) );
   _sys_module      = boost::python::handle<> ( boost::python::borrowed( PyImport_AddModule("sys") ) );

   runString("from math import *");
   runString("import os,sys");
   runString("import code");
   runString("import  basic_toolkit");
   runString("import  mxyzptlk");
   runString("import  beamline");
   runString("import  physics_toolkit");
   runString("import  bmlfactory");
   runString("import  chefplot");
   /// runString("import  chef"); ***DISABLED***
   runString("sys.path += '.'");
}


Interpreter::~Interpreter() 
{

  Py_Finalize(); //  terminate the interpreter and release its resources.
  if (_instance) delete _instance;
  _instance = 0; 
} 


void Interpreter::runString(const char* s) 
{

  // save Jet environment ( a script can change the environment !)

  Jet__environment  * storedEnv    =   Jet::_lastEnv;
  JetC__environment * storedEnvC   =   JetC::_lastEnv; 

  
  //  int Py_file_input 
  //  int Py_single_input:  This is the symbol used for the interactive interpreter loop
  try { 
  boost::python::handle<> pyrun_handle( PyRun_String( s , Py_single_input,
             _main_dictionary.get(), _main_dictionary.get() ));
  }
  catch(std::exception& e)
  {
    std::cout << e.what() <<std::endl;
    throw e;
  }
  
 // restore Jet environment

  Jet::_lastEnv = storedEnv;
  JetC::_lastEnv = storedEnvC;
} 


void Interpreter::redirectIO( IORedirector*& ior_stderr_ptr, IORedirector*& ior_stdout_ptr) 
{

 
  object        ior_stdout_obj  = object( _sys_module ).attr("stdout");
  ior_stdout_ptr  = extract<IORedirector*>(ior_stdout_obj);

  object        ior_stderr_obj  = object( _sys_module ).attr("stderr");
  ior_stderr_ptr  = extract<IORedirector*>(ior_stderr_obj);

}

Interpreter* Interpreter::getInstance() 
{

  if (!_instance) _instance = new Interpreter();

  return _instance;

} 

const char* Interpreter::getPythonPrompt1() { 
  return extract<const char*>( object( _sys_module ).attr("ps1") );
}



const char* Interpreter::getPythonPrompt2() {
  return extract<const char*>( object( _sys_module ).attr("ps2") );
}


void Interpreter::readFile(const char* fname) 
{

  QFileInfo qfinfo(fname);  

  if ( !qfinfo.exists() )
    throw GenericException(__FILE__, __LINE__, "Interpreter::readFile(const char* fname)", "File not found.");

  QString   qdirpath_old = QDir::currentDirPath(); 
  QDir::setCurrent( qfinfo.dir().path() );

  FILE* fp = fopen(fname, "r");
  PyRun_SimpleFile(fp, fname); 
   
  QDir::setCurrent(qdirpath_old);
  fclose(fp);

}
