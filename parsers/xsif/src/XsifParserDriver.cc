/****************************************************************************
*****************************************************************************
*****************************************************************************
******
******  File:      XsifParserDriver.cc 
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

#include "XsifParserDriver.h"

#include <iomanip>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string_regex.hpp>
#include <basic_toolkit/PhysicsConstants.h>
#include <beamline/Alignment.h>
#include <beamline/beamline.h>
#include <beamline/bmlnElmnt.h>
#include <beamline/beamline_elements.h>
#include <cstdlib>


using namespace std;
using namespace boost;


using std::setw;
using std::left;



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


struct AttributesEqual { 
  AttributesEqual( string const& str): m_str(str) {} 

  bool operator()( attrib_pair_t const& attr ) const
    { return (attr.first == m_str); }

  string m_str;  
};


bool eval( string const& attribute, vector<attrib_pair_t> const& attributes, any& result ) {

  // **** Returns true if attribute has been explicitly specified. 
  //      In that case, the value evaluated, is also returned via the 'result' argument. 

  if (attributes.empty()) return false; 

  vector<attrib_pair_t>::const_iterator it;

  if ( (it = find_if(attributes.begin(), attributes.end(), AttributesEqual(attribute) )) ==  attributes.end()  ) return false;

  result = it->second;

  if ( Expression::is_type<Expression>(it->second) )  
     result = any_cast<Expression>(result).evaluate( ); 

  return true; 
}    


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& operator<<(ostream& os, values_map_t const& m) {

  values_map_t::const_iterator it;
  for ( it = m.begin(); it != m.end();  ++it) 
  {
    if ( it->second.type() == typeid(double) )
      os << setw(16) << left << it->first  << " = " <<  setw(16) <<  boost::any_cast<double>(it->second)           << endl;
    else 
      os << setw(16) << left << it->first  << " = " <<  setw(16) <<  boost::any_cast<parser_string_t>(it->second)  << endl;
  
  }

  return os;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& operator<<(ostream& os, beamline_list_t const& bl) {

  beamline_list_t::const_iterator it;
  int elno = 0; 
  for ( it = bl.begin(); it != bl.end();  ++it) 
  {
    os << setw(16) << ++elno <<" : " << setw(16) << *it  << endl;
  }

  return os;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// The code below is useful for debugging. It should print an actual tree. 

ostream& operator<<(ostream& os, expr_map_t  const& xm) {

 
  for ( expr_map_t::const_iterator it = xm.begin(); it != xm.end();  ++it) 
  {
    os << setw(16) << it->first << " = " << it->second << endl;
  }
  return os;

}



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

XsifParserDriver::XsifParserDriver(): m_trace_scanning(false), m_trace_parsing(false)
{


  // Predefined constants in XSIF

  m_constants["PI"]       = M_PI;          
  m_constants["TWOPI"]    = 2.0*M_PI;          
  m_constants["DEGRAD"]   = 180.0/M_PI;
  m_constants["RADDEG"]   = M_PI/180.0;
  m_constants["E"]        = exp(1.0);
  m_constants["EMASS"]    = 0.510998902e-3;     // electron mass in GeV
  m_constants["PMASS"]    = 0.938271998;        // proton mass in GeV
  m_constants["MUMASS"]   = 0.1056583568;       // muon mass in GeV
  m_constants["CLIGHT"]   = 2.99792458e8;       // speed of light in m/s
  m_constants["QELECT"]   = 1.602176462e-19;    // elementary charge  in A-s
   
  init(); // intialize jump table


}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

XsifParserDriver::~XsifParserDriver() 
{

  xsif_yylex_destroy(m_yyscanner);

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int  
XsifParserDriver::parse(string const& f)
{
 
  m_file = f; 
  m_input_is_file = true;    

  xsif_yylex_init(&m_yyscanner);

  xsif_yy::XsifParser parser(m_yyscanner, *this);

  parser.set_debug_level(m_trace_parsing);

  scan_begin( m_yyscanner);

  int ret = parser.parse();

  scan_end(  m_yyscanner);
  
  return ret;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int  
XsifParserDriver::parseFromBuffer(char const* buffer)
{
 
  m_buffer = buffer; 
  m_input_is_file = false;    
   
  xsif_yylex_init(&m_yyscanner);

  xsif_yy::XsifParser parser(m_yyscanner, *this);

  parser.set_debug_level(m_trace_parsing);

  scan_begin( m_yyscanner);

  int ret = parser.parse();

  scan_end(  m_yyscanner);
  
  return ret;

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void
XsifParserDriver::error(  xsif_yy::location const& l, const string& m) const
{

  cerr << l <<": "    << m << endl;

  xsif_yy::location loc;

  while( !m_locations_stack.empty() ) {

    loc = m_locations_stack.top();
    m_locations_stack.pop();

    cerr <<"Included from: " << loc << endl;

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

  throw ParserException( __FILE__, __LINE__, "XsifParserDriver::error()", "Error" );  
 
} 


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void
XsifParserDriver::error( const string& m) 
{

  cerr << "XsifParserDriver::error( const string& m): " << m << endl;
  
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void
XsifParserDriver::instantiateLine( xsif_yy::location const& yyloc, string const& name, vector<string> const & elements ) { 

  
  beamline* bl = new beamline( name.c_str() ); 

  map<string,  beamline*>::iterator bml_it;  
  map<string, bmlnElmnt*>::iterator elm_it;  
 
  for ( vector<string>::const_iterator it=elements.begin(); it != elements.end(); ++it ) {

    if      ( (bml_it = m_lines.find(*it) )    !=  m_lines.end() ) {
  
        bl->append( bml_it->second->Clone());
    } 
    else if ( (elm_it = m_elements.find(*it) ) !=  m_elements.end() ){ 
        bl->append(elm_it->second->Clone());
    }
    else if( contains((*it), "(") ) {
        bl->append( expandLineMacro(yyloc, *it) );
    } 
    else {
       delete bl;
       error( yyloc, string(" Error: Element ") + (*it) + string(" is undefined."));
    }
  } 
 
   double BRHO  = m_variables["BRHO"].evaluate();
 
   bl->setEnergy( BRHO* PH_CNV_brho_to_p); // THIS is wrong argument should be TOTAL energy.  FIXME ! 

   m_lines[name] = bl;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

  
void
XsifParserDriver::defineLineMacro(xsif_yy::location const& yyloc, string const& name_and_args, vector<string> const& elements ) { 

  //---------------------------------------------------------------------------------------------------------
  // Define a macro. 'name_and_args' includes the parentheses and a list of comma separated dummy arguments. 
  // The macro is defined in a std::map structure, with the key set to an UPPERCASE version of the full
  // 'name_and_args' string with the number of dummy arguments appended after the separator character '#'. 
  // 'elements' is simply a list of names or macros (the elements in the line), but with the dummy arguments 
  // REPLACED with the standard names ARG_1, ARG_2, ARG_3 ,..,, ARG_N. 
  // --------------------------------------------------------------------------------------------------------
  // ** NOTE: by construction, there should be no whitespaces, since the lexer (tokenizer) strips them. 

  using namespace boost::algorithm;

  string macro_name = name_and_args;

  // ... extract the arguments

 
  iterator_range<string::iterator> ir = find_regex( macro_name, regex("\\(.*\\)") ); 
  string basename(macro_name.begin(), ir.begin() );
  string args( ++ir.begin(), --ir.end() );

  vector<string> argv;
  split_regex( argv, args, regex(",") ); 

  std::stringstream ss; 
  ss << basename << "#" << argv.size();
  macro_name = ss.str();

  // ... substitute std dummy args strings 

  vector<string> elms = elements; 

  for ( vector<string>::iterator ite=elms.begin(); ite != elms.end(); ++ite ) { 

    int idx = 0;
   for( vector<string>::iterator ita = argv.begin(); ita !=  argv.end(); ++ita ) {

            std::stringstream ss; 
            ss << "ARG_" << ++idx;
            string arg_n= ss.str();

            if ( *ite == *ita ) { 
              *ite = arg_n;
               continue;
            }
 
            string::size_type pos1 = (*ite).find("("); 
            if ( pos1 == string::npos )  continue; // elm is not a macro, so just leave it as is and continue 
            string::size_type pos2 = (*ite).find(")");
	    string::size_type pos;  

            ++pos1;
            while ( (pos2 = ((pos = ite->find("," ,  pos1) ) != string::npos) ? pos : ite->find(")",pos1) ) != string::npos  ) { 
	      if ( ite->substr(pos1, pos2-pos1) == *ita )  ite->replace(pos1, pos2-pos1, arg_n);
              pos1 = pos2+1;
	    }       


    }
  }

  m_line_macros[macro_name] = elms;

}
  

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


beamline*
XsifParserDriver::expandLineMacro(xsif_yy::location const& yyloc, string const& name_and_args) { 

  //-------------------------------------------------------------------------------------------------------------------------------------
  // Recursively expands the line defined by the macro call 'name_and_args', which includes the parentheses and 
  // comma separated arguments. If the line does not exists it is created; otherwise, the existing one is cloned. 
  // The expanded line is given a name formed by concatenating its basename and its arguments.
  // Upon return, 'expanded line' contains a text version of the expanded line. 
  // ------------------------------------------------------------------------------------------------------------------------------------

  vector<string> expanded_line; 

  string macro_name = name_and_args;
 
  iterator_range<string::iterator> ir = find_regex(macro_name, regex("\\(.*\\)") ); 

  string basename(macro_name.begin(), ir.begin() );
  string args( ++ir.begin(), --ir.end() );

  //--------------------------------------------------------------------------------------------------------
  /* determine the number of arguments */
  //--------------------------------------------------------------------------------------------------------

  // ... extract the arguments  

  vector<string> argv;
  split_regex( argv, args, regex(",") ); 

  std::stringstream ss; 
  ss << basename << "#"<< argv.size();
 
  macro_name = ss.str();

  if  ( m_line_macros.find(macro_name) ==  m_line_macros.end() ) {
     error(yyloc, string("No definition found for macro ") + basename );
  }

  expanded_line =  m_line_macros[macro_name];

  // extract the no of arguments (embeded into the macro name)

  string::size_type pos =  macro_name.rfind('#');  
  int nargs = atoi( macro_name.substr(++pos).c_str() ); 

  // substitute arguments and return the expanded line ... 
 
  beamline* bl = new beamline ( name_and_args.c_str() ); 

  map<string, bmlnElmnt*>::iterator elm_it;
  map<string,  beamline*>::iterator bml_it;

  for( vector<string>::iterator ite = expanded_line.begin(); ite !=  expanded_line.end(); ++ite ) {

    int idx =0;
    for ( vector<string>::iterator it=argv.begin(); it != argv.end(); ++it ) { 

            std::stringstream ss; 
            ss << "ARG_" << ++idx;
            string arg_n= ss.str();

            if ( *ite == arg_n ) { 
              *ite = *it;
               continue;
            }
 
            string::size_type pos1 = (*ite).find("("); 
            if ( pos1 == string::npos )  continue; // elm is not a macro, so just leave it as is and continue 
            string::size_type pos2 = (*ite).find(")");
	    string::size_type pos;  

            ++pos1;
            while ( (pos2 = ((pos = ite->find("," ,  pos1) ) != string::npos) ? pos : ite->find(")",pos1) ) != string::npos  ) { 
	      if ( ite->substr(pos1, pos2-pos1) == arg_n )  ite->replace(pos1, pos2-pos1, *it);
              pos1 = pos2+1;
	    }       
    }
      
    // if the element is a macro, *always* expand it  

     if ( contains((*ite), "(") ) {
        bl->append( expandLineMacro(yyloc, *ite) ); 

     } 
     else if ( (bml_it = m_lines.find(*ite)) != m_lines.end() ) {   
	
         bl->append( bml_it->second->Clone() ); 

     } 
     else if ( (elm_it = m_elements.find(*ite)) != m_elements.end() ) {   
	
         bl->append( elm_it->second->Clone() ); 

     }
     else {
       delete bl;    
       error(yyloc, string("Error: element ") + (*ite) + string(" is undefined. ") );

     }
   
  }
  return bl;  

}
  

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline*
XsifParserDriver::getLine(string label) { 

  if  ( m_lines.find(label) ==  m_lines.end() ) return 0; // line not found
  return m_lines[label]; 
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

   
double  XsifParserDriver::getElmAttributeVal( xsif_yy::location const& yyloc, string const& elm, string const& attrib) const
{

     std::map<std::string, bmlnElmnt*>::const_iterator it_elm = m_elements.find( elm );

     if ( it_elm == m_elements.end() ) 
     { 
       error(yyloc, string("Error: element attribute ") + elm 
                           + string("[") + attrib + string("]") +string(" is undefined.") );
     }

     return it_elm->second->Length() ;
}
                                             

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void XsifParserDriver::setBeamParameters( xsif_yy::location const& yyloc, std::vector<attrib_pair_t> const& attributes) {

  // for the moment hardwired to positron --- FIXME !

  any value;

  if ( ! eval( string("ENERGY"), attributes, value) ) return;
 
   double mass = PH_NORM_me;

   double et        = any_cast<double>(value );
   double ek        = et - mass;
   double gamma     = et/mass;
   double momentum  = std::sqrt((et*et)-(mass*mass));
   double brho      = momentum/PH_CNV_brho_to_p;
 
   Expression exp; exp.insert(ExprData(brho));
  
   m_variables["BRHO"] = exp; 
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void
XsifParserDriver::init() 
{
  
     m_makefncs["DRIFT"      ] = &XsifParserDriver::make_drift;
     m_makefncs["MARKER"     ] = &XsifParserDriver::make_marker;
     m_makefncs["SBEND"      ] = &XsifParserDriver::make_sbend;
     m_makefncs["RBEND"      ] = &XsifParserDriver::make_rbend;
     m_makefncs["QUADRUPOLE" ] = &XsifParserDriver::make_quadrupole;
     m_makefncs["SEXTUPOLE"  ] = &XsifParserDriver::make_sextupole;
     m_makefncs["OCTUPOLE"   ] = &XsifParserDriver::make_octupole;
     m_makefncs["MULTIPOLE"  ] = &XsifParserDriver::make_multipole;
     m_makefncs["SOLENOID"   ] = &XsifParserDriver::make_solenoid;
     m_makefncs["KICKER"     ] = &XsifParserDriver::make_kicker;
     m_makefncs["VKICKER"    ] = &XsifParserDriver::make_vkicker;
     m_makefncs["HKICKER"    ] = &XsifParserDriver::make_hkicker;
     m_makefncs["RFCAVITY"   ] = &XsifParserDriver::make_rfcavity;
     m_makefncs["HMONITOR"   ] = &XsifParserDriver::make_hmonitor;
     m_makefncs["VMONITOR"   ] = &XsifParserDriver::make_vmonitor;
     m_makefncs["MATRIX"     ] = &XsifParserDriver::make_matrix;
     m_makefncs["BEAMBEAM"   ] = &XsifParserDriver::make_beambeam;
     m_makefncs["SROT"       ] = &XsifParserDriver::make_srot;
     m_makefncs["YROT"       ] = &XsifParserDriver::make_yrot;
     m_makefncs["LUMP"       ] = &XsifParserDriver::make_lump;
     m_makefncs["LCAVITY"    ] = &XsifParserDriver::make_lcavity;
     m_makefncs["ECOLLIMATOR"] = &XsifParserDriver::make_matrix;
     m_makefncs["RCOLLIMATOR"] = &XsifParserDriver::make_rcollimator;
     m_makefncs["INSTRUMENT" ] = &XsifParserDriver::make_instrument;


}



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


 // The class Ordering below defines a function object needed for std::equal_range(). 
 // For some reason g++ 4.1 will accept a local class definition, but equal_range
 // does not seem to be able to search the local namespace for operator(). As a workaround,
 // we define Ordering in the anonymous namespace. 
 // ---------------------------------------------------------------------------------------- 

 
namespace {

 class Ordering : public binary_function< const pair<string, XsifParserDriver::make_fnc_ptr>,  const pair<string, XsifParserDriver::make_fnc_ptr>, bool > { 

 public: 

    bool operator()( pair<string, XsifParserDriver::make_fnc_ptr> const& lhs, 
                    pair<string, XsifParserDriver::make_fnc_ptr> const& rhs ) const
   { return (lhs.first < rhs.first); }

 };


}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void
XsifParserDriver::instantiateElement(xsif_yy::location const& yyloc, string const& label, vector<attrib_pair_t> const & attributes ) 
{
  
 any    value;
 string elm_type;

 if (attributes[0].second.empty() ) { 
    elm_type  = attributes[0].first; 
 } 
 else if ( eval( string("TYPE"), attributes, value) ) {  
   std:: cout << "TYPE attribute found " << std::endl; 
    elm_type  = any_cast<string>(value);
 }
 else {
   error(yyloc, string("TYPE attribute is undefined for ")+ label + string(".") );
 }
 
 pair<string, XsifParserDriver::make_fnc_ptr> search (elm_type, 0);

 pair< map<string, XsifParserDriver::make_fnc_ptr>::iterator,  map<string, XsifParserDriver::make_fnc_ptr>::iterator> result = 
 equal_range( m_makefncs.begin(), m_makefncs.end(), search, Ordering() ); 
 
 double BRHO   = m_variables["BRHO"].evaluate();
 
 if (  (result.first != m_makefncs.end()) && (distance(result.first,result.second) < 2) )  
    m_elements[label] = m_makefncs[result.first->first]( BRHO,  label, attributes);   // unambiguous match;
 else
   throw GenericException( __FILE__, __LINE__, "XsifParserDriver::instantiateElement", "Type name is ambiguous." );

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


bmlnElmnt* XsifParserDriver::make_drift( double const& BRHO, std::string const& label,  vector<attrib_pair_t> const& attributes ) 
{
   any value;
   eval( string("L"), attributes, value);
   return new drift( label.c_str(), any_cast<double>(value) );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
 
bmlnElmnt* XsifParserDriver::make_marker(  double const& BRHO, std::string const& label,  vector<attrib_pair_t> const& attributes ) 
{
      return new marker(  label.c_str() );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
 
bmlnElmnt*  XsifParserDriver::make_sbend(  double const& BRHO, std::string const& label,  vector<attrib_pair_t> const& attributes ) 
{
  //---------------------------------------------------------------------
  // valid attributes for type SBEND  
  // --------------------------------------------------------------------
  // L       length in                   [m  ]
  // ANGLE   bend angle                  [rad]
  // K1      quad component of the field [   ]  
  // E1      entrance edge angle 
  // E2      exit edge angle
  // TILT    tilt angle
  // K2      sextupole comp  
  // H1      entrance pole face curvature
  // H2      exit pole face curvature
  // HGAP    entrance half-gap size
  // FINT    entrace fringe field integral (default = 0.5)
  // HGAPX   exit half-gap size
  // FINTX   exit fringe field integral
  //--------------------------------------------------------------------- 


  any value;
  bmlnElmnt* elm = 0;


  double length = 0.0;
  double angle  = 0.0;
  double k1     = 0.0;
  double k2     = 0.0;
  double k3     = 0.0;
  double tilt   = 0.0;
  double e1     = 0.0;
  double e2     = 0.0;
  alignmentData aligner;

  bool simple = true;

  if ( eval( string("L"),      attributes, value) )    length = any_cast<double>(value); 
  if ( eval( string("ANGLE"),  attributes, value) )    angle  = any_cast<double>(value); 
  if ( eval( string("K1"),     attributes, value) )  { k1     = any_cast<double>(value); simple = false; } 
  if ( eval( string("K2"),     attributes, value) )  { k2     = any_cast<double>(value); simple = false; }
  if ( eval( string("K3"),     attributes, value) )  { k3     = any_cast<double>(value); simple = false; }
  ///// ********if ( eval( string("TILT"),   attributes, value) )  { tilt   = any_cast<double>(value); simple = false; } 
  if ( eval( string("E1"),     attributes, value) )    e1     = any_cast<double>(value); 
  if ( eval( string("E2"),     attributes, value) )    e2     = any_cast<double>(value); 



  if( simple) {
    elm = new sbend( label.c_str(), length, BRHO*angle/length, angle, e1, e2 );
    if ( tilt != 0.0  ) {
      aligner.xOffset = 0.0;
      aligner.yOffset = 0.0;
      aligner.tilt    = tilt;
      elm->setAlignment( aligner );
	}
    return elm; 
  }

  elm = new CF_sbend( label.c_str(), length, BRHO*angle/length, angle, e1, e2 );

  double multipoleStrength;

  multipoleStrength = k1*BRHO*length;
  if( multipoleStrength != 0.0 ) {
      dynamic_cast<CF_sbend*>(elm)->setQuadrupole( multipoleStrength );
  }

  multipoleStrength = k2*BRHO*length/2.0;
  if( multipoleStrength != 0.0 ) {
      dynamic_cast<CF_sbend*>(elm)->setSextupole( multipoleStrength );
  }

  multipoleStrength = k3*BRHO*length/6.0;
    if( multipoleStrength != 0.0 ) {
      dynamic_cast<CF_sbend*>(elm)->setOctupole( multipoleStrength );
  }
  
  return elm;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt*  XsifParserDriver::make_rbend(  double const& BRHO, std::string const& label,  vector<attrib_pair_t> const& attributes ) 
{

  //---------------------------------------------------------------------
  // valid attributes for type RBEND  
  // --------------------------------------------------------------------
  // L       length in                   [m  ]
  // ANGLE   bend angle                  [rad]
  // K1      quad component of the field [   ]  
  // E1      entrance edge angle 
  // E2      exit edge angle
  // TILT    tilt angle
  // K2      sextupole comp  
  // H1      entrance pole face curvature
  // H2      exit pole face curvature
  // HGAP    entrance half-gap size
  // FINT    entrance fringe field integral (default = 0.5)
  // HGAPX   exit half-gap size
  // FINTX   exit fringe field integral
  //--------------------------------------------------------------------- 

  any value;
  bmlnElmnt* elm = 0;

  double length = 0.0;
  double angle  = 0.0;
  double k1     = 0.0;
  double k2     = 0.0;
  double k3     = 0.0;
  double tilt   = 0.0;
  double e1     = 0.0;
  double e2     = 0.0;

  bool simple = true;

  if ( eval( string("L"),      attributes, value) )    length = any_cast<double>(value); 
  if ( eval( string("ANGLE"),  attributes, value) )    angle  = any_cast<double>(value); 
  if ( eval( string("K1"),     attributes, value) )  { k1     = any_cast<double>(value); simple = false; } 
  if ( eval( string("K2"),     attributes, value) )  { k2     = any_cast<double>(value); simple = false; }
  if ( eval( string("K3"),     attributes, value) )  { k3     = any_cast<double>(value); simple = false; }
  if ( eval( string("TILT"),   attributes, value) )  { tilt   = any_cast<double>(value); simple = false; } 
  if ( eval( string("E1"),     attributes, value) )    e1     = any_cast<double>(value); 
  if ( eval( string("E2"),     attributes, value) )    e2     = any_cast<double>(value); 

  if( simple ) {
    if( (0.0 == e1) && (0.0 == e2) ) {
      elm = new rbend( label.c_str(), length, BRHO*(2.0*sin(0.5*angle))/length, (angle/2.0) );
	}
    else {
      elm = new rbend( label.c_str(), length, BRHO*(2.0*sin(0.5*angle))/length, (angle/2.0), e1, e2 );
    }
   
    return elm;
    // Ignored parameters: K1, K2, K3, E1, E2, H1, H2, HGAP, FINT
  }

  if( (0.0 == e1) && (0.0 == e2) ) {
      elm = new CF_rbend( label.c_str(),  length, BRHO*(2.0*sin(0.5*angle))/length, (angle/2.0) );
  } else {
      elm = new CF_rbend( label.c_str(), length, BRHO*(2.0*sin(0.5*angle))/length, (angle/2.0), e1, e2 );
  }

  double multipoleStrength;

  multipoleStrength = k1*BRHO*length;
  if( multipoleStrength != 0.0 ) {
      dynamic_cast<CF_rbend*>(elm)->setQuadrupole( multipoleStrength );
  }

  multipoleStrength = k2*BRHO*length/2.0;
    if( multipoleStrength != 0.0 ) {
      dynamic_cast<CF_rbend*>(elm)->setSextupole( multipoleStrength );
  }

   multipoleStrength = k3*BRHO*length/6.0;
    if( multipoleStrength != 0.0 ) {
      dynamic_cast<CF_rbend*>(elm)->setOctupole( multipoleStrength );
  }

  return elm;     
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt*  XsifParserDriver::make_quadrupole( double const& BRHO, std::string const& label,  vector<attrib_pair_t> const& attributes ) 
{ 

  //---------------------------------------------------------------------
  // valid attributes for type QUADRUPOLE
  // --------------------------------------------------------------------
  // L         length in                   [m  ]
  // K1        quad component of the field [   ]  
  // TILT      tilt angle
  // APERTURE  aperture radius ?  
  //--------------------------------------------------------------------- 

  
  any value;
  bmlnElmnt* elm = 0;
  alignmentData aligner;

  double length   = 0.0;
  double k1       = 0.0;
  double tilt     = 0.0;
  double aperture = 0.0;

  if ( eval( string("LENGTH"),   attributes, value) )    length   = any_cast<double>(value); 
  if ( eval( string("K1"),       attributes, value) )    k1       = any_cast<double>(value); 
  if ( eval( string("TILT"),     attributes, value) )    tilt     = any_cast<double>(value); 
  if ( eval( string("APERTURE"), attributes, value) )    aperture = any_cast<double>(value); 
  
  if ( length != 0.0 ) {
    elm = new quadrupole( label.c_str(), length, BRHO*k1 );
  } else {
    elm = new thinQuad( label.c_str(), length );
  }
  
  if ( tilt != 0.0  ) {
    aligner.xOffset = 0.0;
    aligner.yOffset = 0.0;
    aligner.tilt    = tilt;
    elm->setAlignment( aligner );
  }

  return elm;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt*  XsifParserDriver::make_sextupole(  double const& BRHO, std::string const& label,  vector<attrib_pair_t> const& attributes )
{ 

  //---------------------------------------------------------------------
  // valid attributes for type SEXTUPOLE
  // --------------------------------------------------------------------
  // L         length in                   [m  ]
  // K2        sextupole component of the field [   ]  
  // TILT      tilt angle
  // APERTURE  aperture radius ?  
  //--------------------------------------------------------------------- 

  any value;
  bmlnElmnt* elm = 0;
  alignmentData aligner;

  double length   = 0.0;
  double k2       = 0.0;
  double tilt     = 0.0;
  double aperture = 0.0;

  if ( eval( string("LENGTH"),   attributes, value) )    length   = any_cast<double>(value); 
  if ( eval( string("K2"),       attributes, value) )    k2       = any_cast<double>(value); 
  if ( eval( string("TILT"),     attributes, value) )    tilt     = any_cast<double>(value); 
  if ( eval( string("APERTURE"), attributes, value) )    aperture = any_cast<double>(value); 
  
  if ( length != 0.0 ) {
    elm = new sextupole( label.c_str(), length, BRHO*k2/2.0 );
  } else {
    elm = new thinSextupole( label.c_str(), length );
  }
  
  if ( tilt != 0.0  ) {
    aligner.xOffset = 0.0;
    aligner.yOffset = 0.0;
    aligner.tilt    = tilt;
    elm->setAlignment( aligner );
  }

  return elm;
}

 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt*  XsifParserDriver::make_octupole(  double const& BRHO, std::string const& label,  vector<attrib_pair_t> const& attributes )
{
  //---------------------------------------------------------------------
  // valid attributes for type SEXTUPOLE
  // --------------------------------------------------------------------
  // L         length in                   [m  ]
  // K3        octupole component of the field [   ]  
  // TILT      tilt angle
  // APERTURE  aperture radius ?  
  //--------------------------------------------------------------------- 

  
  any value;
  bmlnElmnt* elm = 0;
  alignmentData aligner;

  double length   = 0.0;
  double k3       = 0.0;
  double tilt     = 0.0;
  double aperture = 0.0;

  if ( eval( string("LENGTH"),   attributes, value) )    length   = any_cast<double>(value); 
  if ( eval( string("K3"),       attributes, value) )    k3       = any_cast<double>(value); 
  if ( eval( string("TILT"),     attributes, value) )    tilt     = any_cast<double>(value); 
  if ( eval( string("APERTURE"), attributes, value) )    aperture = any_cast<double>(value); 
  
  if ( length != 0.0 || k3 != 0.0 ) {
    elm = new octupole( label.c_str(), length, BRHO*k3/6.0 );
  } else {
    elm = new thinOctupole( label.c_str(), length );
  }
  
  if ( tilt != 0.0 ) {
    aligner.xOffset = 0.0;
    aligner.yOffset = 0.0;
    aligner.tilt    = tilt;
    elm->setAlignment( aligner );
  }

  return elm;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt*  XsifParserDriver::make_multipole( double const& BRHO, std::string const& label,  vector<attrib_pair_t> const& attributes )
{

//---------------------------------------------------------------------
// valid attributes for type MULTIPOLE
// --------------------------------------------------------------------
// L             length in                   [m  ]
// K0L -- K20L   integrated strength 
// T0  -- T20    tilt angles.
// SCALEFAC      octupole component of the field [   ]  
// TILT          tilt angle
// APERTURE      aperture radius ?  
//--------------------------------------------------------------------- 


any value;
bmlnElmnt* elm = 0;
alignmentData aligner;

bmlnElmnt* q    = 0;
beamline* temp  = new beamline( label.c_str() ); 
// temp->setEnergy( this->getEnergy() ); 

double   kl[21]; { for (int i=0; i<21; ++i)   kl[i] = 0.0; } // LEAK !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
double tilt[21]; { for (int i=0; i<21; ++i) tilt[i] = 0.0; }

if ( eval( string("K0L"),   attributes, value) )    kl[0]   = any_cast<double>(value); 
if ( eval( string("K1L"),   attributes, value) )    kl[1]   = any_cast<double>(value); 
if ( eval( string("K2L"),   attributes, value) )    kl[2]   = any_cast<double>(value); 
if ( eval( string("K3L"),   attributes, value) )    kl[3]   = any_cast<double>(value); 

if ( eval( string("T0"),    attributes, value) )    tilt[0] = any_cast<double>(value); 
if ( eval( string("T1"),    attributes, value) )    tilt[1] = any_cast<double>(value); 
if ( eval( string("T2"),    attributes, value) )    tilt[2] = any_cast<double>(value); 
if ( eval( string("T3"),    attributes, value) )    tilt[3] = any_cast<double>(value); 

if( kl[0] != 0.0 ) {
  q    = new thin2pole(BRHO*kl[0]);
  if( tilt[0] != 0.0 ) {
    aligner.xOffset = 0.0;
    aligner.yOffset = 0.0;
    aligner.tilt    = tilt[0];
    q->setAlignment( aligner );
	}
  temp->append( q );
}

if( kl[1] != 0.0 ) {
  q = new thinQuad( BRHO*kl[1] );
  if( 0.0 != tilt[1] ) {
    aligner.xOffset = 0.0;
    aligner.yOffset = 0.0;
    aligner.tilt    = tilt[1];
    q->setAlignment( aligner );
	}
  temp->append( q );
}

if( kl[2] != 0.0 ) {
  q = new thinSextupole( BRHO*kl[2]/2.0 );
  if( tilt[2] != 0.0 ) {
    aligner.xOffset = 0.0;
    aligner.yOffset = 0.0;
    aligner.tilt    = tilt[2];
    q->setAlignment( aligner );
	}
  temp->append( q );
}

if( kl[3] != 0.0 ) {
  q = new thinOctupole( BRHO*kl[3]/6.0 );
  if( 0.0 != tilt[3] ) {
    aligner.xOffset = 0.0;
    aligner.yOffset = 0.0;
    aligner.tilt    = tilt[3];
    q->setAlignment( aligner );
	}
  temp->append( q );
}

int n = temp->howMany();

if( n == 0 ) {
  elm = new marker( label.c_str() );
  delete temp;
}
else if( n == 1 ) {
  elm = temp->firstElement();
  elm->rename( label.c_str() );
  delete temp;  // This should not delete the element.
}
else {
  elm = temp;
}

return elm;

}



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt*  XsifParserDriver::make_solenoid(  double const& BRHO, std::string const& label,  vector<attrib_pair_t> const& attributes )  
{

  //---------------------------------------------------------------------
  // valid attributes for type SOLENOID
  // --------------------------------------------------------------------
  // L             length                [m  ]
  // KS            solenoid strength     
  // K1            quadrupole strength
  // TILT          tilt angle
  // APERTURE      aperture radius ?  
  //--------------------------------------------------------------------- 

  any value;
  bmlnElmnt* elm = 0;

  double length   = 0.0;
  double ks       = 0.0;
  double k1       = 0.0;
  double tilt     = 0.0;
  double aperture = 0.0;


  if ( eval( string("L"),        attributes, value) )    length   = any_cast<double>(value); 
  if ( eval( string("KS"),       attributes, value) )    ks       = any_cast<double>(value); 
  if ( eval( string("K1"),       attributes, value) )    k1       = any_cast<double>(value); 
  if ( eval( string("TILT"),     attributes, value) )    tilt     = any_cast<double>(value); 
  if ( eval( string("APERTURE"), attributes, value) )    aperture = any_cast<double>(value); 

  //elm = make_solenoid( label.c_str() , length);
  elm = 0;
  return elm;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt*  XsifParserDriver::make_hkicker( double const& BRHO, std::string const& label,  vector<attrib_pair_t> const& attributes ) 
{

 //---------------------------------------------------------------------
 // valid attributes for type HKICK
 // --------------------------------------------------------------------
 // LENGTH                                   <- extension 
 // KICK          kick                [m  ]
 // TILT          tilt angle
 //--------------------------------------------------------------------- 
 
  any value;
  bmlnElmnt* elm = 0;
  alignmentData aligner;

  double length = 0.0;
  double kck    = 0.0;
  double tilt   = 0.0;

  if ( eval( string("LENGTH"),      attributes, value) )    length  = any_cast<double>(value); 
  if ( eval( string("KICK"),        attributes, value) )    kck     = any_cast<double>(value); 
  if ( eval( string("TILT"),        attributes, value) )    tilt    = any_cast<double>(value); 

  if ( length != 0.0 || kck != 0.0 ) {
    elm = new hkick( label.c_str(), length, kck );
  } else {
    elm = new hkick(label.c_str(), kck );
  }
  
  if ( tilt != 0.0  ) {
    aligner.xOffset = 0.0;
    aligner.yOffset = 0.0;
    aligner.tilt    = tilt;
    elm->setAlignment( aligner );
  }

  return elm;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt*  XsifParserDriver::make_vkicker( double const& BRHO, std::string const& label,  vector<attrib_pair_t> const& attributes ) 
{
 //---------------------------------------------------------------------
 // valid attributes for type VKICK
 // --------------------------------------------------------------------
 // LENGTH                                   <- extension 
 // KICK          kick                [m  ]
 // TILT          tilt angle
 //--------------------------------------------------------------------- 
 
  
  any value;
  bmlnElmnt* elm = 0;
  alignmentData aligner;

  double length = 0.0;
  double kck    = 0.0;
  double tilt   = 0.0;

  if ( eval( string("LENGTH"),      attributes, value) )    length  = any_cast<double>(value); 
  if ( eval( string("KICK"),        attributes, value) )    kck     = any_cast<double>(value); 
  if ( eval( string("TILT"),        attributes, value) )    tilt    = any_cast<double>(value); 

  
  if ( length != 0.0 || kck != 0.0 ) {
    elm = new vkick( label.c_str(), length, kck );
  } else {
    elm = new vkick( label.c_str(), kck );
  }
  
  if ( tilt != 0.0 ) { 
    aligner.xOffset = 0.0;
    aligner.yOffset = 0.0;
    aligner.tilt    = tilt;
    elm->setAlignment( aligner );
  }

  return elm;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt*  XsifParserDriver::make_kicker(  double const& BRHO, std::string const& label,  vector<attrib_pair_t> const& attributes ) 
{

 //---------------------------------------------------------------------
 // valid attributes for type KICKER
 // --------------------------------------------------------------------
 // LENGTH                                   <- extension 
 // KICK          hkck                
 // KICK          vkck              
 // TILT          tilt angle
 //--------------------------------------------------------------------- 
 
  
  any value;
  bmlnElmnt* elm = 0;
  alignmentData aligner;

  double length = 0.0;
  double hkck   = 0.0;
  double vkck   = 0.0;
  double tilt   = 0.0;

  if ( eval( string("LENGTH"),      attributes, value) )    length  = any_cast<double>(value); 
  if ( eval( string("HKICK"),       attributes, value) )    hkck    = any_cast<double>(value); 
  if ( eval( string("VKICK"),       attributes, value) )    vkck    = any_cast<double>(value); 
  if ( eval( string("TILT"),        attributes, value) )    tilt    = any_cast<double>(value); 


  if ( length != 0.0 || hkck != 0.0 || vkck != 0.0 ) {
    elm = new kick(  label.c_str(), length, hkck, vkck );
  } else {
    elm = new kick(  label.c_str(), hkck, vkck );
  }
  
  if ( tilt != 0.0 ) {
    aligner.xOffset = 0.0;
    aligner.yOffset = 0.0;
    aligner.tilt    = tilt;
    elm->setAlignment( aligner );
  }
}



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt*  XsifParserDriver::make_lcavity(  double const& BRHO, std::string const& label,  vector<attrib_pair_t> const& attributes ) 
{
  
  /***************
  rfcavity( const char* Name
            double length [m]
            double RFfreq
            double,   // max energy gain per turn [eV] (strength*10**9)
            double,   // synchronous phase [radians]
            double,   // Q
            double    // R shunt impedance
          );
  ******************/

  any value;
  bmlnElmnt* elm   = 0;

  double length    =  0.0;        
  double deltae    =  0.0;
  double freq      =  0.0;
  double phi0      =  0.0;
  double eloss     =  0.0;
  double aperture  =  0.0;
  string lfile     =  "";
  string tfile     =  "";
   
  double shunt     =  0.0; // shunt impedance ?


   if ( eval( string("L"),         attributes, value) ) length   = any_cast<double>(value);
   if ( eval( string("DELTAE"),    attributes, value) ) deltae   = any_cast<double>(value);
   if ( eval( string("FREQ"),      attributes, value) ) freq     = any_cast<double>(value);
   if ( eval( string("PHI0"),      attributes, value) ) phi0     = any_cast<double>(value);
   if ( eval( string("ELOSS"),     attributes, value) ) eloss    = any_cast<double>(value);
   if ( eval( string("APERTURE"),  attributes, value) ) aperture = any_cast<double>(value);
   if ( eval( string("LFILE"),     attributes, value) ) lfile    = any_cast<string>(value);
   if ( eval( string("TFILE"),     attributes, value) ) tfile    = any_cast<string>(value);

  elm = new rfcavity(  label.c_str(), length, freq,  deltae*1.0e6, phi0*(2.0*M_PI), 0, 0);

  /***********
  std::cout <<  "L = "         << length    << std::endl;        
  std::cout <<  "DELTAE = "    << deltae    << std::endl;
  std::cout <<  "FREQ = "      << freq      << std::endl;
  std::cout <<  "PHI0 = "      << phi0      << std::endl;
  std::cout <<  "ELOSS = "     << eloss     << std::endl;
  std::cout <<  "APERTURE = "  << aperture  << std::endl;
  std::cout <<  "LFILE = "     << lfile     << std::endl;
  std::cout <<  "TFILE = "     << tfile     << std::endl;
  ***********/ 
   
  return elm;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt*  XsifParserDriver::make_rfcavity(  double const& BRHO, std::string const& label,  vector<attrib_pair_t> const& attributes ) 
{
 //----------------------------------------------------------------------------------------------
 // valid attributes for type RFCAVITY
 // ---------------------------------------------------------------------------------------------
 // L                              
 // VOLT                            
 // LAG                             
 // FREQ                              
 // HARMON                              
 // ENERGY                            
 // ELOSS                            
 // LFILE
 // TFILE
 // NBIN
 // BINMAX
 // APERTURE
 // SHUNT                        extension
 //-----------------------------------------------------------------------------------------------
  
  
  any value;
  bmlnElmnt* elm = 0;

  double length   = 0.0;
  double volt     = 0.0;
  double lag      = 0.0;
  double freq     = 0.0;
  int    harmon   = 0;
  double energy   = 0.0;
  double eloss    = 0.0;
  string lfile    = "";
  string tfile    = "";
  int    nbin     = 0;
  int    binmax   = 0;
  double aperture = 0.0;
  double shunt    = 0.0;

  if ( eval( string("L"),        attributes, value) )    length   = any_cast<double>(value); 
  if ( eval( string("VOLT"),     attributes, value) )    volt     = any_cast<double>(value); 
  if ( eval( string("LAG"),      attributes, value) )    lag      = any_cast<double>(value); 
  if ( eval( string("FREQ"),     attributes, value) )    freq     = any_cast<double>(value); 
  if ( eval( string("HARMON"),   attributes, value) )    harmon   = any_cast<int>(value); 
  if ( eval( string("ENERGY"),   attributes, value) )    energy   = any_cast<double>(value); 
  if ( eval( string("ELOSS"),    attributes, value) )    eloss    = any_cast<double>(value); 
  if ( eval( string("LFILE"),    attributes, value) )    lfile    = any_cast<string>(value); 
  if ( eval( string("TFILE"),    attributes, value) )    tfile    = any_cast<string>(value); 
  if ( eval( string("NBIN"),     attributes, value) )    nbin     = any_cast<int>(value); 
  if ( eval( string("BINMAX"),   attributes, value) )    binmax   = any_cast<int>(value); 
  if ( eval( string("APERTURE"), attributes, value) )    aperture = any_cast<double>(value); 
  if ( eval( string("SHUNT"),    attributes, value) )    shunt    = any_cast<double>(value); 

  
  elm = new rfcavity(  label.c_str(), length, 0, volt*1.0e6, lag*(2.0*M_PI), 0, shunt );
  // ......................................................................
  // NOTE: I cannot set the frequency of the rf cavity from harmonic number
  // until the revolution frequency is known; i.e. until the beamline
  // model is instantiated.  Thus, the rf cavity must be completed
  // by a registration visitor before being used.
  // ......................................................................

  // Ignored parameters: BETRF, PG, TFILL

  return elm;
  
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 bmlnElmnt*  XsifParserDriver::make_hmonitor(  double const& BRHO, std::string const& label,  vector<attrib_pair_t> const& attributes ) 
{
 //----------------------------------------------------------------------------------------------
 // valid attributes for type HMONITOR
 // ---------------------------------------------------------------------------------------------
 // LENGTH                              
 // XSERR                            
 // YSERR                           
 // XRERR                             
 // YRERR                              
 //-----------------------------------------------------------------------------------------------
                       
  
  any value;
  bmlnElmnt* elm = 0;

  double length = 0.0; 
  double xserr  = 0.0; 
  double yserr  = 0.0; 
  double xrerr  = 0.0; 
  double yrerr  = 0.0; 

  if ( eval( string("LENGTH"),   attributes, value) )    length   = any_cast<double>(value); 
  if ( eval( string("XSERR"),    attributes, value) )    xserr    = any_cast<double>(value); 
  if ( eval( string("YSERR"),    attributes, value) )    yserr    = any_cast<double>(value); 
  if ( eval( string("XRERR"),    attributes, value) )    xrerr    = any_cast<double>(value); 
  if ( eval( string("YRERR"),    attributes, value) )    yrerr    = any_cast<double>(value); 

  if( length > 0.0 ) {
    elm = new hmonitor( label.c_str(), length );
  }
  else {
    elm = new hmonitor( label.c_str() );
  }
  return elm;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 bmlnElmnt*  XsifParserDriver::make_vmonitor(  double const& BRHO, std::string const& label,  vector<attrib_pair_t> const& attributes) 
 {

 //----------------------------------------------------------------------------------------------
 // valid attributes for type HMONITOR
 // ---------------------------------------------------------------------------------------------
 // LENGTH                              
 // XSERR                            
 // YSERR                           
 // XRERR                             
 // YRERR                              
 //-----------------------------------------------------------------------------------------------
                       
  
  any value;
  bmlnElmnt* elm = 0;

  double length = 0.0; 
  double xserr  = 0.0; 
  double yserr  = 0.0; 
  double xrerr  = 0.0; 
  double yrerr  = 0.0; 

  if ( eval( string("LENGTH"),   attributes, value) )    length   = any_cast<double>(value); 
  if ( eval( string("XSERR"),    attributes, value) )    xserr    = any_cast<double>(value); 
  if ( eval( string("YSERR"),    attributes, value) )    yserr    = any_cast<double>(value); 
  if ( eval( string("XRERR"),    attributes, value) )    xrerr    = any_cast<double>(value); 
  if ( eval( string("YRERR"),    attributes, value) )    yrerr    = any_cast<double>(value); 

  if( length > 0.0 ) {
    elm = new vmonitor( label.c_str(), length );
  }
  else {
    elm = new vmonitor( label.c_str() );
  }

  return elm;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 bmlnElmnt*  XsifParserDriver::make_monitor(  double const& BRHO, std::string const& label,  vector<attrib_pair_t> const& attributes ) 
 {

 //----------------------------------------------------------------------------------------------
 // valid attributes for type MONITOR
 // ---------------------------------------------------------------------------------------------
 // LENGTH                              
 // XSERR                            
 // YSERR                           
 // XRERR                             
 // YRERR                              
 //-----------------------------------------------------------------------------------------------
                       
  
  any value;
  bmlnElmnt* elm = 0;

  double length = 0.0; 
  double xserr  = 0.0; 
  double yserr  = 0.0; 
  double xrerr  = 0.0; 
  double yrerr  = 0.0; 

  if ( eval( string("LENGTH"),   attributes, value) )    length   = any_cast<double>(value); 
  if ( eval( string("XSERR"),    attributes, value) )    xserr    = any_cast<double>(value); 
  if ( eval( string("YSERR"),    attributes, value) )    yserr    = any_cast<double>(value); 
  if ( eval( string("XRERR"),    attributes, value) )    xrerr    = any_cast<double>(value); 
  if ( eval( string("YRERR"),    attributes, value) )    yrerr    = any_cast<double>(value); 

  if( length > 0.0 ) {
    elm = new monitor(  label.c_str(), length );
  }
  else {
    elm = new monitor(  label.c_str() );
  }

  return elm;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt*  XsifParserDriver::make_instrument(  double const& BRHO, std::string const& label,  vector<attrib_pair_t> const& attributes ) 
{

  
  any value;
  bmlnElmnt* elm = 0;

  elm = 0;
  return elm; 

}
 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt*  XsifParserDriver::make_ecollimator(  double const& BRHO, std::string const& label,  vector<attrib_pair_t> const& attributes) 
{

  
  any value;
  bmlnElmnt* elm = 0;

   double length = 0.0;
   if ( eval( string("LENGTH"),   attributes, value) )    length   = any_cast<double>(value); 

   // elm = make_ecollimator(  label.c_str(), length );
   elm = 0;
   return elm; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt*  XsifParserDriver::make_rcollimator(  double const& BRHO, std::string const& label,  vector<attrib_pair_t> const& attributes) 
{
  
  any value;
  bmlnElmnt* elm = 0;

  double length = 0.0;
  if ( eval( string("LENGTH"),   attributes, value) )    length   = any_cast<double>(value); 

  //elm = make_rcollimator(  label.c_str(), length );
  elm = 0;
  return elm; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt*  XsifParserDriver::make_yrot(  double const& BRHO, std::string const& label,  vector<attrib_pair_t> const& attributes ) 
{

  
  any value;
  bmlnElmnt* elm = 0;

  double length = 0.0;
  if ( eval( string("LENGTH"),   attributes, value) )    length   = any_cast<double>(value); 

  //elm = make_yrot(  label.c_str(), length);
  elm = 0;
  return elm; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt*  XsifParserDriver::make_srot( double const& BRHO, std::string const& label,  vector<attrib_pair_t> const& attributes ) 
{
  
  any value;
  bmlnElmnt* elm = 0;

  double angle = 0.0;
  if ( eval( string("ANGLE"),   attributes, value) )    angle   = any_cast<double>(value); 

  //elm = new srot( label.c_str(), angle );
  elm = 0;
  return elm;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt*  XsifParserDriver::make_beambeam( double const& BRHO, std::string const& label,  vector<attrib_pair_t> const& attributes) 
{
  
  any value;
  bmlnElmnt* elm = 0;

  double length = 0.0;
  if ( eval( string("LENGTH"),   attributes, value) )    length   = any_cast<double>(value); 

  //     elm = make_beambeam(  label.c_str(), length);
  elm = 0;
  return elm;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt*  XsifParserDriver::make_matrix( double const& BRHO, std::string const& label,  vector<attrib_pair_t> const& attributes) 
{

  
  any value;
  bmlnElmnt* elm = 0;

  double length = 0.0;
  if ( eval( string("LENGTH"),   attributes, value) )    length   = any_cast<double>(value); 

  // elm = make_matrix( label.c_str(), length);
  elm = 0;
  return elm;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt*  XsifParserDriver::make_lump( double const& BRHO, std::string const& label,  vector<attrib_pair_t> const& attributes) 
{

  any value;
  bmlnElmnt* elm = 0;

  double length = 0.0;
  if ( eval( string("LENGTH"),   attributes, value) )    length   = any_cast<double>(value); 

  //elm = make_lump( label.c_str(), length);
  elm = 0;
  return elm;
}

