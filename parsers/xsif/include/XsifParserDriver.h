/****************************************************************************
*****************************************************************************
*****************************************************************************
******
******  Version:   1.0                    
******                                    
******  File:      XsifParserDriver.h 
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

#ifndef XSIFPARSERDRIVER_H
#define XSIFPARSERDRIVER_H

#include <string>
#include <sstream>
#include <map>
#include <list>
#include <stack>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <boost/regex.hpp>
#include <parsers/xsif/tree-container/multitree.h>
#include <parsers/xsif/Expression.h>
#include <basic_toolkit/GenericException.h>
#include <beamline/BmlPtr.h>

class bmlnElmnt; 
class beamline; 


typedef std::pair<std::string, boost::any>                                                          value_t;   
typedef std::map<std::string,  boost::any>                                                     values_map_t;   
typedef std::map<std::string,  Expression>                                                       expr_map_t;   
typedef std::map<std::string,  boost::any>                                                   elements_map_t;   
typedef std::list<std::string>                                                              beamline_list_t;   



typedef std::string parser_string_t;
typedef void* yyscan_t;

int xsif_yylex_init(yyscan_t *);
int xsif_yylex_destroy(yyscan_t);


// **** Forward declarations 

union YYSTYPE;

namespace xsif_yy {

  class location;
  class XsifParser;

}

#include "XsifParser_ypp.hh"

class XsifParserDriver;

//-----------------------------------------------------------------------------
// **** Announce to Flex the prototype we want for the lexing function ... 
//---------------------------------------------------------------------------
#define YY_DECL \
 xsif_yy::XsifParser::token_type  \
 xsif_yylex( xsif_yy::XsifParser::semantic_type* yylval, \
             xsif_yy::XsifParser::location_type* yylloc, \
             yyscan_t yyscanner, \
             XsifParserDriver& driver)
//-----------------------------------------------------------------------------
// *** Declare the lexer function for the parser's sake
//-----------------------------------------------------------------------------

YY_DECL;

//-----------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& os, expr_map_t        const& xm);
std::ostream& operator<<(std::ostream& os, values_map_t      const& vm);
std::ostream& operator<<(std::ostream& os, beamline_list_t   const& bl);
std::ostream& operator<<(std::ostream& os, elements_map_t    const& em);


typedef std::pair<std::string, boost::any> attrib_pair_t;
 
class XsifParserDriver {

 public:
 
  friend std::ostream& operator<<(std::ostream& os, expr_map_t        const& xm);
  friend std::ostream& operator<<(std::ostream& os, values_map_t      const& vm);
  friend std::ostream& operator<<(std::ostream& os, beamline_list_t   const& bl);
     
            XsifParserDriver();
  virtual  ~XsifParserDriver();

  void      scan_begin( yyscan_t yyscanner);
  void      scan_end  ( yyscan_t yyscanner);

  int                parse (std::string const& f);
  int       parseFromBuffer(char const* buffer);  

  void      error(xsif_yy::location const& l, std::string const& m) const;
  void      error(std::string const& m);

  void      setCurrentFilename(std::string s);


  void      defineLineMacro( xsif_yy::location const& yyloc, std::string const& name, std::vector<std::string> const& elements);
  BmlPtr    expandLineMacro(xsif_yy::location const& yyloc, std::string const& name);

  BmlPtr    instantiateLine( xsif_yy::location const& yyloc, std::string const& name, std::vector<std::string> const& elements);
  void      addLineToDictionary( std::string const& label, BmlPtr& bml); 

  std::vector<std::string>* 
            instantiateAnonymousLine( xsif_yy::location const& yyloc, std::vector<std::string> const & elements, int ntimes=1); 

  ElmPtr    instantiateElement( xsif_yy::location const& yyloc, std::string const &label,  std::vector<attrib_pair_t> const& attributes); 
  void      addElmToDictionary( std::string const& label, ElmPtr& bml); 

  double    getElmAttributeVal( xsif_yy::location const& yyloc, std::string const& elm, std::string const& attrib) const; 

  BmlPtr    getLine(std::string label);

  void      command_BEAM  (  xsif_yy::location const& yyloc,  std::vector<attrib_pair_t> const& attributes);
  void      command_CALL  (  xsif_yy::location      & yyloc,  std::vector<attrib_pair_t> const& attributes, yyscan_t yyscanner);
  void      command_RETURN(  xsif_yy::location      & yyloc,  yyscan_t yyscanner);

  void init(); 

  static ElmPtr        make_drift(double const&, std::string const& label, std::vector<attrib_pair_t> const& attributes );
  static ElmPtr       make_marker(double const&, std::string const& label, std::vector<attrib_pair_t> const& attributes );
  static ElmPtr        make_sbend(double const&, std::string const& label, std::vector<attrib_pair_t> const& attributes );
  static ElmPtr        make_rbend(double const&, std::string const& label, std::vector<attrib_pair_t> const& attributes );
  static ElmPtr   make_quadrupole(double const&, std::string const& label, std::vector<attrib_pair_t> const& attributes );
  static ElmPtr    make_sextupole(double const&, std::string const& label, std::vector<attrib_pair_t> const& attributes );
  static ElmPtr     make_octupole(double const&, std::string const& label, std::vector<attrib_pair_t> const& attributes );
  static ElmPtr    make_multipole(double const&, std::string const& label, std::vector<attrib_pair_t> const& attributes );
  static ElmPtr     make_solenoid(double const&, std::string const& label, std::vector<attrib_pair_t> const& attributes );
  static ElmPtr       make_kicker(double const&, std::string const& label, std::vector<attrib_pair_t> const& attributes );
  static ElmPtr      make_vkicker(double const&, std::string const& label, std::vector<attrib_pair_t> const& attributes );
  static ElmPtr      make_hkicker(double const&, std::string const& label, std::vector<attrib_pair_t> const& attributes );
  static ElmPtr     make_rfcavity(double const&, std::string const& label, std::vector<attrib_pair_t> const& attributes );
  static ElmPtr      make_monitor(double const&, std::string const& label, std::vector<attrib_pair_t> const& attributes );
  static ElmPtr     make_hmonitor(double const&, std::string const& label, std::vector<attrib_pair_t> const& attributes );
  static ElmPtr     make_vmonitor(double const&, std::string const& label, std::vector<attrib_pair_t> const& attributes );
  static ElmPtr     make_beambeam(double const&, std::string const& label, std::vector<attrib_pair_t> const& attributes );
  static ElmPtr         make_srot(double const&, std::string const& label, std::vector<attrib_pair_t> const& attributes );
  static ElmPtr         make_yrot(double const&, std::string const& label, std::vector<attrib_pair_t> const& attributes );
  static ElmPtr         make_lump(double const&, std::string const& label, std::vector<attrib_pair_t> const& attributes );
  static ElmPtr      make_lcavity(double const&, std::string const& label, std::vector<attrib_pair_t> const& attributes );
  static ElmPtr       make_matrix(double const&, std::string const& label, std::vector<attrib_pair_t> const& attributes );
  static ElmPtr  make_ecollimator(double const&, std::string const& label, std::vector<attrib_pair_t> const& attributes );
  static ElmPtr  make_rcollimator(double const&, std::string const& label, std::vector<attrib_pair_t> const& attributes );
  static ElmPtr   make_instrument(double const&, std::string const& label, std::vector<attrib_pair_t> const& attributes );
  static ElmPtr    make_blmonitor(double const&, std::string const& label, std::vector<attrib_pair_t> const& attributes );
  static ElmPtr         make_wire(double const&, std::string const& label, std::vector<attrib_pair_t> const& attributes );

 
 // data members (public) _____________________________________________________________________________________________

  typedef  ElmPtr (*make_fnc_ptr)(double const&, std::string const& label, std::vector<attrib_pair_t> const& attributes );

  std::string                                       m_file;
  const char*                                       m_buffer;              // read from a text buffer instead of a file 
  bool                                              m_input_is_file;           

  mutable std::stack<xsif_yy::location>             m_locations_stack;
  mutable std::stack<void*>                         m_buffer_stack;

  bool                                              m_trace_scanning;
  bool                                              m_trace_parsing;

  values_map_t                                      m_constants;
  expr_map_t                                        m_variables;


  std::map<std::string, BmlPtr>                     m_lines;
  std::map<std::string, std::vector<std::string> >  m_line_macros;
  std::map<std::string, ElmPtr>                     m_elements;

  std::vector<attrib_pair_t>                        m_attributes;
  std::map<std::string,make_fnc_ptr>                m_makefncs;

  yyscan_t                                          m_yyscanner; 

  double                                            m_BRHO;
  double                                            m_energy;
  std::string                                       m_particle_type_name;

 

  class ParserException: public GenericException {

  public:

  ParserException( std::string file, int lineno, const char* fnc_name= "", const char* msg= "" )
    :  GenericException(file,lineno,fnc_name,msg) {} 
  };

};

#endif  // ! PARSERDRIVER_H

