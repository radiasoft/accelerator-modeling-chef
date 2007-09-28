/****************************************************************************
*****************************************************************************
*****************************************************************************
******
******  Version:   1.0                    
******                                    
******  File:      parse_madx.cc 
******                                                                
******  Copyright (c) Universities Research Association, Inc./ Fermilab    
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
******  Author:   Jean-Francois Ostiguy, ostiguy@fnal.gov                                      
******            Fermilab                                           
******
******
**********************************************************************************
**********************************************************************************
*********************************************************************************/
// --------------------------------------------------------------------
// A driver program for testing the Madx parser  
// --------------------------------------------------------------------


#include <iostream>
#include <MadxParserDriver.h>


//=====================================================================================

int main(int argc, const char* argv[] ) { 
 
 
 if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " Madx_input_file"  << std::endl;  
    return 1;
 };

 MadxParserDriver parser;

 parser.m_trace_scanning = false;
 parser.m_trace_parsing  = false;

 //parser.m_trace_scanning = true;
 //parser.m_trace_parsing  = true;


 if (  parser.parse( argv[1] ) != 0 )
  return 1;


 std::cout << "------------------------------- " << std::endl;
 std::cout << " **** End of parsing phase **** "   << std::endl;
 std::cout << "------------------------------- " << std::endl;

 std::cout << "-------------------" << std::endl; 
 std::cout << "Constants          " << std::endl; 
 std::cout << "-------------------" << std::endl; 

 std::cout << parser.m_constants    << std::endl;

 std::cout << "-------------------" << std::endl; 
 std::cout << "Variables          " << std::endl; 
 std::cout << "-------------------" << std::endl; 

 std::cout << parser.m_variables    << std::endl;

 std::cout << "-------------------" << std::endl; 
 std::cout << "elements          "  << std::endl; 
 std::cout << "-------------------" << std::endl; 

 std::cout << parser.m_elements     << std::endl;
 for (elements_map_t::iterator it  = parser.m_elements.begin();
                               it != parser.m_elements.end(); ++it) {

   std::cout << "Element label   : " << it->first << "       Element type : " <<  boost::any_cast<std::string>(it->second) << std::endl; 
 }                 


 return 0;  

}

