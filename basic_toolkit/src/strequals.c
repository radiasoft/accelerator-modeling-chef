#include <strings.h>

strEquals( str_1, str_2 )
char *str_1, *str_2;
{
int answer;

answer = ( strlen(str_1) == strlen(str_2) ) && 
         ( strcmp( str_1, str_2 ) == 0 );

return answer;

}
