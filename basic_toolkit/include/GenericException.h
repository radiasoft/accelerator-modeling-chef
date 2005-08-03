/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      GenericException.h
******  Version:   1.0
******                                                                
******  Copyright (c) 2004 Universities Research Association, Inc.    
******                All Rights Reserved                             
******                                                                
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


#ifndef GENEXC_H
#define GENEXC_H

#include <exception>
#include <string>
#include <sstream>

class GenericException : public std::exception
{

 public:
  GenericException( std::string, int, const char* = "", const char* = "" );
  // Miscellaneous errors
  // 1st argument: name of file in which exception is thrown
  // 2nd         : line from which exception is thrown
  // 3rd         : identifies function containing throw
  // 4th         : identifies type of error

  ~GenericException() throw() {}
  const char* what() const throw();
  std::string errorString;
  int lineNumber() const throw();

 private:
  int _lineno;
};

#endif // GENEXC_H
