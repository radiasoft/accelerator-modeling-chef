#ifndef MLJ_H
#define MLJ_H

#include <iostream.h>
#include <math.h>
#include <complex.h>
//#include "complexAddon.h"
#include "Jet.h"

///////////////////////////////////////////////////////////////////////
struct MLJ {

  Jet **m;
  int r,c;			// number of rows and columns
  int rc;			// reference count

  // Private member functions_________________________________________
  void clear();
  void error(char * msgl, char * msg2 = " ")const ;

  // Constructors and destructors_____________________________________
  MLJ();
  MLJ(const MLJ&);
  MLJ(int rows ,int columns , const Jet& initval);
  MLJ(int rows, int columns, Jet* initval);
  ~MLJ();

  // Public member functions__________________________________________ 
 MLJ& operator=(const MLJ&);
 char operator==(const Jet&);
 MLJ& operator+=(const Jet&);
 MLJ& operator-=(const Jet&);
friend  char operator==(const MLJ&,const MLJ&);

#ifdef OBJECT_DEBUG
  static int objectCount;
#endif
};


//////////////////////////////////////////////////////////////////////////

#endif
