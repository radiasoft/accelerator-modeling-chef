   /* -*- C -*- */

#include <assert.h>
#include <ctype.h>
#include <stddef.h>

#if !defined(str_to_upper_h)
#include "str_to_upper.h"
#endif /* str_to_upper_h */

char*
str_to_upper( char* str ) {
  char* p;
  
  assert( str != NULL );
    
  for ( p = str; *p != '\0'; ++p ) {
    *p = toupper(*p);
  }
  return str;
}
