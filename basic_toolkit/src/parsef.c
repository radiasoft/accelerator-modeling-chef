#include <stdio.h>

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
