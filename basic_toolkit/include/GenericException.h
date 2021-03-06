/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      GenericException.h
******  Version:   1.0
******                                                                
******  Copyright  Universities Research Association, Inc./Fermilab    
******             All Rights Reserved                             
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
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws.
******                                      
******                                                                
**************************************************************************
*************************************************************************/

#ifndef GENEXC_H
#define GENEXC_H

#include <basic_toolkit/globaldefs.h>
#include <boost/exception.hpp>

#include <exception>
#include <string>
#include <sstream>

struct exception_base: virtual std::exception, virtual boost::exception { };

class DLLEXPORT GenericException : virtual public exception_base
{

 public:
  GenericException( std::string, int, std::string const& = "", std::string const& = "" );
  // Miscellaneous errors
  // 1st argument: name of file in which exception is thrown
  // 2nd         : line from which exception is thrown
  // 3rd         : identifies function containing throw
  // 4th         : identifies type of error

 ~GenericException()                 throw() {}

  char const*     what()       const throw();
  std::string     errorString;
  int             lineNumber() const throw();

 private:

  int lineno_;

};

#endif // GENEXC_H
