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

