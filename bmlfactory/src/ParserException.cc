/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                       
******                                                
******
******  File:      ParserException.cc
******                                                                
******  Copyright (c) Universities Research Association, Inc.   
******                All Rights Reserved
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
******  Authors:   Jean-Francois Ostiguy
******                                                   
******  Contact:   Leo Michelotti or Jean-Francois Ostiguy            
******                                                                
******
******             Phone: (630) 840 4956                              
******                    (630) 840 2231                              
******             Email: michelotti@fnal.gov                         
******                    ostiguy@fnal.gov                            
******                                                                
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <bmlfactory/ParserException.h>

ParserException::ParserException( int inmemory, const char* filename, int lineno, const char* errmsg):
  _inmemory(inmemory), _filename(filename), _lineno(lineno), _errmsg(errmsg)
{ 

}

const char* 
ParserException::what() const throw()
{

  return _errmsg.c_str();

}
