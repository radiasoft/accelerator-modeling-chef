////////////////////////////////////////////////////////////
// 
// File:          normalFormSage.h
//                                                                
// -------------------------------------------------------------
// 
// Copyright Universities Research Association, Inc./ Fermilab    
//           All Rights Reserved                             
//
// Usage, modification, and redistribution are subject to terms          
// of the License supplied with this software.
// 
// Software and documentation created under 
// U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
// The U.S. Government retains a world-wide non-exclusive, 
// royalty-free license to publish or reproduce documentation 
// and software for U.S. Government purposes. This software 
// is protected under the U.S. and Foreign Copyright Laws.
// 
// -------------------------------------------------------------
// 
// Author:        Leo Michelotti
// 
// REVISION HISTORY
// 
// January 14, 2011, EGS rework for normalFormSage
// Thursday, September 30, 2010.    (original version)
// 
////////////////////////////////////////////////////////////


#ifndef NORMALFORMSAGE_H
#define NORMALFORMSAGE_H

#include <iostream>
#include <stdlib.h>

#include <basic_toolkit/Matrix.h>
#include <basic_toolkit/VectorD.h>
#include <mxyzptlk/mxyzptlk.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>

#define DIM    6
#define I_X 0
#define I_Y 1
#define I_T 2
#define I_PX 3
#define I_PY 4
#define I_DE 5

class normalFormSage
{
 public:
  normalFormSage( Mapping & Map, const JetParticle & jpr, int theOrder = 1);
  ~normalFormSage();

  // The following variables are public just for convenience.
  // They should only be modified by class EquilibriumSage!

  MatrixC          _E;    // matrix of eigenvectors
  MatrixC          _invE; // and its inverse
  CLieOperator*    _XG;   // it turns out that _N is predefined
  CLieOperator*    _T;    // by compiler
  MappingC*        _f;
  MappingC*        _g;

  void      setState( const Vector& );
  void      cnvDataToNormalForm( const Vector& , VectorC&);
  void      cnvDataFromNormalForm( const VectorC&, Vector& );
  const MatrixC&  getNormalEigenvectors(void);
  bool       checkLinearNormalForm();
  bool       checkLinearNormalForm(double toler);
  Mapping   getCanonMap();
  bool      isIterating();
  Mapping   CanonToChef;
  Mapping   ChefToCanon;
  std::vector<double> stationaryActions(const double sx, const double sy, const double sz);

 // private:
  Vector           _state;

  int              _order;
  bool             _initialized;
  Jet__environment_ptr _p_je;
 
  Mapping*           _p_rf;   // the input beamline map
  Mapping         _canonMap;    // the canonical map used for normal form calc
 bool             _deleteContext;
  bool             _is_iterating;
};

#endif // NORMALFORMSAGE_H
