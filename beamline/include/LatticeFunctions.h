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

#include <boost/variant.hpp>

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

struct closed_orbit_t {
   double x;
   double xp;
   double y;
   double yp;
};

struct dispersion_t {
    double x;
    double xp;
    double y;
    double yp;
};
 
struct CSLattFuncs {

  double arcLength;

  closed_orbit_t closed_orbit;
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


struct ETLattFuncs{

  double         arcLength;

  closed_orbit_t closed_orbit;
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


struct CVLattFuncs {

  double arcLength;

  closed_orbit_t closed_orbit;
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


struct EMLattFuncs {

  double arcLength;

  closed_orbit_t closed_orbit;
  dispersion_t   dispersion;

  double beta1x; 
  double beta1y; 
  double alpha1x; 
  double alpha1y; 
  double psi1;
  double beta2x; 
  double beta2y; 
  double alpha2x; 
  double alpha2y; 
  double psi2;


};  // eigenmodes


typedef boost::variant<CSLattFuncs, ETLattFuncs, EMLattFuncs, CVLattFuncs>  LattFuncs;


#if 0
std::ostream& operator<<(std::ostream&, LattRing const&);
std::ostream& operator<<(std::ostream&, LattFunc const&);
std::istream& operator>>(std::istream&, LattFunc&);
#endif

#endif // LATTICEFUNCTIONS_H
