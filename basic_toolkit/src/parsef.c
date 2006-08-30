/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.0                    
******                                    
******  File:      parsef.c
******                                                                
******  Copyright (c) 1990 Universities Research Association, Inc.    
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
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
******                                                                
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif


#include <stdio.h>
#include <strings.h>

#define term_char_size 80   /* Number of characters in a terminal line.    */
#define maxchar 1300        /* Max number of characters in a file line.    */
#define maxtokens 50        /* Max number of tokens in a parse list.        */
#define max_token_size 25   /* Max number of characters in a token. */

typedef char token[max_token_size];
struct array_of_tokens
 {
    token piece[maxtokens];
 };

struct array_of_tokens parsef( line, numitems )
char *line;
int  *numitems;
{
int counter, place, length;
struct array_of_tokens item;

  place = 0;
  counter = 0;
  length = strspn( &line[place], " \t" );
  place = place + length;
  while( sscanf( &line[place], "%s", &(item.piece[counter]) ) != EOF 
         && counter < maxtokens )
  {
    length = strlen( item.piece[counter] );
    place = place + length;
    length = strspn( &line[place], " \t" );
    place = place + length;
    counter = counter + 1;
  };  

*numitems = counter;
return item;

}
