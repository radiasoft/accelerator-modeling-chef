/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-vector.cpp
******                                                                
******  Copyright (c) Universities Research Association, Inc./ Fermilab    
******                All Rights Reserved                             
******
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S.and Foreign Copyright Laws. 
******                                                                
******  Author:    Jean-Francois Ostiguy                                     
******                                                                
******             Fermi National Laboratory, Batavia, IL   60510                                
******             ostiguy@fnal.gov                         
******
****************************************************************************/

#include <boost/python.hpp>
#include <boost/shared_array.hpp>
#include <VectorD.h>

using namespace boost::python;

struct VectorWrapper: public Vector {

  PyObject* m_self;  

  VectorWrapper(PyObject* self, const Vector& vec) : m_self(self), Vector(vec)  {}

  VectorWrapper(PyObject* self, int dimension ): m_self(self), Vector(dimension) { }  

  VectorWrapper(PyObject* self,  boost::python::tuple components ) : m_self(self) { 
    int dim = extract<int>( components.attr("__len__")() );
    double* comps = new double[dim];
    for (int i=0; i<dim; ++i) {
      comps[i] = extract<double>( components[i]);
    } 
    Vector* v = this;
    new(v) Vector(dim, comps); 
    // NOTE: comps is owned and destroyed by Vector
  }
   
  operator Vector () const {
    int dim = Dim();
      double* v = new double[ dim ];
      for ( int i=0; i< dim; ++dim ) {
	v[i] = (*this)(i);
      }
      return *( new Vector(dim,v) ); 
  }            

  void Set( boost::python::tuple components ) {
    
    int dim = extract<int>( components.attr("__len__")() );

    double* comps = new double[dim];
    for (int i=0; i<dim; ++i) {
      comps[i] = extract<double>( components[i] );
    } 
    Vector::Set(comps);
    delete [] comps; 
  }
};


// static TJet<double>  (*log_ptr  )(const TJet<double>& )          = &::log;


void wrap_vector () {

 class_<Vector,VectorWrapper> Vector_class_("Vector", init<int>() );

 Vector_class_.def( init<boost::python::tuple>() );
 Vector_class_.def("Dim",       &VectorWrapper::Dim);
 Vector_class_.def("Set",       &VectorWrapper::Set);     // void        Set              ( const double* );
 Vector_class_.def("IsNull",    &VectorWrapper::IsNull);  // char        IsNull           () const;
 Vector_class_.def("IsUnit",    &VectorWrapper::IsUnit);  // char        IsUnit           () const;
 Vector_class_.def("Abs",       &VectorWrapper::Abs);     // Vector      Abs              () const;
 Vector_class_.def("Norm",      &VectorWrapper::Norm);    // double      Norm             () const;
 Vector_class_.def("Unit",      &VectorWrapper::Unit);    // Vector      Unit             () const;           // returns unit vector
 Vector_class_.def("Rotate",    &VectorWrapper::Rotate);  // void        Rotate           ( Vector& v,       double  theta ) const;


//-----------------------------------
// *** addition ***
//-----------------------------------
Vector_class_.def(self + self);
//-----------------------------------
// *** subtraction ***
//-----------------------------------
Vector_class_.def(self - self);
//-----------------------------------
// *** multiplication ***
//-----------------------------------
Vector_class_.def(self * self);
Vector_class_.def(self * double() );
Vector_class_.def(double() * self );
//-----------------------------------
// *** division ****
//-----------------------------------
Vector_class_.def( self / double() );
//-----------------------------------
// *** string representation ****
//-----------------------------------
Vector_class_.def( self_ns::str(self) );         

}

#if  0 
=================================================================================================
class Vector 
{
private:
  int           dim;
  double*       comp;
  OutputFormat* ofPtr;

  static OutputFormat* defOFPtr;  // default OutputFormat

public:
  // Constructors and the destructor ...
  Vector(       int      /* dimension */  = 3,
          const double*  /* components */ = 0,
                OutputFormat*             = 0 );
  Vector( const Vector& );
  ~Vector();

  // Assignment ...
  void        Set              ( const double* );
  void        set              ( double, double, double );  // anachronistic
  double      operator()       ( int ) const; // return component
  double&     operator()       ( int );       // set    component

  // Algebraic functions ...
  Vector&       operator=      ( const Vector& );

  Vector        operator+      ( const Vector& ) const;
  Vector        operator+=     ( const Vector& );
  friend Vector operator-      ( const Vector& );
  Vector        operator-      ( const Vector& ) const;
  Vector        operator-=     ( const Vector& );
  Vector        operator*      (       double  ) const;
  friend Vector operator*      (       double, const Vector& );
  Vector        operator*=     (       double  );
  Vector        operator/      (       double  ) const;
  Vector        operator/=     (       double  );

  double        operator*      ( const Vector& ) const; // dot product
  Vector        operator^      ( const Vector& ) const; // cross product:
                                                        // only works if
                                                        // the vector is
                                                        // three-dimensional

  // Boolean functions ...
  char          operator==     ( const Vector& ) const;
  char          operator!=     ( const Vector& ) const;
  char          operator<      ( const Vector& ) const;
  char          operator<=     ( const Vector& ) const;
  char          operator>      ( const Vector& ) const;
  char          operator>=     ( const Vector& ) const;
  char          IsNull         () const;
  char          IsUnit         () const;

  // Queries ...
  int  Dim() const { return dim; }

  // Utilities ..
  static 
  void        setDefaultFormat ( const OutputFormat& );
  void        setOutputFormat  ( OutputFormat* x ) { ofPtr = x; }
  OutputFormat* getOutputFormat()                  { return ofPtr; }

  Vector      Abs              () const;
  double      Norm             () const;
  Vector      Unit             () const;           // returns unit vector
  void        Rotate           ( Vector& v, 
                                 double  theta ) const;
                                                   // rotates v through 
                                                   // an angle theta using
                                                   // *this as the axis

  friend std::ostream& operator<<( std::ostream&, const Vector& );

  // Exceptions ...
  struct GenericException : public std::exception
  {
    GenericException( const char* fcn, const char* msg );
    // 1st argument: identifies function containing throw
    // 2nd         : identifies type of error
    ~GenericException() throw() {}
    const char* what() const throw();
    std::string w;
  };
==========================================================================================
#endif
