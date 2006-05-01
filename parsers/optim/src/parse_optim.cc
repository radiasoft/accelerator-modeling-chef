/****************************************************************************
*****************************************************************************
*****************************************************************************
******
******  Version:   1.0                    
******                                    
******  File:      parse_optim.cc 
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
// A driver program for testing the OptiM parser  
// --------------------------------------------------------------------


#include <iostream>
#include <OptimParserDriver.h>


int main(int argc, const char* argv[] ) { 
 
 
 if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " OptiM_input_file"  << std::endl;  
    return 1;
 };

 OptimParserDriver parser;

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
 std::cout << "Variables          " << std::endl; 
 std::cout << "-------------------" << std::endl; 

 std::cout << parser.m_variables << std::endl;

 std::cout << "-------------------" << std::endl; 
 std::cout << "General Attributes " << std::endl; 
 std::cout << "-------------------" << std::endl; 

 std::cout << parser.m_general_attributes << std::endl;
 
 std::cout << "----------------------------- " << std::endl; 
 std::cout << "Initial Conditions Attributes " << std::endl; 
 std::cout << "------------------------------" << std::endl; 

 std::cout <<  parser.m_initial_conds_attributes << std::endl;

 std::cout << "---------------------" << std::endl; 
 std::cout << "Emittance Attributes " << std::endl; 
 std::cout << "---------------------" << std::endl; 

 std::cout <<  parser.m_emittance_attributes << std::endl;

 std::cout << "---------------------" << std::endl; 
 std::cout << "Twiss Fit Attributes " << std::endl; 
 std::cout << "----------- ---------" << std::endl; 

 std::cout << parser.m_twiss_fit_attributes << std::endl;

 std::cout << "-------------------" << std::endl; 
 std::cout << "4D Fit Attributes "  << std::endl; 
 std::cout << "-------------------" << std::endl; 

 std::cout << parser.m_4d_fit_attributes   << std::endl;

 std::cout << "----------------------- "  << std::endl; 
 std::cout << "Space Charge Attributes "  << std::endl; 
 std::cout << "----------------------- "  << std::endl; 

 std::cout << parser.m_space_charge_attributes<< std::endl;


 std::cout << "-------------------" << std::endl; 
 std::cout << "Elements           " << std::endl; 
 std::cout << "-------------------" << std::endl; 

 std::cout << parser.m_elements     << std::endl;

 std::cout << "-------------------" << std::endl; 
 std::cout << "Beamline           " << std::endl; 
 std::cout << "-------------------" << std::endl; 

 std::cout << parser.m_beamline     << std::endl;

 return 0;  

}

