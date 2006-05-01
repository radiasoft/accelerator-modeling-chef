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
/* A lexer for OptiM input files                                                */
/*------------------------------------------------------------------------------*/

%option debug 

%option batch 
%option stack
%option reentrant       
%option prefix="optim_yy"

%x OPTIM
%x LATDEF
%x ELMDEF
%x BETAFIT
%x BETA4DFIT
%x SPCHARGE
%x COMMENT

%{

#include <iostream>
#include <string>
#include <cstdlib>
#include <errno.h>
#include <err.h>

#include "OptimParserDriver.h"
#include "OptimParser_ypp.hh"
#include "location.hh"

static std::string& trim(std::string& str);

#define YY_USER_ACTION yylloc->columns(yyleng); 

%}



real            (([0-9]+)|([0-9]*\.?[0-9]*)([eEdD][-+]?[0-9]+)?)  

integer         [1-9]+[0-9]*  

identifier      [a-zA-Z][_:&a-zA-Z0-9]*((\[[^ \t=]+\])|[_a-zA-Z0-9])*              

variable        \$[a-zA-Z0-9:_]+  

array_variable  \$[a-zA-Z0-9:_]+\[.*\]  

concatenation   {variable}&[a-zA-Z0-9:_]+

blk             [ \t]                                  


%% 
%{ 
         yylloc->step();

%}

   /*-----------------------------------------------Comments  -------------------------------------------------------------*/


<COMMENT>.*                                               ; /* ignore comments*/ 
                                                           

<COMMENT>\n                                                { yy_pop_state(yyscanner);
                                                             yylloc->lines(1);
                                                             yylloc->step();   
                                                           }                                            

   /*-----------------------------------------------Beam parameters  -------------------------------------------------------------*/

<OPTIM>^Emittance:                                         { return OPTIM_EMITTANCE_SPEC; }
<OPTIM>^Initial:                                           { return OPTIM_INITCOND_SPEC;; }                           

OptiM[ \t]*\n                                              {  
                                                              yy_push_state(OPTIM, yyscanner);
                                                              yylloc->lines(1);
                                                              yylloc->step();
                                                              return OPTIM_GENERAL;
                                                           }
<OPTIM>[ \t]*                                              { yylloc->step();                   /* throw away blank space */  }
<OPTIM>\n                                                  { yylloc->step(); yylloc->lines(1); /* throw away newline     */  }



   /*-----------------------------------------------Lattice definition -------------------------------------------------------------*/

<OPTIM>begin{blk}+lattice\.                                { 
                                                             yylloc->lines(1);
                                                             yy_pop_state(yyscanner);      /******* NOTE !!! *******/
                                                             yy_push_state(LATDEF,yyscanner);
                                                           }
<LATDEF>Number{blk}+of{blk}+periods{blk}*=.*\n             {  std::cout << "*** IGNORED***" <<  yytext << std::endl; }

<LATDEF>{identifier}                                       {  driver.addElement(yytext);  }

<LATDEF>end[ \t]*lattice.*\n                               {  yy_pop_state(yyscanner);  }

   /*-----------------------------------------------elements definition -------------------------------------------------------------*/

begin{blk}+list                                            { yy_push_state(ELMDEF,yyscanner); }

<ELMDEF>end[ \t]+list[\ t]+of[ \t]+elements.*\n            {   
                                                              yylloc->lines(yyleng);  yylloc->step();
                                                              yy_pop_state(yyscanner); 
                                                           }
<ELMDEF>{identifier}                                       { yylval->sval = new parser_string_t(yytext); 
                                                             return OPTIM_ELEMENT; 
                                                           }


   /*-----------------------------------------------uncoupled fit parameters -------------------------------------------------------------*/

BetaFitBlock                                               {   
                                                             yy_push_state(BETAFIT,yyscanner);
                                                             return OPTIM_BETAFIT;
                                                           }
<BETAFIT>EndBetaFitBlock                                   { yy_pop_state(yyscanner); }  

<BETAFIT>(L|l):                                            { return OPTIM_GROUP_L; } 
<BETAFIT>(B|b):                                            { return OPTIM_GROUP_B; } 
<BETAFIT>(G|g):                                            { return OPTIM_GROUP_G; } 

<BETAFIT>{identifier}                                      { yylval->sval = new parser_string_t(yytext); 
                                                             return OPTIM_GROUP_ELEMENT; 
                                                           }


   /*----------------------------------------------- coupled fit parameters -------------------------------------------------------------*/

4D_BetaBlock                                               {    
                                                              yy_push_state(BETA4DFIT, yyscanner);
                                                              return OPTIM_BETAFIT4D;
                                                           }

<BETA4DFIT>End4DBetaBlock                                  {  yy_pop_state(yyscanner); }


   /*----------------------------------------------- space charge parameters -------------------------------------------------------------*/

SpaceChargeBlock                                           { 
                                                              yy_push_state(SPCHARGE, yyscanner);
                                                              return OPTIM_SPCHARGE;
                                                           } 
<SPCHARGE>EndSpaceChargeBlock                              { 
                                                              yy_pop_state(yyscanner);
                                                           } 

   /*----------------------------------------------- GLOBAL  -------------------------------------------------------------*/

<*>{

#                                                       { yy_push_state(COMMENT, yyscanner); } 
 
^#(include|INCLUDE)[ \t]+.*                                         {  
                                                              std::string* filename = new std::string(&yytext[8]);
                                                              *filename = trim(*filename); // remove quotes, if any 
                                                              FILE* fp = fopen( filename->c_str(), "r");
                                                                if (!fp) {
                                                                    err(1,"%s", filename->c_str());
                                                                }
                                                             driver.m_locations_stack.push(*yylloc);
                                                             yypush_buffer_state( optim_yy_create_buffer(fp, YY_BUF_SIZE, yyscanner), yyscanner);

                                                             yy::position bpos; bpos.filename = filename;   
                                                             yy::position epos; epos.filename = filename; 
                                                           
                                                             yylloc->begin = bpos;
                                                             yylloc->end   = epos;


                                                        }


{identifier}=                                           {  
                                                              yytext[strlen(yytext)-1] = 0;
                                                              yylval->sval =  new parser_string_t(yytext);
                                                              unput('='); /* note: calling unput changes the location of yytext ! */
                                                              return OPTIM_ATTRIBUTE; 
                                                        } 
                                                             
{integer}                                               {   yylval->ival = atoi(yytext); 
                                                            return OPTIM_INT_VALUE;   
                                                        }

{real}                                                   { yylval->dval = strtof(yytext,0);
                                                           return OPTIM_VALUE;   }

{variable}                                               { yylval->sval = new parser_string_t(yytext);  return OPTIM_VARIABLE;}

{array_variable}                                         { 
                                                             std::cout << "WARNING:  Array variable detected."         << std::endl; 
                                                             std::cout << "This feature is currently not supported "   << std::endl;
                                                         }

{concatenation}                                          {  /* Expression of the type $var&name evaluated and expanded. */ 
                                                            /* The result is returned to the lexer.                     */ 
                                                               
                                                             const char* expanded = driver.expandConcatenation(yytext).c_str();
                                                             for (int i=strlen(expanded)-1; i>=0; --i) {
                                                                unput(expanded[i]);
                                                             }
                                                          }

sqrt{blk}*\(                                            {  unput('('); 
                                                           return OPTIM_SQRT;
                                                        }
sin{blk}*\(                                                   {  
                                                           unput('('); 
                                                           return OPTIM_SIN;
                                                        }
cos{blk}*\(                                                   {  
                                                           unput('('); 
                                                           return OPTIM_COS;
                                                        }
tan{blk}*\(                                                   {  
                                                           unput('('); 
                                                           return OPTIM_TAN;
                                                        }
log{blk}*\(                                                   {  
                                                           unput('('); 
                                                           return OPTIM_LOG;
                                                        }
exp{blk}*\(                                                   {  
                                                           unput('('); 
                                                           return OPTIM_EXP;
                                                         }

[-()=+*/;]                                               { return *yytext;        }

\".*\"                                                   { 
                                                           yylval->sval = new parser_string_t(yytext);
                                                             return OPTIM_STRING_VALUE; 
                                                         }                                     

[\t ]+                                                  ;/* ignore whitespace */


=\>.*                                                   ; /* OptiM writes the result of its variable evaluation after the marker "=>".  */
                                                          /* We ignore those values. Treated as a comment                               */

\n                                                      { yylloc->lines(1); yylloc->step();} 

<<EOF>> {     
  
             YY_BUFFER_STATE old_buffer = YY_CURRENT_BUFFER;

             yypop_buffer_state(yyscanner); // NOTE: yy_delete_buffer is called by yypop_buffer() 

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

}  /****** end of START state scope *****/


%%


// --------------------------------------------------------------------


/* always return 1 */ 

int yywrap( yyscan_t yyscanner) { 
   return 1; 
} 


// --------------------------------------------------------------------

void
OptimParserDriver::scan_begin( yyscan_t yyscanner ) {

  yyguts_t* yyg =  (yyguts_t*) yyscanner;

  yy_flex_debug = m_trace_scanning;

  FILE* input_file = 0;

  if ( !(  input_file = fopen(m_file.c_str(), "r")))
    error( std::string("Cannot open ") + m_file );
  

  if (m_input_is_file)
    optim_yy_switch_to_buffer ( optim_yy_create_buffer( input_file, YY_BUF_SIZE, yyscanner), yyscanner);
  else
    optim_yy_switch_to_buffer ( optim_yy_scan_string( m_buffer, yyscanner), yyscanner);

}

// --------------------------------------------------------------------


void
OptimParserDriver::scan_end( yyscan_t yyscanner ) {

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

