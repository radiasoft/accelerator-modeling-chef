/****************************************************************************
*****************************************************************************
*****************************************************************************
******
******  Version:   1.0                    
******                                    
******  File:      MadxParserDriver.h 
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

#ifndef MADXPARSERDRIVER_H
#define MADXPARSERDRIVER_H

#include <string>
#include <map>
#include <list>
#include <stack>
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <multitree.h>
#include <Expression.h>

typedef std::pair<std::string, boost::any>                                                          value_t;   
typedef std::map<std::string,  boost::any>                                                     values_map_t;   
typedef std::map<std::string,  Expression>                                                       expr_map_t;   
//typedef std::map<std::string,  boost::shared_ptr<values_map_t> >                           elements_map_t;   
typedef std::map<std::string,  boost::any>                                                   elements_map_t;   
typedef std::list<std::string>                                                              beamline_list_t;   
typedef std::list<std::string>                                                              sequence_list_t;   


typedef std::string parser_string_t;
typedef void* yyscan_t;

int madx_yylex_init(yyscan_t *);
int madx_yylex_destroy(yyscan_t);


// **** Forward declarations 

union YYSTYPE;

namespace yy {

  class location;
  class MadxParser;

}

class MadxParserDriver;

// **** Announce to Flex the prototype we want for the lexing function ... 

#define YY_DECL \
 int madx_yylex( YYSTYPE* yylval, yy::location* yylloc, yyscan_t yyscanner, MadxParserDriver& driver)

// *** Declare the lexer function for the parser's sake

YY_DECL;

std::ostream& operator<<(std::ostream& os, expr_map_t        const& xm);
std::ostream& operator<<(std::ostream& os, values_map_t      const& vm);
std::ostream& operator<<(std::ostream& os, beamline_list_t   const& bl);
std::ostream& operator<<(std::ostream& os, elements_map_t    const& em);
 
class MadxParserDriver {

 public:
 
  friend std::ostream& operator<<(std::ostream& os, expr_map_t        const& xm);
  friend std::ostream& operator<<(std::ostream& os, values_map_t      const& vm);
  friend std::ostream& operator<<(std::ostream& os, beamline_list_t   const& bl);
  friend std::ostream& operator<<(std::ostream& os, elements_map_t    const& em);
     
           MadxParserDriver();
  virtual ~MadxParserDriver();

  void     scan_begin( yyscan_t yyscanner);
  void     scan_end  ( yyscan_t yyscanner);
  int      parse (const std::string& f);
  int      parseFromBuffer(const char* buffer);  
  void     error(const yy::location& l, const std::string& m);
  void     error(const std::string& m);
  void     setCurrentFilename(std::string s);

  //void defineElement(std::string const& name);
  void defineElement(std::string const& label, std::string const& type, bool shared);
  void defineSequence(std::string const& label, std::string const& type, bool shared);
  void addSequenceElement(std::string const& label, std::string const& type);
  void setElementAttributes(); 


  void clearAttributes();

  // void clearVariables();
  // void clearBeamline();
  


  std::string                        m_file;
  const char*                        m_buffer;              // read from a text buffer instead of a file 
  bool                               m_input_is_file;           

  std::stack<yy::location>           m_locations_stack;

  bool                               m_trace_scanning;
  bool                               m_trace_parsing;

  expr_map_t                         m_variables;
  values_map_t                       m_constants;


  sequence_list_t*                   m_current_sequence_ptr;

  beamline_list_t                    m_beamline;
  elements_map_t                     m_elements;

  std::stack<value_t>                m_attributes_stack;


  std::string                        m_current_element_name;

  yyscan_t                           m_yyscanner; 



};

#endif  // ! PARSERDRIVER_H

