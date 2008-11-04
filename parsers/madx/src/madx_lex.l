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
%s ATTRIBUTE_MODE
%s EXPRESSION
%s SEQ_STATE

%{

#include <iostream>
#include <string>
#include <basic_toolkit/iosetup.h>
#include <unistd.h>

#include "MadxParserDriver.h"
#include "location.hh"

static std::string& trim(std::string& str);

#define YY_USER_ACTION yylloc->columns(yyleng); 

#define yyterminate() return madx_yy::MadxParser::token_type(0); 

%}

comment         "!"|"//" 
comment_begin  "COMMENT"
comment_end    "ENDCOMMENT"

real            ([0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?)|([-+]?[1-9]+\.?+([eE][-+]?[0-9]+))

integer         [0-9]+[0-9]*  

string          (\"[^\"]*\")|(\'[^\']*\')

identifier      [a-zA-Z][_\.a-zA-Z0-9]*

label           [a-zA-Z][_\.a-zA-Z0-9]*[\t ]*:

blk             [ \t]                                  


%% 

%{
       typedef madx_yy::MadxParser::token token;
%}

%{ 
         yylloc->step();

%}


  /*-----------------------------------------------Comments  -------------------------------------------------------------*/



<COMMENT>{comment_end}                                   { yy_pop_state(yyscanner);
                                                            yylloc->lines(1); yylloc->step();   
                                                         }                                            
<COMMENT>.*\n                                            { yylloc->lines(1); yylloc->step(); }  /* ignore comments*/ 
                                                           

{comment_begin}                                          { yy_push_state(COMMENT, yyscanner); } 

{comment}.*\n                                            { yylloc->lines(1); yylloc->step(); }  /* ignore comments*/
 


^#(include|INCLUDE)[ \t]+.*                              {  
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



,/{blk}*{identifier}                                     { /*set in attribute mode if it is not already set !!! */ 
                                                           yy_push_state(ATTRIBUTE_MODE, yyscanner);
                                                           return madx_yy::MadxParser::token_type(','); 
                                                         }

%{ 
/*************
<ATTRIBUTE_MODE>{identifier}[ \t]*,                      {    unput(',');
                                                               yylval->sval =  new parser_string_t(yytext);
                                                               return token::ATTRIBUTE; 
               
                                                         } 
                                                             
<ATTRIBUTE_MODE>{identifier}[ \t]*(:=|=)                 {     unput('=');
                                                               unput(':');
                                                               yylval->sval =  new parser_string_t(yytext);
                                                               return token::ATTRIBUTE; 
                                                        } 
***************/
%}


(CONST|const)                                            { return token::CONST;  }
(REAL|real)                                              { return token::REAL;   }
(STRING|string)                                          { return token::STRING; }
(VECTOR|vector)                                          { return token::VECTOR; }
(TABLE|table)                                            { return token::TABLE;  }
(COLUMN|column)                                          { return token::COLUMN; }
(ROW|row)                                                { return token::ROW;    }
 

{identifier}/{blk}*=                                     { 
                                                           yy_push_state(EXPRESSION, yyscanner);
                                                           yylval->sval =  new parser_string_t(yytext);
                                                           return token::IDENTIFIER;
                                                         }
{identifier}/{blk}*:=                                    { 
                                                           yy_push_state(EXPRESSION, yyscanner);
                                                           yylval->sval =  new parser_string_t(yytext);
                                                           return token::IDENTIFIER;  
                                                         }


{identifier}/{blk}*:                                     {  
                                                           yylval->sval =  new parser_string_t(yytext);
                                                           return token::IDENTIFIER; 
                                                         }

                                                             
{identifier}                                             {  
                                                           yylval->sval =  new parser_string_t(yytext);
                                                           return token::IDENTIFIER; 
               
                                                         } 
                                                             
{integer}                                                {  
                                                            yylval->ival = atoi(yytext); 
                                                            return token::INTEGER_LITERAL;   
                                                         }

{real}                                                   {    
                                                            yylval->dval = strtof(yytext,0);
                                                            return token::REAL_LITERAL;   
                                                         }


<EXPRESSION>(RANF|ranf)/{blk}*\(                         { /* random number, uniformly distributed in [0,1]                         */   
                                                           return token::RANF;
                                                         }

<EXPRESSION>(GAUSS|gauss)/{blk}*\(                       { /* random number, gaussian distribution with unit standard deviation,    */   
                                                           return token::GAUSS;
                                                         }
<EXPRESSION>(USER0|user0)/{blk}*\(                       {  
                                                            return token::USER0;
                                                         }
<EXPRESSION>(SI|si)/{blk}*\(                             {  
                                                            return token::SI;
                                                         }
<EXPRESSION>(SC|sc)/{blk}*\(                             {  
                                                            return token::SC;
                                                         }
<EXPRESSION>(SO|so)/{blk}*\(                             {  
                                                            return token::SO;
                                                         }
<EXPRESSION>(ABS|abs)/{blk}*\(                           {  
                                                            return token::ABS;
                                                         }
<EXPRESSION>(TRUNC|trunc)/{blk}*\(                       {  
                                                            return token::TRUNC;
                                                         }
<EXPRESSION>(ROUND|round)/{blk}*\(                       {  
                                                            return token::ROUND;
                                                         }
<EXPRESSION>(FLOOR|floor)/{blk}*\(                       {  
                                                            return token::FLOOR;
                                                         }
<EXPRESSION>(CEIL|ceil)/{blk}*\(                         {  
                                                            return token::CEIL;
                                                         }
<EXPRESSION>(SIGN|sign)/{blk}*\(                         {  
                                                            return token::SIGN;
                                                         }
<EXPRESSION>(SQRT|sqrt)/{blk}*\(                         {  
                                                            return token::SQRT;
                                                         }
<EXPRESSION>(LOG|log)/{blk}*\(                           {  
                                                            return token::LOG;
                                                         }
<EXPRESSION>(EXP|exp)/{blk}*\(                           {  
                                                            return token::EXP;
                                                         }
<EXPRESSION>(SIN|sin)/{blk}*\(                           {  
                                                            return token::SIN;
                                                         }
<EXPRESSION>(COS|cos)/{blk}*\(                           {  
                                                            return token::COS;
                                                         }
<EXPRESSION>(TAN|tan)/{blk}*\(                           {  
                                                            return token::TAN;
                                                         }
<EXPRESSION>(ASIN|asin)/{blk}*\(                         {  
                                                            return token::ASIN;
                                                         }
<EXPRESSION>(ACOS|acos)/{blk}*\(                         {  
                                                            return token::ACOS;
                                                         }
<EXPRESSION>(ATAN|atan)/{blk}*\(                         {  
                                                            return token::ATAN;
                                                         }
<EXPRESSION>(TGAUSS|tgauss)/{blk}*\(                     {  /* random number, gaussian distribution with unit standard deviation, 
                                                              truncated at x standard deviations */  
                                                            return token::TGAUSS;
                                                         }
<EXPRESSION>(USER1|user1)/{blk}*\(                       {  
                                                            return token::USER1;
                                                         }
<EXPRESSION>(ATAN2|atan2)/{blk}*\(                       {  
                                                            return token::ATAN2;
                                                         }
<EXPRESSION>(MAX|max)/{blk}*\(                           {  
                                                            return token::MAX;
                                                         }
<EXPRESSION>(MIN|min)/{blk}*\(                           {  
                                                            return token::MIN;
                                                         }
<EXPRESSION>(MOD|mod)/{blk}*\(                           {  
                                                            return token::MOD;
                                                         }
<EXPRESSION>(POW|pow)/{blk}*\(                           {  
                                                            return token::POW;
                                                         }
<EXPRESSION>(USER2|user2)/{blk}*\(                       {  
                                                           return token::USER2;
                                                         }

<EXPRESSION>(VMIN|vmin)/{blk}*\(                         {  
                                                            return token::VMIN;
                                                         }
<EXPRESSION>(VMAX|vmax)/{blk}*\(                         {  
                                                            return token::VMAX;
                                                         }
<EXPRESSION>(VRMS|vrms)/{blk}*\(                         {  
                                                            return token::VRMS;
                                                         }
<EXPRESSION>(VABSMAX|vabsmax)/{blk}*\(                   {  
                                                            return token::VABSMAX;
                                                         }


;                                                        {
                                                          if  ( ( yy_top_state(yyscanner) != INITIAL )  ) {
                                                                 yy_pop_state(yyscanner); 
                                                          }   
                                                            return madx_yy::MadxParser::token_type (yytext[0]);  
                                                         }         

:=                                                       { 
                                                            return token::ASSIGN;  
                                                         }

=                                                        { 
                                                            return madx_yy::MadxParser::token_type('='); 
                                                         }
&&                                                       {  
                                                            return token::AND;   
                                                         }                                    
\|\|                                                     {  
                                                            return token::OR;   
                                                         }                                    
\>=                                                      {  
                                                            return token::GT_OR_EQ;   
                                                         }                                    
\<=                                                      {  
                                                            return token::LT_OR_EQ;   
                                                         }                                    

[-()+/*:\{\},&]                                          {  return madx_yy::MadxParser::token_type (yytext[0]);  }

->                                                       {  return token::ARROW;                                 }

\".*\"                                                   { 
                                                            yylval->sval = new parser_string_t(yytext);
                                                            return token::STRING_LITERAL; 
                                                         }                                     
\'.*\'                                                   { 
                                                            yylval->sval = new parser_string_t(yytext);
                                                            return token::STRING_LITERAL; 
                                                         }                                     

[\t ]+                                                   { /* ignore whitespace */ }


\n                                                       {  yylloc->lines(1); yylloc->step();} 


<<EOF>>                                                  {     
                                                          std::cout << "EOF reached." << std::endl;
                                                          YY_BUFFER_STATE old_buffer = YY_CURRENT_BUFFER;

                                                          yypop_buffer_state(yyscanner); /* NOTE: yy_delete_buffer is called by yypop_buffer() */ 

                                                          if (! YY_CURRENT_BUFFER ) {
                                                             std::cout << "calling yyterminate" << std::endl;
                                                             return madx_yy::MadxParser::token_type(0); 
                                                          }

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
                                                           
                                                          return madx_yy::MadxParser::token_type(0); 
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

