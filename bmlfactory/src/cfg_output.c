   /* -*- C -*- */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <glib.h>

#if !defined(beamel_table_h)
#include "beamel_table.h"
#endif /* beamel_table_h */

#if !defined(bml_table_h)
#include "bml_table.h"
#endif /* bml_table_h */

#if !defined(comment_arr_h)
#include "comment_arr.h"
#endif /* comment_arr_h */

#if !defined(const_table_h)
#include "const_table.h"
#endif /* const_table_h */

#if !defined(var_table_h)
#include "var_table.h"
#endif /* var_table_h */

#if !defined(expression_h)
#include "expression.h"
#endif /* expression_h */

#if !defined(pair_h)
#include "pair.h"
#endif /* pair_h */

#if !defined(cfg_output_h)
#include "cfg_output.h"
#endif /* cfg_output_h */

   /*
     Takes a line number, and if there are comments associated with that line, 
     outputs those comments (in C++ style)
   */
static void
print_comments( FILE*      out,
                int        linenum,
                GPtrArray* comment_arr ) {
     
  comment* ptr = (comment*)comment_arr_lookup( linenum, comment_arr );
  if ( ptr != NULL )
    fprintf( out, "// %s\n", ptr->svalue_);
}

   /*
     Outputs the C++ code for all the constant definitions, sorted by line number
   */
void
const_def_output( FILE*       out,
                  GHashTable* const_table,
                  GHashTable* var_table,
                  GHashTable* bel_table,
                  GPtrArray*  comment_arr ) {
  
  constant **const_arr;
  int i;
  int size = const_table_to_array( &const_arr, const_table );
  qsort( const_arr, size, sizeof(constant*), const_compare );
  fprintf( out, "// *** Constants     ***\n");
  for ( i = 0; i < size; ++i ) {
    enum expression_kind kind = ((expr_struct*)((*(const_arr+i))->expr_->data))->kind_;
    print_comments( out, (*(const_arr+i))->linenum_, comment_arr );
    if ( kind == STRING_EXPR || kind == STR_IDENT_EXPR ) {
      fprintf( out, "const char *%s = ", (*(const_arr+i))->name_);
    } else {
      fprintf( out, "const double %s = ", (*(const_arr+i))->name_);
    }

    expr_display( out, (*(const_arr+i))->expr_, var_table, bel_table );
    fprintf( out, ";\n");
  }
  free( const_arr );
}

   /*
     Outputs C++ code for all the variable definitions, without forward references
   */
void
var_def_output( FILE*       out,
                GHashTable* const_table,
                GHashTable* var_table,
                GHashTable* bel_table,
                GPtrArray*  comment_arr ) {
  
  variable** var_arr;
  int i;
  int size = var_table_to_array( &var_arr, var_table );
  if ( size > 1 ) {
    qsort( var_arr, size, sizeof(variable*), var_compare );
    if ( var_table_check_circ( var_arr, size, &i, var_table, bel_table ) != NULL ) {
      fprintf( stderr, "Variable %s is circullary defined, exiting\n", var_arr[i]->name_ );
      exit( EXIT_FAILURE );
    }
    var_remove_forward( &var_arr, size, var_table ); /* When removing forward references, line order isn't preserved */
  }
  fprintf( out, "// *** Variables     ***\n" );
  for ( i = 0; i < size; ++i ) {
    enum expression_kind kind = ((expr_struct*)((*(var_arr+i))->expr_->data))->kind_;
    print_comments( out, (*(var_arr+i))->linenum_, comment_arr );
    if ( kind == STRING_EXPR || kind == STR_IDENT_EXPR ||
         ( kind == VAR_IDENT_EXPR &&
           expr_is_string( (*(var_arr+i))->expr_, const_table, var_table ) != NULL ) ) {
      fprintf( out, "char *%s = ", (*(var_arr+i))->name_);
    } else {
      fprintf( out, "double %s = ", (*(var_arr+i))->name_);
    }

    expr_display( out, (*(var_arr+i))->expr_, var_table, bel_table );
    fprintf( out, ";\n");
  }
  free( var_arr );
}

   /*
     Outputs C++ code for all the bel definitions, sorted by line number
   */
void
bel_def_output( FILE*       out,
                GHashTable* var_table,
                GHashTable* bel_table,
                GPtrArray*  comment_arr ) {
  
  beam_element** bel_arr;
  int i;
  int size = bel_table_to_array( &bel_arr, bel_table );
  qsort( bel_arr, size, sizeof(beam_element*), bel_compare );
  fprintf( out, "// *** Beam elements ***\n");
  fprintf( out, "alignmentData* aligner = new alignmentData;\n");
  for ( i = 0; i < size; ++i ) {
    print_comments( out, (*(bel_arr+i))->linenum_, comment_arr );
    beam_element_display( out, *(bel_arr+i), var_table, bel_table );
  }
  fprintf( out, "delete aligner;\n");
  free( bel_arr );
}

   /*
     Outputs C++ code for all the bml definitions, without forward references
    */
void
bml_def_output( FILE*       out, 
                GHashTable* bml_table,
                GPtrArray*  comment_arr ) {
  beam_line **bml_arr;
  int i;
  int size = bml_table_to_array( &bml_arr, bml_table );
  
  if ( size > 1 ) {
    qsort( bml_arr, size, sizeof(beam_line*), bml_compare );
    
       /*
         There should be a check for circular references here
        */
    bml_remove_forward( &bml_arr, size, bml_table ); /* When removing forward references, line order isn't preserved */
  }
  
  fprintf( out, "// *** Beam lines    ***\n");
  for ( i = 0; i < size; ++i ) {
    print_comments( out, (*(bml_arr+i))->linenum_, comment_arr );
    beam_line_display( out, *(bml_arr+i), bml_table );
  }
  
  free( bml_arr );

  fprintf( out, "\n" );
  print_comments( out, INT_MAX, comment_arr );
}
