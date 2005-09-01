/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  bmlfactory: mad8 language parsing
******                                    
******  File:      ParserException.h
******  Version:   1.0
******                                                                
******  Copyright (c) Universities Research Association, Inc.    
******                All Rights Reserved 
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
******  Author:    Jean-Francois Ostiguy                                     
******                                                                
******             Phone: (630) 840 2231                              
******             Email: ostiguy@fnal.gov                         
******                                                                
******                                                                
**************************************************************************
*************************************************************************/

#ifndef PARSEREXCEPT_H
#define PARSEREXCEPT_H

#include <exception>
#include <string>
#include <sstream>

class ParserException : public std::exception
{

 public:

  ParserException( int inmemory, const char* filename, int lineno, const char* errmsg = 0);
  ~ParserException() throw() {}

  const char* what() const throw();

  const int    _lineno;
  std::string  _filename;
  std::string  _errmsg;
  const int    _inmemory;
};

#endif // PARSEREXCEPT_H
