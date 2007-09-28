/****************************************************************************
*****************************************************************************
*****************************************************************************
******
******  File:      MadxParserDriver.cc 
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
**********************************************************************************
**********************************************************************************
*********************************************************************************/

#include <iomanip>
#include "MadxParserDriver.h"
#include "MadxParser_ypp.hh"

using std::setw;
using std::left;

std::ostream& operator<<(std::ostream& os, values_map_t const& m) {

  values_map_t::const_iterator it;
  for ( it = m.begin(); it != m.end();  ++it) 
  {
    if ( it->second.type() == typeid(double) )
      os << setw(16) << left << it->first  << " = " <<  setw(16) <<  boost::any_cast<double>(it->second)           << std::endl;
    else 
      os << setw(16) << left << it->first  << " = " <<  setw(16) <<  boost::any_cast<parser_string_t>(it->second)  << std::endl;
  
  }

  return os;

}

std::ostream& operator<<(std::ostream& os, beamline_list_t const& bl) {

  beamline_list_t::const_iterator it;
  int elno = 0; 
  for ( it = bl.begin(); it != bl.end();  ++it) 
  {
    os << setw(16) << ++elno <<" : " << setw(16) << *it  << std::endl;
  }

  return os;

}

#if 0 
std::ostream& operator<<(std::ostream& os, elements_map_t const& em) {

  elements_map_t::const_iterator it;
    values_map_t::const_iterator itv;
 
  boost::shared_ptr<values_map_t> attributes_map_ptr;  
 
  for ( it = em.begin(); it != em.end();  ++it) 
  {
    os << it->first << " : " << std::endl;

      attributes_map_ptr = it->second;
      for ( itv =  attributes_map_ptr->begin(); itv != attributes_map_ptr->end();  ++itv) 
      if ( itv->second.type() == typeid(double) )
 	std::cout <<"          "<< setw(16) << itv->first << " = " << setw(16) << boost::any_cast<double>(itv->second)            << std::endl;
      else  
       	std::cout <<"          "<< setw(16) << itv->first << " = " << setw(16) <<  boost::any_cast<parser_string_t>(itv->second)  << std::endl;
  }
  return os;

}
#endif

std::ostream& operator<<(std::ostream& os, expr_map_t  const& xm) {

 
  for ( expr_map_t::const_iterator it = xm.begin(); it != xm.end();  ++it) 
  {
    os << setw(16) << it->first << " = " << it->second << std::endl;
  }
  return os;

}



MadxParserDriver::MadxParserDriver(): m_trace_scanning(false), m_trace_parsing(false)
{


  m_current_sequence_ptr =  new sequence_list_t(); 

  // Predefined constants in MADX

  m_constants["PI"]       = M_PI;          
  m_constants["TWOPI"]    = 2.0*M_PI;          
  m_constants["DEGRAD"]   = 180.0/M_PI;
  m_constants["RADDEG"]   = M_PI/180.0;
  m_constants["E"]        = std::exp(1.0);
  m_constants["EMASS"]    = 0.510998902e-3;     // electron mass in GeV
  m_constants["PMASS"]    = 0.938271998;        // proton mass in GeV
  m_constants["MUMASS"]   = 0.1056583568;       // muon mass in GeV
  m_constants["CLIGHT"]   = 2.99792458e8;       // speed of light in m/s
  m_constants["QELECT"]   = 1.602176462e-19;    // elementary charge  in A-s
   

}


MadxParserDriver::~MadxParserDriver() 
{

  madx_yylex_destroy(m_yyscanner);

}


int  
MadxParserDriver::parse(const std::string& f)
{
 
  m_file = f; 
  m_input_is_file = true;    

  madx_yylex_init(&m_yyscanner);

  yy::MadxParser parser(m_yyscanner, *this);

  parser.set_debug_level(m_trace_parsing);

  // clean the beamline m_beamline
  // clean              m_variables  

  scan_begin( m_yyscanner);

  int ret = parser.parse();

  scan_end(  m_yyscanner);
  
  return ret;

}

int  
MadxParserDriver::parseFromBuffer(const char* buffer)
{
 
  m_buffer = buffer; 
  m_input_is_file = false;    
   
  madx_yylex_init(&m_yyscanner);

  yy::MadxParser parser(m_yyscanner, *this);

  parser.set_debug_level(m_trace_parsing);

  // clean the beamline m_beamline
  // clean              m_variables  

  scan_begin( m_yyscanner);

  int ret = parser.parse();

  scan_end(  m_yyscanner);
  
  return ret;

}


void
MadxParserDriver::error( const yy::location& l, const std::string& m)
{

  std::cerr << l <<": "    << m << std::endl;

  yy::location loc;

  while( !m_locations_stack.empty() ) {

   loc = m_locations_stack.top();
   m_locations_stack.pop();

   std::cerr <<"Included from: " << loc << std::endl;

   // **** unwind the stack and free memory allocated for strings (filenames) **** 
     
   if ((loc.begin.filename == &m_file) || (loc.end.filename == &m_file) ) break; 

   if ( loc.begin.filename == loc.end.filename ) 
   { 
     delete (loc.begin.filename);
   }
   else
   {
     delete (loc.begin.filename);
     delete (loc.end.filename);
   }
  }

   // std::cerr << m_variables << std::endl;
 
} 


void
MadxParserDriver::error( const std::string& m)
{

  std::cerr << m << std::endl;

} 


#if 0 
void 
MadxParserDriver::defineElement(std::string const& name) 
{
  
 
  boost::shared_ptr<values_map_t> map_ptr( new values_map_t() );

  while (!m_attributes_stack.empty() ) {

  const value_t attribute = m_attributes_stack.top();    
  m_attributes_stack.pop();

   (*map_ptr)[ attribute.first ] =  attribute.second; 
 
  }

  m_elements[name] = map_ptr;

}
#endif
  

void 
MadxParserDriver::clearAttributes() 
{
  
  while (!m_attributes_stack.empty() ) m_attributes_stack.pop() ;

}
  

#if 0
void 
MadxParserDriver::addElement(std::string const& name)
{ 
  m_beamline.push_back(name); 

} 
#endif


void
MadxParserDriver::defineElement(std::string const& label, std::string const& type, bool shared) {

  
 
  std::cout << "Adding Element" << std::endl;
  std::cout << "Label      : " << label  << std::endl;    
  std::cout << "Type       : " << type   << std::endl;  
  std::cout << "Shared     : " << shared << std::endl;
  std::cout << "Attributes : " << std::endl;
 
  value_t value;
  while ( !m_attributes_stack.empty() ) {
   
    value = m_attributes_stack.top(); m_attributes_stack.pop();
    std::cout << "             " <<  value.first  << " => " << value.second  << std::endl;
    
  }
 
 
  if (label != std::string("") ) { 
    m_elements[label] =  type;    // need to verify that type is one of the basic types
                                  // or that "type" has already been defined.   
  } 
  else
    std::cout << "ERROR: Anonymous definition of an element outside of a sequence " << std::endl; 
}


void
MadxParserDriver::defineSequence(std::string const& label, std::string const& type, bool shared) {

  std::cout << "Defining SEQUENCE: " << label << std::cout;  
  m_elements[label] = type;

  m_current_sequence_ptr =  new sequence_list_t(); 

}

void
MadxParserDriver::addSequenceElement(std::string const& label, std::string const& type) {

  std::cout << "Adding SEQUENCE Element" << std::endl;
  std::cout << "Label      : " << label  << std::endl;    
  std::cout << "Type       : " << type   << std::endl;  
  std::cout << "Attributes : " << std::endl;
 
  value_t value;
  while ( !m_attributes_stack.empty() ) {
   
    value = m_attributes_stack.top(); m_attributes_stack.pop();
    std::cout << "             " <<  value.first  << " => " << value.second  << std::endl;
    
  }
  
  m_current_sequence_ptr->push_back( label );


}
