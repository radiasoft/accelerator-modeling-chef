/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  XSIF Parser:  Interprets XSIF input files and             
******                creates instances of class beamline.                 
******                                                
******  File:      xsif_lex.l
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
******  Author: Jean-Francois Ostiguy
******          ostiguy@fnal.gov 
******
**************************************************************************
**************************************************************************/

/*------------------------------------------------------------------------------*/
/* A lexer for xsif input files                                                */
/*------------------------------------------------------------------------------*/

   /* %option debug */

%option batch 
%option stack
%option reentrant       
%option prefix="xsif_yy"

%x COMMENT
%s EXPRESSION
%s SEQUENCE

%{

#include <iostream>
#include <string>
#include <basic_toolkit/iosetup.h>
#include <unistd.h>

#include "XsifParserDriver.h"
#include "XsifParser_ypp.hh"

#include "location.hh"

static std::string& trim(std::string& str);

#define YY_USER_ACTION yylloc->columns(yyleng); 

#define yyterminate() return xsif_yy::XsifParser::token::XSIF_END;

%}

comment        "!"
comment_begin  "COMMENT"
comment_end    "ENDCOMMENT"

real            ([0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?)|([-+]?[1-9]+\.?+([eE][-+]?[0-9]+))

integer         [0-9]+[0-9]*  

string          (\"[^\"]*\")|(\'[^\']*\')

identifier      [a-zA-Z][_\.a-zA-Z0-9]*(\[[a-zA-Z][_\.a-zA-Z0-9]*\])?            

blk             [ \t]                                  


%% 

%{
       typedef xsif_yy::XsifParser::token token;
%}

%{ 
         yylloc->step();

%}


  /*-----------------------------------------------Comments  -------------------------------------------------------------*/


<COMMENT>{comment_end}{blk}*\n                            { yy_pop_state(yyscanner);
                                                            yylloc->lines(1); yylloc->step();   
                                                          }                                            

<COMMENT>.*\n                                             { yylloc->lines(1); yylloc->step(); }  /* ignore comments*/ 
                                                           

{comment_begin}                                           { yy_push_state(COMMENT, yyscanner); } 

[^{blk}{comment}\n]+{comment}.*\n                         {   /* --- partial comment line ---                 */
                                                              /* eliminate the comment text                   */
                                                              /* then, unput the buffer in *reverse order*    */ 
                                                              unput('\n');
                                                              int i=yyleng-1; 
                                                              while ( yytext[i--] != '!' ); 
                                                              while ( i-- >0 ) unput(yytext[i]);
                                                          }  

{blk}*{comment}.*\n                                       {  yylloc->lines(1); yylloc->step(); }  /*  full line comment  */


{integer}                                                {     yylval->ival = atoi(yytext); 
                                                               return token::XSIF_INT_VALUE;   
                                                         }

{real}                                                   {    
                                                              yylval->dval = strtof(yytext,0);
                                                              return token::XSIF_VALUE;   
                                                         }

{string}                                                 { 
                                                           yylval->sval = new parser_string_t(yytext);
                                                            boost::algorithm::erase_first( *yylval->sval, "\"" );
                                                            boost::algorithm::erase_last(  *yylval->sval, "\""  );
                                                            boost::algorithm::erase_first( *yylval->sval, "'"  );
                                                            boost::algorithm::erase_last(  *yylval->sval, "'"   );

                                                           return token::XSIF_STRING_VALUE; 
                                                         }                                     



   /* ----------------------------------------------- */
   /* COMMANDS                                        */
   /* ----------------------------------------------- */

(CALL|call)                                              { return token::XSIF_CALL;       } 
(CLOSE|close)                                            { return token::XSIF_CLOSE;      }
(CONSTANT|constant)                                      { return token::XSIF_CONSTANT;   }  
(ECHO|echo)                                              { return token::XSIF_ECHO;       }  
(LINE|line)                                              { return token::XSIF_LINE;       }  
(NOECHO|noecho)                                          { return token::XSIF_NOECHO;     }  
(OPEN|open)                                              { return token::XSIF_OPEN;       }  
(PARAMETER|parameter)                                    { return token::XSIF_PARAMETER;  }  
(PATH|path)                                              { return token::XSIF_PATH;       }  
(RETURN|return)                                          { return token::XSIF_RETURN;     }  
(USE|use)                                                { return token::XSIF_USE;        }  
(NLC|nlc)                                                { return token::XSIF_NLC;        }  
(NONLC|nonlc)                                            { return token::XSIF_NONLC;      }  
(BEAM|beam)                                              { return token::XSIF_BEAM;       }  


   /* ----------------------------------------------- */
   /* MATH FUNCTIONS                                  */
   /* ----------------------------------------------- */

<EXPRESSION>(SQRT|sqrt){blk}*\(                          { unput('('); return token::XSIF_SQRT;        }
<EXPRESSION>(LOG|log){blk}*\(                            { unput('('); return token::XSIF_LOG;         }
<EXPRESSION>(LOG10|log10){blk}*\(                        { unput('('); return token::XSIF_LOG10;       }
<EXPRESSION>(EXP|exp){blk}*\(                            { unput('('); return token::XSIF_EXP;         }
<EXPRESSION>(SIN|sin){blk}*\(                            { unput('('); return token::XSIF_SIN;         }
<EXPRESSION>(COS|cos){blk}*\(                            { unput('('); return token::XSIF_COS;         }
<EXPRESSION>(TAN|tan){blk}*\(                            { unput('('); return token::XSIF_TAN;         }
<EXPRESSION>(ASIN|asin){blk}*\(                          { unput('('); return token::XSIF_ASIN;        }
<EXPRESSION>(ACOS|acos){blk}*\(                          { unput('('); return token::XSIF_ACOS;        }
<EXPRESSION>(ATAN|atan){blk}*\(                          { unput('('); return token::XSIF_ATAN;        }
<EXPRESSION>(RANF|ranf){blk}*\(                          { unput('('); return token::XSIF_RANF;        } /* random number, uniformly distributed in [0,1]      */ 
<EXPRESSION>(GAUSS|gauss){blk}*\(                        { unput('('); return token::XSIF_GAUSS;       } /* random number, gaussian distribution with          */
                                                                                                /*                unit standard deviation             */
<EXPRESSION>(TGAUSS|tgauss){blk}*\(                      { unput('('); return token::XSIF_TGAUSS;      } /* random number, gaussian distribution with          */
                                                                                                         /*                unit standard deviation,            */
                                                                                                         /*                truncated at x standard deviations  */ 

   /***********************************************************************************************************
   {blk}*(TYPE|type){blk}*=                                 { 
                                                             unput('='); return token::XSIF_TYPE_ATTRIBUTE;  
                                                            }
   **************************************************************************************************************/

{blk}*(PARTICLE|particle){blk}*=                         { 
                                                            unput('='); return token::XSIF_PARTICLE_ATTRIBUTE;  
                                                         }


{identifier}                                             {  
                                                              yylval->sval =  new parser_string_t(yytext);
                                                              boost::algorithm::to_upper( *yylval->sval ); // All identifiers are UPPERCASE 
                                                              return token::XSIF_IDENTIFIER; 
                                                         } 
                                                             
:=                                                       { BEGIN(EXPRESSION);
                                                           return token::XSIF_VARASSIGNMENT;  
                                                         }

=                                                        { BEGIN(EXPRESSION);
                                                           return xsif_yy::XsifParser::token_type (yytext[0]);  
                                                         }

[-()+*/:;\{\},]                                          { return xsif_yy::XsifParser::token_type (yytext[0]);  }


\&.*\n                                                   { yylloc->lines(1); yylloc->step(); } /* line continuation */

[\t ]+                                                   ;/* ignore whitespace */

\n                                                       { yylloc->lines(1); yylloc->step();
                                                           BEGIN(0);  
                                                           return token::XSIF_EOL;
                                                         } 


<<EOF>>                                                  { 
                                                           //delete (yylloc->begin.filename);    
                                                           yyterminate();
                                                         }

%%

// --------------------------------------------------------------------


/* always return 1 */ 

int yywrap( yyscan_t yyscanner) { 
   return 1; 
} 


// --------------------------------------------------------------------

void
XsifParserDriver::scan_begin( yyscan_t yyscanner ) {

  yyguts_t* yyg =  (yyguts_t*) yyscanner;

  yy_flex_debug = m_trace_scanning;

  FILE* input_file = 0;

  // m_cwd = string( getcwd() ); // save current working directory

  if ( !(  input_file = fopen(m_file.c_str(), "r")))
    error( std::string("Cannot open ") + m_file );
  
  std::string file_dir_path             = m_file;

  std::string::size_type slash_pos = file_dir_path.find_last_of("/");

  // if the input file string includes a path, set current dir to that path
  // and strip it from the name 

  if( slash_pos !=  std::string::npos ) {
      file_dir_path.erase(slash_pos); 
      chdir (file_dir_path.c_str() ); 
  }

  if (m_input_is_file)
    xsif_yy_switch_to_buffer ( xsif_yy_create_buffer( input_file, YY_BUF_SIZE, yyscanner), yyscanner);
  else
    xsif_yy_switch_to_buffer ( xsif_yy_scan_string( m_buffer, yyscanner), yyscanner);

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#include <boost/any.hpp>
#include <stdio.h>

// ------------------------------------------------------------------------------
// The implemention of the following parse driver member functions is put here 
// so that they can access the internals of the flex lexer (we use the reentrant
// version of the C lexer) which is available only at the *.l file scope.
// -------------------------------------------------------------------------------  

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void XsifParserDriver::scan_end( yyscan_t yyscanner ) {

  yyguts_t* yyg =  (yyguts_t*) yyscanner;

  yy_delete_buffer(YY_CURRENT_BUFFER, yyscanner);  

  if (m_input_is_file)
    fclose(yyin);

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void XsifParserDriver::command_CALL( xsif_yy::location& yylloc, std::map<std::string,boost::any> const& attributes, yyscan_t yyscanner)
{

  yyguts_t* yyg =  (yyguts_t*) yyscanner;
 
  std::string filename = boost::any_cast<std::string const>( attributes[std::string("FILENAME")] ); 

  filename = trim(filename);  

  FILE* fp = fopen( filename.c_str(), "r");
 
  if (!fp) error( yylloc, std::string( "Error opening file: ") + filename ); 

  yyin = fp;

  m_locations_stack.push(yylloc);

  m_buffer_stack.push( YY_CURRENT_BUFFER );
  xsif_yy_switch_to_buffer( xsif_yy_create_buffer( yyin, YY_BUF_SIZE, yyscanner ), yyscanner );

  xsif_yy::position bpos;  bpos.filename = new std::string( filename );   
  xsif_yy::position epos;  epos.filename = new std::string( filename ); 
                                                           
  yylloc.begin = bpos;  
  yylloc.end   = epos;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void XsifParserDriver::command_RETURN(  xsif_yy::location&  yyloc, yyscan_t yyscanner)
{

 if (m_locations_stack.empty() ) {
         return;                     // EXTRA RETURN STATEMENT IN TOP LEVEL FILE
 } 
   else {
       //delete (yyloc.begin.filename);
       //delete (yyloc.end.filename);
 }

 yyguts_t* yyg =  (yyguts_t*) yyscanner;

 xsif_yy_delete_buffer( YY_CURRENT_BUFFER, yyscanner);
 xsif_yy_switch_to_buffer( (YY_BUFFER_STATE) m_buffer_stack.top(), yyscanner);
 m_buffer_stack.pop();

 yyloc = m_locations_stack.top();  
 m_locations_stack.pop();


}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//-----------------------------------------------------------------------------------------------------------

std::string& trim(std::string& str) {

// trim whitespace

  std::string::size_type  notwhite = str.find_first_not_of(" \t\n");
  str.erase(0,notwhite);

  // trim trailing whitespace and quotes
  notwhite = str.find_last_not_of(" \t\n"); 
  str.erase(notwhite+1); 
  return str;
}

//-----------------------------------------------------------------------------------------------------------
