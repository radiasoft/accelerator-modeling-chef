/****************************************************************************
*****************************************************************************
*****************************************************************************
******
******  File:      OptimParserDriver.cc 
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
#include "OptimParserDriver.h"
#include "OptimParser_ypp.hh"

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



OptimParserDriver::OptimParserDriver(): m_trace_scanning(false), m_trace_parsing(false) 
{
  m_variables["$PI"]      = M_PI;          
  m_variables["$pi"]      = M_PI;          
  m_variables["$e"]       = M_E;           
  m_variables["$Me"]      = 0.511006;      // electron mass  [MeV/c**2]
  m_variables["$Mp"]      = 938.2723;      // proton mass    [MeV/c**2]
  m_variables["$Mmu"]     = 105.6;         // muon   mass    [MeV/c**2]
  m_variables["$c"]       = 2.99792458e10; // speed of light [cm/s]
  // m_variables["$_turn"]   = 0;  /** ignored for the moment **/           
}


OptimParserDriver::~OptimParserDriver() 
{

  optim_yylex_destroy(m_yyscanner);

}


int  
OptimParserDriver::parse(const std::string& f)
{
 
  m_file = f; 
  m_input_is_file = true;    

  optim_yylex_init(&m_yyscanner);

  yy::OptimParser parser(m_yyscanner, *this);

  parser.set_debug_level(m_trace_parsing);

  // clean the beamline m_beamline
  // clean              m_variables  

  scan_begin( m_yyscanner);

  int ret = parser.parse();

  scan_end(  m_yyscanner);
  
  return ret;

}

int  
OptimParserDriver::parseFromBuffer(const char* buffer)
{
 
  m_buffer = buffer; 
  m_input_is_file = false;    
   
  optim_yylex_init(&m_yyscanner);

  yy::OptimParser parser(m_yyscanner, *this);

  parser.set_debug_level(m_trace_parsing);

  // clean the beamline m_beamline
  // clean              m_variables  

  scan_begin( m_yyscanner);

  int ret = parser.parse();

  scan_end(  m_yyscanner);
  
  return ret;

}


void
OptimParserDriver::error( const yy::location& l, const std::string& m)
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
OptimParserDriver::error( const std::string& m)
{

  std::cerr << m << std::endl;

} 


void 
OptimParserDriver::defineElement(std::string const& name) 
{
  
 
  boost::shared_ptr<values_map_t> map_ptr( new values_map_t() );

  while (!m_attributes_stack.empty() ) {

  const value_t attribute = m_attributes_stack.top();    
  m_attributes_stack.pop();

   (*map_ptr)[ attribute.first ] =  attribute.second; 
 
  }

  m_elements[name] = map_ptr;

}
  

void 
OptimParserDriver::clearAttributes() 
{
  
  while (!m_attributes_stack.empty() ) m_attributes_stack.pop() ;

}
  

void 
OptimParserDriver::addElement(std::string const& name)
{ 
/* A:  RF cavity(PI standing wave)                                                            */
/* B:  combined function sector magnet                                                        */
/* C:  C-solenoid, CC-only focusing part for solenoid                                         */
/* D:       combined functiom sector magnet                                                   */
/* E:  electrostatic acc.section                                                              */
/* F:  Lithium lens                                                                            */
/* G:  Dipole edge                                                                            */
/* H:  Halo scraper (aperture limitation)                                                     */
/* I:  Instrumentation, Marker                                                                */
/* J:                                                                                         */
/* K : corrector                                                                              */
/* L:  electrostatic quad                                                                     */
/* M:       multipole(length=0)                                                               */
/* N:                                                                                         */
/* O:       free space                                                                        */
/* P                                                                                          */
/* Q:       quad                                                                              */
/* R:       combined magneto-electrostatic bend                                               */
/* S:       sextupole                                                                         */
/* T:  Transverse particle scattering in the matter                                           */
/* U:  Longitudinal particle scattering in the matter                                         */
/* V:                                                                                         */
/* W:  standing wave RF cavity with numerical integration of particle motion                  */
/* X:  transfer matrix (next 6 lines have to hold the matrix itself)                          */
/* Y:                                                                                         */
/* Z:  longitudinal corrector                                                                 */


#if 0
===================================================================================================
  char etype = name[0];

  switch ( toupper(etype) ) { 
   
   case 'A':  std::cout << "adding RF cavity"   << std::endl; 
             break;
   case 'B':  std::cout << "adding CF s-bend"   << std::endl; 
             break;
   case 'C':  std::cout << "adding solenoid"    << std::endl; 
             break;
   case 'D':  std::cout << "adding CF s-bend"   << std::endl; 
             break;
   case 'E':  std::cout << "adding Electrostatic Acc" << std::endl; 
             break;
   case 'F':  std::cout << "adding Lithium lens" << std::endl; 
             break;
   case 'G':  std::cout << "adding Dipole edge" << std::endl; 
             break;
   case 'H':  std::cout << "adding Halo scraper" << std::endl; 
             break;
   case 'I':  std::cout << "adding Marker" << std::endl; 
             break;
   case 'J':  std::cout << "adding Unused" << std::endl; 
             break;
   case 'K':  std::cout << "adding Corrector" << std::endl; 
             break;
   case 'L':  std::cout << "adding electrostatic quad" << std::endl; 
             break;
   case 'M':  std::cout << "adding thin multipole" << std::endl; 
             break;
   case 'N':  std::cout << "adding Unused" << std::endl; 
             break;
   case 'O':  std::cout << "adding drift" << std::endl; 
             break;
   case 'P':  std::cout << "adding Unused" << std::endl; 
             break;
   case 'Q':  std::cout << "adding quadrupole" << std::endl; 
             break;
   case 'R':  std::cout << "adding magnetic/electric bend" << std::endl; 
             break;
   case 'S':  std::cout << "adding sextupole" << std::endl; 
             break;
   case 'T':  std::cout << "adding transverse particle scattering" << std::endl; 
             break;
   case 'U':  std::cout << "adding long particle scattering" << std::endl; 
             break;
   case 'V':  std::cout << "adding Unused" << std::endl; 
             break;
   case 'W':  std::cout << "adding Standing wave RF cav" << std::endl; 
             break;
   case 'X':  std::cout << "adding transfer matrix" << std::endl; 
             break;
   case 'Y':  std::cout << "adding Unused" << std::endl; 
             break;
   case 'Z': std::cout << "adding  longitudinal corrector" << std::endl; 
             break;
   default:  std::cout << "Unknown element" << std::endl; 

   }
#endif

  m_beamline.push_back(name); 

} 

void  
OptimParserDriver::setGeneralAttributes() 
{                              
 
  while (!m_attributes_stack.empty() ) {

    const value_t attribute = m_attributes_stack.top();    
     m_attributes_stack.pop();

     m_general_attributes[ attribute.first ] =  attribute.second; 
 
  }
}

void  
OptimParserDriver::setInitialConditionsAttributes() 
{

  while (!m_attributes_stack.empty() ) {

    const value_t attribute = m_attributes_stack.top();    
     m_attributes_stack.pop();

     m_initial_conds_attributes[ attribute.first ] =  attribute.second; 
 
  }
}

void  
OptimParserDriver::setEmittanceAttributes()                
{
  while (!m_attributes_stack.empty() ) {

    const value_t attribute = m_attributes_stack.top();    
     m_attributes_stack.pop();

     m_emittance_attributes[ attribute.first ] =  attribute.second; 

  }
}

void  
OptimParserDriver::setTwissFitAttributes()                 
{
  
  while (!m_attributes_stack.empty() ) {

    const value_t attribute = m_attributes_stack.top();    
     m_attributes_stack.pop();

     m_twiss_fit_attributes[ attribute.first ] =  attribute.second; 

  }
}

void  
OptimParserDriver::set4DFitAttributes()                     
{

  while (!m_attributes_stack.empty() ) {

    const value_t attribute = m_attributes_stack.top();    
     m_attributes_stack.pop();

     m_4d_fit_attributes[ attribute.first ] =  attribute.second; 

  }
}

void  
OptimParserDriver::setSpaceChargeAttributes()              
{

  while (!m_attributes_stack.empty() ) {

    const value_t attribute = m_attributes_stack.top();    
     m_attributes_stack.pop();

     m_space_charge_attributes[ attribute.first ] =  attribute.second; 

  }

}

std::string 
OptimParserDriver::expandConcatenation(std::string s) {
  
    int amps = s.find("&");  

    size_t blank    = 0;
    size_t notblank = 0;
    
    std::string result; 
    std::string vs =  boost::any_cast<std::string>(m_variables[s.substr(0,amps)]);  
 

    std::string cs =  s.substr(amps);
    
   
    while (true) {

      notblank = vs.find_first_not_of(" \t\n", blank       ); 
      blank    = vs.find_first_of(" \t\n",     notblank    ); 
            
      if (notblank == vs.npos) break;
      if (blank    == vs.npos) blank = vs.length();

      result += vs.substr(notblank,blank-notblank) + cs + " ";  
     } 

    return result;    
 
 }


  void  
  OptimParserDriver::setCurrentFilename(std::string s) {

    m_file = s;
  
  
  }
