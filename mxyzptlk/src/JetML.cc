#include "JetML.h"
#define M_SMALL  1.0e-14

// const Complex complex_0 = Complex(0.0,0.0);
// const Complex complex_1 = Complex(1.0,0.0);

JetML::JetML() {
  m = 0;
  r = 0;
  c = 0;
  rc = 1;
#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

JetML::JetML(int rows, int columns, const Jet& initval ) {
  int i,j;
  r = rows;
  c = columns;
  rc = 1;
  m = new Jet *[r];
  for(i = 0; i < r; i++)
    m[i] = new Jet[c];
  for(i = 0; i< r; i++) {
    for(j = 0; j < c; j++)
      m[i][j] = initval;

  }
#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

JetML::JetML(int rows, int columns, Jet* values) {
  int i,j;
  r = rows;
  c = columns;
  rc = 1;
  m = new Jet *[r];
  for(i = 0; i < r; i++)
    m[i] = new Jet[c];
  int index = 0;
  for(i = 0; i< r; i++) {
    for(j = 0; j < c; j++)
      m[i][j] = values[index++];

  }
#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

JetML::JetML(const JetML& X) {
  static char firstCall= 1;

  if(firstCall) {
    firstCall = 0;
    cerr << "*** WARNING *** The JetML copy constructor was called for" << endl;
    cerr   <<   "*** WARNING *** some inexplicable reason." << endl;
    cerr   <<   "*** WARNING *** You have a problem!" << endl;
  }

  int i,j;
  r = X.r;
  c = X.c;
  m = new Jet *[r];
  for(i = 0; i < r; i++)
    m[i] = new Jet[c];
  for(i = 0; i< r; i++) {
    for(j = 0; j < c; j++)
      m[i][j] = X.m[i][j];

  }
  rc = 1;
#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

JetML::~JetML() {
  clear();
#ifdef OBJECT_DEBUG
 objectCount--;
#endif
}

void JetML::clear() {
  if((r == 0) && (c == 0)) return;
  for(int i = 0; i < r; i++)
    delete []m[i];
  delete m;
  r = 0;
  c = 0;
}

JetML& JetML::operator=(const JetML& X) {

  if( this != &X ) {
  
  clear();
  int i,j;

  r = X.r;
  c = X.c;
  m = new Jet *[r];
  for(i = 0; i < r; i++)
    m[i] = new Jet[c];
  for(i = 0; i< r; i++) {
    for(j = 0; j < c; j++)
      m[i][j] = X.m[i][j];

  }
  }
  
  return *this;
}

char JetML::operator==(const Jet& x) {
  if(r != c) return 0;
  for(int i=0; i< r; i++) {
    if(m[i][i] != x) return 0;
    for(int j =0; j < c; j++)
      if((i != j) && (m[i][j] != 0.0)) return 0;
  }
  return 1;
}

JetML& JetML::operator+=( const Jet& x) {
  for(int i = 0; i < r; i++) {
    m[i][i] += x;
    if(fabs(m[i][i]) < M_SMALL*fabs(x))
      m[i][i] = 0.0;
  }
  return *this;
}

JetML& JetML::operator-=( const Jet& x) {
  for(int i = 0; i < r; i++) {
    m[i][i] -= x;
    if(fabs(m[i][i]) < M_SMALL*fabs(x))
      m[i][i] = 0.0;
  }
  return *this;
}

char operator==(const JetML& x, const JetML& y) {
  if((x.r != y.r) || (x.c != y.c)) return 0;
  for(int i = 0; i < x.r; i++) {
    for(int j = 0; j < x.c; j++)
      if(x.m[i][j] != y.m[i][j]) return 0;
  }
  return 1;
}

