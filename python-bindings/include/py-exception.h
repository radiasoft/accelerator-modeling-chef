#include <exception>
#include <string>

class PyBindingsException: public std::exception 
{

 public:

  PyBindingsException( const std::string& whatstring ): _whatstring(whatstring) {}
  ~PyBindingsException() throw() {};

  virtual const char* what() const throw() 
    {
      return _whatstring.c_str();
    }
 private:

  std::string _whatstring;
 
};   
