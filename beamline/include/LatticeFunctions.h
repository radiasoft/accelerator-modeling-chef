/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      LatticeFunctions.h
******                                                                
******  Copyright Fermi Research Alliance / Fermilab    
******            All Rights Reserved                             
*****
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-07CH11359 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws.
******
****** 
******  Author: Jean-Francois Ostiguy 
******          ostiguy@fnal.gov                                                              
******                                                                
**************************************************************************
**************************************************************************
*************************************************************************/

#ifndef LATTICEFUNCTIONS_H
#define LATTICEFUNCTIONS_H

#include <basic_toolkit/TVector.h>


struct Tunes {
    double hor;
    double ver;
    double lng;
};

struct LattRing  {
  struct {
    double hor;
    double ver;
  } tune;
  struct {
    double hor;
    double ver;
  } chromaticity;

} ;

struct reference_orbit_t {

   double x;
   double xp;
   double y;
   double yp;
};

struct dispersion_t {
  
  // the dispersion is stored in a vector of dimension 6 for compatibility with 6D transfer matrices

  dispersion_t( );
  dispersion_t( dispersion_t const& o );
  dispersion_t& operator=( dispersion_t const& rhs );
  
  TVector<double> eta;

  double& x;
  double& xp;
  double& y;
  double& yp;
};
 
//------------------------------------------------------
// Standard, uncoupled Courant-Snyder Lattice functions 
//------------------------------------------------------

struct CSLattFuncs {

  double arcLength;

  reference_orbit_t reference_orbit;

  dispersion_t   dispersion;

  struct beta_t {
    double hor;
    double ver;
  } beta;
  struct alpha_t {
    double hor;
    double ver;
  } alpha;
  struct psi_t {
    double hor;
    double ver;
  } psi;

}; // Courant-Snyder


//------------------------------------------------------
//  Generalized (4D) Courant-Snyder Lattice functions 
//------------------------------------------------------
//
//  beta_1x = mode1.beta.hor
//  beta_1y = mode1.beta.ver

//  beta_2x = mode2.beta.hor
//  beta_2y = mode2.beta.ver

// etc ....

struct CSLattFuncs4D{

  CSLattFuncs4D() {}

  explicit   CSLattFuncs4D( CSLattFuncs const& o);

  double arcLength;

  reference_orbit_t reference_orbit;

  dispersion_t   dispersion;
  
  struct beta_t { 
     double hor;
     double ver;
  };

  struct alpha_t { 
     double hor;
     double ver;
  };

  struct mode_t {
     beta_t  beta; 
     alpha_t alpha; 
     double  psi;
  };


  mode_t mode1;
  mode_t mode2;

}; 

//------------------------------------------------------
//  Edwards-Teng (4D) Lattice Functions 
//------------------------------------------------------

struct ETLattFuncs{

  ETLattFuncs() {} 

  explicit ETLattFuncs( CSLattFuncs const& lf); 

  double         arcLength;

  reference_orbit_t reference_orbit;
  dispersion_t   dispersion;

  struct  beta_t {
    double hor;
    double ver;
  } beta;
  struct alpha_t {
    double hor;
    double ver;
  } alpha;
  struct psi_t { 
    double hor;
    double ver;
  } psi;

  double  phi;

}; // Edwards-Teng


//------------------------------------------------------
//  Covariance  
//------------------------------------------------------

struct CVLattFuncs {

  CVLattFuncs() {} 

  explicit CVLattFuncs( CSLattFuncs const& lf); 

  double arcLength;

  reference_orbit_t reference_orbit;

  dispersion_t   dispersion;

  struct beta_t {
    double hor;
    double ver;
  } beta;
  struct alpha_t {
    double hor;
    double ver;
  } alpha;
  struct  psi_t {
    double hor;
    double ver;
  } psi;

   double covariance; 

};  // covariance



#endif // LATTICEFUNCTIONS_H
