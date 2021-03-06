/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  XSIF Parser:  Interprets XSIF input files and             
******                creates instances of class beamline.                       
******                                                
******                                    
******  File:      XSIFFactory.cc
******                                                                
******  Copyright (c) Universities Research Association, Inc. / Fermilab  
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
******                                                                
******  Author:    Jean-Francois Ostiguy      
******             ostiguy@fnal.gov
******                                                                
******
**************************************************************************
*************************************************************************/

#include <parsers/xsif/XSIFFactory.h>
#include <map>
#include <string>

using namespace std;


XSIFFactory::XSIFFactory( string filename, double brho, const char* stringbuffer)
  : bmlfactory(filename, brho, stringbuffer)
{



 driver_.m_trace_scanning = false;
 driver_.m_trace_parsing  = false;


 driver_.parse( filename ); 


}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


XSIFFactory::XSIFFactory( string filename, const char* stringbuffer)
  : bmlfactory(filename, stringbuffer)
 
{

 driver_.m_trace_scanning = false;
 driver_.m_trace_parsing  = false;

 driver_.parse( filename ); 
 
 // *** uncomment for debugging *** 
 // dumpVariables();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


XSIFFactory::~XSIFFactory() 
{

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BmlPtr XSIFFactory::create_beamline( string bmlname,  double brho) 
{

 return driver_.getLine(bmlname); 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BmlPtr XSIFFactory::create_beamline( string bmlname)  
{

 return driver_.getLine(bmlname); 

}        

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::list<std::string>   XSIFFactory::getBeamlineList()
{

  list<string> bml_list;

  for ( std::map<std::string, BmlPtr>::iterator it  = driver_.m_lines.begin(); it != driver_.m_lines.end(); ++it) {
     bml_list.push_back(it->first);
  }

  return  bml_list;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* XSIFFactory::getUseStatementBeamlineName() const
{           

  return 0;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* XSIFFactory::getParticleType() const 
{

  return driver_.m_particle_type_name.c_str();


}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool    XSIFFactory::variableIsDefined(const char* varname) const
{

   map<string, Expression>::const_iterator it_var;

   return ( (it_var = driver_.m_variables.find( string(varname) )) != driver_.m_variables.end() );

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double  XSIFFactory::getVariableValue(const char* varname) const
{
  
  map<string,Expression>::const_iterator it;

  if  ( variableIsDefined(varname) ){  

           it = driver_.m_variables.find(string(varname));
           return it->second.evaluate();

  } 
  else {
   return 0.0; // should throw exception here 
  }

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double XSIFFactory::getMomentum() const
{

  return driver_.m_momentum;


}//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double XSIFFactory::getBrho() const
{

  return driver_.m_BRHO;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void XSIFFactory::dumpVariables() const
{

 for ( expr_map_t::const_iterator it  = driver_.m_variables.begin();   
                                  it != driver_.m_variables.end(); ++it ) {
   std::cout.precision(14);
   std::cout << it->first << " = " << std::scientific << it->second.evaluate() << std::endl; 
 }

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CSLattFuncs XSIFFactory::getInitialValues()  const 
{   
  return driver_.initial_values_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
