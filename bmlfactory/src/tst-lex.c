/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                       
******                                                
******  Version:   1.2                    
******                                    
******  File:      tst-lex.c
******                                                                
******  Copyright (c) 1999  Universities Research Association, Inc.   
******                All Rights Reserved                             
******                                                                
******  Author:    Dmitri Mokhov and Oleg Krivosheev                  
******                                                                
******  Contact:   Leo Michelotti or Jean-Francois Ostiguy            
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******                    (630) 840 2231                              
******             Email: michelotti@fnal.gov                         
******                    ostiguy@fnal.gov                            
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


#include <stdio.h>

   /*
    *  prototypes
    */

int yylex( void );

   /*
    * external variables
    */

extern FILE*    yyin;
extern int      line_num;

extern int      yyleng;
extern char*    yytext;

static int      end_of_file = 1;

int
main( int argc, char** argv ) {

  int retcode = 0;

  yyin = fopen( "lattice.mad", "r" );

  do {
    retcode = yylex();
    printf( "! ln=%i yylex=%i yyleng=%i yytext=%s \n", line_num, retcode, yyleng, yytext );
  } while ( end_of_file != 0 );
    
  fclose( yyin );
  
  return 0;
}

int
yywrap( void ) {
  end_of_file = 0;
  return 1;
}

