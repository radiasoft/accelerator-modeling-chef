char nexcom( int n, int k, int* r ) {
int t, h, i;
static char firstEntry = 1;
static char allDone = 0;

if( allDone ) {
  firstEntry = 1;
  allDone = 0;
  return 0;
  }

if( firstEntry ) {
  r[0] = n;
  if( k > 1 ) {
    for( i = 1; i < k; i++ ) r[i] = 0;
    firstEntry = 0;
    }
  else allDone = 1;
  return 1;
  }

h = 0;
while( r[h++] == 0 ) ;
h--;

t = r[h];
r[h] = 0;
r[0] = --t;
r[++h]++;

allDone = ( r[k-1] == n );

return 1;

}
