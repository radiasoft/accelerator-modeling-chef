int bcfRec( n, m ) 
int n;
int m;
{
if( ( n < 0 ) || ( m < 0 )  || ( n < m )  ) return 0;
if( ( n < 2 ) || ( m == 0 ) || ( m == n ) ) return 1;
return bcfRec( n-1, m ) + bcfRec( n-1, m-1);
}
