/****************************************************************************
*****************************************************************************
*****************************************************************************
******
******  File:      madx_lex.l 
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
******            ostiguy@fnal.gov                                      
******            Fermilab                                           
******
******
**********************************************************************************
**********************************************************************************
*********************************************************************************/

/*------------------------------------------------------------------------------*/
/* A lexer for Madx input files                                                */
/*------------------------------------------------------------------------------*/

   /* %option debug */

%option batch 
%option stack
%option reentrant       
%option prefix="madx_yy"

%x COMMENT
%s EXPRESSION
%s SEQ_STATE

%{

#include <iostream>
#include <string>
#include <basic_toolkit/iosetup.h>
#include <unistd.h>

#include "MadxParser_ypp.hh"
#include "MadxParserDriver.h"

#include "location.hh"

static std::string& trim(std::string& str);

#define YY_USER_ACTION yylloc->columns(yyleng); 

#define yyterminate() return token::MADX_END;

%}

comment         "!"|"//" 
comment_begin  "COMMENT"
comment_end    "ENDCOMMENT"

real            ([0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?)|([-+]?[1-9]+\.?+([eE][-+]?[0-9]+))

integer         [0-9]+[0-9]*  

string          (\"[^\"]*\")|(\'[^\']*\')

identifier      [a-zA-Z][_\.a-zA-Z0-9]*(\[[a-zA-Z][_\.a-zA-Z0-9]*\])?            

blk             [ \t]                                  


%% 

%{
       typedef madx_yy::MadxParser::token token;
%}

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
 


(real|REAL)                                               { return token::MADX_REAL; }

(integer|INTEGER)                                         { return token::MADX_INTEGER; }

(const|CONST)                                             { return token::MADX_CONST; }

(shared|SHARED)                                           { return token::MADX_SHARED; }

(sequence|SEQUENCE)                                       { std::cout << "pushing state " << std::endl; 

                                                             std::cout << " state = " << yy_top_state(yyscanner) <<std::endl; 
                                                             yy_push_state(SEQ_STATE, yyscanner);
                                                             std::cout << " state = " << yy_top_state(yyscanner) <<std::endl; 
                                                             std::cout << " state SEQ_STATE  = " << SEQ_STATE <<std::endl; 
                                                             std::cout << " state COMMENT    = " << COMMENT <<std::endl; 
                                                             std::cout << " state EXPRESSION = " << EXPRESSION <<std::endl; 

                                                             return token::MADX_SEQUENCE; 
                                                          }

ENDSEQUENCE                                               { 
                                                            yy_pop_state(yyscanner); 
                                                            return token::MADX_ENDSEQUENCE; }

<SEQ_STATE>((at)|(AT)){blk}*=                              { unput('='); return token::MADX_AT;     } 

<SEQ_STATE>((from)|(FROM)){blk}*=                          { unput('='); return token::MADX_FROM;   }


^#(include|INCLUDE)[ \t]+.*                               {  
                                                              std::string* filename = new std::string(&yytext[8]);
                                                              *filename = trim(*filename); // remove quotes, if any 
                                                              FILE* fp = fopen( filename->c_str(), "r");
                                                              /*  if (!fp) {
                                                                    err (1,"%s", filename->c_str());
                                                                }
                                                              */
                                                             driver.locations_stack_.push(*yylloc);
                                                             yypush_buffer_state( madx_yy_create_buffer(fp, YY_BUF_SIZE, yyscanner), yyscanner);

                                                             madx_yy::position bpos; bpos.filename = filename;   
                                                             madx_yy::position epos; epos.filename = filename; 
                                                           
                                                             yylloc->begin = bpos;
                                                             yylloc->end   = epos;
                                                         }


{identifier}                                             {  
                                                              yylval->sval =  new parser_string_t(yytext);
                                                              return token::MADX_IDENTIFIER; 
                                                         } 
                                                             
{integer}                                                {     yylval->ival = atoi(yytext); 
                                                               return token::MADX_INT_VALUE;   
                                                         }

{real}                                                   {    
                                                              yylval->dval = strtof(yytext,0);
                                                              return token::MADX_VALUE;   
                                                         }


<EXPRESSION>(SQRT|sqrt){blk}*\(                          {  
                                                           unput('('); return token::MADX_SQRT;
                                                         }
<EXPRESSION>(LOG|log){blk}*\(                            {  
                                                           unput('('); return token::MADX_LOG;
                                                         }
<EXPRESSION>(LOG10|log10){blk}*\(                        {  
                                                           unput('('); return token::MADX_LOG10;
                                                         }
<EXPRESSION>(EXP|exp){blk}*\(                            { 
                                                           unput('('); return token::MADX_EXP;
                                                         }
<EXPRESSION>(SIN|sin){blk}*\(                            {  
                                                           unput('('); return token::MADX_SIN;
                                                         }
<EXPRESSION>(COS|cos){blk}*\(                            {  
                                                           unput('('); return token::MADX_COS;
                                                         }
<EXPRESSION>(TAN|tan){blk}*\(                            {  
                                                           unput('('); return token::MADX_TAN;
                                                         }

<EXPRESSION>(ASIN|asin){blk}*\(                          {  
                                                           unput('('); return token::MADX_ASIN;
                                                         }

<EXPRESSION>(ACOS|acos){blk}*\(                          {  
                                                           unput('('); return token::MADX_COS;
                                                         }

<EXPRESSION>(ATAN|atan){blk}*\(                          {  
                                                           unput('('); return token::MADX_ATAN;
                                                         }



<EXPRESSION>(RANF|ranf){blk}*\(                          { /* random number, uniformly distributed in [0,1]                         */ 
                                                           unput('('); return token::MADX_RANF;
                                                         }
<EXPRESSION>(GAUSS|gauss){blk}*\(                        { /* random number, gaussian distribution with unit standard deviation,    */
                                                           unput('('); return token::MADX_GAUSS;
                                                         }
 
<EXPRESSION>(TGAUSS|tgauss){blk}*\(                      { /* random number, gaussian distribution with unit standard deviation, 
                                                              truncated at x standard deviations */ 
                                                           unput('('); return token::MADX_TGAUSS;
                                                         }


:=                                                       { yy_push_state(EXPRESSION, yyscanner);
                                                           return token::MADX_ASSIGN;  
                                                         }

=                                                        { yy_push_state(EXPRESSION, yyscanner);
                                                           return madx_yy::MadxParser::token_type (yytext[0]);  
                                                         }

;                                                        {
                                                          if  ( ( yy_top_state(yyscanner) == COMMENT )   ||  
                                                                ( yy_top_state(yyscanner) == EXPRESSION ) )  { 
                                                                 std::cout << "about to pop ! " << std::endl;
                                                                 yy_pop_state(yyscanner); 
                                                          }   
                                                          return madx_yy::MadxParser::token_type (yytext[0]);  
                                                         }         

[-()+*/:\{\},]                                           { return madx_yy::MadxParser::token_type (yytext[0]);  }


\".*\"                                                   { 
                                                           yylval->sval = new parser_string_t(yytext);
                                                           return token::MADX_STRING_VALUE; 
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

                                                          *yylloc = driver.locations_stack_.top();
                                                          driver.locations_stack_.pop();

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

  yy_flex_debug = trace_scanning_;

  FILE* input_file = 0;

  if ( !(  input_file = fopen(file_.c_str(), "r")))
    error( std::string("Cannot open ") + file_ );
  

  if (input_is_file_)
    madx_yy_switch_to_buffer ( madx_yy_create_buffer( input_file, YY_BUF_SIZE, yyscanner), yyscanner);
  else
    madx_yy_switch_to_buffer ( madx_yy_scan_string( buffer_, yyscanner), yyscanner);

}

// --------------------------------------------------------------------


void
MadxParserDriver::scan_end( yyscan_t yyscanner ) {

  yyguts_t* yyg =  (yyguts_t*) yyscanner;

  yy_delete_buffer(YY_CURRENT_BUFFER, yyscanner);  

  if (input_is_file_)
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

