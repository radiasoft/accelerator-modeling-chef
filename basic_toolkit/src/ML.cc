/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.0                    
******                                    
******  File:      ML.cc
******                                                                
******  Copyright (c) 1990 Universities Research Association, Inc.    
******                All Rights Reserved                             
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
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


#include "ML.h"
#define M_SMALL  1.0e-14

// ================================================================
//      Global variables
//


#ifdef OBJECT_DEBUG
int MLD::objectCount = 0;
int MLC::objectCount = 0;
int MLI::objectCount = 0;
#endif

// ================================================================
//      Constructors and the destructor ...
//


using namespace std;

MLD::MLD() {
  m = 0;
  r = 0;
  c = 0;
  rc = 1;

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

MLD::MLD(int rows, int columns, double initval) {
  int i,j;
  r = rows;
  c = columns;
  rc = 1;
  m = new double *[r];
  for(i = 0; i < r; i++)
    m[i] = new double[c];
  for(i = 0; i< r; i++) {
    for(j = 0; j < c; j++)
      m[i][j] = initval;

  }

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

MLD::MLD(int rows, int columns, double* values) {
  int i,j;
  r = rows;
  c = columns;
  rc = 1;
  m = new double *[r];
  for(i = 0; i < r; i++)
    m[i] = new double[c];
  int index = 0;
  for(i = 0; i< r; i++) {
    for(j = 0; j < c; j++)
      m[i][j] = values[index++];

  }

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

MLD::MLD(const MLD& X) {
  static char firstCall= 1;

  if(firstCall) {
    firstCall = 0;
    cerr << "*** WARNING *** The MLD copy constructor was called for" << endl;
    cerr   <<   "*** WARNING *** some inexplicable reason." << endl;
    cerr   <<   "*** WARNING *** You have a problem!" << endl;
  }

  int i,j;
  r = X.r;
  c = X.c;
  m = new double *[r];
  for(i = 0; i < r; i++)
    m[i] = new double[c];
  for(i = 0; i< r; i++) {
    for(j = 0; j < c; j++)
      m[i][j] = X.m[i][j];

  }
  rc = 1;

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}



MLD::~MLD() {
  clear();

#ifdef OBJECT_DEBUG
 objectCount--;
#endif
}

void MLD::clear() {
  if((r == 0) && (c == 0)) return;
  for(int i = 0; i < r; i++)
    delete []m[i];
  delete [] m;
  r = 0;
  c = 0;
}

MLD& MLD::operator=(const MLD& X) {

  if( this != &X ) {
    clear();
    int i,j;

    r = X.r;
    c = X.c;
    m = new double *[r];
    for(i = 0; i < r; i++)
      m[i] = new double[c];
    for(i = 0; i< r; i++) {
      for(j = 0; j < c; j++)
        m[i][j] = X.m[i][j];
    }
  }
  return *this;
}

char MLD::operator==(const double& x) {
  if(r != c) return 0;
  for(int i=0; i< r; i++) {
    if(m[i][i] != x) return 0;
    for(int j =0; j < c; j++)
      if((i != j) && (m[i][j] != 0.0)) return 0;
  }
  return 1;
}

MLD& MLD::operator+=( const double& x) {
  for(int i = 0; i < r; i++) {
    m[i][i] += x;
    if(fabs(m[i][i]) < M_SMALL*fabs(x))
      m[i][i] = 0.0;
  }
  return *this;
}

MLD& MLD::operator-=( const double& x) {
  for(int i = 0; i < r; i++) {
    m[i][i] -= x;
    if(fabs(m[i][i]) < M_SMALL*fabs(x))
      m[i][i] = 0.0;
  }
  return *this;
}

char operator==(const MLD& x, const MLD& y) {
  if((x.r != y.r) || (x.c != y.c)) return 0;
  for(int i = 0; i < x.r; i++) {
    for(int j = 0; j < x.c; j++)
      if(x.m[i][j] != y.m[i][j]) return 0;
  }
  return 1;
}


MLC::MLC() {
  m = 0;
  r = 0;
  c = 0;
  rc = 1;

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

MLC::MLC(int rows, int columns, const Complex& initval ) {
  int i,j;
  r = rows;
  c = columns;
  rc = 1;
  m = new Complex *[r];
  for(i = 0; i < r; i++)
    m[i] = new Complex[c];
  for(i = 0; i< r; i++) {
    for(j = 0; j < c; j++)
      m[i][j] = initval;

  }

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

MLC::MLC(int rows, int columns, Complex* values) {
  int i,j;
  r = rows;
  c = columns;
  rc = 1;
  m = new Complex *[r];
  for(i = 0; i < r; i++)
    m[i] = new Complex[c];
  int index = 0;
  for(i = 0; i< r; i++) {
    for(j = 0; j < c; j++)
      m[i][j] = values[index++];

  }

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

MLC::MLC(const MLC& X) {
  static char firstCall= 1;

  if(firstCall) {
    firstCall = 0;
    cerr << "*** WARNING *** The MLC copy constructor was called for" << endl;
    cerr   <<   "*** WARNING *** some inexplicable reason." << endl;
    cerr   <<   "*** WARNING *** You have a problem!" << endl;
  }

  int i,j;
  r = X.r;
  c = X.c;
  m = new Complex *[r];
  for(i = 0; i < r; i++)
    m[i] = new Complex[c];
  for(i = 0; i< r; i++) {
    for(j = 0; j < c; j++)
      m[i][j] = X.m[i][j];

  }
  rc = 1;

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}
MLC::MLC(const MLD& X) {
  int i,j;
  r = X.r;
  c = X.c;
  m = new Complex *[r];
  for(i = 0; i < r; i++)
    m[i] = new Complex[c];
  for(i = 0; i< r; i++) {
    for(j = 0; j < c; j++)
      m[i][j] = Complex(X.m[i][j],0.0);
  }
  rc = 1;

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}



MLC::~MLC() {
  clear();

#ifdef OBJECT_DEBUG
 objectCount--;
#endif
}

void MLC::clear() {
  if((r == 0) && (c == 0)) return;
  for(int i = 0; i < r; i++)
    delete []m[i];
  delete [] m;
  r = 0;
  c = 0;
}

MLC& MLC::operator=(const MLC& X) {

  if( this != &X ) {
  
  clear();
  int i,j;

  r = X.r;
  c = X.c;
  m = new Complex *[r];
  for(i = 0; i < r; i++)
    m[i] = new Complex[c];
  for(i = 0; i< r; i++) {
    for(j = 0; j < c; j++)
      m[i][j] = X.m[i][j];

  }
  }
  
  return *this;
}

char MLC::operator==(const Complex& x) {
  if(r != c) return 0;
  for(int i=0; i< r; i++) {
    if(m[i][i] != x) return 0;
    for(int j =0; j < c; j++)
      if((i != j) && (m[i][j] != complex_0)) return 0;
  }
  return 1;
}

MLC& MLC::operator+=( const Complex& x) {
  for(int i = 0; i < r; i++) {
    m[i][i] += x;
    if(abs(m[i][i]) < M_SMALL*abs(x))
      m[i][i] = complex_0;
  }
  return *this;
}

MLC& MLC::operator-=( const Complex& x) {
  for(int i = 0; i < r; i++) {
    m[i][i] -= x;
    if(abs(m[i][i]) < M_SMALL*abs(x))
      m[i][i] = complex_0;
  }
  return *this;
}

char operator==(const MLC& x, const MLC& y) {
  if((x.r != y.r) || (x.c != y.c)) return 0;
  for(int i = 0; i < x.r; i++) {
    for(int j = 0; j < x.c; j++)
      if(x.m[i][j] != y.m[i][j]) return 0;
  }
  return 1;
}

MLI::MLI() {
  m = 0;
  r = 0;
  c = 0;
  rc = 1;

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

MLI::MLI(int rows, int columns, int initval) {
  int i,j;
  r = rows;
  c = columns;
  rc = 1;
  m = new int *[r];
  for(i = 0; i < r; i++)
    m[i] = new int[c];
  for(i = 0; i< r; i++) {
    for(j = 0; j < c; j++)
      m[i][j] = initval;

  }

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

MLI::MLI(int rows, int columns, int* values) {
  int i,j;
  r = rows;
  c = columns;
  rc = 1;
  m = new int *[r];
  for(i = 0; i < r; i++)
    m[i] = new int[c];
  int index = 0;
  for(i = 0; i< r; i++) {
    for(j = 0; j < c; j++)
      m[i][j] = values[index++];

  }

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

MLI::MLI(const MLI& X) {
  static char firstCall= 1;

  if(firstCall) {
    firstCall = 0;
    cerr << "*** WARNING *** The MLI copy constructor was called for" << endl;
    cerr   <<   "*** WARNING *** some inexplicable reason." << endl;
    cerr   <<   "*** WARNING *** You have a problem!" << endl;
  }

  int i,j;
  r = X.r;
  c = X.c;
  m = new int *[r];
  for(i = 0; i < r; i++)
    m[i] = new int[c];
  for(i = 0; i< r; i++) {
    for(j = 0; j < c; j++)
      m[i][j] = X.m[i][j];

  }
  rc = 1;

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}



MLI::~MLI() {
  clear();

#ifdef OBJECT_DEBUG
 objectCount--;
#endif
}

void MLI::clear() {
  if((r == 0) && (c == 0)) return;
  for(int i = 0; i < r; i++)
    delete []m[i];
  delete [] m;
  r = 0;
  c = 0;
}

MLI& MLI::operator=(const MLI& X) {

  if( this == &X ) {
  
  clear();
  int i,j;

  r = X.r;
  c = X.c;
  m = new int *[r];
  for(i = 0; i < r; i++)
    m[i] = new int[c];
  for(i = 0; i< r; i++) {
    for(j = 0; j < c; j++)
      m[i][j] = X.m[i][j];

  }
  }
  
  return *this;
}

char MLI::operator==(const int& x) {
  if(r != c) return 0;
  for(int i=0; i< r; i++) {
    if(m[i][i] != x) return 0;
    for(int j =0; j < c; j++)
      if((i != j) && (m[i][j] != 0)) return 0;
  }
  return 1;
}

MLI& MLI::operator+=( const int& x) {
  for(int i = 0; i < r; i++) {
    m[i][i] += x;
  }
  return *this;
}

MLI& MLI::operator-=( const int& x) {
  for(int i = 0; i < r; i++) {
    m[i][i] -= x;
  }
  return *this;
}

char operator==(const MLI& x, const MLI& y) {
  if((x.r != y.r) || (x.c != y.c)) return 0;
  for(int i = 0; i < x.r; i++) {
    for(int j = 0; j < x.c; j++)
      if(x.m[i][j] != y.m[i][j]) return 0;
  }
  return 1;
}


