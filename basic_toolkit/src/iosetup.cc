/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   1.0                    
******                                    
******  File:      iosetup.cc
******                                                                
******  Copyright (c) Universities Research Association, Inc.    
******                All Rights Reserved                             
******
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******                                                                
******  Author:    Jean-Francois Ostiguy                                     
******             ostiguy@fnal.gov                         
******                                                                
******                                                                
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <iomanip>
#include <cstdarg>

namespace FNAL {

  std::ostream* pcout = &std::cout;
  std::ostream* pcerr = &std::cerr;

}

void iosetup(std::ostream* pcout, std::ostream* pcerr) {

  FNAL::pcout = pcout; 
  FNAL::pcerr = pcerr; 

}

//  The following functions are meant to emulate the behavior of fprintf. 
//  This provide a convenient way to redirect output from code written 
//  in C without having to modify all the format statements.    
//  Note that the emulation is probably not perfect.   

extern "C" {

void send_to_stderr_stream(FILE* file, const char* fmt, ...) {

#if 0
  double arg_double[32];
  double arg_int[32];
  double arg_str[32];

  enum arg_type {int_type, double_type, string_type};  

  va_list ap;
  va_start(ap, fmt);

  switch (arg_type) {

  case  int_type: 
        int_argva_arg(ap, int) 
	break;

  case  double_type: 
        va_arg(ap, (const char*)) 
	break;

  case  string_type: 
        va_arg(ap, (const char*)) 
	break;


  va_end(ap);
  
  // (*FNAL::pcerr) << msg << std::ends;

#endif

}  

void send_to_stdout_stream(FILE* file, const char* fmt, ...) {

  // (*FNAL::pcout) << msg << std::ends;

}

} // extern "C"

