#if HAVE_CONFIG_H
#include <config.h>
#endif
/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                       
******                                                
******  Version:   1.2                    
******                                    
******  File:      beam_line.c
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


   /* -*- C -*- */

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if !defined(beamel_table_h)
#include "beamel_table.h"
#endif /* beamel_table_h */

#if !defined(bml_table_h)
#include "bml_table.h"
#endif /* bml_table_h */

#if !defined(troika_h)
#include "troika.h"
#endif /* troika_h */

#if !defined(beam_line_h)
#include "beam_line.h"
#endif /* beam_line_h */

   /*
     Takes a beam line name, manipulates it, and returns the result. 
     Used for outputting beam lines with desired names.
   */
static const char*
beam_line_make_name( const char* name ) {
  return name;
}

   /*
     Takes a pointer to a bml, allocates memory for that bml, sets its name and 
     bel list to NULL, its line number and counter to 0, and returns an OK value
   */
beam_line*
beam_line_init( fb_allocator* bml_alloc ) {
  beam_line* bml = NULL;
  
  PRIVATE_ALLOCATE( bml, bml_alloc );
  if ( bml != NULL ) {
    memset( bml, 0, sizeof(beam_line) );
  }

  return bml;
}

   /*
     Takes a bml and a char pointer "name", appends "+<name>" to the bml's 
     bel list, and returns an OK or ERROR value
   */
int
beam_line_add_bel( beam_line*  bml,
                   char*       name,
                   GHashTable* bel_table,
                   GHashTable* bml_table ) {
  
  int retcode         = BEAM_LINE_ADD_OK;
  beam_element* bel   = (beam_element*)bel_table_lookup( name, bel_table );
  char* sign_and_name = (char*)malloc( strlen( name ) + 2 );

  assert( bml != NULL );
  assert( name != NULL );
  assert( sign_and_name != NULL );

  if ( bel == NULL && (beam_line*)bml_table_lookup( name, bml_table ) == NULL ) {
    fprintf(stderr, "warning ! beam element/line %s not yet defined\n", name);
  }

  strcpy( sign_and_name, "+" );
  strcat( sign_and_name, name );
  bml->bel_list_ = g_list_append( bml->bel_list_, sign_and_name );

  return retcode;
}

   /*
     Takes 2 bml's (bml1 and bml2), appends the contents of bml2's bel list 
     to bml1's bel list (copies them too), and returns an OK value
   */
int
beam_line_add_bml( beam_line*       bml1,
                   const beam_line* bml2 ) {
  GList *list;
  GList *new_list = NULL;

  assert( bml1 != NULL );
  assert( bml2 != NULL );

  list = bml2->bel_list_;

  if ( list ) {
    GList *last;
       /* beam_element *bel; */

    new_list = g_list_alloc();
       /* bel = (beam_element*)(list->data);
          if ( bel->kind_ == BEL_BEAMLINE )
          new_list->data = beam_element_copy( bel );
          else
          new_list->data = bel; */
    new_list->data = (char*)malloc( strlen( (char*)(list->data) ) + 1);
    strcpy( (char*)new_list->data, (char*)list->data );

    last = new_list;
    list = list->next;
    while ( list ) {
      last->next = g_list_alloc();
      last->next->prev = last;
      last = last->next;
         /* bel = (beam_element*)(list->data);
            if ( bel->kind_ == BEL_BEAMLINE )
            last->data = beam_element_copy( bel );
            else
            last->data = bel; */
      last->data = (char*)malloc( strlen( (char*)(list->data) ) + 1);
      strcpy( (char*)last->data, (char*)list->data );

      list = list->next;
    }
  }

  bml1->bel_list_ = g_list_concat( bml1->bel_list_, new_list );
  return BEAM_LINE_ADD_OK;
}

   /*
     Takes a bml and a name, and sets the bml's name to the given value
   */
void
beam_line_set_name( beam_line* bml, const char* name ) {
  assert( bml != NULL );
  assert( name != NULL );
  assert( strlen(name) < BML_NAME_LENGTH );

  strcpy( bml->name_, name );
}

   /*
     Takes a bml and a line number, and sets the bml's line number to the given value
   */
void
beam_line_set_linenum( beam_line*  bml,
                       int         linenum,
                       const char* filename,
                       int         local_linenum ) {
  assert( bml != NULL );
  bml->linenum_       = linenum;
  bml->filename_      = filename;
  bml->local_linenum_ = local_linenum;
}

   /*
     Takes a bml, deallocates the memory given to its bel list and to the bml 
     itself, and returns an OK value
   */
int
beam_line_delete( beam_line*    bml,
                  fb_allocator* bml_alloc ) {
  g_list_free( bml->bel_list_ );
  PRIVATE_DEALLOCATE( bml, bml_alloc );
  
  return BEAM_LINE_DELETE_OK;
}

   /*
     Takes a bel name (data) and a bml name (user_data), and outputs           
     "<bml name>.append( <bel name>)" ('-' in bel name is output, but not '+')
   */
static void
beam_line_display_element( gpointer data,
                           gpointer user_data ) {
  troika* p = (troika*)user_data;
  
  char* ptr = (char*)data;
  if ( *ptr == '+' ) {
    ++ptr;
  } else if ( (beam_line*)bml_table_lookup( ptr+1, (GHashTable*)(p->second_) ) == NULL ) {
    ++ptr;
  }

  fprintf( (FILE*)(p->first_),
           "         %s.append( %s );\n", (char*)(p->third_), ptr );
}

   /*
     Takes a bml and outputs corresponding C++ code for the bml's definition
   */
void
beam_line_display( FILE*       out,
                   beam_line*  bml,
                   GHashTable* bml_table ) {

  troika t;

  t.first_  = (void*)out;
  t.second_ = (void*)bml_table;
  t.third_  = (void*)bml->name_;
  
  fprintf( out,
           "beamline %s ( \"%s\" );\n",
           bml->name_,
           beam_line_make_name( bml->name_) );
  
  g_list_foreach( bml->bel_list_, (GFunc)beam_line_display_element, &t );
}
