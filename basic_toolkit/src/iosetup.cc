#if HAVE_CONFIG_H
#include <config.h>
#endif

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
#include <iostream>
#include <iomanip>

namespace FNAL {

  std::ostream* pcout = &std::cout;
  std::ostream* pcerr = &std::cerr;

}

void iosetup(std::ostream* pcout, std::ostream* pcerr) {

  FNAL::pcout = pcout; 
  FNAL::pcerr = pcerr; 

}
