#ifndef JETML_H
#define JETML_H


#include <math.h>

#ifdef __VISUAL_CPP__
#include <iostream>
#include <complex>
#else
#include <iostream.h>
#include <complex.h>
#endif

#include "complexAddon.h"
#include "Jet.h"

struct JetML {

  Jet **m;
  int r,c;			// number of rows and columns
  int rc;			// reference count

  // Private member functions_________________________________________

  void clear();
  void error(char * msgl, char * msg2 = " ")const ;
  // Constructors and destructors_____________________________________

  JetML();
  JetML(const JetML&);
  JetML(int rows ,int columns , const Jet& initval);
  JetML(int rows, int columns, Jet* initval);
  ~JetML();

  // Public member functions__________________________________________

 JetML& operator=(const JetML&);
 char operator==(const Jet&);
 JetML& operator+=(const Jet&);
 JetML& operator-=(const Jet&);
friend  char operator==(const JetML&,const JetML&);
#ifdef OBJECT_DEBUG
  static int objectCount;
#endif

};

#endif // JETML_H
