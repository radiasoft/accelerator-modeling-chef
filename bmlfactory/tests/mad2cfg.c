#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if !defined(str_to_upper_h)
#include "str_to_upper.h"
#endif /* str_to_upper_h */

#if !defined(expression_h)
#include "expression.h"
#endif /* expression_h */

#if !defined(beamel_table_h)
#include "beamel_table.h"
#endif /* beamel_table_h */

#if !defined(bml_table_h)
#include "bml_table.h"
#endif /* bml_table_h */

#if !defined(cfg_output_h)
#include "cfg_output.h"
#endif /* cfg_output_h */

#if !defined(const_table_h)
#include "const_table.h"
#endif /* const_table_h */

#if !defined(var_table_h)
#include "var_table.h"
#endif /* var_table_h */

#if !defined(comment_arr_h)
#include "comment_arr.h"
#endif /* comment_arr_h */

#if !defined(madparser_h)
#include "madparser.h"
#endif /* madparser_h */

   /*
    * external variables
    */

madparser*  mp = NULL;

static int  end_of_file = 1;

static void
print_usage( void ) {
  fprintf(stderr, "Usage: mad2cfg [options] mad_file [cfg_file]\n\n");
  fprintf(stderr, "If cfg_file exists, it will be overwritten.\n\n");
}

static void
set_debug_flag( int flag ) {
  extern int  yydebug; /* nonzero means print parse trace */
  yydebug = flag;      /* Change to nonzero to print parse trace */
}

static madparser*
init_madparser( int argc, char* argv[] ) {
  
  madparser* mpl = NULL;
  
  if ( argc < 2 || argc > 3 ) {
    print_usage();
    exit(EXIT_SUCCESS);
  } else {
    if ( argc == 2 ) {
      mpl = madparser_init( argv[1], NULL );
    } else {
      mpl = madparser_init( argv[1], argv[2] );
    }
  }
  
  return mpl;
}

static void
output_parsed( madparser* mpl ) {
  const_def_output( madparser_file_out(mpl),
                    madparser_const_table(mpl),
                    madparser_var_table(mpl),
                    madparser_bel_table(mpl),
                    madparser_comment_arr(mpl) );
  
  var_def_output( madparser_file_out(mpl),
                  madparser_const_table(mpl),
                  madparser_var_table(mpl),
                  madparser_bel_table(mpl),
                  madparser_comment_arr(mpl) );
  
  bel_def_output( madparser_file_out(mpl),
                  madparser_var_table(mpl),
                  madparser_bel_table(mpl),
                  madparser_comment_arr(mpl) );
  
  bml_def_output( madparser_file_out(mpl),
                  madparser_bml_table(mpl),
                  madparser_comment_arr(mpl) );
}


static void
print_tables( madparser* mpl ) {
  const_table_display( stdout, madparser_const_table(mpl), madparser_var_table(mpl), madparser_bel_table(mpl) );
  var_table_display( stdout, madparser_const_table(mpl), madparser_var_table(mpl), madparser_bel_table(mpl) );
  bel_table_display( stdout, madparser_var_table(mpl), madparser_bel_table(mpl) );
  bml_table_display( stdout, madparser_bml_table(mpl) );
  comment_arr_display( stdout, madparser_comment_arr(mpl) );
}

   /*
     The main function for mad2cfg
    */
int
main( int argc, char* argv[] ) {

  static const int YES_DEBUG=1;
  static const int NO_DEBUG=0;

  set_debug_flag(NO_DEBUG);
  mp = init_madparser( argc, argv ); /* parse command line and init parser itself */
  assert( mp != NULL );

  madparser_parse( mp );

#if 0
  print_tables( mp );
#endif

  output_parsed( mp );
  madparser_delete( mp );
  
  return 0;
}

int
yywrap( void ) {
  end_of_file = 0;
  return 1;
}
