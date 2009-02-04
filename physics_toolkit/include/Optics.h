/*************************************************************************
**************************************************************************
**************************************************************************
******
******  PHYSICS TOOLKIT: Library of utilites and Sage classes
******             which facilitate calculations with the
******             BEAMLINE class library.
******
******  File:      Optics.h
******
******  Copyright (c) Fermi Research Alliance LLC
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-07CH11359.
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******
******                                                                
******  Author:    Jean-Francois Ostiguy                                     
******             ostiguy@fnal.gov                         
******
**************************************************************************
*************************************************************************/

#ifndef OPTICS_H
#define OPTICS_H

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <basic_toolkit/VectorD.h>
#include <basic_toolkit/Matrix.h>
#include <beamline/LatticeFunctions.h>
#include <beamline/ElmPtr.h>

class JetParticle;
class Particle;
class beamline;

namespace sqlite { class connection; }

namespace Optics {

  void initdb( sqlite::connection& db );

  Vector tunes( sqlite::connection& db, JetParticle const& oneturnjp);

  JetParticle           find_closed_orbit( sqlite::connection& db, beamline const& bml, JetParticle const& jp );
  void                              orbit( sqlite::connection& db, beamline const& bml, JetParticle const& jp );

  CSLattFuncs     periodicCourantSnyder2D( sqlite::connection& db,                      JetParticle const& jp );
  int            propagateCourantSnyder2D( sqlite::connection& db, beamline const& bml, JetParticle const& jp,     CSLattFuncs const& initial );

  CSLattFuncs4D   periodicCourantSnyder4D( sqlite::connection& db,                      JetParticle const& jp );
  int            propagateCourantSnyder4D( sqlite::connection& db, beamline const& bml, JetParticle const& jp,     CSLattFuncs4D const& initial );

  Vector               periodicDispersion( sqlite::connection& db,                      JetParticle const& jp );
  int                 propagateDispersion( sqlite::connection& db, beamline const& bml, JetParticle const& jp,     Vector const& eta0 );

  MatrixD              periodicCovariance( sqlite::connection& db,                      JetParticle const& jp );
  void                propagateCovariance( sqlite::connection& db, beamline const& bml, JetParticle const& jp,     MatrixD const& cov     );

  ETLattFuncs          periodicEdwardTeng( sqlite::connection& db,                      JetParticle const& jp );
  void               propagateEdwardsTeng( sqlite::connection& db, beamline const& bml, JetParticle const& jp,     ETLattFuncs const& lf  );


  MatrixC            periodicEigenVectors( sqlite::connection& db,                      JetParticle const& oneturnjp );
  void              propagateEigenVectors( sqlite::connection& db, beamline const& bml, JetParticle const& jp, MatrixC const& ev0, Vector const& eta0 );


  std::vector<double>      lattice_function(std::string const& dbname, std::string const& colname);
  std::vector<double>            arclength( std::string const& dbname );
  std::vector<double>               beta_x( std::string const& dbname );
  std::vector<double>               beta_y( std::string const& dbname );
  std::vector<double>              alpha_x( std::string const& dbname );
  std::vector<double>              alpha_y( std::string const& dbname );
  std::vector<double>                psi_x( std::string const& dbname );
  std::vector<double>                psi_y( std::string const& dbname );
  std::vector<double>              beta_1x( std::string const& dbname );
  std::vector<double>              beta_1y( std::string const& dbname );
  std::vector<double>             alpha_1x( std::string const& dbname );
  std::vector<double>             alpha_1y( std::string const& dbname );
  std::vector<double>              beta_2x( std::string const& dbname );
  std::vector<double>              beta_2y( std::string const& dbname );
  std::vector<double>             alpha_2x( std::string const& dbname );
  std::vector<double>             alpha_2y( std::string const& dbname );
  std::vector<double>                eta_x( std::string const& dbname );
  std::vector<double>                eta_y( std::string const& dbname );
  std::vector<double>               etap_x( std::string const& dbname );
  std::vector<double>               etap_y( std::string const& dbname );


  Vector                     chromaticity( sqlite::connection& db, beamline const& bml, JetParticle const& jp, Vector const& eta );

  MatrixC             courantSnyder4DtoEV( CSLattFuncs4D const& lf );

  double getHTune( sqlite::connection& db );
  double getVTune( sqlite::connection& db );

  void cleardb( sqlite::connection& db); 

  bool     reference_orbit_ok( sqlite::connection& db );
  bool          dispersion_ok( sqlite::connection& db ); 
  bool        eigenvectors_ok( sqlite::connection& db );
  bool    courant_snyder2d_ok( sqlite::connection& db );
  bool        edwards_teng_ok( sqlite::connection& db ); 
  bool          covariance_ok( sqlite::connection& db ); 
  bool    courant_snyder4d_ok( sqlite::connection& db ); 


} // Optics namespace

#endif // OPTICS_H

