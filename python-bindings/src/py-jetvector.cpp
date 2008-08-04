/***************************************************************************                                                               
***************************************************************************                                                               
***************************************************************************                                                               
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
****************************************************************************
****************************************************************************
****************************************************************************/

#include <boost/python.hpp>
#include <mxyzptlk/JetVector.h>
#include <basic_toolkit/VectorD.h>

//------------------------------------------------------------------------------
// Local auxiliary functions and wrapper classe(s)
//------------------------------------------------------------------------------

using namespace boost::python;

namespace {

void SetComponent( JetVector& v, int i, Jet const& value) 
{
  v[i] = value;
}    

void SetComponentC( JetCVector& v, int i, JetC const& value) 
{
  v[i] = value;
}    


JetVector        (JetVector::*Rotate_1_ptr   )(JetVector const&, double     )  const  =  &JetVector::Rotate;
JetVector        (JetVector::*Rotate_2_ptr   )(JetVector const&, Jet const& )  const  =  &JetVector::Rotate;
void             (JetVector::*printCoeffs_ptr)()                         const  =  &JetVector::printCoeffs;
Jet              (JetVector::*Norm_ptr       )()                         const  =  &JetVector::Norm; 
JetVector const  (JetVector::*Unit_ptr       )()                         const  =  &JetVector::Unit;
JetVector        (JetVector::*filter_1_ptr   )(int, int                ) const   = &JetVector::filter;

// not implemented 
// JetCVector       (JetCVector::*RotateC_1_ptr    )(JetCVector const&, double       ) const  = &JetCVector::Rotate;
// JetCVector       (JetCVector::*RotateC_2_ptr    )(JetCVector const&, JetC const&  ) const  = &JetCVector::Rotate;

void             (JetCVector::*printCoeffsC_ptr )()                           const  = &JetCVector::printCoeffs;
JetC             (JetCVector::*NormC_ptr        )()                           const  = &JetCVector::Norm; 
JetCVector const (JetCVector::*UnitC_ptr        )()                           const  = &JetCVector::Unit;
JetCVector       (JetCVector::*filterC_1_ptr    )(int, int                )   const  = &JetCVector::filter;

} // anonymous namespace

//------------------------------------------------------------------------------
// Python wrappers
//------------------------------------------------------------------------------



void wrap_mxyzptlk_jetvector() {

  class_<JetVector> JetVector_class_ ("JetVector", init<>() );

  JetVector_class_.def( "Dim",           &JetVector::Dim          );
  JetVector_class_.def( "Env",           &JetVector::Env          );
  JetVector_class_.def( "AccuWgt",       &JetVector::AccuWgt      );
  JetVector_class_.def( "Weight",        &JetVector::Weight       );
  JetVector_class_.def( "SetComponent",  &SetComponent            );
  JetVector_class_.def( "filter",        filter_1_ptr             );
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

  class_<JetCVector>  JetCVector_class_("JetCVector", init<>() );

  JetCVector_class_.def( "Dim",           &JetCVector::Dim          );
  JetCVector_class_.def( "Env",           &JetCVector::Env          );
  JetCVector_class_.def( "AccuWgt",       &JetCVector::AccuWgt      );
  JetCVector_class_.def( "Weight",        &JetCVector::Weight       );
  JetCVector_class_.def( "SetComponent",  &SetComponentC            );
  JetCVector_class_.def( "filter",        filterC_1_ptr             );
  JetCVector_class_.def( "printCoeffs",   printCoeffsC_ptr          );
  JetCVector_class_.def( "Norm",          NormC_ptr                 );
  JetCVector_class_.def( "Unit",          UnitC_ptr                 );
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
