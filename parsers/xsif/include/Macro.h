/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  XSIF Parser:  Interprets XSIF input files and instantiate 
******                beamline objects.                 
******                                                
******  File:      Macro.h
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
******  Author:  Jean-Francois Ostiguy
******           ostiguy@fnal.gov 
******
**************************************************************************
**************************************************************************/
#ifndef MACRO_H
#define MACRO_H

#include <string>
#include <vector>


struct Macro {

  std::string              m_name; 
  std::vector<std::string> m_args; 
  std::string fullName()  { std::string name( m_name); 
                           name += std::string("("); 
                           for ( std::vector<std::string>::iterator it = m_args.begin(); it != m_args.end(); ++it) name += *it; 
                           name += std::string(")");
                           return name;
                           }
};

#endif
