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
******  Author:   Jean-Francois Ostiguy 
******            Fermilab                                           
******            ostiguy@fnal.gov                                      
******
******************************************************************************
******************************************************************************
******************************************************************************/
#include "XsifParserDriver.h"

#include <iomanip>
#include <algorithm>
#include <functional>
#include <sstream>

#include <boost/assign/std/vector.hpp> 
#include <boost/shared_ptr.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string_regex.hpp>
#include <basic_toolkit/PhysicsConstants.h>
#include <beamline/Particle.h>
#include <beamline/Alignment.h>
#include <beamline/beamline.h>
#include <beamline/bmlnElmnt.h>
#include <beamline/beamline_elements.h>
#include <cstdlib>
#include <sstream>

using namespace std;
using namespace boost;
using namespace boost::algorithm;


using std::setw;
using std::left;


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::map<std::string, boost::any> merge_attributes(std::map<std::string, boost::any> const&  old_attributes,  
                                                   std::map<std::string, boost::any> const&  new_attributes )
{  

  std::map<std::string, boost::any> attributes =  old_attributes;

  for (std::map<std::string, boost::any>::const_iterator it = new_attributes.begin();  it != new_attributes.end(); ++it ) {
    
    attributes[ it->first ] = it->second;
  }

  return  attributes; 

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



struct AttributesEqual { 
  AttributesEqual( string const& str): m_str(str) {} 

  bool operator()(  std::pair<string, boost::any> const& attr ) const
    { return iequals(attr.first, m_str); }

  string m_str;  
};


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool eval( string const& attribute,map<string,boost::any> const& attributes, any& result ) {

  // **** Returns true if attribute has been explicitly specified. 
  //      In that case, the value evaluated, is also returned via the 'result' argument. 

  if (attributes.empty()) return false; 

  map<string,boost::any>::const_iterator it;

  if ( (it = find_if(attributes.begin(), attributes.end(), AttributesEqual(attribute) )) ==  attributes.end()  ) 
     return false;

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

XsifParserDriver::XsifParserDriver()
 : m_trace_scanning(false), m_trace_parsing(false)
{

  //------------------------------------------------------------------------------------ 
  // Predefined constants in XSIF spec
  //------------------------------------------------------------------------------------

  m_constants["PI"]       = M_PI;          
  m_constants["TWOPI"]    = 2.0*M_PI;          
  m_constants["DEGRAD"]   = 180.0/M_PI;
  m_constants["RADDEG"]   = M_PI/180.0;
  m_constants["E"]        = exp(1.0);
  m_constants["EMASS"]    = PH_NORM_me;    //  ~0.510998902e-3;     electron mass in GeV
  m_constants["PMASS"]    = PH_NORM_mp;    //  ~0.938271998;        proton mass in GeV
  m_constants["MUMASS"]   = PH_NORM_mmu;   //  ~0.1056583568;       muon mass in GeV
  m_constants["CLIGHT"]   = PH_MKS_c;      //  ~2.99792458e8;       speed of light in m/s
  m_constants["QELECT"]   = PH_MKS_e;      //  ~1.602176462e-19;    elementary charge  in A-s
   
  //------------------------------------------------------------------------------------ 
  // jump table for element creation  
  //------------------------------------------------------------------------------------

  m_makefncs["DRIFT"      ] = &XsifParserDriver::make_drift;
  m_makefncs["SBEND"      ] = &XsifParserDriver::make_sbend;
  m_makefncs["RBEND"      ] = &XsifParserDriver::make_rbend;
  m_makefncs["QUADRUPOLE" ] = &XsifParserDriver::make_quadrupole;
  m_makefncs["SEXTUPOLE"  ] = &XsifParserDriver::make_sextupole;
  m_makefncs["QUADSEXT"   ] = &XsifParserDriver::make_notimplemented; 
  m_makefncs["OCTUPOLE"   ] = &XsifParserDriver::make_octupole;
  m_makefncs["MULTIPOLE"  ] = &XsifParserDriver::make_multipole;
  m_makefncs["DIMULTIPOLE"] = &XsifParserDriver::make_notimplemented; 
  m_makefncs["SOLENOID"   ] = &XsifParserDriver::make_solenoid;
  m_makefncs["RFCAVITY"   ] = &XsifParserDriver::make_rfcavity; 
  m_makefncs["LCAVITY"    ] = &XsifParserDriver::make_lcavity;
  m_makefncs["SROT"       ] = &XsifParserDriver::make_srot;
  m_makefncs["YROT"       ] = &XsifParserDriver::make_yrot;
  m_makefncs["HKICKER"    ] = &XsifParserDriver::make_hkicker;
  m_makefncs["VKICKER"    ] = &XsifParserDriver::make_vkicker;
  m_makefncs["KICKER"     ] = &XsifParserDriver::make_kicker;
  m_makefncs["GKICK"      ] = &XsifParserDriver::make_gkick;
  m_makefncs["HMONITOR"   ] = &XsifParserDriver::make_hmonitor;
  m_makefncs["VMONITOR"   ] = &XsifParserDriver::make_vmonitor;
  m_makefncs["MONITOR"    ] = &XsifParserDriver::make_monitor;
  m_makefncs["BLMONITOR"  ] = &XsifParserDriver::make_blmonitor;
  m_makefncs["PROFILE"    ] = &XsifParserDriver::make_notimplemented; 
  m_makefncs["WIRE"       ] = &XsifParserDriver::make_wire;
  m_makefncs["SLMONITOR"  ] = &XsifParserDriver::make_notimplemented; 
  m_makefncs["IMONITOR"   ] = &XsifParserDriver::make_notimplemented;   
  m_makefncs["INSTRUMENT" ] = &XsifParserDriver::make_instrument;
  m_makefncs["MARKER"     ] = &XsifParserDriver::make_marker;
  m_makefncs["ECOLLIMATOR"] = &XsifParserDriver::make_matrix;
  m_makefncs["RCOLLIMATOR"] = &XsifParserDriver::make_rcollimator;
  m_makefncs["ARBITLM"    ] = &XsifParserDriver::make_notimplemented; 
  m_makefncs["MTWISS"     ] = &XsifParserDriver::make_notimplemented; 
  m_makefncs["MATRIX"     ] = &XsifParserDriver::make_matrix;

  //----------------------------------------------------
  // EXTENSIONS 
  // Elements types not formally part of the XSIF spec.
  //----------------------------------------------------

  m_makefncs["BEAMBEAM"   ] = &XsifParserDriver::make_beambeam;
  m_makefncs["LUMP"       ] = &XsifParserDriver::make_lump;

  init();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void
XsifParserDriver::init() 
{

  //------------------------------------------------------------------------------------ 
  // valid command/element attributes
  //------------------------------------------------------------------------------------

  using namespace boost::assign;

  valid_attributes_["BEAM"       ] += "LABEL", "ET", "EX", "EY", "EXN", "EYN", "SIGT", "SIGE", "KBUNCH", "NPART", "BCURRENR", "BUNCHED", "RADIATE";
  valid_attributes_["BETA0"      ] += "LABEL", "BETX", "ALFX", "MUX",  "BETY", "ALFY", "MUY", "DX",    "DPX", "X", "PX", "Y", "PY", "T", "PT", "WX", 
                                      "PHIX",  "DMUX", "WY",   "PHIY", "DMUY", "DDX",  "DDY", "DDPX", "DDPY";
  valid_attributes_["DRIFT"      ] += "LABEL", "L"; 
  valid_attributes_["SBEND"      ] += "LABEL", "L", "ANGLE",  "K1",  "E1", "E2", "TILT", "K2", "H1", "H2", "HGAP", "FINT", "HGAPX", "FINTX";
  valid_attributes_["RBEND"      ] += "LABEL", "L", "ANGLE",  "K1",  "E1", "E2", "TILT", "K2", "H1", "H2", "HGAP", "FINT", "HGAPX", "FINTX";
  valid_attributes_["QUADRUPOLE" ] += "LABEL", "L", "K1",          "TILT",  "APERTURE";
  valid_attributes_["SEXTUPOLE"  ] += "LABEL", "L", "K2",          "TILT",  "APERTURE";
  valid_attributes_["QUADSEXT"   ] += "LABEL", "L", "K1",  "K2",   "TILT",  "APERTURE";
  valid_attributes_["OCTUPOLE"   ] += "LABEL", "L", "K3",          "TILT",  "APERTURE";
  valid_attributes_["MULTIPOLE"  ] += "LABEL", "L", "LRAD",   
    "K0L","K1L", "K2L","K3L", "K4L", "K5L", "K6L", "K7L", "K8L", "K9L", "K10L", "K11L", "K12L", "K13L", "K14L", "K15L", "K16L", "K17L", "K18L", "K19L", "K20L",  
    "T0", "T1",  "T2", "T3",  "T4",  "T5",  "T6",  "T7",  "T8",  "T9",  "T10",  "T11",  "T12",  "T13",  "T14",  "T15",  "T16",  "T17",  "T18",  "T19",  "T20",  
    "KZL", "KRL", "THETA", "Z", "SCALEFAC",  "TILT", "APERTURE";

  valid_attributes_["DIMULTIPOLE"] += "LABEL", "L", "LRAD",   
    "K0", "K1", "K2","K3", "K4", "K5", "K6", "K7", "K8", "K9", "K10", "K11", "K12", "K13", "K14", "K15", "K16", "K17", "K18", "K19", "K20",  
    "T0", "T1", "T2","T3", "T4", "T5", "T6", "T7", "T8", "T9", "T10", "T11", "T12", "T13", "T14", "T15", "T16", "T17", "T18", "T19", "T20",  
    "SCALEFAC",  "TILT", "APERTURE";

  valid_attributes_["SOLENOID"   ] += "LABEL", "L", "KS",  "K1", "TILT", "APERTURE";
  valid_attributes_["RFCAVITY"   ] += "LABEL", "L", "VOLTS", "LAG", "FREQ", "HARMON", "ENERGY", "ELOSS", "LFILE", "TFILE", "NBIN", "BINMAX", "APERTURE";
  valid_attributes_["LCAVITY"    ] += "LABEL", "L", "E0", "DELTAE", "PHI0", "FREQ", "ELOSS", "LFILE", "TFILE", "NBIN", "BINMAX", "APERTURE", "WAKEON"; 
                                 // WAKEON is an extension    

  valid_attributes_["SROT"       ] += "LABEL", "ANGLE";
  valid_attributes_["YROT"       ] += "LABEL", "ANGLE";
  valid_attributes_["HKICKER"    ] += "LABEL", "L", "KICK", "TILT"; // Note: L is an extension of the XSIF spec
  valid_attributes_["VKICKER"    ] += "LABEL", "L", "KICK", "TILT"; // Note: L is an extension of the XSIF spec
  valid_attributes_["KICKER"     ] += "LABEL", "L", "KICK", "TILT"; // Note: L is an extension of the XSIF spec
  valid_attributes_["GKICK"      ] += "LABEL", "L", "DX",  "DXP",  "DY",  "DYP",  "DL",  "DP",  "ANGLE", "DZ", "V", "T";      

  valid_attributes_["HMONITOR"   ] += "LABEL", "L", "XSERR", "YSERR",  "XRERR", "YRERR";
  valid_attributes_["VMONITOR"   ] += "LABEL", "L", "XSERR", "YSERR",  "XRERR", "YRERR";
  valid_attributes_["MONITOR"    ] += "LABEL", "L", "XSERR", "YSERR",  "XRERR", "YRERR";
  valid_attributes_["BLMONITOR"  ] += "LABEL", "L";
  valid_attributes_["PROFILE"    ] += "LABEL", "L";
  valid_attributes_["WIRE"       ] += "LABEL", "L";
  valid_attributes_["SLMONITOR"  ] += "LABEL", "L";
  valid_attributes_["IMONITOR"   ] += "LABEL", "L";
  valid_attributes_["INSTRUMENT" ] += "LABEL", "L";

  valid_attributes_["MARKER"     ] += "LABEL"; 
  valid_attributes_["ECOLLIMATOR"] += "LABEL", "L", "XSIZE", "YSIZE";
  valid_attributes_["RCOLLIMATOR"] += "LABEL", "L";

  valid_attributes_["ARBITLM"    ] += "LABEL", "L", "P1", "P2","P3","P4","P5","P6","P7","P8","P9","P10",
                                                    "P11","P12","P13","P14","P15","P16","P17","P18","P19", "P20";
  valid_attributes_["MTWISS"     ] += "LABEL", "L", "MUX", "BETAX", "ALPHAX", "MUY", "BETAY", "ALPHAY";
  valid_attributes_["MATRIX"     ] += "LABEL", "L", "R11", "R12", "R13", "R14", "R15", "R16",
                                                    "R21", "R22", "R23", "R24", "R25", "R26",
                                                    "R31", "R32", "R33", "R34", "R35", "R36",
                                                    "R41", "R42", "R43", "R44", "R45", "R46",
                                                    "R51", "R52", "R53", "R54", "R55", "R56",
                                                    "R61", "R62", "R63", "R64", "R65", "R66"; // L is an extension of the XSIF spec.  

  //----------------------------------------------------
  // EXTENSIONS 
  // Elements types not formaly part of the XSIF spec.
  //----------------------------------------------------

  valid_attributes_["BEAMBEAM"   ] += "LABEL", "L";
  valid_attributes_["LUMP"       ] += "LABEL", "L";


}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

XsifParserDriver::~XsifParserDriver() 
{
  xsif_yylex_destroy(m_yyscanner);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool XsifParserDriver::validate_attributes( std::string const& name, std::map<std::string, std::vector<std::string> > const& attributes) const
{
  using namespace std;
  
  std::vector<string> const& vattr =  const_cast<map<string,vector<string> >& >(valid_attributes_)[name];   

  for (   std::map<std::string, std::vector<std::string> >::const_iterator it = attributes.begin();  
                                                                           it != attributes.end(); ++it ) {
    if ( vattr.end() != std::find_if(vattr.begin(), vattr.end(), bind2nd(equal_to<string>(), it->first ) ) ) 
       return false; // **** this should throw   
  }

  return true;   
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int XsifParserDriver::parse(string const& f)
{
  int bufsize     = 1024; 
  char* saved_cwd = new char[bufsize];

#ifdef _WIN32
  GetFullPathName( f.cstr(), bufsize, saved_cwd,  0); // this is done in order to reliably receive the drive letter prefix.  
  for (int i=strlen(saved_cwd)-1; i >=0; --i) {
   if (saved_cwd[i] == '\\') { saved_cwd[i] = 0; break; 
  }

#else
  getcwd( saved_cwd, bufsize);
#endif
 
  m_file = f; 
  m_input_is_file = true;    

  xsif_yylex_init(&m_yyscanner);

  xsif_yy::XsifParser parser(m_yyscanner, *this);

  parser.set_debug_level(m_trace_parsing);

  scan_begin( m_yyscanner);

  int ret = parser.parse();

  scan_end(  m_yyscanner);
  
#ifdef _WIN32
  setCurrentDirectory(saved_cwd);
#else
  chdir(saved_cwd);  
#endif 

  delete [] saved_cwd; 
  return ret;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int XsifParserDriver::parseFromBuffer(char const* buffer)
{
 
  int bufsize     = 1024; 
  char* saved_cwd = new char[bufsize];

#ifdef _WIN32
  GetFullPathName( f.cstr(), bufsize, saved_cwd,  0); // this is done in order to reliably receive the drive letter prefix.  
  for (int i=strlen(saved_cwd)-1; i >=0; --i) {
   if (saved_cwd[i] == '\\') { saved_cwd[i] = 0; break; 
  }
#else
  getcwd( saved_cwd, bufsize);
#endif
 
  m_buffer = buffer; 
  m_input_is_file = false;    
   
  xsif_yylex_init(&m_yyscanner);

  xsif_yy::XsifParser parser(m_yyscanner, *this);

  parser.set_debug_level(m_trace_parsing);

  scan_begin( m_yyscanner);

  int ret = parser.parse();

  scan_end(  m_yyscanner);
  
#ifdef _WIN32
  setCurrentDirectory(saved_cwd);
#else
  chdir(saved_cwd);  
#endif 

  delete [] saved_cwd; 
  return ret;

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void XsifParserDriver::error(  xsif_yy::location const& l, const string& m) const
{

  stringstream ss; 

  ss <<  "XSIF Parser error : " << l  << m << endl;

  xsif_yy::location loc;

  while( !m_locations_stack.empty() ) {

    loc = m_locations_stack.top();
    m_locations_stack.pop();

    ss <<"Included from : " << loc << endl;

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

  throw ParserException( __FILE__, __LINE__, "XsifParserDriver::error()", ss.str().c_str() );  
 
} 


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void XsifParserDriver::error( const string& m) 
{

  cerr << "XsifParserDriver::error( const string& m): " << m << endl;
  
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

vector<string>* XsifParserDriver::instantiateAnonymousLine( xsif_yy::location const& yyloc, vector<string> const & elements, int ntimes) 
{

 //------------------------------------------------------------------------------------------------------------------------------------
 // An "anonymous" line is created when a statement like N*( elem1, elm2 ... elmn) etc is encountered.
 // The N copies of the lines are expanded into a *single* line as follows: 
 // ( elm1, elm2, ....,elmn, elm1, elm2, ....,elmn, elm1, elm2, ....,elmn, ...)
 // -------------------------------------------------------------------------------------------------------------------------------------  
 
 vector<string>* anonymous  = new  vector<string>;

 enum ElementOrder { FORWARD, REVERSE };
 
  ElementOrder     order = FORWARD;
  if (ntimes < 0 ) order = REVERSE; 

  switch (order) {

  case FORWARD:
 
      for (int i=0; i < abs(ntimes); ++i ) {   
         for ( vector<string>::const_iterator it = elements.begin(); it != elements.end(); ++it ){
	   anonymous->push_back( *it );   
	 }
      }
      break;

  case REVERSE:  
      for (int i=0; i < abs(ntimes); ++i ) {   
         for ( vector<string>::const_reverse_iterator it = elements.rbegin(); it != elements.rend(); ++it ){
	   anonymous->push_back( *it );   
	 }
      }
      break;
  }

  return anonymous;
 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


BmlPtr XsifParserDriver::instantiateLine( xsif_yy::location const& yyloc, string const& name, vector<string> const & elements ) 
{ 

  beamline* bl = new beamline( name.c_str() ); 
  bl->setMomentum( m_momentum); 

  map<string,  BmlPtr>::iterator bml_it;  
  map<string, ElmData>::iterator elm_it;  
  

  for ( vector<string>::const_iterator it=elements.begin(); it != elements.end(); ++it ) {

     if( (bml_it = m_lines.find(*it) )    !=  m_lines.end() ) {
  
          bl->append( BmlPtr( bml_it->second->Clone() ));   
     } 
     else if ( (elm_it = m_elements.find(*it) ) !=  m_elements.end() ){ 
          bl->append( ElmPtr( elm_it->second.elm->Clone() ));  
     }
     else if( ( (*it)[0] == '-' ) && (( bml_it = m_lines.find( (*it).substr(1)) ) !=  m_lines.end() )) {   // a reversed line 

       BmlPtr bml( bml_it->second->reverse() );
       bml->rename( "[" + (*it) + "]" );  
       bl->append(bml);

     } 
     else if( contains((*it), "(") ) {
          bl->append( expandLineMacro(yyloc, *it) );
     } 
          else {
     delete bl;
          error( yyloc, string(" Error: Element ") + (*it) + string(" is undefined."));
     }
  } 
 
  return BmlPtr(bl);
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void XsifParserDriver::addLineToDictionary( std::string const& label, BmlPtr& bml ) 
{
  m_lines[label] = BmlPtr(bml);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void XsifParserDriver::addElmToDictionary( std::string const& label, ElmPtr& elm ,std::map<std::string, boost::any> const& attributes ) 
{
  m_elements[label] = ElmData( elm, attributes );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

  
void XsifParserDriver::defineLineMacro(xsif_yy::location const& yyloc, string const& name_and_args, vector<string> const& elements ) { 

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


BmlPtr XsifParserDriver::expandLineMacro(xsif_yy::location const& yyloc, string const& name_and_args) 
{ 

  //-------------------------------------- --------------------------------------------------------------------
  // Recursively expands the line defined by the macro call 'name_and_args', which includes the parentheses and 
  // comma separated arguments. If the line does not exists it is created; otherwise, the existing one is cloned. 
  // The expanded line is given a name formed by concatenating its basename and its arguments.
  // Upon return, 'expanded line' contains a text version of the expanded line. 
  // -----------------------------------------------------------------------------------------------------------

  vector<string> expanded_line; 

  string macro_name = name_and_args;
 
  iterator_range<string::iterator> ir = find_regex(macro_name, regex("\\(.*\\)") ); 

  string basename(macro_name.begin(), ir.begin() );
  string args( ++ir.begin(), --ir.end() );

  //----------------------------------------
  /* determine the number of arguments */
  //----------------------------------------

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
 
  BmlPtr bl( new beamline ( name_and_args.c_str() ) ); 

  bl->setMomentum( m_momentum );

  map<string,  ElmData>::iterator elm_it;
  map<string,   BmlPtr>::iterator bml_it;

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
	
       bl->append( BmlPtr( bml_it->second->Clone() ) );  // is cloning required here ????

     } 
     else if ( (elm_it = m_elements.find(*ite)) != m_elements.end() ) {   
	
       bl->append( ElmPtr( elm_it->second.elm->Clone() ) ); //  is cloning required here ????

     }
     else {
       error(yyloc, string("Error: element ") + (*ite) + string(" is undefined. ") );

     }
   
  }

  return bl;  

}
  

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BmlPtr XsifParserDriver::getLine(string label) { 

  if  ( m_lines.find(label) ==  m_lines.end() ) return BmlPtr(); // line not found
  return m_lines[label]; 
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

   
double  XsifParserDriver::getElmAttributeVal( xsif_yy::location const& yyloc, string const& elm, string const& attrib) const
{

   //------------------------------------------------------------------------------------------
   // WARNING:
   // this is *incomplete*. At the moment, the only attribute which is recognized is the length.
   //-------------------------------------------------------------------------------------------

     std::map<std::string, ElmData>::const_iterator it_elm = m_elements.find( elm );

     if ( it_elm == m_elements.end() ) 
     { 
       error(yyloc, string("Error: element attribute ") + elm 
                           + string("[") + attrib + string("]") +string(" is undefined.") );
     }

     return it_elm->second.elm->Length() ;
}
                                             

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// void XsifParserDriver::command_CALL     **** implementation located in Xsif_lex.l ****    

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// void XsifParserDriver::command_RETURN   **** implementation located in Xsif_lex.l ****

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 void XsifParserDriver::command_BEAM( xsif_yy::location const& yyloc,std::map<std::string, boost::any> const& attributes) {

  //-----------------------------------------------------------------------------------------------------------------
  //    BEAM, PARTICLE=name, ENERGY=real,PC=real,GAMMA=real,&
  //    EX=real,EXN=real,EY=real,EYN=real,ET=real,SIGT=real,SIGE=real,&
  //    KBUNCH=integer,NPART=real,BCURRENT=real,&
  //    BUNCHED=logical,RADIATE=logical
  //------------------------------------------------------------------------------------------------------------------ 

  bool    pc_defined   = false;
  bool gamma_defined   = false;
  bool energy_defined  = false;
 
  any value;

  string pname    =  eval( string("PARTICLE"),     attributes, value) ? any_cast<string>(value) : string("PROTON"); // default is proton 
  to_upper(pname);
 
  double et       = eval( string("ET"),         attributes, value) ? any_cast<double> (value): 0.0; // longitudinal emittance
  double ex       = eval( string("EX"),         attributes, value) ? any_cast<double> (value): 0.0;
  double ey       = eval( string("EY"),         attributes, value) ? any_cast<double> (value): 0.0; 
  double exn      = eval( string("EXN"),        attributes, value) ? any_cast<double> (value): 0.0;
  double eyn      = eval( string("EYN"),        attributes, value) ? any_cast<double> (value): 0.0;
  double sigt     = eval( string("SIGT"),       attributes, value) ? any_cast<double> (value): 0.0;
  double sige     = eval( string("SIGE"),       attributes, value) ? any_cast<double> (value): 0.0;
  int    kbunch   = eval( string("KBUNCH"),     attributes, value) ? any_cast<double> (value): 0.0;
  double npart    = eval( string("NPART"),      attributes, value) ? any_cast<double> (value): 0.0;
  double bcurrent = eval( string("BCURRENR"),   attributes, value) ? any_cast<double> (value): 0.0;
  bool   bunched  = eval( string("BUNCHED"),    attributes, value) ? any_cast<bool>   (value): false;
  bool   radiate  = eval( string("RADIATE"),    attributes, value) ? any_cast<bool>   (value): false;
 
  std::auto_ptr<Particle> p = std::auto_ptr<Particle>(new Proton(0.0));  

  if ( iequals( pname, "PROTON"    )  ) { m_particle_type_name = pname;  p = std::auto_ptr<Particle>(new Proton(0.0)    );  }  
  if ( iequals( pname, "ANTIPROTON")  ) { m_particle_type_name = pname;  p = std::auto_ptr<Particle>(new AntiProton(0.0));  }  
  if ( iequals( pname, "ELECTRON"  )  ) { m_particle_type_name = pname;  p = std::auto_ptr<Particle>(new Electron(0.0)  );  }  
  if ( iequals( pname, "POSITRON"  )  ) { m_particle_type_name = pname;  p = std::auto_ptr<Particle>(new Positron(0.0)  );  }  
  if ( iequals( pname, "MUON"      )  ) { m_particle_type_name = pname;  p = std::auto_ptr<Particle>(new Muon(0.0)      );  }  
  if ( iequals( pname, "ANTIMUON"  )  ) { m_particle_type_name = pname;  p = std::auto_ptr<Particle>(new AntiMuon(0.0)  );  }  

  double energy   = (energy_defined = eval( string("ENERGY"),     attributes, value) ) ? any_cast<double>(value) : 0.0; 

  if ( !energy_defined ) {
         energy   = (energy_defined = eval( string("E"),          attributes, value) ) ? any_cast<double>(value) : 0.0; 
  }

  double pc       = (pc_defined     = eval( string("PC"),         attributes, value) ) ? any_cast<double>(value) : 0.0; 
  double gamma    = (gamma_defined  = eval( string("GAMMA"),      attributes, value) ) ? any_cast<double>(value) : 1.0; 

  if ( energy_defined ) { p->SetReferenceEnergy( energy );                        }
  if ( gamma_defined )  { p->SetReferenceEnergy( p->ReferenceEnergy()*gamma );    } 
  if ( pc_defined    )  { p->SetReferenceMomentum(pc);                            }

  
   m_BRHO          = p->ReferenceBRho();
   m_momentum      = p->ReferenceMomentum();

   Expression exp; exp.insert(ExprData(m_BRHO));

   m_variables["__BRHO"] = exp; 

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void XsifParserDriver::command_BETA0( xsif_yy::location const& yyloc,std::map<std::string, boost::any> const& attributes) 
{

  //---------------------------------------------------------------------------
  //  Attributes for element type BETA0
  //---------------------------------------------------------------------------
  //
  // BETX   initial beta_x  [m]
  // ALFX   initial alpha_x 
  // MUX    initial phase   [rad ]
  // BETY   initial beta_y  [m]
  // ALFY   initial alpha_y
  // MUY    initial mu_y
  // DX     initial hor dispersion
  // DPX    initial hor dispersion derivative,
  // DY     initial ver dispersion
  // DPY    initial ver dispersion derivative
  // X      initial hor position x
  // PX     initial hor momentum 
  // Y      initial ver position
  // PY     initial ver momentum
  // T      initial (long) time
  // PT     initial long momentum
  // WX     
  // PHIX   
  // DMUX   
  // WY     
  // PHIY   
  // DMUY   
  // DDX    
  // DDY    
  // DDPX   
  // DDPY   
  //--------------------------------------------------------------------

  any value;

  double betx    = 0.0; bool attribute_betx       = false; 
  double alfx    = 0.0; bool attribute_alfx       = false; 
  double mux     = 0.0; bool attribute_mux        = false; 
  double bety    = 0.0; bool attribute_bety       = false; 
  double alfy    = 0.0; bool attribute_alfy       = false; 
  double muy     = 0.0; bool attribute_muy        = false; 
  double dx      = 0.0; bool attribute_dx         = false; 
  double dpx     = 0.0; bool attribute_dpx        = false; 
  double dy      = 0.0; bool attribute_dy         = false; 
  double dpy     = 0.0; bool attribute_dpy        = false; 
  double x       = 0.0; bool attribute_x          = false; 
  double px      = 0.0; bool attribute_px         = false; 
  double y       = 0.0; bool attribute_y          = false; 
  double py      = 0.0; bool attribute_py         = false; 
  double t       = 0.0; bool attribute_t          = false; 
  double pt      = 0.0; bool attribute_pt         = false; 
  double wx      = 0.0; bool attribute_wx         = false; 
  double phix    = 0.0; bool attribute_phix       = false; 
  double dmux    = 0.0; bool attribute_dmux       = false; 
  double wy      = 0.0; bool attribute_wy         = false; 
  double phiy    = 0.0; bool attribute_phiy       = false; 
  double dmuy    = 0.0; bool attribute_dmuy       = false; 
  double ddx     = 0.0; bool attribute_ddx        = false; 
  double ddy     = 0.0; bool attribute_ddy        = false; 
  double ddpx    = 0.0; bool attribute_ddpx       = false; 
  double ddpy    = 0.0; bool attribute_ddpy       = false; 

  if ( attribute_betx = eval( string("BETX"),     attributes, value) )    { betx       = any_cast<double>(value); } 
  if ( attribute_alfx = eval( string("ALFX"),     attributes, value) )    { alfx       = any_cast<double>(value); } 
  if ( attribute_mux  = eval( string("MUX" ),     attributes, value) )    { mux        = any_cast<double>(value); } 
  if ( attribute_bety = eval( string("BETY"),     attributes, value) )    { bety       = any_cast<double>(value); } 
  if ( attribute_alfy = eval( string("ALFY"),     attributes, value) )    { alfy       = any_cast<double>(value); } 
  if ( attribute_muy  = eval( string("MUY" ),     attributes, value) )    { muy        = any_cast<double>(value); } 
  if ( attribute_dx   = eval( string("DX"  ),     attributes, value) )    { dx         = any_cast<double>(value); } 
  if ( attribute_dpx  = eval( string("DPX" ),     attributes, value) )    { dpx        = any_cast<double>(value); } 
  if ( attribute_dy   = eval( string("DY"  ),     attributes, value) )    { dy         = any_cast<double>(value); } 
  if ( attribute_dpy  = eval( string("DPY" ),     attributes, value) )    { dpy        = any_cast<double>(value); } 
  if ( attribute_x    = eval( string("X"   ),     attributes, value) )    { x          = any_cast<double>(value); } 
  if ( attribute_px   = eval( string("PX"  ),     attributes, value) )    { px         = any_cast<double>(value); } 
  if ( attribute_y    = eval( string("Y"   ),     attributes, value) )    { y          = any_cast<double>(value); }
  if ( attribute_py   = eval( string("PY"  ),     attributes, value) )    { py         = any_cast<double>(value); } 
  if ( attribute_t    = eval( string("T"   ),     attributes, value) )    { t          = any_cast<double>(value); } 
  if ( attribute_pt   = eval( string("PT"  ),     attributes, value) )    { pt         = any_cast<double>(value); } 
  if ( attribute_wx   = eval( string("WX"  ),     attributes, value) )    { wx         = any_cast<double>(value); } 
  if ( attribute_phix = eval( string("PHIX"),     attributes, value) )    { phix       = any_cast<double>(value); } 
  if ( attribute_dmux = eval( string("DMUX"),     attributes, value) )    { dmux       = any_cast<double>(value); } 
  if ( attribute_wy   = eval( string("WY"  ),     attributes, value) )    { wy         = any_cast<double>(value); } 
  if ( attribute_phiy = eval( string("PHIY"),     attributes, value) )    { phiy       = any_cast<double>(value); } 
  if ( attribute_dmuy = eval( string("DMUY"),     attributes, value) )    { dmuy       = any_cast<double>(value); } 
  if ( attribute_ddx  = eval( string("DDX" ),     attributes, value) )    { ddx        = any_cast<double>(value); } 
  if ( attribute_ddy  = eval( string("DDY" ),     attributes, value) )    { ddy        = any_cast<double>(value); } 
  if ( attribute_ddpx = eval( string("DDPX"),     attributes, value) )    { ddpx       = any_cast<double>(value); }
  if ( attribute_ddpy =	eval( string("DDPY"),     attributes, value) )    { ddpy       = any_cast<double>(value); }


  initial_values_.arcLength        = 0.0;
  initial_values_.dispersion.x     = dx;
  initial_values_.dispersion.y     = dy;
  initial_values_.dispersion.xp    = dpx; 
  initial_values_.dispersion.yp    = dpy;
  initial_values_.beta.hor         = betx;
  initial_values_.beta.ver         = bety;
  initial_values_.alpha.hor        = alfx;
  initial_values_.alpha.ver        = alfy;
  initial_values_.psi.hor          = mux;
  initial_values_.psi.ver          = muy;

  std::cout << "setting initial values :" <<  betx << std::endl;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 //------------------------------------------------------------------------------------------- 
 // The class Ordering below defines a function object needed for std::equal_range(). 
 // We define Ordering in the anonymous namespace. 
 // ---------------------------------------------------------------------------------------- 


namespace {

 class Ordering : public binary_function< pair<string, XsifParserDriver::make_fnc_ptr> const&,  
                                          pair<string, XsifParserDriver::make_fnc_ptr> const&, bool > { 

 public: 

    bool operator()( pair<string, XsifParserDriver::make_fnc_ptr> const& lhs, 
                     pair<string, XsifParserDriver::make_fnc_ptr> const& rhs ) const

   { return (lhs.first < rhs.first); } 

 };


}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ElmPtr
XsifParserDriver::instantiateElement(xsif_yy::location const& yyloc, string const& label, string const& type,  map<string,boost::any> const & attributes ) 
{

 // ------------------------------------------------------------------------------------------
 // search for a match for type among the existing elements. 
 // If found, retrieve previous attributes and override with 
 // new ones if necessary
 // ------------------------------------------------------------------------------------------

 ConstElmPtr user_defined_elm;  

 map<string, ElmData>::iterator elm_it = m_elements.find(type);
 
 if ( elm_it !=  m_elements.end() ) {
   double BRHO   = m_variables["__BRHO"].evaluate();
   string basic_type(elm_it->second.elm->getTag()); // use label instead of intrinsic CHEF element type
                                                    // tags can go away once attributes can be uniformly set 
                                                    // element clones  
   to_upper(basic_type);

   std::map<std::string, boost::any> new_attributes = merge_attributes( elm_it->second.attributes, attributes);

   ElmPtr elm = (this->*m_makefncs[basic_type])( user_defined_elm,  BRHO,  label, new_attributes);   // unambiguous match;

   addElmToDictionary( label, elm, new_attributes ); 

   return elm;
 }
 
 // ------------------------------------------------------------------------------------------
 // if no match was found, then type must be a basic type. Look for closest unambiguous match
 // ------------------------------------------------------------------------------------------
  
  pair<string, XsifParserDriver::make_fnc_ptr> search (type, 0);

  pair< map<string, XsifParserDriver::make_fnc_ptr>::iterator,  map<string, XsifParserDriver::make_fnc_ptr>::iterator> result = 
                                                       equal_range( m_makefncs.begin(), m_makefncs.end(), search, Ordering() ); 
 

 ElmPtr elm;
 std::stringstream msg;

 if ( ( result.first != m_makefncs.end() ) && (distance(result.first, result.second) < 2)) {
    
    std::string::const_iterator it_res = result.first->first.begin();

    bool match = true;

    for ( std::string::const_iterator it = type.begin(); it != type.end(); ++it, ++it_res) {
      if (  it_res   ==  result.first->first.end() ) { match = false;  break; } 
      if ( (*it)     !=  (*it_res) )                 { match = false;  break; } 

    }

    if ( !match ) {
      msg.str(""); 
      msg << "Element type \"" << type << "\" is unknown. " << std::ends;
      error (yyloc, msg.str() );
    }

    boost::any value;
    double length = ( eval( string("L"),  attributes, value) )  ?  any_cast<double>(value) : 0.0;

    if ( length < 0.0) { 
      msg.str(""); 
      msg << "Element " << label << " has negative length attribute L = " << length << std::ends; 
      error (  yyloc, msg.str() ); 
    }

    expr_map_t::iterator it = m_variables.find("__BRHO");  
    if ( it == m_variables.end() ) {
      msg.str(""); 
      msg << "Particle momentum has not been set. Is the BEAM statement missing or incomplete ? "<< std::ends; 
      error (  yyloc, msg.str() ); 
    }

   double BRHO   = m_variables["__BRHO"].evaluate();
   elm = (this->*m_makefncs[result.first->first])(user_defined_elm, BRHO,  label, attributes);   // unambiguous match;
 }
 else {

    msg.str(""); 
    msg << "Element type \"" << type << "\" is ambiguous. " << std::ends;
    error (yyloc, msg.str() );
 }

 addElmToDictionary( label, elm, attributes ); 

 return elm;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


ElmPtr XsifParserDriver::make_drift(  ConstElmPtr& udelm, double const& BRHO, std::string const& label, map<string,boost::any> const& attributes ) 
{
 
   any value;
   drift* elm = 0; 

   double length = 0.0;  bool attribute_length = false; 
   if ( eval( string("L"), attributes, value) )  { attribute_length = true; length = any_cast<double>(value); } 
   
   elm = (udelm) ?  dynamic_cast<drift*>( udelm->Clone() ) : new drift();           

   elm->rename( label.c_str() );
   if (  attribute_length ) elm->setLength( length );

   elm->setTag("DRIFT");
   return ElmPtr(elm);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
 
ElmPtr XsifParserDriver::make_marker(   ConstElmPtr& udelm, double const& BRHO, std::string const& label, map<string,boost::any> const& attributes ) 
{
      marker* elm = 0;

      elm = (udelm) ?  dynamic_cast<marker*>( udelm->Clone() ) : new marker();  

      elm->rename( label.c_str() );

      elm->setTag("MARKER");

      return ElmPtr( elm );


}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
 
ElmPtr  XsifParserDriver::make_sbend(   ConstElmPtr& udelm, double const& BRHO, std::string const& label, map<string,boost::any> const& attributes ) 
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
  // FINT    entrance fringe field integral (default = 0.5)
  // HGAPX   exit half-gap size
  // FINTX   exit fringe field integral
  //--------------------------------------------------------------------- 

  any value;
  bmlnElmnt* elm = 0;


  double length = 0.0;  bool attribute_length = false;
  double angle  = 0.0;  bool attribute_angle  = false; 
  double k1     = 0.0;  bool attribute_k1     = false;
  double k2     = 0.0;  bool attribute_k2     = false;
  double k3     = 0.0;  bool attribute_k3     = false;
  double tilt   = 0.0;  bool attribute_tilt   = false;
  double e1     = 0.0;  bool attribute_e1     = false;
  double e2     = 0.0;  bool attribute_e2     = false;
  alignmentData aligner;

  bool simple = true;

  if ( eval( string("L"),      attributes, value) )  { attribute_length = true;  length = any_cast<double>(value);                         }
  if ( eval( string("ANGLE"),  attributes, value) )  { attribute_angle  = true;  angle  = any_cast<double>(value);                         }
  if ( eval( string("K1"),     attributes, value) )  { attribute_k1     = true;  k1     = any_cast<double>(value); simple = ( k1 == 0.0 ); } 
  if ( eval( string("K2"),     attributes, value) )  { attribute_k2     = true;  k2     = any_cast<double>(value); simple = ( k2 == 0.0 ); }
  if ( eval( string("K3"),     attributes, value) )  { attribute_k3     = true;  k3     = any_cast<double>(value); simple = ( k3 == 0.0 ); }

  if ( eval( string("TILT"),   attributes, value) )  { tilt = value.empty() ?  M_PI/2.0 : tilt  = any_cast<double>(value); } 

  if ( eval( string("E1"),     attributes, value) )    e1     = any_cast<double>(value); 
  if ( eval( string("E2"),     attributes, value) )    e2     = any_cast<double>(value); 


  aligner.xOffset = 0.0;
  aligner.yOffset = 0.0;
  aligner.roll    = tilt;

  if( simple) {
    elm =  new sbend( label.c_str(), length, BRHO*angle/length, angle, e1, e2 );
    elm->setTag("SBEND");
    if (tilt != 0.0 ) elm->setAlignment( aligner );
    return ElmPtr( elm );
  }

  elm = new CF_sbend( label.c_str(), length, BRHO*angle/length, angle, e1, e2 );

 
  if (tilt != 0.0 ) elm->setAlignment( aligner );

  double multipoleStrength = k1*BRHO*length;

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
  
  
  elm->setTag("SBEND");
  return ElmPtr(elm);

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ElmPtr  XsifParserDriver::make_rbend(    ConstElmPtr& udelm, double const& BRHO, std::string const& label, map<string,boost::any> const& attributes ) 
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
  alignmentData aligner;

  bool simple = true;

  if ( eval( string("L"),      attributes, value) )    length = any_cast<double>(value); 
  if ( eval( string("ANGLE"),  attributes, value) )    angle  = any_cast<double>(value); 
  if ( eval( string("K1"),     attributes, value) )  { k1     = any_cast<double>(value); simple = false; } 
  if ( eval( string("K2"),     attributes, value) )  { k2     = any_cast<double>(value); simple = false; }
  if ( eval( string("K3"),     attributes, value) )  { k3     = any_cast<double>(value); simple = false; }
  if ( eval( string("TILT"),   attributes, value) )  {  if (value.empty() ) 
                                                           { simple = false; tilt = M_PI/2.0; } 
                                                        else 
                                                           { tilt  = any_cast<double>(value); simple = ( tilt == 0.0); } 
                                                     }; 

  if ( eval( string("E1"),     attributes, value) )    e1     = any_cast<double>(value); 
  if ( eval( string("E2"),     attributes, value) )    e2     = any_cast<double>(value); 

  if( simple ) {
    if( (0.0 == e1) && (0.0 == e2) ) {
      elm = new rbend( label.c_str(), length, BRHO*(2.0*sin(0.5*angle))/length, angle );
	}
    else {
      elm = new rbend( label.c_str(), length, BRHO*(2.0*sin(0.5*angle))/length, angle, e1, e2 );
    }
      
    elm->setTag("RBEND");
    return ElmPtr(elm);
  }

  if( (0.0 == e1) && (0.0 == e2) ) {
      elm = new CF_rbend( label.c_str(),  length, BRHO*(2.0*sin(0.5*angle))/length, angle );
  } else {
      elm = new CF_rbend( label.c_str(), length, BRHO*(2.0*sin(0.5*angle))/length, angle, e1, e2 );
  }

  aligner.xOffset = 0.0;
  aligner.yOffset = 0.0;
  aligner.roll    = tilt;

  if (tilt != 0.0 ) elm->setAlignment( aligner );

  double multipoleStrength = k1*BRHO*length;

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

  elm->setTag("RBEND");
  return ElmPtr(elm);     
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ElmPtr  XsifParserDriver::make_quadrupole(    ConstElmPtr& udelm, double const& BRHO, std::string const& label, map<string,boost::any> const& attributes ) 
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

  if ( eval( string("L"),        attributes, value) )    length   = any_cast<double>(value); 
  if ( eval( string("K1"),       attributes, value) )    k1       = any_cast<double>(value); 
  if ( eval( string("TILT"),     attributes, value) )  {  if (value.empty() ) 
                                                           { tilt = M_PI/4.0; } 
                                                        else 
                                                           { tilt  = any_cast<double>(value); } 
                                                     }; 
  if ( eval( string("APERTURE"), attributes, value) )    aperture = any_cast<double>(value); 
  
  if ( length != 0.0 ) {
    elm = new quadrupole( label.c_str(), length, BRHO*k1 );
  } else {
    elm = new thinQuad( label.c_str(),  BRHO*k1 );  // in this case, k1 is K1L, the integrated strength 
  }
  
  if ( tilt != 0.0  ) {
    aligner.xOffset = 0.0;
    aligner.yOffset = 0.0;
    aligner.roll    = tilt;
    elm->setAlignment( aligner );
  }

  elm->setTag("QUADRUPOLE");
  return ElmPtr(elm);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ElmPtr  XsifParserDriver::make_sextupole(    ConstElmPtr& udelm,  double const& BRHO, std::string const& label, map<string,boost::any> const& attributes )
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

  if ( eval( string("L"),        attributes, value) )    length   = any_cast<double>(value); 
  if ( eval( string("K2"),       attributes, value) )    k2       = any_cast<double>(value); 
  if ( eval( string("TILT"),     attributes, value) )  {  if (value.empty() ) 
                                                           { tilt = M_PI/6.0; } 
                                                        else 
                                                           { tilt  = any_cast<double>(value); } 
                                                     }; 
  if ( eval( string("APERTURE"), attributes, value) )    aperture = any_cast<double>(value); 
  
  if ( length != 0.0 ) {
    elm = new sextupole( label.c_str(), length, BRHO*k2/2.0 );
  } else {
    elm = new thinSextupole( label.c_str(),  length ); // ???????????
  }
  
  if ( tilt != 0.0  ) {
    aligner.xOffset = 0.0;
    aligner.yOffset = 0.0;
    aligner.roll    = tilt;
    elm->setAlignment( aligner );
  }

  elm->setTag("SEXTUPOLE");
  return ElmPtr(elm);
}

 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ElmPtr  XsifParserDriver::make_octupole(    ConstElmPtr& udelm,  double const& BRHO, std::string const& label, map<string,boost::any> const& attributes )
{
  //---------------------------------------------------------------------
  // valid attributes for type OCTUPOLE
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

  if ( eval( string("L"),        attributes, value) )    length   = any_cast<double>(value); 
  if ( eval( string("K3"),       attributes, value) )    k3       = any_cast<double>(value); 
  if ( eval( string("TILT"),     attributes, value) )  {  if (value.empty() ) 
                                                           { tilt = M_PI/8.0; } 
                                                        else 
                                                           { tilt  = any_cast<double>(value); } 
                                                     }; 
  if ( eval( string("APERTURE"), attributes, value) )    aperture = any_cast<double>(value); 
  
  if ( length != 0.0 || k3 != 0.0 ) {
    elm = new octupole( label.c_str(), length, BRHO*k3/6.0 );
  } else {
    elm = new thinOctupole( label.c_str(), length);
  }
  
  if ( tilt != 0.0 ) {
    aligner.xOffset = 0.0;
    aligner.yOffset = 0.0;
    aligner.roll    = tilt;
    elm->setAlignment( aligner );
  }

  elm->setTag("OCTUPOLE");
  return ElmPtr(elm);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ElmPtr  XsifParserDriver::make_multipole(    ConstElmPtr& udelm, double const& BRHO, std::string const& label, map<string,boost::any> const& attributes )
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
ElmPtr elm;
alignmentData aligner;

ElmPtr  q;

beamline*  temp  = new beamline( label.c_str() ); 

temp->setMomentum( BRHO*PH_CNV_brho_to_p ); 

double   kl[21]; { for (int i=0; i<21; ++i)   kl[i] = 0.0; } 
double tilt[21]; { for (int i=0; i<21; ++i) tilt[i] = 0.0; }

if ( eval( string("K0L"),   attributes,   value) )    kl[0]   = any_cast<double>(value); 
if ( eval( string("K1L"),   attributes,   value) )    kl[1]   = any_cast<double>(value); 
if ( eval( string("K2L"),   attributes,   value) )    kl[2]   = any_cast<double>(value); 
if ( eval( string("K3L"),   attributes,   value) )    kl[3]   = any_cast<double>(value); 

if ( eval( string("T0"),    attributes, value) )    {  tilt[0] = (value.empty() ) ?  M_PI/2.0 : any_cast<double>(value); } 
if ( eval( string("T1"),    attributes, value) )    {  tilt[1] = (value.empty() ) ?  M_PI/4.0 : any_cast<double>(value); } 
if ( eval( string("T2"),    attributes, value) )    {  tilt[2] = (value.empty() ) ?  M_PI/6.0 : any_cast<double>(value); } 
if ( eval( string("T3"),    attributes, value) )    {  tilt[3] = (value.empty() ) ?  M_PI/8.0 : any_cast<double>(value); } 


if( kl[0] != 0.0 ) {
  
  q    = ElmPtr( new thin2pole("", BRHO*kl[0]) );
  
  if( tilt[0] != 0.0 ) {
    aligner.xOffset = 0.0;
    aligner.yOffset = 0.0;
    aligner.roll    = tilt[0];
    q->setAlignment( aligner );
  }

  temp->append( q );
}

if( kl[1] != 0.0 ) {
  q = ElmPtr( new thinQuad( "", BRHO*kl[1] ) );
  if( 0.0 != tilt[1] ) {
    aligner.xOffset = 0.0;
    aligner.yOffset = 0.0;
    aligner.roll    = tilt[1];
    q->setAlignment( aligner );
	}
  temp->append( q );
}

if( kl[2] != 0.0 ) {
  q = ElmPtr( new thinSextupole( "", BRHO*kl[2]/2.0 ) );
  if( tilt[2] != 0.0 ) {
    aligner.xOffset = 0.0;
    aligner.yOffset = 0.0;
    aligner.roll    = tilt[2];
    q->setAlignment( aligner );
	}
  temp->append( q );
}

if( kl[3] != 0.0 ) {
  q = ElmPtr( new thinOctupole( "", BRHO*kl[3]/6.0 ) );
  if( 0.0 != tilt[3] ) {
    aligner.xOffset = 0.0;
    aligner.yOffset = 0.0;
    aligner.roll    = tilt[3];
    q->setAlignment( aligner );
	}
  temp->append( q );
}

int n = temp->size();

if( n == 0 ) {
  elm = ElmPtr( new marker( label.c_str() ) );
}
else if( n == 1 ) {
  elm = temp->front();
  elm->rename( label.c_str() );
}
else {
  elm = BmlPtr(temp);
}

elm->setTag("MULTIPOLE");
return elm;

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ElmPtr XsifParserDriver::make_solenoid(    ConstElmPtr& udelm, double const& BRHO, std::string const& label, map<string,boost::any> const& attributes )  
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

  double length   = 0.0;
  double ks       = 0.0;
  double k1       = 0.0;
  double tilt     = 0.0;
  double aperture = 0.0;


  if ( eval( string("L"),        attributes, value) )    length   = any_cast<double>(value); 
  if ( eval( string("KS"),       attributes, value) )    ks       = any_cast<double>(value); 
  if ( eval( string("K1"),       attributes, value) )    k1       = any_cast<double>(value); // not used for now
  if ( eval( string("TILT"),     attributes, value) )    tilt     = any_cast<double>(value); // not used for now 
  if ( eval( string("APERTURE"), attributes, value) )    aperture = any_cast<double>(value); // not used for now

  Solenoid* elm = new Solenoid( label.c_str(), length, BRHO*ks );

  elm->setTag("SOLENOID");
  return ElmPtr(elm);

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ElmPtr  XsifParserDriver::make_hkicker(    ConstElmPtr& udelm, double const& BRHO, std::string const& label, map<string,boost::any> const& attributes ) 
{

 //---------------------------------------------------------------------
 // valid attributes for type HKICK
 // --------------------------------------------------------------------
 // LENGTH                                   <- extension 
 // KICK          kick                [m  ]
 // TILT          tilt angle
 //--------------------------------------------------------------------- 
 
  any value;
  hkick* elm = 0;
  alignmentData aligner;

  double length = 0.0;  bool attribute_length = false;
  double kck    = 0.0;  bool attribute_kck    = false;
  double tilt   = 0.0;  bool attribute_tilt   = false;

  if ( eval( string("L"),           attributes, value) )    { attribute_length = true; length  = any_cast<double>(value); } 
  if ( eval( string("KICK"),        attributes, value) )    { attribute_kck    = true; kck     = any_cast<double>(value); }
  if ( eval( string("TILT"),        attributes, value) )    { attribute_tilt   = true; tilt    = value.empty() ? M_PI/2.0 : any_cast<double>(value); } 

  elm = (udelm) ? dynamic_cast<hkick*>( udelm->Clone() ) : new hkick();

  elm->rename(label);
  if ( attribute_length)    elm->setLength(length);
  if ( attribute_kck   )    elm->setStrength(kck*BRHO);
  if ( attribute_tilt  ) 
  {  
    aligner.xOffset = 0.0;
    aligner.yOffset = 0.0;
    aligner.roll    = tilt;
    elm->setAlignment( aligner );
  }

  elm->setTag("HKICKER");
  return ElmPtr(elm);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ElmPtr  XsifParserDriver::make_vkicker(    ConstElmPtr& udelm, double const& BRHO, std::string const& label, map<string,boost::any> const& attributes ) 
{
 //---------------------------------------------------------------------
 // valid attributes for type VKICK
 // --------------------------------------------------------------------
 // LENGTH                                   <- extension 
 // KICK          kick                [m  ]
 // TILT          tilt angle
 //--------------------------------------------------------------------- 
 
  
  any value;
  vkick* elm = 0;
  alignmentData aligner;

  double length = 0.0;  bool attribute_length = false;
  double kck    = 0.0;  bool attribute_kck    = false;
  double tilt   = 0.0;  bool attribute_tilt   = false;

  if ( eval( string("L"),           attributes, value) )    { attribute_length = true; length  = any_cast<double>(value); } 
  if ( eval( string("KICK"),        attributes, value) )    { attribute_kck    = true; kck     = any_cast<double>(value); }
  if ( eval( string("TILT"),        attributes, value) )    { attribute_tilt   = true; tilt    = value.empty() ? M_PI/2.0 : any_cast<double>(value); } 
                          
  elm = (udelm) ? dynamic_cast<vkick*>( udelm->Clone() ) : new vkick();

  elm->rename(label);
  if ( attribute_length )    { elm->setLength(length); }
  if ( attribute_kck    )    { elm->setStrength(kck*BRHO);  }
  if ( attribute_tilt   ) 
  {  
    aligner.xOffset = 0.0;
    aligner.yOffset = 0.0;
    aligner.roll    = tilt;
    elm->setAlignment( aligner );
  }
  
  elm->setTag("VKICKER");
  return ElmPtr(elm);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ElmPtr  XsifParserDriver::make_kicker(     ConstElmPtr& udelm, double const& BRHO, std::string const& label, map<string,boost::any> const& attributes ) 
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
  kick* elm = 0;
  alignmentData aligner;

  double length = 0.0;  bool attribute_length  = false; 
  double hkck   = 0.0;  bool attribute_hkck    = false;
  double vkck   = 0.0;  bool attribute_vkck    = false;
  double tilt   = 0.0;  bool attribute_tilt    = false;

  if ( eval( string("L"),           attributes, value) )    { attribute_length = true; length  = any_cast<double>(value); } 
  if ( eval( string("HKICK"),       attributes, value) )    { attribute_hkck   = true; hkck    = any_cast<double>(value); }
  if ( eval( string("VKICK"),       attributes, value) )    { attribute_vkck   = true; vkck    = any_cast<double>(value); }
  if ( eval( string("TILT"),        attributes, value) )    { attribute_tilt   = true; tilt    = value.empty() ? M_PI/2.0 : any_cast<double>(value); } 


  elm = (udelm) ? dynamic_cast<kick*>( udelm->Clone() ) : new kick();

  elm->rename(label);
  if ( attribute_length )   elm->setLength(length);
  if ( attribute_hkck   )   elm->setHorStrength(hkck*BRHO);
  if ( attribute_vkck   )   elm->setVerStrength(vkck*BRHO);
  if ( attribute_tilt   ) 
  {  
    aligner.xOffset = 0.0;
    aligner.yOffset = 0.0;
    aligner.roll    = tilt;
    elm->setAlignment( aligner );
  }
  
  elm->setTag("KICKER");
  return ElmPtr(elm);
}



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ElmPtr XsifParserDriver::make_lcavity(     ConstElmPtr& udelm, double const& BRHO, std::string const& label, map<string,boost::any> const& attributes ) 
{
  
 //----------------------------------------------------------------------------------------------
 // valid attributes for type LCAVITY
 // ---------------------------------------------------------------------------------------------
 // L         cavity length                      
 // E0        injection energy, typically supplied only for the first cavity                            
 // DELTAE    energy gain on crest, without beam loading in MeV                          
 // PHI0      phase offset for as referennce particle in radians/2*pi (fraction of period) 
 //           phi0>0 == ref crest is ahead of the bunch                          
 // FREQ      frequency in MHz                              
 // ELOSS     beam loading factor in V/C                       
 // LFILE     Longitudinal wakefield  (Impulse response) V/C/m
 // TFILE     Transverse   wakefield  (Impulse response) V/C/m/m
 // NBIN      Number of bins used for histogramming (sampling)
 // BINMAX 
 // APERTURE
 // WAKEON    T when the wakefield is on (default = F) 
 //---------------------------------------------------------------------------------------------------
  
  any value;
  LinacCavity* elm   = 0;

  double length    =  0.0;    bool attribute_length   = false;       
  double e0        =  0.0;    bool attribute_e0       = false; 
  double deltae    =  0.0;    bool attribute_deltae   = false;
  double phi0      =  0.0;    bool attribute_phi0     = false;
  double freq      =  0.0;    bool attribute_freq     = false;
  double eloss     =  0.0;    bool attribute_eloss    = false;
  string lfile     =  "";     bool attribute_lfile    = false;
  string tfile     =  "";     bool attribute_tfile    = false;
  int    nbin      =  256;    bool attribute_nbin     = false;
  int    binmax    =  2048;   bool attribute_binmax   = false;
  double aperture  =  0.0;    bool attribute_aperture = false;
  bool   wakeon    =  true;   bool attribute_wakeon   = false; // wake is enabled by default
   
  if ( eval( string("L"),         attributes, value) ) {  attribute_length  = true; length   = any_cast<double>(value); }
  if ( eval( string("E0"),        attributes, value) ) {  attribute_e0      = true; e0       = any_cast<double>(value); }
  if ( eval( string("DELTAE"),    attributes, value) ) {  attribute_deltae  = true; deltae   = any_cast<double>(value); }
  if ( eval( string("PHI0"),      attributes, value) ) {  attribute_phi0    = true; phi0     = any_cast<double>(value); }
  if ( eval( string("FREQ"),      attributes, value) ) {  attribute_freq    = true; freq     = any_cast<double>(value); }
  if ( eval( string("ELOSS"),     attributes, value) ) {  attribute_eloss   = true; eloss    = any_cast<double>(value); }
  if ( eval( string("LFILE"),     attributes, value) ) {  attribute_lfile   = true; lfile    = any_cast<string>(value); }
  if ( eval( string("TLFILE"),    attributes, value) ) {  attribute_tfile   = true; tfile    = any_cast<string>(value); }
  if ( eval( string("NBIN")  ,    attributes, value) ) {  attribute_nbin    = true; nbin     = any_cast<double>(value); }
  if ( eval( string("BINMAX"),    attributes, value) ) {  attribute_binmax  = true; binmax   = any_cast<double>(value); }
  if ( eval( string("APERTURE"),  attributes, value) ) {  attribute_aperture= true; aperture = any_cast<double>(value); }
  if ( eval( string("WAKEON"),    attributes, value) ) {  attribute_wakeon  = true; wakeon   = any_cast<bool>(value);   }

  //------------------------- ------------------------
  // NOTE: for a linac, PHI0=0 implies being on crest.  
  //--------------------------------------------------

   elm = (udelm) ? dynamic_cast<LinacCavity*>( udelm->Clone() ) : new LinacCavity(  label.c_str(), length,  freq*1.0e6,  deltae*1.0e6, phi0*M_TWOPI, wakeon);

   elm->rename( label.c_str() );
   elm->setLength(length);
   elm->setFrequency(freq*1.0e6);
   elm->setStrength(deltae*1.0e-3);
   elm->setPhi(phi0*2.0*M_PI);
     
   elm->setTag("LCAVITY");
   return ElmPtr(elm);

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ElmPtr  XsifParserDriver::make_rfcavity(   ConstElmPtr& udelm,  double const& BRHO, std::string const& label, map<string,boost::any> const& attributes ) 
{
 //----------------------------------------------------------------------------------------------
 // valid attributes for type RFCAVITY
 // ---------------------------------------------------------------------------------------------
 // L           length in m                   
 // VOLT        cavity voltage                    
 // LAG         phase lag of the cavity w/r to a nominal particle in radians/2*pi                    
 // FREQ        frequency of the cavity in MHz                       
 // HARMON      harmonic no of the cavity                         
 // ENERGY      energy in GeV                      
 // ELOSS       energy loss factor of the cavity in V/C                     
 // LFILE       longitudinal wakefield data in V/C/m
 // TFILE       transverse wakefield data in V/C/m**2
 // NBIN        used by some programs to histogram particles
 // BINMAX      according to longtudinal positions
 // APERTURE    
 //----------------------------------------------------------------------------------------------
 // SHUNT       extension: shunt impedance
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
  if ( eval( string("HARMON"),   attributes, value) )    harmon   = any_cast<double>(value); 
  if ( eval( string("ENERGY"),   attributes, value) )    energy   = any_cast<double>(value); 
  if ( eval( string("ELOSS"),    attributes, value) )    eloss    = any_cast<double>(value); 
  if ( eval( string("LFILE"),    attributes, value) )    lfile    = any_cast<string>(value); 
  if ( eval( string("TFILE"),    attributes, value) )    tfile    = any_cast<string>(value); 
  if ( eval( string("NBIN"),     attributes, value) )    nbin     = any_cast<double>(value); 
  if ( eval( string("BINMAX"),   attributes, value) )    binmax   = any_cast<double>(value); 
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

  elm->setTag("RFCAVITY");
  return ElmPtr(elm);
  
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 ElmPtr  XsifParserDriver::make_monitor(    ConstElmPtr& udelm,  double const& BRHO, std::string const& label, map<string,boost::any> const& attributes ) 
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

  double length = 0.0;   bool attribute_length = false;
  double xserr  = 0.0;   bool attribute_xserr  = false;
  double yserr  = 0.0;   bool attribute_yserr  = false;
  double xrerr  = 0.0;   bool attribute_xrerr  = false;
  double yrerr  = 0.0;   bool attribute_yrerr  = false;

  if ( eval( string("L"),        attributes, value) )    { attribute_length = true; length  = any_cast<double>(value); } 
  if ( eval( string("XSERR"),    attributes, value) )    { attribute_xserr  = true; xserr   = any_cast<double>(value); }
  if ( eval( string("YSERR"),    attributes, value) )    { attribute_yserr  = true; yserr   = any_cast<double>(value); }
  if ( eval( string("XRERR"),    attributes, value) )    { attribute_xrerr  = true; xrerr   = any_cast<double>(value); }
  if ( eval( string("YRERR"),    attributes, value) )    { attribute_yrerr  = true; yrerr   = any_cast<double>(value); }

  elm = (udelm) ? dynamic_cast<monitor*>( udelm->Clone() ) : new monitor();

  elm->rename(label);

  if ( attribute_length ) elm->setLength( length );
  //if ( attribute_xserr )  elm->
  //if ( attribute_yserr )  elm->
  //if ( attribute_xrerr )  elm->
  //if ( attribute_xyerr )  elm->

  elm->setTag("MONITOR");
  return ElmPtr(elm);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 ElmPtr  XsifParserDriver::make_hmonitor(    ConstElmPtr& udelm, double const& BRHO, std::string const& label, map<string,boost::any> const& attributes ) 
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
  hmonitor* elm = 0;

  double length = 0.0;  bool attribute_length = false;
  double xserr  = 0.0;  bool attribute_xserr  = false;
  double yserr  = 0.0;  bool attribute_yserr  = false;
  double xrerr  = 0.0;  bool attribute_xrerr  = false;
  double yrerr  = 0.0;  bool attribute_yrerr  = false;


  if ( eval( string("L"),        attributes, value) )    { attribute_length = true; length   = any_cast<double>(value); } 
  if ( eval( string("XSERR"),    attributes, value) )    { attribute_xserr  = true; xserr    = any_cast<double>(value); }
  if ( eval( string("YSERR"),    attributes, value) )    { attribute_yserr  = true; yserr    = any_cast<double>(value); }
  if ( eval( string("XRERR"),    attributes, value) )    { attribute_xrerr  = true; xrerr    = any_cast<double>(value); }
  if ( eval( string("YRERR"),    attributes, value) )    { attribute_yrerr  = true; yrerr    = any_cast<double>(value); }

  elm = (udelm) ? dynamic_cast<hmonitor*>( udelm->Clone() ) : new hmonitor();

  elm->rename(label);

  if ( attribute_length ) elm->setLength( length );
  //if ( attribute_xserr )  elm->
  //if ( attribute_yserr )  elm->
  //if ( attribute_xrerr )  elm->
  //if ( attribute_xyerr )  elm->

  elm->setTag("HMONITOR");
  return ElmPtr(elm);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 ElmPtr  XsifParserDriver::make_vmonitor(    ConstElmPtr& udelm, double const& BRHO, std::string const& label, map<string,boost::any> const& attributes ) 
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
  vmonitor* elm = 0;

  double length = 0.0;  bool attribute_length = false;
  double xserr  = 0.0;  bool attribute_xserr  = false;
  double yserr  = 0.0;  bool attribute_yserr  = false;
  double xrerr  = 0.0;  bool attribute_xrerr  = false;
  double yrerr  = 0.0;  bool attribute_yrerr  = false;

  if ( eval( string("L"),        attributes, value) )    {  attribute_length = true; length   = any_cast<double>(value); } 
  if ( eval( string("XSERR"),    attributes, value) )    {  attribute_xserr  = true; xserr    = any_cast<double>(value); }
  if ( eval( string("YSERR"),    attributes, value) )    {  attribute_yserr  = true; yserr    = any_cast<double>(value); }
  if ( eval( string("XRERR"),    attributes, value) )    {  attribute_xrerr  = true; xrerr    = any_cast<double>(value); }
  if ( eval( string("YRERR"),    attributes, value) )    {  attribute_yrerr  = true; yrerr    = any_cast<double>(value); }

  elm = (udelm) ? dynamic_cast<vmonitor*>( udelm->Clone() ) : new vmonitor();

  elm->rename(label);
   
  if ( attribute_length ) elm->setLength( length );
  //if ( attribute_xserr )  elm->
  //if ( attribute_yserr )  elm->
  //if ( attribute_xrerr )  elm->
  //if ( attribute_xyerr )  elm->

  elm->setTag("VMONITOR");
  return ElmPtr(elm);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ElmPtr  XsifParserDriver::make_instrument(   ConstElmPtr& udelm, double const& BRHO, std::string const& label, map<string,boost::any> const& attributes ) 
{
 
  any value;
  drift* elm = 0;

  double length = 0.0;    bool attribute_length = false;

  if ( eval( string("L"),   attributes,  value) )    length   = any_cast<double>(value); 
 
  elm = (udelm) ? dynamic_cast<drift*>( udelm->Clone() ) : new drift();

  elm = new drift();
  elm->rename(label);
   
  if ( attribute_length ) elm->setLength( length );

  elm->setTag("INSTRUMENT");
  return ElmPtr(elm); 

}
 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ElmPtr  XsifParserDriver::make_ecollimator(    ConstElmPtr& udelm, double const& BRHO, std::string const& label, map<string,boost::any> const& attributes) 
{

  
  any value;
  drift* elm = 0;

  double length = 0.0;  bool attribute_length = false;

  if ( eval( string("L"),   attributes, value) )    length   = any_cast<double>(value); 

   // elm = make_ecollimator(  label.c_str(), length );

  elm = (udelm) ? dynamic_cast<drift*>( udelm->Clone() ) : new drift();

  elm = new drift();
  elm->rename(label);
   
  if ( attribute_length ) elm->setLength( length );

  elm->setTag("ECOLLIMATOR");
  return ElmPtr(elm); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ElmPtr  XsifParserDriver::make_rcollimator(    ConstElmPtr& udelm, double const& BRHO, std::string const& label, map<string,boost::any> const& attributes) 
{
  
  any value;
  drift* elm = 0;

  double length = 0.0; bool attribute_length = false;

  if ( eval( string("L"),   attributes, value) )    { attribute_length = true; length   = any_cast<double>(value); } 

  elm = (udelm) ? dynamic_cast<drift*>( udelm->Clone() ) : new drift();

  elm = new drift();
  elm->rename(label);
   
  if ( attribute_length ) elm->setLength( length );

  elm->setTag("RCOLLIMATOR");
  return ElmPtr(elm); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ElmPtr  XsifParserDriver::make_yrot(     ConstElmPtr& udelm, double const& BRHO, std::string const& label, map<string,boost::any> const& attributes ) 
{
  
  any value;
  drift* elm = 0;

  double length = 0.0;  bool attribute_length = false;

  if ( eval( string("L"),   attributes, value) )     { attribute_length = true; length   = any_cast<double>(value); } 

  elm = (udelm) ? dynamic_cast<drift*>( udelm->Clone() ) : new drift();

  elm = new drift();
  elm->rename(label);
   
  if ( attribute_length ) elm->setLength( length );

  elm->setTag("YROT");
  return ElmPtr(elm); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ElmPtr  XsifParserDriver::make_srot(    ConstElmPtr& udelm, double const& BRHO, std::string const& label, map<string,boost::any> const& attributes ) 
{
  
  any value;
  drift* elm = 0;

  double angle = 0.0;    bool attribute_angle  = false;  

  if ( eval( string( "ANGLE"),   attributes, value) )    { attribute_angle  = true; angle = any_cast<double>(value); } 

  elm = (udelm) ? dynamic_cast<drift*>( udelm->Clone() ) : new drift();
  elm->rename( label.c_str() );

  elm->setTag("SROT");
  return ElmPtr(elm);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ElmPtr  XsifParserDriver::make_beambeam(    ConstElmPtr& udelm, double const& BRHO, std::string const& label, map<string,boost::any> const& attributes) 
{
  
  any value;
  drift* elm = 0;

  double length = 0.0;  bool attribute_length = false;

  if ( eval( string("L"),   attributes, value) )    length   = any_cast<double>(value); 

  elm = (udelm) ? dynamic_cast<drift*>( udelm->Clone() ) : new drift();

  elm->rename( label.c_str() );
  if (attribute_length)  elm->setLength( length );

  elm->setTag("BEAMBEAM");
  return ElmPtr(elm);

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ElmPtr  XsifParserDriver::make_matrix(    ConstElmPtr& udelm, double const& BRHO, std::string const& label, map<string,boost::any> const& attributes) 
{

  
  any value;
  drift* elm = 0;  

  double length = 0.0;  bool attribute_length = false;

  if ( eval( string("L"),   attributes, value) )    { attribute_length = true; length   = any_cast<double>(value); } 

  elm = (udelm) ? dynamic_cast<drift*>( udelm->Clone() ) : new drift();

  elm->rename( label.c_str() );
  if (attribute_length)  elm->setLength( length );

  elm->setTag("MATRIX");
  return ElmPtr(elm);

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ElmPtr  XsifParserDriver::make_lump(    ConstElmPtr& udelm, double const& BRHO, std::string const& label, map<string,boost::any> const& attributes) 
{

  any value;
  drift* elm = 0;

  double length = 0.0;  bool attribute_length = false;

  if ( eval( string("L"),   attributes, value) )    length   = any_cast<double>(value); 

  elm = (udelm) ? dynamic_cast<drift*>( udelm->Clone() ) : new drift();

  elm->rename( label.c_str() );
  if (attribute_length)  elm->setLength( length );

  elm->setTag("LUMP");
  return ElmPtr(elm);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ElmPtr  XsifParserDriver::make_wire(    ConstElmPtr& udelm, double const& BRHO, std::string const& label,  map<string,boost::any> const& attributes) 
{

  any value;
  drift* elm = 0;

  double length = 0.0;  bool attribute_length = false;
  if ( eval( string("L"),   attributes, value) )    { attribute_length = true; length   = any_cast<double>(value); } 

  elm = (udelm) ? dynamic_cast<drift*>( udelm->Clone() ) : new drift();

  elm->rename(label.c_str() );

  if (attribute_length)  elm->setLength( length );

  elm->setTag("WIRE");
  return ElmPtr(elm);
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ElmPtr  XsifParserDriver::make_blmonitor(    ConstElmPtr& udelm, double const& BRHO, std::string const& label,  map<string,boost::any> const& attributes) 
{
  any value;
  drift* elm = 0;

  double length = 0.0; bool attribute_length = false;

  if ( eval( string("L"),   attributes, value) )   { attribute_length = true; length  = any_cast<double>(value); } 

  elm = (udelm) ? dynamic_cast<drift*>( udelm->Clone() ) : new drift();

  elm->rename( label.c_str());

  if ( attribute_length) elm->setLength( length );

  elm->setTag("BLMONITOR");
   
  return ElmPtr(elm);
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ElmPtr  XsifParserDriver::make_gkick(    ConstElmPtr& udelm, double const& BRHO, std::string const& label,  map<string,boost::any> const& attributes) 
{
  //---------------------------------------------------------------------------
  //  Attributes for element type GKICK
  //---------------------------------------------------------------------------
  //       L        is the length.
  //       DX       is the change in x.
  //       DXP      is the change in x'.
  //       DY       is the change in y.
  //       DYP      is the change in y'.
  //       DL       is the change in path length.
  //       DP       is the change in dp/p.
  //       ANGLE    is  the  angle  through  which  the  coordinates  are
  //                rotated about the longitudinal axis.
  //       DZ       is the longitudinal displacement.
  //       V        is the  extrance-exit parameter  of the  kick.  v  is
  //                positive for an  entrance kick,  and negative  for an
  //                exit kick.   The absolute value of v is used to force
  //                the  kick to  be applied  every  abs(v)  turns.   The
  //                default value of v is 1.
  //       T   
  //---------------------------------------------------------------------------

  any value;
  bmlnElmnt* elm = 0;

  double length   = 0.0; bool attribute_length  = false;
  double dx       = 0.0; bool attribute_dx      = false;
  double dxp      = 0.0; bool attribute_dxp     = false;
  double dy       = 0.0; bool attribute_dy      = false;
  double dyp      = 0.0; bool attribute_dyp     = false;
  double dl       = 0.0; bool attribute_dl      = false;
  double dp       = 0.0; bool attribute_dp      = false;
  double dz       = 0.0; bool attribute_dz      = false;
  double v        = 0.0; bool attribute_v       = false;
  double t        = 0.0; bool attribute_t       = false;

  if ( eval( string("L"),     attributes, value) )    {  attribute_length = true; length   = any_cast<double>(value);} 
  if ( eval( string("DX"),    attributes, value) )    {  attribute_dx     = true; dx       = any_cast<double>(value);} 
  if ( eval( string("DXP"),   attributes, value) )    {  attribute_dxp    = true; dxp      = any_cast<double>(value);}
  if ( eval( string("DY"),    attributes, value) )    {  attribute_dy     = true; dy       = any_cast<double>(value);}
  if ( eval( string("DYP"),   attributes, value) )    {  attribute_dyp    = true; dyp      = any_cast<double>(value);}
  if ( eval( string("DL"),    attributes, value) )    {  attribute_dl     = true; dl       = any_cast<double>(value);}
  if ( eval( string("DP"),    attributes, value) )    {  attribute_dp     = true; dp       = any_cast<double>(value);}
  if ( eval( string("DZ"),    attributes, value) )    {  attribute_dz     = true; dz       = any_cast<double>(value);}
  if ( eval( string("V"),     attributes, value) )    {  attribute_v      = true; v        = any_cast<double>(value);}
  if ( eval( string("T"),     attributes, value) )    {  attribute_t      = true; t        = any_cast<double>(value);}


  gkick* q = new gkick();

  q->rename(label.c_str() );

  if ( attribute_length ) q->setLength(length);
  if ( attribute_dx     ) q->set_dx(dx);
  if ( attribute_dxp    ) q->set_dxp(dxp);
  if ( attribute_dy     ) q->set_dy(dy);
  if ( attribute_dyp    ) q->set_dyp(dyp);
  if ( attribute_dl     ) q->set_dl(dl);
  if ( attribute_dp     ) q->set_dp(dp);
  if ( attribute_dz     ) q->set_dz(dz);
  if ( attribute_v      ) q->set_v(v);
  if ( attribute_t      ) q->set_t(t);

  q->setTag("GKICK");
  return ElmPtr(q);
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
						      
ElmPtr XsifParserDriver::make_notimplemented( ConstElmPtr& udelm, double const& BRHO, std::string const& label,  
                                             map<string,boost::any> const& attributes) 
{						      
						      
  //---------------------------------------------------------------------------------------------------------  
  // catchall for not implemented element types: substitute a DRIFT of length L (if this parameter is defined)
  //---------------------------------------------------------------------------------------------------------
   any value;					      
   drift* elm = 0;				      
						      
   double length = 0.0;  bool attribute_length = false; 
   if ( eval( string("L"), attributes, value) )  { attribute_length = true; length = any_cast<double>(value); } 
   
   elm = (udelm) ?  dynamic_cast<drift*>( udelm->Clone() ) : new drift();           

   elm->rename( label.c_str() );
   if (  attribute_length ) elm->setLength( length );

   elm->setTag("NOTIMPLEMENTED");
   return ElmPtr(elm);

}
