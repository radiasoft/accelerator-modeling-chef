#ifndef ML_H
#define ML_H

#include <iostream.h>
#include <math.h>
#include <complex.h>
#include "complexAddon.h"

struct MLD {

  double **m;
  int r,c;			// number of rows and columns
  int rc;			// reference count

  // Private member functions_________________________________________

  void clear();
  void error(char * msgl, char * msg2 = " ")const ;
  // Constructors and destructors_____________________________________

  MLD();
  MLD(const MLD&);
  MLD(int rows ,int columns , double initval);
  MLD(int rows, int columns, double* initval);
  ~MLD();

  // Public member functions__________________________________________

 MLD& operator=(const MLD&);
 char operator==(const double&);
 MLD& operator+=(const double&);
 MLD& operator-=(const double&);
friend  char operator==(const MLD&,const MLD&);
#ifdef OBJECT_DEBUG
  static int objectCount;
#endif
};

struct MLC {

  Complex **m;
  int r,c;			// number of rows and columns
  int rc;			// reference count

  // Private member functions_________________________________________

  void clear();
  void error(char * msgl, char * msg2 = " ")const ;
  // Constructors and destructors_____________________________________

  MLC();
  MLC(const MLD&);
  MLC(const MLC&);
  MLC(int rows ,int columns , const Complex& initval);
  MLC(int rows, int columns, Complex* initval);
  ~MLC();

  // Public member functions__________________________________________

 MLC& operator=(const MLC&);
 char operator==(const Complex&);
 MLC& operator+=(const Complex&);
 MLC& operator-=(const Complex&);
friend  char operator==(const MLC&,const MLC&);
#ifdef OBJECT_DEBUG
  static int objectCount;
#endif

};

struct MLI {

  int **m;
  int r,c;			// number of rows and columns
  int rc;			// reference count

  // Private member functions_________________________________________

  void clear();
  void error(char * msgl, char * msg2 = " ")const ;
  // Constructors and destructors_____________________________________

  MLI();
  MLI(const MLI&);
  MLI(int rows ,int columns , int initval);
  MLI(int rows, int columns, int* initval);
  ~MLI();

  // Public member functions__________________________________________

 MLI& operator=(const MLI&);
 char operator==(const int&);
 MLI& operator+=(const int&);
 MLI& operator-=(const int&);
friend  char operator==(const MLI&,const MLI&);

#ifdef OBJECT_DEBUG
  static int objectCount;
#endif
};

#endif //ML_H
