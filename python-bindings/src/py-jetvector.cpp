/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                   
******  File:      py-jetvector.cpp
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
#include <mxyzptlk/JetVector.h>
#include <basic_toolkit/VectorD.h>

//------------------------------------------------------------------------------
// Local auxiliary functions and wrapper classe(s)
//------------------------------------------------------------------------------

using namespace boost::python;

template<typename T>
class JetVectorWrapper: public TJetVector<T> {
 
 private:
  
   PyObject* m_self;

 public:

  JetVectorWrapper( PyObject* self): m_self(self), TJetVector<T>() {}
  JetVectorWrapper( PyObject* self, TJetVector<T> const& v ): m_self(self), TJetVector<T>( v) {} 


};


static void       (JetVector::*Rotate_1_ptr   )(JetVector&, double     )  const  =  &JetVector::Rotate;
static void       (JetVector::*Rotate_2_ptr   )(JetVector&, const Jet& )  const  =  &JetVector::Rotate;
static void       (JetVector::*peekAt_ptr     )()                         const  =  &JetVector::peekAt; 
static void       (JetVector::*printCoeffs_ptr)()                         const  =  &JetVector::printCoeffs;
static Jet        (JetVector::*Norm_ptr       )()                         const  =  &JetVector::Norm; 
static JetVector  (JetVector::*Unit_ptr       )()                         const  =  &JetVector::Unit;
static JetVector  (JetVector::*filter_1_ptr   )(int, int                ) const   = &JetVector::filter;

static void       (JetCVector::*RotateC_1_ptr    )(JetCVector&, double       ) const  = &JetCVector::Rotate;
static void       (JetCVector::*RotateC_2_ptr    )(JetCVector&, const JetC&  ) const  = &JetCVector::Rotate;
static void       (JetCVector::*peekAtC_ptr      )()                           const  = &JetCVector::peekAt; 
static void       (JetCVector::*printCoeffsC_ptr )()                           const  = &JetCVector::printCoeffs;
static JetC       (JetCVector::*NormC_ptr        )()                           const  = &JetCVector::Norm; 
static JetCVector (JetCVector::*UnitC_ptr        )()                           const  = &JetCVector::Unit;
static JetCVector (JetCVector::*filterC_1_ptr    )(int, int                )   const  = &JetCVector::filter;


//------------------------------------------------------------------------------
// Python wrappers
//------------------------------------------------------------------------------



void wrap_mxyzptlk_jetvector() {

  class_<JetVector, JetVectorWrapper<double> > JetVector_class_ ("JetVector", init<>() );

  JetVector_class_.def( "Dim",           &JetVector::Dim          );
  JetVector_class_.def( "Env",           &JetVector::Env          );
  JetVector_class_.def( "AccuWgt",       &JetVector::AccuWgt      );
  JetVector_class_.def( "Weight",        &JetVector::Weight       );
  JetVector_class_.def( "SetComponent",  &JetVector::SetComponent );
  JetVector_class_.def( "filter",        filter_1_ptr             );
  JetVector_class_.def( "peekAt",        peekAt_ptr               ); 
  JetVector_class_.def( "printCoeffs",   printCoeffs_ptr          );
  JetVector_class_.def( "Norm",          Norm_ptr                 );
  JetVector_class_.def( "Unit",          Unit_ptr                 );
  JetVector_class_.def( "Rotate",        Rotate_1_ptr             );
  JetVector_class_.def( "Rotate",        Rotate_2_ptr             );
  JetVector_class_.def( "IsNull",        &JetVector::IsNull       );
  JetVector_class_.def( "IsUnit",        &JetVector::IsUnit       );
  JetVector_class_.def( "IsNilpotent",   &JetVector::IsNilpotent  );


  //-----------------------------------
  // *** addition ***
  //-----------------------------------
  JetVector_class_.def(self     + self);
  JetVector_class_.def(self     + other<Vector>() );
  JetVector_class_.def(other<Vector>() + self );
  JetVector_class_.def(self     += self);
  JetVector_class_.def(self     += other<Vector>() );
  //-----------------------------------
  // *** subtraction ***
  //-----------------------------------
  JetVector_class_.def(self - self);
  JetVector_class_.def(self - other<Vector>() );
  JetVector_class_.def(other<Vector>() - self );
  JetVector_class_.def(self     -= self);
  JetVector_class_.def(self     -= other<Vector>() );

  //-----------------------------------
  // *** multiplication ***
  //-----------------------------------
  JetVector_class_.def(self * double()); 
  JetVector_class_.def(double() * self);
  // JetVector_class_.def(self *= self);
  JetVector_class_.def(self *= double());
  //JetVector_class_.def(self * self);             // dot product 
  //JetVector_class_.def(self * other<Vector>());  // dot product 
 
  //-----------------------------------
  // *** division ****
  //-----------------------------------
  JetVector_class_.def(self  / double()   );
  JetVector_class_.def(self  /= double()  );


}

void wrap_mxyzptlk_jetvectorc() {

  class_<JetCVector, JetVectorWrapper<std::complex<double> > >JetCVector_class_("JetCVector", init<>() );

  JetCVector_class_.def( "Dim",           &JetCVector::Dim          );
  JetCVector_class_.def( "Env",           &JetCVector::Env          );
  JetCVector_class_.def( "AccuWgt",       &JetCVector::AccuWgt      );
  JetCVector_class_.def( "Weight",        &JetCVector::Weight       );
  JetCVector_class_.def( "SetComponent",  &JetCVector::SetComponent );
  JetCVector_class_.def( "filter",        filterC_1_ptr             );
  JetCVector_class_.def( "peekAt",        peekAtC_ptr               ); 
  JetCVector_class_.def( "printCoeffs",   printCoeffsC_ptr          );
  JetCVector_class_.def( "Norm",          NormC_ptr                 );
  JetCVector_class_.def( "Unit",          UnitC_ptr                 );
  JetCVector_class_.def( "Rotate",        RotateC_1_ptr             );
  JetCVector_class_.def( "Rotate",        RotateC_2_ptr             );
  JetCVector_class_.def( "IsNull",        &JetCVector::IsNull       );
  JetCVector_class_.def( "IsUnit",        &JetCVector::IsUnit       );
  JetCVector_class_.def( "IsNilpotent",   &JetCVector::IsNilpotent  );

  //-----------------------------------
  // *** addition ***
  //-----------------------------------
  JetCVector_class_.def(self     + self);
//JetCVector_class_.def(self     + other<VectorC>() );
//JetCVector_class_.def(other<VectorC>() + self );
  JetCVector_class_.def(self     += self);
// JetCVector_class_.def(self    += other<VectorC>() );
  //-----------------------------------
  // *** subtraction ***
  //-----------------------------------
  JetCVector_class_.def(self - self);
  // JetCVector_class_.def(self - other<VectorC>() );
  //  JetCVector_class_.def(other<VectorC>() - self )
  JetCVector_class_.def(self     -= self);
  //JetCVector_class_.def(self     -= other<VectorC>() );

  //-----------------------------------
  // *** multiplication ***
  //-----------------------------------
  // JetCVector_class_.def(self * self);     // dot product, not implemented  
  JetCVector_class_.def(self * std::complex<double>()); 
  JetCVector_class_.def(std::complex<double>() * self);
  //JetCVector_class_.def(self *= self);
  JetCVector_class_.def(self *= std::complex<double>());
  // JetCVector_class_.def(self * other<VectorC>());     // dot product 
 
  //-----------------------------------
  // *** division ****
  //-----------------------------------
  JetCVector_class_.def(self  / std::complex<double>()  );
  JetCVector_class_.def(self  /= std::complex<double>()  );

}


#if 0
===================================================================================== 
  TJetVector( int   dim = -1,
              const TJet<T>* components = 0, 
              const typename EnvPtr<T>::Type = (TJet<T>::lastEnvironment()) );

  void             Set             ( const TJet<T>* );
  void             SetComponent    ( int, const TJet<T>& );
  TJet<T>      operator()      ( int ) const; 
  TJet<T>&     operator()      ( int ); 
                   // WARNING: There is no way to use this and be
                   //          assured that environments are consistent.

  // Algebraic functions ...
  TJetVector&       operator=      ( const TJetVector& );

  TJetVector        operator+      ( const TJetVector& ) const;
  TJetVector        operator+      ( const Vector& ) const;
  TJetVector        operator+=     ( const TJetVector& );
  TJetVector        operator+=     ( const Vector& );
  friend TJetVector operator-<>    ( const TJetVector& );
  TJetVector        operator-      ( const TJetVector& ) const;
  TJetVector        operator-      ( const Vector& ) const;
  TJetVector        operator-=     ( const TJetVector& );
  TJetVector        operator-=     ( const Vector& );

  friend TJetVector operator*<>    ( const TJet<T>&, const TJetVector& );
  friend TJetVector operator*<>    ( const T&, const TJetVector& );
  friend TJetVector operator*<>    ( const TMatrix<T>&, const TJetVector& );

  TJetVector        operator*      ( const TJet<T>&    ) const;
  TJetVector        operator*      ( const T& ) const;
  TJetVector        operator*=     ( const TJet<T>&    );
  TJetVector        operator*=     (       T  );
  TJetVector        operator/      ( const TJet<T>&    ) const;
  TJetVector        operator/      (       T  ) const;
  TJetVector        operator/=     ( const TJet<T>&    );
  TJetVector        operator/=     (       T  );


  TJet<T>       operator*      ( const TJetVector& ) const; // dot product
  TJet<T>       operator*      ( const Vector& )     const; // dot product

  friend TJetVector operator^<>    ( const Vector&, const TJetVector& );

  TJetVector        operator^      ( const TJetVector& ) const; // cross product:
                                                                // only works if
                                                                // the vector is
                                                                // three-dimensional
  TJetVector        operator^      ( const Vector& ) const;


  // Boolean functions ...
  bool operator==     ( const TJetVector& ) const;
  bool operator==     ( const T&    ) const;
  bool operator!=     ( const TJetVector& ) const;
  bool operator!=     ( const T&    ) const;
  bool operator<      ( const TJetVector& ) const;
  bool operator<=     ( const TJetVector& ) const;
  bool operator>      ( const TJetVector& ) const;
  bool operator>=     ( const TJetVector& ) const;
  bool IsNull         () const;
  bool IsUnit         () const;
  bool IsNilpotent    () const;


  // Functions related to differentiation

  void weightedDerivative( int*, T* );
  void derivative( int*, T* );
  

  // Queries ...
  int  Dim() const;
  const typename EnvPtr<T>::Type Env() const;
  int AccuWgt() const;
  int Weight()  const;
  void standardPart( T* ) const;
  void getReference( T* ) const;

  // Utilities ..
  void        peekAt           () const;
  void        printCoeffs      () const;
  TJet<T> Norm             () const;
  TJetVector  Unit             () const;           // returns unit vector

  void        Rotate           ( TJetVector& v, double theta ) const;
  void        Rotate           ( TJetVector& v, const TJet<T>& theta ) const;
                                                   // rotates v through 
                                                   // an angle theta using
                                                   // *this as the axis

  friend std::ostream& operator<<<>( std::ostream&, const TJetVector& );
  friend std::istream& operator>><>( std::istream&, TJetVector& );

  TJetVector filter( int, int );
  TJetVector filter( bool (*[]) ( const IntArray&, const T& ) );
} ;


template<typename T>
TJetVector<T> operator*( const TJet<T>&, const Vector& );


====================================================================================================
#endif
