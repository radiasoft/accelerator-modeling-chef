/****************************************************************************
*****************************************************************************
*****************************************************************************
******
******  Version:   1.0                    
******                                    
******  File:      optim.l 
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

/*------------------------------------------------------------------------------*/
/* A lexer for Madx input files                                                */
/*------------------------------------------------------------------------------*/

%option debug 

%option batch 
%option stack
%option reentrant       
%option prefix="madx_yy"

%x COMMENT
%s SEQUENCE

%{

#include <iostream>
#include <string>
#include <cstdlib>
#include <errno.h>
#include <err.h>

#include "MadxParserDriver.h"
#include "MadxParser_ypp.hh"
#include "location.hh"

static std::string& trim(std::string& str);

#define YY_USER_ACTION yylloc->columns(yyleng); 

%}

comment        "//"
comment_begin  "/*"
comment_end    "*/"

real            (([0-9]+)|([0-9]*\.?[0-9]*)([eEdD][-+]?[0-9]+)?)  

integer         [0-9]+[0-9]*  

identifier      [a-zA-Z][_\.a-zA-Z0-9]*              

blk             [ \t]                                  


%% 

%{ 
         yylloc->step();

%}

   /*-----------------------------------------------Comments  -------------------------------------------------------------*/


<COMMENT>{comment_end}                                    { yy_pop_state(yyscanner);
                                                            yylloc->lines(1); yylloc->step();   
                                                          }                                            
<COMMENT>.*\n                                             { yylloc->lines(1); yylloc->step(); }  /* ignore comments*/ 
                                                           

{comment_begin}                                           { yy_push_state(COMMENT, yyscanner); } 

{comment}.*\n                                             { yylloc->lines(1); yylloc->step(); }  /* ignore comments*/
 


REAL                                                      { return MADX_REAL; }

INTEGER                                                   { return MADX_INTEGER; }

CONST                                                     { return MADX_CONST; }

SHARED                                                    { return MADX_SHARED; }

SEQUENCE                                                  {  yy_push_state(SEQUENCE, yyscanner);
                                                             return MADX_SEQUENCE; }

ENDSEQUENCE                                               { yy_pop_state(yyscanner); 
                                                            return MADX_ENDSEQUENCE; }

((at)|(AT)){blk}*=                                        { unput('='); return MADX_AT;     } 

((from)|(FROM)){blk}*=                                    { unput('='); return MADX_FROM;   }

:{blk}*=                                                  { return MADX_ASSIGN; }

^#(include|INCLUDE)[ \t]+.*                               {  
                                                              std::string* filename = new std::string(&yytext[8]);
                                                              *filename = trim(*filename); // remove quotes, if any 
                                                              FILE* fp = fopen( filename->c_str(), "r");
                                                                if (!fp) {
                                                                    err(1,"%s", filename->c_str());
                                                                }
                                                             driver.m_locations_stack.push(*yylloc);
                                                             yypush_buffer_state( madx_yy_create_buffer(fp, YY_BUF_SIZE, yyscanner), yyscanner);

                                                             yy::position bpos; bpos.filename = filename;   
                                                             yy::position epos; epos.filename = filename; 
                                                           
                                                             yylloc->begin = bpos;
                                                             yylloc->end   = epos;
                                                         }


{identifier}                                             {  
                                                              yylval->sval =  new parser_string_t(yytext);
                                                              return MADX_IDENTIFIER; 
                                                         } 
                                                             
{integer}                                                {     yylval->ival = atoi(yytext); 
                                                               return MADX_INT_VALUE;   
                                                         }

{real}                                                   {    
                                                              yylval->dval = strtof(yytext,0);
                                                              return MADX_VALUE;   
                                                         }


(SQRT|sqrt){blk}*\(                                      {  
                                                           unput('('); 
                                                           return MADX_SQRT;
                                                         }
(LOG|log){blk}*\(                                              {  
                                                           unput('('); 
                                                           return MADX_LOG;
                                                         }
(LOG10|log10){blk}*\(                                              {  
                                                           unput('('); 
                                                           return MADX_LOG10;
                                                         }
(EXP|exp){blk}*\(                                              {  
                                                           unput('('); 
                                                           return MADX_EXP;
                                                         }
(SIN|sin){blk}*\(                                              {  
                                                           unput('('); 
                                                           return MADX_SIN;
                                                         }
(COS|cos){blk}*\(                                              {  
                                                           unput('('); 
                                                           return MADX_COS;
                                                         }
(TAN|tan){blk}*\(                                              {  
                                                           unput('('); 
                                                           return MADX_TAN;
                                                         }

(ASIN|asin){blk}*\(                                      {  
                                                           unput('(');
                                                           return MADX_ASIN;
                                                         }

(ACOS|acos){blk}*\(                                      {  
                                                           unput('(');
                                                           return MADX_COS;
                                                         }

(ATAN|atan){blk}*\(                                      {  
                                                           unput('(');
                                                           return MADX_ATAN;
                                                         }



(RANF|ranf){blk}*\(                                      { /* random number, uniformly distributed in [0,1]                         */ 
                                                           unput('(');
                                                           return MADX_RANF;
                                                         }
(GAUSS|gauss){blk}*\(                                    { /* random number, gaussian distribution with unit standard deviation,    */
                                                           unput('(');
                                                           return MADX_GAUSS;
                                                         }
 
(TGAUSS|tgauss){blk}*\(                                  { /* random number, gaussian distribution with unit standard deviation, truncated at x standard deviations */ 
                                                           unput('(');
                                                            return MADX_TGAUSS;
                                                         }


[-()=+*/:;\{\},]                                         { return *yytext;        }

\".*\"                                                   { 
                                                           yylval->sval = new parser_string_t(yytext);
                                                           return MADX_STRING_VALUE; 
                                                         }                                     

[\t ]+                                                  ;/* ignore whitespace */


\n                                                       { yylloc->lines(1); yylloc->step();} 


<<EOF>>                                                  {     
                                                          YY_BUFFER_STATE old_buffer = YY_CURRENT_BUFFER;

                                                          yypop_buffer_state(yyscanner); /* NOTE: yy_delete_buffer is called by yypop_buffer() */ 

                                                          if (! YY_CURRENT_BUFFER ) 
                                                             yyterminate();

                                                          /* returning form #include                             */
                                                          /* destroy the current location and restore the old    */

                                                          if (yylloc->begin.filename == yylloc->end.filename)
                                                          { 
                                                             delete (yylloc->begin.filename);
                                                          }
                                                          else
                                                          {
                                                             delete (yylloc->begin.filename);
                                                             delete (yylloc->end.filename);
                                                          }

                                                          *yylloc = driver.m_locations_stack.top();
                                                          driver.m_locations_stack.pop();

                                                        }

%%

// --------------------------------------------------------------------


/* always return 1 */ 

int yywrap( yyscan_t yyscanner) { 
   return 1; 
} 


// --------------------------------------------------------------------

void
MadxParserDriver::scan_begin( yyscan_t yyscanner ) {

  yyguts_t* yyg =  (yyguts_t*) yyscanner;

  yy_flex_debug = m_trace_scanning;

  FILE* input_file = 0;

  if ( !(  input_file = fopen(m_file.c_str(), "r")))
    error( std::string("Cannot open ") + m_file );
  

  if (m_input_is_file)
    madx_yy_switch_to_buffer ( madx_yy_create_buffer( input_file, YY_BUF_SIZE, yyscanner), yyscanner);
  else
    madx_yy_switch_to_buffer ( madx_yy_scan_string( m_buffer, yyscanner), yyscanner);

}

// --------------------------------------------------------------------


void
MadxParserDriver::scan_end( yyscan_t yyscanner ) {

  yyguts_t* yyg =  (yyguts_t*) yyscanner;

  yy_delete_buffer(YY_CURRENT_BUFFER, yyscanner);  

  if (m_input_is_file)
    fclose(yyin);

}


//-----------------------------------------------------------------------------------------------------------

std::string& trim(std::string& str) {

  // trim leading whitespace and quotes
  std::string::size_type  notwhite = str.find_first_not_of(" \t\n");
  str.erase(0,notwhite);
  // trim trailing whitespace and quotes
  notwhite = str.find_last_not_of(" \t\n"); 
  str.erase(notwhite+1); 
  return str;
}

//-----------------------------------------------------------------------------------------------------------

