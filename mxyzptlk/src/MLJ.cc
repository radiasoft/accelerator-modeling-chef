/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******  Version:   4.1                    
******                                    
******  File:      MLJ.cc
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


#include "MLJ.h"

// ================================================================
//      Global variables
//

#ifdef OBJECT_DEBUG
int MLJ::objectCount = 0;
#endif

using namespace std;

// ================================================================
//      Constructors and the destructor ...
//

MLJ::MLJ() {
  m = 0;
  r = 0;
  c = 0;
  rc = 1;

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}


MLJ::MLJ(int rows, int columns, const Jet& initval) {
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

MLJ::MLJ(int rows, int columns, Jet* values) {
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

MLJ::MLJ(const MLJ& X) {
  static char firstCall= 1;

  if(firstCall) {
    firstCall = 0;
    cerr << "*** WARNING *** The MLJ copy constructor was called for" << endl;
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


MLJ::~MLJ() {
  clear();

#ifdef OBJECT_DEBUG
 objectCount--;
#endif
}

void MLJ::clear() {
  if((r == 0) && (c == 0)) return;
  for(int i = 0; i < r; i++)
    delete [] m[i];
  delete m;
  r = 0;
  c = 0;
}

MLJ& MLJ::operator=(const MLJ& X) {
  if( this != &X ) {

  clear();
  int i,j;

  r = X.r;
  c = X.c;
  m = new Jet *[r];
  for(i = 0; i < r; i++)
    m[i] = new Jet[c];
  for(i = 0; i< r; i++) {
    for(j = 0; j < c; j++) {
      m[i][j] = X.m[i][j];
       }
  }
  }
  return *this;
}


char MLJ::operator==(const Jet& x) {
  if(r != c) return 0;
  for(int i=0; i< r; i++) {
    if(m[i][i] != x) return 0;
    for(int j =0; j < c; j++)
      if((i != j) && (m[i][j] != 0)) return 0;
  }
  return 1;
}

MLJ& MLJ::operator+=( const Jet& x) {
  for(int i = 0; i < r; i++) {
    m[i][i] += x;
  }
  return *this;
}

MLJ& MLJ::operator-=( const Jet& x) {
  for(int i = 0; i < r; i++) {
    m[i][i] -= x;
  }
  return *this;
}

char operator==(const MLJ& x, const MLJ& y) {
  if((x.r != y.r) || (x.c != y.c)) return 0;
  for(int i = 0; i < x.r; i++) {
    for(int j = 0; j < x.c; j++)
      if(x.m[i][j] != y.m[i][j]) return 0;
  }
  return 1;
}




////////////////////////////////////////////////////////////////////////
