/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                 
******                                                
******  File:      bmlfactory_exit.cc
******                                                                
******  Copyright (c) 2004  Universities Research Association, Inc.   
******                All Rights Reserved                             
******                                                                
******  Authors:    Leo Michelotti
******              Jean-Francois Ostiguy
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******                    (630) 840 2231                              
******             Email: michelotti@fnal.gov                         
******                    ostiguy@fnal.gov                            
******                                                                
**************************************************************************
*************************************************************************/

// This is a single special purpose file 
// to enable a primitive C++ exception handling,
// using an exception class that inherits from std::exception,
// in the MAD parser C files.

// Note (JFO): When this function is called from C, the stack of the C-caller will 
// *not* be unwound correctly, unless the compiler supports it. Unfortunately, the 
// C/C++ standards are unclear about what should happen when a C function calls a C++ 
// function that throws. With GNU gcc, everything works correctly *only* if the C 
// code is compiled with the -fexceptions flag.   

#include "GenericException.h"
#include <strstream>

extern "C" 
{

  void bmlfactory_exit(const char* filename, int lineno, const char* errmessage);
  void bmlfactory_parse_error(const char* input_filename, int input_file_lineno, const char* parser_message);

}

void 
bmlfactory_exit(const char* filename, int lineno, const char* errmessage) 
{

  throw( GenericException( filename, lineno, 
      "void bmlfactory_exit()", errmessage) );
}

void 
bmlfactory_parse_error(const char* input_filename, int input_file_lineno, const char* parser_message) 
{
  
  std::stringstream errs;
  errs <<" The MAD parser encountered an error while parsing line ";
  errs <<  input_file_lineno << std::endl;
  errs <<" in file " << input_filename << std::endl;
  errs <<" The error message was: " <<  parser_message << std::ends;   

  throw( GenericException( __FILE__, input_file_lineno, errs.str().c_str()) );
}



