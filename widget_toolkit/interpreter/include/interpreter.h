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
