/*************************************************************************
**************************************************************************
**************************************************************************
******
******  PHYSICS TOOLKIT: Library of utilites and Sage classes
******             which facilitate calculations with the
******             BEAMLINE class library.
******
******  File:      Optics.cc
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
******  Authors:  Jean-Francois Ostiguy  ostiguy@fnal.gov
*****             Leo Michelotti         michelotti@fnal.gov
******              
******       
******
**************************************************************************
**************************************************************************
*************************************************************************/

#include <physics_toolkit/Optics.h>

#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/MathConstants.h>
#include <basic_toolkit/NewtonSolver.h>
#include <mxyzptlk/Jet__environment.h>
#include <beamline/beamline.h>
#include <beamline/rfcavity.h>
#include <beamline/Monitor.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/LatticeFunctions.h>
#include <physics_toolkit/BmlUtil.h>
#include <algorithm>
#include <limits>
#include <cmath>

#include <physics_toolkit/Sage.h> // REMOVE needed for fpsolver.


// sqlite -----------------

#include <unistd.h>
#include <sstream>
#include <sqlite/query.hpp>
#include <sqlite/command.hpp>
#include <sqlite/execute.hpp>
#include <sqlite/transaction.hpp>
#include <basic_toolkit/dbutils.h>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <basic_toolkit/Matrix.h>
#include <basic_toolkit/VectorD.h>

using FNAL::pcout;
using FNAL::pcerr;

using namespace MathConstants;
using namespace std;

namespace {

 typedef PhaseSpaceIndexing::index index;

 index const i_x    = Particle::i_x;
 index const i_y    = Particle::i_y;
 index const i_cdt  = Particle::i_cdt;
 index const i_npx  = Particle::i_npx;
 index const i_npy  = Particle::i_npy;
 index const i_ndp  = Particle::i_ndp;

}


namespace Optics {

struct null_deleter
{
    void operator()(void const *) const
    {}
};


void initdb( sqlite::connection& db )
{
  std::ostringstream sql;

  sql.str("");
  sql << "CREATE TABLE PROPERTIES"
      << "( is_ring INTEGER, p0 REAL, imuH INTEGER, fmuH REAL, imuV INTEGER, fmuV REAL,  imuL INTEGER, fmuL REAL, chromaH REAL, chromaV REAL, gammat REAL )"
      << std::ends;

  sqlite::execute(db, sql.str(),true );

  sql.str("");
  sql << "INSERT INTO PROPERTIES "  
      << " ( is_ring, p0, imuH, fmuH, imuV, fmuV,  imuL, fmuL,  chromaH, chromaV, gammat )  VALUES ( ?,?, ?,?, ?,?, ?,?,  ?,?, ? ) " << std::ends;

  sqlite::command cmd_props( db, sql.str());
  
  cmd_props % sqlite::null_type() %sqlite::null_type()  % sqlite::null_type() % sqlite::null_type()
                                                        % sqlite::null_type() % sqlite::null_type()
                                                        % sqlite::null_type() % sqlite::null_type()
                                                        % sqlite::null_type() % sqlite::null_type() % sqlite::null_type() ;
  cmd_props(); 

  sql.str("");
  sql << "CREATE TABLE " 
      << "ELEMENTS "
      << "( iseq INTEGER PRIMARY KEY, object INTEGER UNIQUE, name TEXT, type TEXT, length REAL, strength REAL )"
      << std::ends;

  sqlite::execute(db, sql.str(),true );

  sql.str("");
  sql << "CREATE TABLE DISPERSION "
      << "( iseq INTEGER PRIMARY KEY, "
      << "etax REAL,  etapx REAL,  etay REAL,  etapy REAL )"
      << std::ends;

  sqlite::execute(db, sql.str(),true );
   
  sql.str("");
  sql << "CREATE TABLE " 
      << "COURANT_SNYDER"
      << "( iseq INTEGER PRIMARY KEY, betax REAL, alphax REAL, psix REAL, betay REAL, alphay REAL, psi_y REAL  )" 
      << ends;
  sqlite::execute(db, sql.str(),true );

  sql.str("");
  sql << "CREATE TABLE " 
      << "EIGENVECTORS"
      << "( iseq INTEGER, component INTEGER,  e0_real REAL,   e0_imag REAL,  e1_real REAL,  e1_imag REAL,  e2_real REAL,   e2_imag REAL, "
      <<                                    " e3_real REAL,   e3_imag REAL,  e4_real REAL,  e4_imag REAL,  e5_real REAL,   e5_imag REAL  )"
      << ends;
  sqlite::execute(db, sql.str(),true );

  sql.str("");
  sql << "CREATE TABLE " 
      << "COURANT_SNYDER_4D"
      << " ( iseq INTEGER PRIMARY KEY, beta1x REAL,    alpha1x REAL,   beta1y     REAL, alpha1y    REAL,   psi1 REAL,"
      <<                              "beta2x REAL,    alpha2x REAL,   beta2y     REAL, alpha2y    REAL,   psi2 REAL )"
      << ends;
  sqlite::execute(db, sql.str(),true );

  sql.str("");
  sql << "CREATE TABLE COVARIANCE"
      << "( iseq INTEGER PRIMARY KEY, " 
      << "  cov_xx  REAL,  cov_xpx REAL, cov_xpxp REAL, cov_yx  REAL, cov_yxp  REAL, "  
      << "  cov_yy  REAL,  cov_ypx REAL, cov_ypxp REAL, cov_ypy REAL, cov_ypyp REAL )" 
      << ends;

  sqlite::execute(db, sql.str(),true );

  sql.str("");
  sql << "CREATE TABLE " 
      << "EDWARDS_TENG"
      << "( iseq INTEGER PRIMARY KEY, beta_h REAL, alpha_h REAL, psi_h REAL, beta_v REAL, alpha_v REAL, psi_v REAL, phi REAL  )" 
      << std::ends;

  sqlite::execute(db, sql.str(),true );

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//-------------------------------------------------------------------------------
// local functions used by find_closed_orbit
//-------------------------------------------------------------------------------

static Matrix invjacobian ( JetParticle const& jp0, beamline const& bml, Vector v )
{ 

  static MatrixD M = Matrix(6,6);

  static bool first = true;

  if (!first) return M;

  if (first) first=false;

  static int i_x   = 0; 
  static int i_y   = 1;
  static int i_cdt = 2;
  static int i_npx = 3;
  static int i_npy = 4;
  static int i_ndp = 5;

  static int l_[] = { i_x, i_npx, i_y, i_npy, i_cdt, i_ndp };

  JetParticle jp = jp0;

  jp.state() = Mapping("I", jp.state().Env() );
    
  bml.propagate(jp);

  M = jp.state().jacobian();

  // some ugly index rearranging ... 

  MatrixD M4d( 4, 4 );

   for( int i=0; i<4; ++i) {
     for( int j=0; j<4; ++j ){ 
       M4d[i][j] = M[l_[i]][l_[j]];
     }
   }

   for( int i=0; i<4; ++i) M4d[i][i] -= 1.0;

   M4d = M4d.inverse();

   for (int i=0; i<6; ++i )  { 
     for (int j=0; j<6 ; ++j ) {
         M[i][j] = 0.0;
     }
   }
      
   for (int i=0; i<4; ++i ) {
     for (int j=0; j<4; ++j ) {
       M[ l_[i] ] [l_[j] ] = M4d[i][j];
     }
   } 
   
   return M;  // contains inverse of the 4D (transverse) jacobian. 
              // All other components set to 0.  
  
}

static Vector map ( Particle const& p0, beamline const& bml, Vector const& v0 )
{ 
   Particle   p = p0;
  
   p.state() = v0;

   bml.propagate(p);

   Vector v = p.state() - v0;
 
   v[5] = 0.0;

   return v;
}

static double norm ( Vector const & v )
{ 

  double value = v[0];

  for (int i=0; i<6; ++i ) {
   if (i == 2) continue;
   if (i == 5) continue;
   value = std::max( value, std::abs(v[i]));
  }
  return value;  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


JetParticle find_closed_orbit( sqlite::connection& db, beamline const& bml, JetParticle const& jp )
{

  // TURN RF OFF **** FIXME     

   Jet__environment::pushEnv( jp.state().Env() );
   JetC__environment::pushEnv( jp.state().Env() );


   JetParticle jpco(jp);


   NewtonSolver newton ( boost::bind<Vector>( &map,      Particle(jpco), boost::cref(bml), _1 ),  
                         boost::function<double(Vector const& )>(&norm), 
                         boost::bind<Matrix>( &invjacobian, jpco,        boost::cref(bml), _1 )  ); 

   Vector co = newton( Particle(jpco).state() );
   co[ i_cdt] = 0.0;

   jpco.setState( co );
    
   bml.propagate( jpco );

   jpco.state()[ i_cdt ].setStandardPart(0.0);


  /// ***** FIXME: NEED TO SET ENV REF TO that of closed orbit ! 

  // Register the closed orbit with the beamline.

   const_cast<beamline&>(bml).registerReference((Particle(jpco) ));

  // TURN RF ON  ****FIXME   

    Jet__environment::popEnv();
   JetC__environment::popEnv();



  return jpco;
} 


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void orbit( sqlite::connection& db, beamline const& bml, JetParticle const& jp_arg )
{
  //------------------------------------
  // propagate particle and store orbit
  //------------------------------------ 

  Particle p(jp_arg); 
  p.cdt(0.0);
  JetParticle jp(p,jp_arg.state().Env());
  jp.cdt(0.0);

  std::ostringstream sql;

  sql << "DROP TABLE IF EXISTS REFERENCE_ORBIT" <<std::ends; 
  sqlite::execute(db, sql.str(),true );

  sql.str("");
  sql << "DROP TABLE IF EXISTS ELEMENTS" <<std::ends; 
  sqlite::execute(db, sql.str(),true );

  sql.str("");
  sql << "CREATE TABLE " 
      << "REFERENCE_ORBIT "
      << "( iseq INTEGER PRIMARY KEY, arclength REAL, xco REAL,  yco REAL, beta REAL, gamma REAL )"
      << std::ends;

  sqlite::execute(db, sql.str(),true );

  sql.str("");
  sql << "CREATE TABLE " 
      << "ELEMENTS "
      << "( iseq INTEGER PRIMARY KEY, object INTEGER UNIQUE, name TEXT, type TEXT, length REAL, strength REAL )"
      << std::ends;

  sqlite::execute(db, sql.str(),true );

  sql.str("");
  sql << "INSERT INTO REFERENCE_ORBIT "  
      << " ( iseq, arclength, xco, yco, beta, gamma )  VALUES (?, ?, ?, ?, ?, ?)" << std::ends;

  sqlite::command cmd_corbit(db, sql.str());


  sql.str("");
  sql << "INSERT INTO ELEMENTS "  
      << " ( iseq, object, name, type, length, strength )  VALUES (?, ?, ?, ?, ?, ? )" << std::ends;

  sqlite::command cmd_elements(db, sql.str());


  sqlite::execute(db, "BEGIN TRANSACTION", true);

  int iseq    = -1;
  double  lng = 0.0;
  for ( beamline::const_deep_iterator it  = bml.deep_begin();  
                                      it != bml.deep_end(); ++it ) {

      (*it)->propagate(p);
     
      lng += (*it)->OrbitLength( p );

      cmd_elements.clear();
      cmd_elements % ++iseq % (int64_t) &(*it) % (*it)->Name() % (*it)->Type() % (*it)->Length() % (*it)->Strength();
      cmd_elements();

      cmd_corbit.clear();
      cmd_corbit     %  iseq %  lng % p.x() %  p.y()  % p.beta() % p.gamma();  
      cmd_corbit();
  };

  sqlite::execute(db, "COMMIT TRANSACTION", true);
  
  //-------------------------------------------------------------------------------------------------
  // verify that the orbit is indeed a closed one.
  //------------------------------------------------------------------------------------------------- 

  return;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Vector tunes( sqlite::connection& db, JetParticle const& oneturnjp)
{

  MatrixD mtrx = oneturnjp.state().jacobian();

  TVector<std::complex<double> > eigenvalues   = mtrx.Matrix::eigenValues();
  TMatrix<std::complex<double> > eigenvectors  = mtrx.Matrix::eigenVectors();
  orderColumns( eigenvalues, eigenvectors ); 

  TVector<std::complex<double> > hlambda(2);
  hlambda[0] = eigenvalues[0];
  hlambda[1] = eigenvalues[3];

  TVector<std::complex<double> > vlambda(2);
  vlambda[0] = eigenvalues[1];
  vlambda[1] = eigenvalues[4];

  TVector<std::complex<double> > slambda(2);
  slambda[0] = eigenvalues[2];
  slambda[1] = eigenvalues[5];
 
 
  if( (std::abs( abs(hlambda[0]) - 1.0 ) > 1.0e-4 ) ||  
      (std::abs( abs(vlambda[0]) - 1.0 ) > 1.0e-4 ) )  
  {
      (*pcout) << "\n"
           << "*** ERROR ***                                     \n"
           << "*** ERROR *** Optics::tunes(...)                  \n"
           << "*** ERROR *** The lattice is linearly unstable.   \n"
           << "*** ERROR *** horizontal lambda has magnitude = " << hlambda[0] << "\n"
           << "*** ERROR *** vertical   lambda has magnitude = " << vlambda[0] << "\n"
           << endl;
  }


   if( (std::abs( hlambda[0] - conj( hlambda[1] ) ) > 1.0e-4 ) ||
       (std::abs( vlambda[0] - conj( vlambda[1] ) ) > 1.0e-4 ) )
  {
  	(*pcout) << "\n"
  	     << "*** ERROR *** Optics::tunes(...) \n"
  	     << "*** ERROR *** Conjugacy condition has been violated\n"
  	     << "*** ERROR *** The lattice may be linearly unstable.\n"
  	     << "*** ERROR *** Eigenvalues =                        \n"
  	     << "*** ERROR *** " << hlambda << endl;
  }

  // the eigenvalues are ordered *negative imaginary value first* 

   Vector tunes(3); 

   tunes[0] =  arg( hlambda[0] )/Math_TWOPI;  
   tunes[1] =  arg( vlambda[0] )/Math_TWOPI;  
   tunes[2] =  arg( slambda[0] )/Math_TWOPI;  
   

   for (int i=0; i<3; ++i) {
     if (  tunes[i] < 0.0  ) { tunes[i]  += 1.0; }  
   }

   std::stringstream sql;
   sql.str("");
   sql << "UPDATE PROPERTIES SET " 
       << " fmuH=?, fmuV=?, fmuL=? " << std::ends; // fractional tunes

   sqlite::command cmd_props(db, sql.str());

   cmd_props %  tunes[0] % tunes[1] % tunes[2]; 
   cmd_props();


   return tunes;  
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Vector periodicDispersion( sqlite::connection& db, JetParticle const& jp_arg )
{


   Jet__environment::pushEnv( jp_arg.state().Env()  );
  JetC__environment::pushEnv( jp_arg.state().Env()  );

  JetParticle jp(jp_arg);

  MatrixD mtrx  = jp.state().jacobian();
  
  // zero out row/col corresponding to cdt coordinate. 

  for ( int i=0; i<6; ++i) { mtrx[i_cdt][i] =  mtrx[i][i_cdt] = (i == i_cdt) ? 1.0 : 0.0; }

  VectorC eigenvalues =  mtrx.Matrix::eigenValues();
  MatrixC eigenvecs   =  mtrx.Matrix::eigenVectors();
  orderColumns( eigenvalues, eigenvecs ); 

  Vector eta(6);

  eta[0] =  real( eigenvecs[i_x  ][i_ndp] );
  eta[1] =  real( eigenvecs[i_y  ][i_ndp] );
  eta[2] =  0.0;
  eta[3] =  real( eigenvecs[i_npx][i_ndp] );
  eta[4] =  real( eigenvecs[i_npy][i_ndp] );
  eta[5] =  0.0;


    Jet__environment::popEnv();
   JetC__environment::popEnv();

  return eta;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int propagateDispersion( sqlite::connection& db, beamline const& bml, JetParticle const& arg_jp,  Vector const& eta0,  bool dppconstant)
{

  //--------------------------------------------------------------------
  // This method computes the dispersion using automatic differentiation.  
  // 
  // NOTE:
  // 
  // By default, dp/p is held constant. This accomplished by by dynamically 
  // rescaling the momentum variable. This should be pretty accurate, 
  // unless the relative energy 
  // change in a single accelerating element is large (say > a few %) 
  // and/or the accelerating elements are dispersive. 
  //   
  // Dispersion is not uniquely defined in the presence of acceleration. 
  // In a linac, this can be circumvented by defining the dispersion 
  // at constant dp/p, which is equivalent to proportionally decrease 
  // the gradient by 1/(1+dp/p) in all cavities when computing the 
  // off-momentum trajectory. 
  //
  // To get the dispersion corresponding to a specific acceleration 
  // profile, set the argument dppconstant = false. 
  //  
  //--------------------------------------------------------------------
 
  int ret = 0;

  JetParticle jp( (Particle(arg_jp)) ); // resets the state and keeps initial conditions

  Mapping& state = jp.state();

  IntArray exp_d(6);
  exp_d[5]  = 1;
 
   state[i_x ].setWeightedDerivative( exp_d, eta0[0] );
  state[i_npx].setWeightedDerivative( exp_d, eta0[3] );

   state[i_y ].setWeightedDerivative( exp_d, eta0[1] );
  state[i_npy].setWeightedDerivative( exp_d, eta0[4] );
 

  const double start_momentum = jp.refMomentum();

  std::ostringstream sql;

  sql << "DROP TABLE IF EXISTS DISPERSION" << std::ends;
  sqlite::execute(db, sql.str(),true );

  sql.str("");
  sql << "CREATE TABLE DISPERSION "
      << "( iseq INTEGER PRIMARY KEY, "
      << "etax REAL,  etapx REAL,  etay REAL,  etapy REAL )"
      << std::ends;

  sqlite::execute(db, sql.str(),true );

  sql.str("");
  sql << "INSERT INTO DISPERSION "  
      << " ( iseq, etax, etapx, etay,  etapy )  VALUES (?, ?, ?, ?, ? )" 
      << std::ends;

  sqlite::command cmd_dispersion(db, sql.str());

  sqlite::execute(db, "BEGIN TRANSACTION", true);

  int iseq = -1;

  for ( beamline::const_deep_iterator it  = bml.deep_begin();  
                                      it != bml.deep_end(); ++it ) {

      Jet   ndp   = state[i_ndp];   
      double p0   = jp.refMomentum();

      (*it)->propagate(jp);
     

      cmd_dispersion.clear();


      if (dppconstant) { 
          
         cmd_dispersion % ++iseq  % ( state[i_x  ].weightedDerivative( exp_d ) ) 
	                          % ( state[i_npx].weightedDerivative( exp_d ) )
		                  % ( state[i_y  ].weightedDerivative( exp_d ) )
                                  % ( state[i_npy].weightedDerivative( exp_d ) );

          // keep dp/p constant ...

         static double const eps =  10.0 * std::numeric_limits<double>::epsilon();
	 if ( std::fabs(p0 - jp.refMomentum()) > eps ) { state[i_ndp] = ndp; } 
      } 

      else { 

 	 double scale = ( 1.0 / state[i_ndp].weightedDerivative( exp_d ));

         cmd_dispersion % ++iseq  % ( scale*state[i_x  ].weightedDerivative( exp_d ) ) 
	                          % ( scale*state[i_npx].weightedDerivative( exp_d ) )
		                  % ( scale*state[i_y  ].weightedDerivative( exp_d ) )
                                  % ( scale*state[i_npy].weightedDerivative( exp_d ) );
      }
      
      cmd_dispersion();
 
  }

  sqlite::execute(db, "COMMIT TRANSACTION", true);
  
  return ret;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Vector chromaticity( sqlite::connection& db, beamline const& bml, JetParticle const& oneturnjp, Vector const& eta )
{
   
  double const dpp = 0.001;

  Matrix oneturnmtrx = oneturnjp.state().jacobian();

  Particle p_off(oneturnjp);

  Vector& state = p_off.state(); 
  
  state[i_x  ] += eta[0]* dpp; 
  state[i_npx] += eta[1]* dpp; 
  state[i_y  ] += eta[2]* dpp; 
  state[i_npy] += eta[3]* dpp; 

  state[i_cdt]  = 0.0;
  state[i_ndp] += dpp; 

  JetParticle jp_off( p_off); 

  bml.propagate(jp_off);

  MatrixD oneturnmtrx_off  = jp_off.state().jacobian();

  Vector nu     = Optics::tunes( db, oneturnjp );
  Vector nu_off = Optics::tunes( db, jp_off    );

  Vector chroma(2); 
  chroma[0]  = ( nu_off[0] - nu[0] ) / dpp;
  chroma[1]  = ( nu_off[1] - nu[1] ) / dpp;


  return chroma;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int propagateCourantSnyder2D( sqlite::connection& db, beamline const& bml, JetParticle const& jparg, CSLattFuncs const& initialConditions )
{
  
  int ret = 0;
  int const N = jparg.state().Dim();


  //-------------------------------------
  // Create a new default Jet environment
  //-------------------------------------- 
 
  Vector  rp = jparg.state().Env()->refPoint();
   
  EnvPtr<double>                 env  = TJetEnvironment<double>::makeJetEnvironment(1, 6, 6,  rp);
  EnvPtr<std::complex<double> > envc  = env; // implicit conversion 
   
   Jet__environment::pushEnv(env );
  JetC__environment::pushEnv(envc); 
 
  Particle    p0(jparg);

  JetParticle jp(p0);   // resets state 

  CSLattFuncs const& lf = initialConditions;

  const double betax0  = lf.beta.hor;
  const double alphax0 = lf.alpha.hor;
  const double gamma_x0 = ( 1.0 + alphax0*alphax0 )/betax0;
     
  const double betay0  = lf.beta.ver;
  const double alphay0 = lf.alpha.ver;
  const double gamma_y0 = ( 1.0 + alphay0*alphay0 )/betay0;

  Vector eta0 = lf.dispersion.eta;

  eta0[ i_cdt]  = 0.0;
  eta0[ i_ndp]  = 1.0;


  double const momentum = jp.refMomentum();

  std::ostringstream sql;

  sql.str("");
  sql << "DROP TABLE IF EXISTS DISPERSION" << std::ends;
  sqlite::execute(db, sql.str(),true );

  sql.str("");
  sql << "CREATE TABLE DISPERSION "
      << "( iseq INTEGER PRIMARY KEY, "
      << "etax REAL,  etapx REAL,  etay REAL,  etapy REAL )"
      << std::ends;

  sqlite::execute(db, sql.str(),true );

  sql.str("");
  sql << "DROP TABLE IF EXISTS COURANT_SNYDER" << std::ends;
  sqlite::execute(db, sql.str(),true );

  sql.str("");
  sql << "CREATE TABLE " 
      << "COURANT_SNYDER"
      << "( iseq INTEGER PRIMARY KEY, betax REAL, alphax REAL, psix REAL, betay REAL, alphay REAL, psiy REAL  )" 
      << ends;
  sqlite::execute(db, sql.str(),true );

  sql.str("");
  sql << "INSERT INTO DISPERSION "  
      << " ( iseq, etax, etapx, etay,  etapy )  VALUES (?, ?, ?, ?, ? )" 
      << std::ends;
  sqlite::command cmd_insert_disp(db, sql.str());

  sql.str("");
  sql << "INSERT INTO COURANT_SNYDER "  
      << " ( iseq, betax, alphax, psix,  betay, alphay, psiy)  VALUES ( ?, ?, ?, ?, ?, ?, ? )" << ends;
  
  sqlite::command cmd_insert_cs(db, sql.str());


  sqlite::execute(db, "BEGIN TRANSACTION", true);

  int iseq = -1;

  double mux    = 0.0;
  double muy    = 0.0;
  double psix0  = 0.0;
  double psiy0  = 0.0;

  for (beamline::const_deep_iterator it =  bml.deep_begin(); 
                                     it != bml.deep_end();  ++it) {

    (*it) -> propagate( jp);

    MatrixD  mtrx = jp.state().jacobian();
 
    Vector eta    = mtrx*eta0;
 
    double a = mtrx[i_x  ] [i_x   ];
    double b = mtrx[i_x  ] [i_npx ];
    double c = mtrx[i_npx] [i_x   ];
    double d = mtrx[i_npx] [i_npx ];

    // Allow for possible acceleration by scaling with momentum

    double const scale =  jp.refMomentum()/momentum; 

    double betax =  scale * ( a*a*betax0 - 2.0*a*b*alphax0 + b*b*gamma_x0 );

    double alphax = scale * ( - a*c*betax0 + (a*d+b*c)*alphax0 - d*b*gamma_x0 );

    double sinpsi =   b / sqrt(betax*betax0);
    double cospsi =   a * sqrt(betax0/betax) - alphax0*sinpsi;

    double psix  =  atan2( sinpsi,cospsi) / Math_TWOPI; if ( psix < 0.0) {psix += 1.0;}
    

    a = mtrx[i_y  ][i_y   ];
    b = mtrx[i_y  ][i_npy ];
    c = mtrx[i_npy][i_y   ];
    d = mtrx[i_npy][i_npy ];

    double betay =  scale * ( a*a*betay0 - 2.0*a*b*alphay0 + b*b*gamma_y0 );

    double alphay = scale * ( - a*c*betay0 + (a*d+b*c)*alphay0 - d*b*gamma_y0 );

    //-----------------------------------------------------------------------------------------
    // NOTE: In absence of a reliable way to figure out a-priori the direction in which 
    //       a given mode is "rotating", we accumulate the absolute value of the phase variation.
    //       We assume that no single element will introduce a phase advance larger than 
    //       than Math_PI (0.5). The test allows one to detect the phase going across the 
    //       boundary between 2*PI and 0. ( 1.0 and 0.0 ) 
    //----------------------------------------------------------------------------------------

    sinpsi =   b /sqrt(betay*betay0);
    cospsi =   a * sqrt(betay0/betay) - alphay0*sinpsi;
    
    double psiy  =  atan2( sinpsi,cospsi) / Math_TWOPI; { if ( psiy < 0.0) psiy += 1.0;}
    
    double dmux = abs(psix - psix0);
    double dmuy = abs(psiy - psiy0);

    mux +=  (dmux>0.5) ? ( 1.0-dmux ) : dmux;
    muy +=  (dmuy>0.5) ? ( 1.0-dmuy ) : dmuy;

    psix0 =   psix;
    psiy0 =   psiy;

    cmd_insert_cs.clear(); 
    cmd_insert_cs % ++iseq % betax % alphax % mux % betay % alphay % muy; 
    cmd_insert_cs(); 

    cmd_insert_disp.clear(); 
    cmd_insert_disp % iseq % eta[i_x] % eta[i_npx] % eta[i_y] % eta[i_npy]; 
    cmd_insert_disp();


  } // end loop over the beamline elements ..............

  sqlite::execute(db, "COMMIT", true);


  // restore env before exiting ... 
  
    Jet__environment::popEnv();
   JetC__environment::popEnv();


  return ret;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CSLattFuncs periodicCourantSnyder2D(  sqlite::connection& db, JetParticle const& jp )
{
  //---------------------------------------------------------------
  // PRECONDITIONS: 
  //
  //  jp  is on the closed orbit and its state is the one-turn map.
  //  bml is an uncoupled ring.
  //---------------------------------------------------------------


   Jet__environment::pushEnv(  jp.state().Env() );
  JetC__environment::pushEnv(  jp.state().Env() ); // implicit conversion 

  JetParticle  jparticle(jp);
  Particle      particle(jp);

  MatrixD mtrx = jp.state().jacobian();

  Vector nu = tunes( db, jp); 

  double csH = cos( nu[0]* Math_TWOPI );
  double snH = ( mtrx[i_x][ i_npx] > 0.0 ) ? sqrt( 1.0 - csH*csH ) : -sqrt( 1.0 - csH*csH );

  double csV = cos( nu[1]* Math_TWOPI );
  double snV = ( mtrx[i_y][ i_npy] > 0.0 ) ? sqrt( 1.0 - csV*csV ) : -sqrt( 1.0 - csV*csV );

  double betax  =   mtrx[ i_x][ i_npx ] / snH;
  double alphax = ( mtrx[ i_x][ i_x ] - mtrx[ i_npx][ i_npx ] ) / ( 2.0*snH );

  double  betay  =   mtrx[ i_y][ i_npy ] / snV;
  double alphay  = ( mtrx[ i_y][ i_y ] - mtrx[ i_npy][ i_npy ] ) / ( 2.0*snV );

  
  Vector eta = periodicDispersion(db, jp);

  CSLattFuncs lf;

  lf.beta.hor        =  betax;
  lf.alpha.hor       =  alphax;
  lf.psi.hor         =  0.0;
  lf.dispersion.x    =  eta[i_x  ];
  lf.dispersion.xp   =  eta[i_npx];

  lf.beta.ver        =  betay;
  lf.alpha.ver       =  alphay;
  lf.psi.ver         =  0.0;
  lf.dispersion.y    =  eta[i_y  ];
  lf.dispersion.yp   =  eta[i_npy];


    Jet__environment::popEnv();
   JetC__environment::popEnv();

  return lf;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

MatrixC periodicEigenVectors( sqlite::connection& db, JetParticle const& oneturnjp )
{

  MatrixD mtrx = oneturnjp.state().jacobian();
  
  //-------------------------------------------------
  // For a 4D solution with no dispersive effects 
  //------------------------------------------------
  for (int i=0; i<6; ++i ) { 
    for (int j=0; j<6; ++j ) { 
      if ( (i==i_cdt) || (j==i_cdt) ) { mtrx[i][j] = (i==j) ? 1.0 : 0.0;}
      if ( (i==i_ndp) || (j==i_ndp) ) { mtrx[i][j] = (i==j) ? 1.0 : 0.0;} 
    }
  }

  TVector<std::complex<double> > eigenvalues   = mtrx.Matrix::eigenValues();
  TMatrix<std::complex<double> > eigenvectors  = mtrx.Matrix::eigenVectors();
  orderColumns( eigenvalues, eigenvectors ); 
  eigenvectors = toNormalForm(eigenvectors);

  return eigenvectors;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void propagateEigenVectors( sqlite::connection& db, beamline const& bml, JetParticle const& jp, TMatrix<std::complex<double> >const& EV0, TVector<double> const& eta0)
{
  std::cerr << "propagateEigenVectors" << std::endl;
  std::cerr << EV0 << std::endl;
 
  std::ostringstream sql;

  sql.str("");
  sql << "DROP TABLE IF EXISTS EIGENVECTORS" << std::ends; 
  sqlite::execute(db, sql.str(),true );

  sql.str("");
  sql << "CREATE TABLE " 
      << "EIGENVECTORS"
      << "( iseq INTEGER, component INTEGER,  e0_real REAL,   e0_imag REAL,  e1_real REAL,  e1_imag REAL,  e2_real REAL,   e2_imag REAL, "
      <<                                    " e3_real REAL,   e3_imag REAL,  e4_real REAL,  e4_imag REAL,  e5_real REAL,   e5_imag REAL  )"
      << ends;
  sqlite::execute(db, sql.str(),true );

  sql.str("");
  sql << " CREATE  UNIQUE INDEX idx ON EIGENVECTORS ( iseq, component )"
      << std::ends;
  sqlite::execute(db, sql.str(),true );


  sql.str("");
  sql << "INSERT INTO EIGENVECTORS "  
     << "( iseq,  component,  e0_real,   e0_imag,   e1_real,   e1_imag,  e2_real,  e2_imag, "
     <<                    "  e3_real,   e3_imag,   e4_real,   e4_imag,  e5_real,  e5_imag  )"
      << " VALUES (?,?, ?,?, ?,?, ?,?, ?,?, ?,?, ?,? )"
      << ends;
  sqlite::command cmd_insert_vectors(db, sql.str());
  
 
  sql.str("");
  sql << "DROP TABLE IF EXISTS DISPERSION" << std::ends;
  sqlite::execute(db, sql.str(),true );

  sql.str("");
  sql << "CREATE TABLE DISPERSION "
      << "( iseq INTEGER PRIMARY KEY, "
      << "etax REAL,  etapx REAL,  etay REAL,  etapy REAL )"
      << std::ends;

  sqlite::execute(db, sql.str(),true );

  sql.str("");
  sql << "INSERT INTO DISPERSION "  
      << " ( iseq, etax, etapx, etay,  etapy )  VALUES (?, ?, ?, ?, ? )" 
      << std::ends;
  sqlite::command cmd_insert_disp(db, sql.str());

  sqlite::execute(db, "BEGIN TRANSACTION", true);
  
  Particle    p(jp);
  JetParticle jpart(p); // mapping reset to identity.

  int iseq = -1;

  double mu1 = 0.0;  // normalize integrated phase advances
  double mu2 = 0.0; 
  
  double const momentum = jpart.refMomentum();
 
  for (beamline::const_deep_iterator it  =  bml.deep_begin(); 
                                     it !=  bml.deep_end();  ++it) {
    ElmPtr be = (*it);

    be->propagate( jpart );
  
    Matrix mtrx = jpart.state().jacobian();

    MatrixC EN  = mtrx*EV0;          // propagate eigenvectors
    Vector eta  = mtrx*eta0;         // propagate dispersion               
   
    // allow for possible acceleration by scaling

    double const scale = jpart.refMomentum()/momentum;


    // --------------------------------------------------------
    // commit the dispersion to the database
    //---------------------------------------------------------

    cmd_insert_disp.clear(); 
    cmd_insert_disp % ++iseq % eta[i_x] % eta[i_npx] % eta[i_y] % eta[i_npy]; 
    cmd_insert_disp();


    // --------------------------------------------------------
    // commit the eigenvectors to the database
    //---------------------------------------------------------


    for (int i=0 ; i<6;  ++i ) {

        cmd_insert_vectors.clear();
        cmd_insert_vectors % iseq % i;

        for (int j=0 ; j<6; ++j ) { 
	  std::complex<double> value =  EN[i][j]*sqrt(scale);
          cmd_insert_vectors % ( value.real()) % ( value.imag() );
        }
 
      cmd_insert_vectors();
    }

  } // for

  sqlite::execute(db, "COMMIT TRANSACTION", true);

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#if 0
MatrixD periodicCovariance( sqlite::connection& db, JetParticle& jp, double eps1=1.0, double eps2=1.0, double eps3=1.0 )
{

  double const mm_mr     = 1.0e-6;
  double const betaGamma = jp.ReferenceBeta() * jp.ReferenceGamma();

  // Convert to action, in meters (crazy units)

  double const I1 = ( std::abs( eps1 )/betaGamma ) * mm_mr / 2.0;
  double const I2 = ( std::abs( eps2 )/betaGamma ) * mm_mr / 2.0;

  double const I3 = ( std::abs( eps3 )/betaGamma ) * mm_mr / 2.0; // WRONG UNITS ... FIXME !


  TVector<std::complex<double> > eigenvalues   = jp.state().jacobian().Matrix::eigenValues();
  MatrixC                        EV            = jp.state().jacobian().Matrix::eigenVectors();

  orderColumns( eigenvalues,  EV ); 
  EV = toNormalForm( EV, Matrix::j_ordering);

  int n = jp.state().Dim();

  MatrixD aa(n,n);

  aa[i_x  ][ i_x  ]  = I1;
  aa[i_npx][ i_npx]  = I1;
  aa[i_y  ][ i_y  ]  = I2;
  aa[i_npy][ i_npy]  = I2;

  if ( n == 6 ) {
    aa[i_cdt  ][ i_cdt  ]  = I3;
    aa[i_ndp  ][ i_ndp  ]  = I3;
  }

  MatrixD cov = real( EV*aa*EV.dagger());

  return cov;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void propagateCovariance( sqlite::connection& db, beamline const& bml, JetParticle const& jparg, MatrixD const& cov )
{

  // A little paranoia check.
  int r = cov.rows();
  if( cov.cols() != r ) {
    (*pcerr) 
         << "\n***ERROR*** File: " << __FILE__ << "  Line: " << __LINE__
         << "\n***ERROR*** int CovarianceSage::doCalc( JetParticle*, MatrixD, beamline::Criterion& )"
         << "\n***ERROR*** Argument covariance matrix is "
         << r
         << "x"
         << (cov.cols())
         << ", not square."
         << std::endl;
  }

  std::ostringstream sql;

  sql << "DROP TABLE IF EXISTS COVARIANCE" <<std::ends; 
  sqlite::execute(db, sql.str(),true );

  sql.str("");
  sql << "CREATE TABLE COVARIANCE"
      << "( iseq INTEGER PRIMARY KEY, " 
      << "  cov_xx  REAL,  cov_xpx REAL, cov_xpxp REAL, cov_yx  REAL, cov_yxp  REAL, "  
      << "  cov_yy  REAL,  cov_ypx REAL, cov_ypxp REAL, cov_ypy REAL, cov_ypyp REAL )" 
      << ends;

  sqlite::execute(db, sql.str(),true );

  sql.str("");
  sql << "INSERT INTO COVARIANCE "  
      << " ( iseq, "
      << "  cov_xx,  cov_xpx, cov_xpxp, cov_yx,  cov_yxp, "  
      << "  cov_yy,  cov_ypx, cov_ypxp, cov_ypy, cov_ypyp) " 
      << " VALUES ( ?,  ?,?,?,?,?, ?,?,?,?,? ) " 
      << ends;
  
  sqlite::command cmd_insert(db, sql.str());

  sqlite::execute(db, "BEGIN TRANSACTION", true);

  //-----------------------------------------
  // Go through the line, element by element
  //-------------------------------------------

  JetParticle jp( Particle(jparg), jp.state().Env() );
  
  int iseq = -1;
  for (beamline::const_deep_iterator it  = bml.deep_begin(); 
                                     it != bml.deep_end(); ++it) 
  {
 
    (*it)->propagate( jp );
  
    Matrix M     =  jp.state().jacobian();
    Matrix lcov  =  M * cov * M.transpose();


    // ... "Horizontal" lattice functions

   double hnormalizer = ( lcov[i_x][i_x]*lcov[i_npx][i_npx] - lcov[i_x][i_npx]*lcov[i_npx][i_x] );

    if( hnormalizer <= 0.0 ) {
      (*pcerr) 
           << "\n***ERROR*** File: " << __FILE__ << "  Line: " << __LINE__
           << "\n***ERROR*** int CovarianceSage::doCalc( JetParticle*, MatrixD, beamline::Criterion& )"
           << "\n***ERROR*** Determinant of (projected) \"horizontal\" covariance matrix = "
           << hnormalizer
           << " <= 0.0."
           << std::endl;
    }

    hnormalizer =1.0/sqrt(hnormalizer);



    // ... "Vertical" lattice functions

   double vnormalizer = (lcov[i_y][i_y]*lcov[i_npy][i_npy] -  lcov[i_y][i_npy]*lcov[i_npy][i_y]);


    if( vnormalizer <= 0.0 ) {
      (*pcerr) 
           << "\n***ERROR*** File: " << __FILE__ << "  Line: " << __LINE__
           << "\n***ERROR*** int CovarianceSage::doCalc( JetParticle*, MatrixD, beamline::Criterion& )"
           << "\n***ERROR*** Determinant of (projected) \"vertical\" covariance matrix = "
           << vnormalizer
           << " <= 0.0."
           << std::endl;
    }

    vnormalizer =1.0/sqrt(vnormalizer);

//    CVLattFuncs lf;
//  lf.beta.hor  =   hnormalizer*lcov[i_x][i_x  ];
//  lf.alpha.hor = - hnormalizer*lcov[i_x][i_npx];

//  lf.beta.ver  =   vnormalizer*lcov[i_x][i_y  ];
//  lf.alpha.ver = - vnormalizer*lcov[i_y][i_npy];

    cmd_insert.clear(); 
    cmd_insert % ++iseq % lcov[i_x][i_x] %  lcov[i_npx][i_x] % lcov[i_npx][i_npx] % lcov[i_y  ][i_x]  %  lcov[i_y  ][i_npx]    
                        % lcov[i_y][i_y] %  lcov[i_npy][i_x] % lcov[i_npy][i_npx] % lcov[i_npy][i_y]  %  lcov[i_npy][i_npy];  
    cmd_insert(); 
   
  }

  sqlite::execute(db, "COMMIT TRANSACTION", true);

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ETLattFuncs periodicETLatticeFunctions( sqlite::connection& db, beamline const& bml, JetParticle const&jp )
{
 
 MatrixD M( 2, 2 ), N( 2, 2 ), m( 2, 2 ), n( 2, 2 ),
                    D( 2, 2 ), A( 2, 2 ), B( 2, 2 );

 static MatrixD const S = Matrix::Smatrix( 2 ), U = Matrix::Imatrix( 2 );

 Vector nu = tunes(db, jp);

 double csH = cos( Math_TWOPI*nu[0]);
 double csV = cos( Math_TWOPI*nu[1]);

 Matrix mtrx = jp.state().jacobian();

 M[0][0] = mtrx[i_x  ][i_x   ];     n[0][0] = mtrx[i_x  ][i_y  ];
 M[0][1] = mtrx[i_x  ][i_npx ];     n[0][1] = mtrx[i_x  ][i_npy];
 M[1][0] = mtrx[i_y  ][i_npy ];     n[1][0] = mtrx[i_npx][i_y  ];
 M[1][1] = mtrx[i_npy][i_npy ];     n[1][1] = mtrx[i_npx][i_npy];

 m[0][0] = mtrx[ i_y  ][i_x   ];    N[0][0] = mtrx[i_y  ][i_y  ];
 m[0][1] = mtrx[ i_y  ][i_npx ];    N[0][1] = mtrx[i_y  ][i_npy];
 m[1][0] = mtrx[ i_npy][i_x   ];    N[1][0] = mtrx[i_npy][i_y  ];
 m[1][1] = mtrx[ i_npy][i_npx ];    N[1][1] = mtrx[i_npy][i_npy];

 double  dcos    = 0.0;
 double  cos2phi = 0.0;
 double  sin2phi = 0.0;
 double  tanphi  = 0.0;

  if(  std::abs( csH - csV ) < 1.0e-4 ) {
    (*pcerr) << "\n"
         << "*** ERROR *** EdwardsTengSage()                    \n"
         << "*** ERROR *** \"Horizontal\" and \"vertical\" tunes\n"
         << "*** ERROR *** are too near each other.             \n"
         << "*** ERROR *** " << endl;
  }
 
  dcos    = (csH - csV);
  cos2phi = ( M - N ).trace() / ( 2.0 *( dcos ) );

  if( std::abs( cos2phi - 1.0 ) < 1.0e-4 ) cos2phi =   1.0;  // ??? Rather coarse,
  if( std::abs( cos2phi + 1.0 ) < 1.0e-4 ) cos2phi = - 1.0;  // ??? isn't it?

  if( abs( cos2phi ) > 1.0 ) {
   (*pcerr) << "\n"
        << "*** ERROR: EdwardsTengSage()                        \n"
	<< "*** ERROR: cos( 2 phi ) = " 
        <<  setprecision(10) << cos2phi 
	<< " has magnitude larger    \n"
        << "*** ERROR: than one. " << std::endl;

  }

  if( cos2phi < 0.0 ) {
   sin2phi =   csH;         // Variable used as dummy register.
    csH    =   csV;
    csV    =   sin2phi;
   dcos    = - dcos;
   cos2phi = - cos2phi;
  }

  sin2phi = sqrt( 1.0 - cos2phi*cos2phi );
  tanphi  = sin2phi / ( 1.0 + cos2phi );

  if( abs( sin2phi ) > 1.0e-8 ) {
    D = -( m + S*n.transpose()*S.transpose() ) * ( 1.0 / ( dcos*sin2phi ));
  } else {
    D[0][0] = 1.0;  D[0][1] = 0.0;
    D[1][0] = 0.0;  D[1][1] = 1.0;
  }

  if( abs( D.determinant() - 1.0 ) > 1.0e-4 ) {
    (*pcerr) << "\n"
         << "*** ERROR *** EdwardsTengSage()                    \n"
         << "*** ERROR *** The matrix D is non-symplectic.      \n"
         << "*** ERROR *** |D| = " << D.determinant() << "      \n"
         << "*** ERROR ***                                        " << endl;
  }

  ETLattFuncs lf; // FIXME !!!! should return coupling matrix M and phase phi. 

  return lf;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void propagateETLatticeFunctions(sqlite::connection& db, beamline const& bml, JetParticle const& jparg, ETLattFuncs const& lf )
{

  
  std::stringstream sql;

  sql << "DROP TABLE IF EXISTS EDWARDS_TENG" << std::ends; 
  sqlite::execute(db, sql.str(),true );

  sql.str("");
  sql << "CREATE TABLE " 
      << "EDWARDS_TENG"
      << "( iseq INTEGER PRIMARY KEY, beta_h REAL, alpha_h REAL, psi_h REAL, beta_v REAL, alpha_v REAL, psi_v REAL, phi REAL  )" 
      << std::ends;

  sqlite::execute(db, sql.str(),true );

  sql.str("");
  sql << "INSERT INTO EDWARDS_TENG "  
      << " ( iseq, beta_h, alpha_h,  psi_h, beta_v, alpha_v, psi_v, phi)  VALUES ( ?, ?, ?, ?, ?, ?, ?, ?)" << std::ends;
  
  sqlite::command cmd_insert(db, sql.str());

  Particle    p(jparg);
  JetParticle jp(p, jparg.state().Env() ); // identity mapping on closed orbit

  sqlite::execute(db, "BEGIN TRANSACTION", true);


  sqlite::execute(db, "COMMIT", true);

}

#endif
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CSLattFuncs4D periodicCourantSnyder4D( sqlite::connection& db, JetParticle const& oneturnjp )
{

  MatrixC EV0 = periodicEigenVectors( db, oneturnjp );

  VectorC ev1(6), ev2(6);

  for (int i=0; i<6; ++i ) {
       ev1[i] = EV0[i][0];
       ev2[i] = EV0[i][1];
  }

  //--------------------
  // beta_1x and beta_2y
  //--------------------

  std::complex<double> temp = ev1[i_x];
          double beta_1x    =  2.0 * ( temp*conj(temp) ).real();
  std::complex<double>  mu1 =  conj(temp)/abs(temp); 
 
                       temp =   ev2[i_y];
            double beta_2y  =  2.0 * ( temp*conj(temp) ).real();
  std::complex<double>  mu2 =  conj(temp)/abs(temp); 

  //----------------------
  // alpha_1x and alpha_2y
  //----------------------
  
                   temp     =  ev1[i_npx]*ev1[i_x];
            double alpha_1x = -2.0 * real( temp*mu1*mu1 );

                       temp =  ev2[i_npy]*ev2[i_y];
            double alpha_2y = -2.0 * real( temp*mu2*mu2 );

  //----------------------
  // beta_1y and beta_2x
  //----------------------

                       temp = ev1[i_y];
             double beta_1y = 2.0 * real( temp*conj(temp) );

                       temp = ev2[i_x];
             double beta_2x = 2.0 * real( temp*conj(temp) );

  //----------------------
  // alpha_1y and alpha_2x
  //----------------------

                       temp = ev1[i_npy]*ev1[i_y];
            double alpha_1y = -2.0*real( temp* mu1*mu1 );

                       temp = ev2[i_npx]*ev2[i_x ];
            double alpha_2x = -2.0*real( temp*mu2*mu2 );
   

  CSLattFuncs4D lf;
  
  lf.mode1.beta.hor  = beta_1x; 
  lf.mode1.alpha.hor = alpha_1x;
  lf.mode1.beta.ver  = beta_1y;
  lf.mode1.alpha.ver = alpha_1y;
  lf.mode1.psi       = 0.0;      // should be 2 pi nu ?

  lf.mode2.beta.hor  = beta_2x;
  lf.mode2.alpha.hor = alpha_2x;
  lf.mode2.beta.ver  = beta_2y;
  lf.mode2.alpha.ver = alpha_2y;
  lf.mode2.psi       = 0.0;

  lf.dispersion.eta = periodicDispersion(db, oneturnjp );


  return lf;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int  propagateCourantSnyder4D( sqlite::connection& db, beamline const& bml, JetParticle const& jp, CSLattFuncs4D const& initial )
{


//-------------------------------------------------------------------------
// 
//  The Generalized Twiss parametrization used here is documented in  
// 
//  V.A. Lebedev and S.A. Bogacz
//  "Betatron Motion with Coupling of Horizontal and Vertical Degrees of Freedom"  
//  JLAB-ACC-99-19
//
//  Notes: 
//  -----
// 
// (1)
// 
//   With J is the symplectic matrix and E the eigenvector matrix
//
//   CHEF      normalization:   E^T J E = -i   J 
//    
//   L-B paper normalization:   E^T J E = -2 i J    
// 
// (2) The eigenvector solver orders eigenvectors w/r to their eigenvalues:
//     The eigenvalue with negative imaginary part is _first_.
//       
//------------------------------------------------------------------------------
 
  std::cerr << initial.mode1.beta.hor  << std::endl; 
  std::cerr << initial.mode1.alpha.hor << std::endl;
  std::cerr << initial.mode1.beta.ver  << std::endl; 
  std::cerr << initial.mode1.alpha.ver << std::endl;

  std::cerr << initial.mode2.beta.hor  << std::endl; 
  std::cerr << initial.mode2.alpha.hor << std::endl;
  std::cerr << initial.mode2.beta.ver  << std::endl; 
  std::cerr << initial.mode2.alpha.ver << std::endl;

  // MatrixC EV0 = courantSnyder4DtoEV(initial); FIXME !!!!!

  MatrixC EV0 = periodicEigenVectors( db, const_cast<JetParticle &>(jp) );

  std::cout <<  EV0 << std::endl;

  propagateEigenVectors( db, bml, jp, EV0, initial.dispersion.eta );


  std::ostringstream sql;

  sql << "DROP TABLE IF EXISTS COURANT_SNYDER_4D" << std::ends; 
  sqlite::execute(db, sql.str(),true );

  sql.str("");
  sql << "CREATE TABLE " 
      << "COURANT_SNYDER_4D"
      << " ( iseq INTEGER PRIMARY KEY, beta1x REAL,    alpha1x REAL,   beta1y     REAL, alpha1y    REAL,   psi1 REAL,"
      <<                              "beta2x REAL,    alpha2x REAL,   beta2y     REAL, alpha2y    REAL,   psi2 REAL )"
      << ends;

  sqlite::execute(db, sql.str(),true );


  sql.str("");
  sql << "INSERT INTO COURANT_SNYDER_4D"  
      << "( iseq, beta1x, alpha1x, beta1y, alpha1y, psi1, "
      <<         "beta2x, alpha2x, beta2y, alpha2y, psi2 )"
      << " VALUES (?, ?,?,?,?,?, ?,?,?,?,? )"  
      << ends;
  sqlite::command cmd_insert(db, sql.str());


  sql.str("");
  sql << "SELECT * FROM EIGENVECTORS " << std::ends;

  sqlite::query q(db,sql.str() ); 
  sqlite::result_type res = q.emit_result();


  TVector<std::complex<double> > ev1(6);
  TVector<std::complex<double> > ev2(6);

  int iseq = 0;
  double fmu10 = 0.0, fmu20 = 0.0;

  double fmu1_adv = 0.0;  // cumulative phase advances
  double fmu2_adv = 0.0; 
  
  sqlite::execute(db, "BEGIN TRANSACTION", true);

  bool done = false;
  bool first_record = true;

  if (res) { 

    do { 
      
      int i=0; 
 
      for ( int k=0; k<6; ++k ) {

        iseq     = res->get_int(0);
            i    = res->get_int(1);

        ev1[i] = std::complex<double>( res->get_double(2),  res->get_double(3) );
        ev2[i] = std::complex<double>( res->get_double(4),  res->get_double(5) );
        
        done = !res->next_row();

      }
 
      //--------------------
      // beta_1x and beta_2y
      //--------------------

      std::complex<double> temp = ev1[i_x];
              double beta_1x    =  2.0 * ( temp*conj(temp) ).real();
      std::complex<double>  mu1 =  conj(temp)/abs(temp); 
 
                           temp =   ev2[i_y];
                double beta_2y  =  2.0 * ( temp*conj(temp) ).real();
      std::complex<double>  mu2 =  conj(temp)/abs(temp); 

      //----------------------
      // alpha_1x and alpha_2y
      //----------------------
   
                       temp     =  ev1[i_npx]*ev1[i_x];
                double alpha_1x = -2.0 * real( temp*mu1*mu1 );

                           temp =  ev2[i_npy]*ev2[i_y];
                double alpha_2y = -2.0 * real( temp*mu2*mu2 );

      //----------------------
      // beta_1y and beta_2x
      //----------------------

                           temp = ev1[i_y];
                 double beta_1y = 2.0 * real( temp*conj(temp) );

                           temp = ev2[i_x];
                 double beta_2x = 2.0 * real( temp*conj(temp) );

      //----------------------
      // alpha_1y and alpha_2x
      //----------------------

                           temp = ev1[i_npy]*ev1[i_y];
                double alpha_1y = -2.0*real( temp* mu1*mu1 );

                           temp = ev2[i_npx]*ev2[i_x ];
                double alpha_2x = -2.0*real( temp*mu2*mu2 );
   

    //--------------------------------------------------------
    // Normalize fmu1, fmu2 to (0,1 ] and accumulate phase.
    // Note: the signs of mu1,mu2 have already been inverted
    //--------------------------------------------------------

    
    double fmu1 =  arg(mu1)/Math_TWOPI;  if ( fmu1 < 0.0 ) fmu1  += 1.0;
    double fmu2 =  arg(mu2)/Math_TWOPI;  if ( fmu2 < 0.0 ) fmu2  += 1.0;

    //--------------------------------
    // after the above normalization 
    // NOTE   0.0 <= fmu1 < 1.0
    // NOTE   0.0 <= fmu2 < 1.0
    //--------------------------------

    if (first_record) {
      fmu10 = fmu1;
      fmu20 = fmu2; 
      first_record = false;
    }

    //-----------------------------------------------------------------------------------------
    // NOTE: There is no way do figure out a-priori the direction in which a given eigenvector 
    //       is "rotating", so we accumulate the absolute value of the phase variation.
    //       We assume that no single element will introduce a phase advance larger than 
    //       than Math_PI (0.5). The test allows one to detect the phase going across the 
    //       boundary between 2*PI and 0. ( 1.0 and 0.0 ) 
    //----------------------------------------------------------------------------------------

    double dmu1 = abs(fmu1-fmu10);
    fmu1_adv += ( dmu1 > 0.5) ? 1.0-dmu1 : dmu1;

    double dmu2 = abs(fmu2-fmu20);
    fmu2_adv += ( dmu2 > 0.5) ? 1.0-dmu2 : dmu2;

    fmu10 = fmu1;
    fmu20 = fmu2;
 
    cmd_insert.clear();

    cmd_insert % iseq % beta_1x % alpha_1x % beta_1y % alpha_1y %  fmu1_adv 
                      % beta_2x % alpha_2x % beta_2y % alpha_2y %  fmu2_adv; 
 
    
    cmd_insert();

    } while ( !done );

  }

  sqlite::execute(db, "COMMIT", true);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

MatrixC courantSnyder4DtoEV( CSLattFuncs4D const& lf )
{

  //----------------------------------------------------------------
  //  From 4D Generalized Lattice Functions to eigenvectors.
  //---------------------------------------------------------------- 

  double    beta_1x = lf.mode1.beta.hor;
  double   alpha_1x = lf.mode1.alpha.hor;
  double    beta_1y = lf.mode1.beta.ver;
  double   alpha_1y = lf.mode1.alpha.ver;
  double     psi_1  = lf.mode1.psi;

  double    beta_2x = lf.mode2.beta.hor;
  double   alpha_2x = lf.mode2.alpha.hor;
  double    beta_2y = lf.mode2.beta.ver;
  double   alpha_2y = lf.mode2.alpha.ver;
  double     psi_2  = lf.mode2.psi;


  double kappa_x = sqrt(beta_2x/beta_1x);
  double kappa_y = sqrt(beta_1y/beta_2y);

  double A_x = (kappa_x != 0.0) ?  kappa_x*alpha_1x - alpha_2x/kappa_x : 0.0;
  double A_y = (kappa_y != 0.0) ?  kappa_y*alpha_2y - alpha_1y/kappa_y : 0.0;

  double kxx = (kappa_x != 0.0) ? (kappa_x- 1.0/ kappa_x ) : 0.0;
  double kyy = (kappa_y != 0.0) ? (kappa_y- 1.0/ kappa_y ) : 0.0;

  double A_p =  A_x*kyy- A_y*kxx; 
  double A_m = -A_x*kyy- A_y*kxx; 

  double B   = (kappa_x != 0.0) ? (kappa_x/kappa_y - kappa_y/kappa_x) : 0.0;

  double C_p =  A_x*kxx - A_y*kyy; 
  double C_m =  A_x*kxx + A_y*kyy; 


  double nu_p2 = (kappa_x != 0.0) ? asin( C_p/sqrt(A_p*A_p + B*B)) + atan2(A_p,B) : 0.0;
  double nu_p1 = (kappa_x != 0.0) ? Math_PI - nu_p2                                  : 0.0;

  double nu_m2 = (kappa_x != 0.0) ? - asin( C_m/sqrt(A_m*A_m + B*B)) - atan2(A_m,B) : 0.0;
  double nu_m1 = (kappa_x != 0.0) ? Math_PI + nu_m2                                    : 0.0;


  double kappa_pxy = (kappa_x != 0.0) ? 2*kappa_x*kappa_y - kappa_y/kappa_x -kappa_x/kappa_y : 0.0;

  double u_1   = (kappa_x != 0.0) ? ( (kappa_x*kappa_x) + (kappa_y*kappa_y) - 2.0 + kappa_pxy*cos(nu_p1)-( A_x*kyy+A_y*kxx )*sin(nu_p1) ) 
	                            / ( (kxx*kxx) + (kyy*kyy) + 2*kxx*kyy*cos(nu_p1) ) : 0.0;

  double u_2   =  (kappa_x != 0.0) ? ( (kappa_x*kappa_x) + (kappa_y*kappa_y) - 2.0 + kappa_pxy*cos(nu_p2)-( A_x*kyy+A_y*kxx )*sin(nu_p2) )
	                            / ( (kxx*kxx) + (kyy*kyy) + 2*kxx*kyy*cos(nu_p2) ) : 0.0;

  double nu_1 = 0.5 * ( nu_p1 - nu_m1); 
  double nu_2 = 0.5 * ( nu_p2 - nu_m2); 

  MatrixC EV(6,6);

  double u   = u_1;

  EV[0][0] = sqrt( beta_1x );  											   
  EV[1][0] = sqrt( beta_1y )*exp(std::complex<double>(0.0, nu_1) );  						   
  EV[2][0] = 0.0;												   
  EV[3][0] = - std::complex<double>( alpha_1x, (1 - u))/sqrt( beta_1x ); 					   
  EV[4][0] =  (kappa_y != 0.0) ? - std::complex<double>( alpha_1y,  u     )/sqrt( beta_1y ) * exp(std::complex<double>(0.0, nu_1) )
                               : 0.0;  
  EV[5][0] = 0.0;                                                                                                  

  EV[0][1] = sqrt( beta_2x )*exp(std::complex<double>(0.0, nu_2) );    											   
  EV[1][1] = sqrt( beta_2y );  						   
  EV[2][1] = 0.0;												   
  EV[3][1] = (kappa_x != 0.0) ? - std::complex<double>( alpha_2x, u      )/ sqrt(beta_2x) * exp(std::complex<double>(0.0, nu_2) )
                              : 0.0;
  EV[4][1] = - std::complex<double>( alpha_2y, (1-u)  )/ sqrt(beta_2y);  
  EV[5][1] = 0.0;                                                                                                  

  EV[0][2] = 0.0;
  EV[1][2] = 0.0;
  EV[2][2] = 1.0;
  EV[3][2] = 0.0;
  EV[4][2] = 0.0;
  EV[5][2] = 0.0;

  EV[0][3] =  conj(EV[0][0]);
  EV[1][3] =  conj(EV[1][0]);
  EV[2][3] =  conj(EV[2][0]);
  EV[3][3] =  conj(EV[3][0]);
  EV[4][3] =  conj(EV[4][0]);
  EV[5][3] =  conj(EV[5][0]);
	     	     
  EV[0][4] =  conj(EV[0][1]);
  EV[1][4] =  conj(EV[1][1]);
  EV[2][4] =  conj(EV[2][1]);
  EV[3][4] =  conj(EV[3][1]);
  EV[4][4] =  conj(EV[4][1]);
  EV[5][4] =  conj(EV[5][1]);

  EV[0][5] = 0.0;
  EV[1][5] = 0.0;
  EV[2][5] = 0.0;
  EV[3][5] = 0.0;
  EV[4][5] = 0.0;
  EV[5][5] = 1.0;

  return ( std::complex<double>(1.0/sqrt(2.0),0.0)*EV ); // Note normalization, which is different from 
                                                         // the one assumed in the Lebedev-Bogacz writup. 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double getVTune( sqlite::connection& db )
{
  sqlite::query q( db,"SELECT  imuV, fmuV FROM PROPERTIES;"); 
  sqlite::result_type res = q.emit_result(); 
  if (!res)  return -1.000;  // error ! // ** read tunes from the database 
  return res->get_double(1); // fractional part;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double getHTune( sqlite::connection& db)
{
  sqlite::query q( db,"SELECT  imuH, fmuH FROM PROPERTIES;" ); 
  sqlite::result_type res = q.emit_result(); 
  if (!res)  return -1.000;  // error ! // ** read tunes from the database 
   return res->get_double(1); // fractional part;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void cleardb( sqlite::connection& db) 
{
  std::ostringstream sql;

  sql.str("");
  sql << "DROP TABLE IF EXISTS PROPERTIES" <<std::ends; 
  sqlite::execute(db, sql.str(),true );

  sql.str("");
  sql << "DROP TABLE IF EXISTS ELEMENTS" <<std::ends; 
  sqlite::execute(db, sql.str(),true );

  sql.str("");
  sql << "DROP TABLE IF EXISTS ELEMENTS" <<std::ends; 
  sqlite::execute(db, sql.str(),true );

  sql.str("");
  sql << "DROP TABLE IF EXISTS REFERENCE_ORBIT" <<std::ends; 
  sqlite::execute(db, sql.str(),true );

  sql.str("");
  sql << "DROP TABLE IF EXISTS DISPERSION" <<std::ends; 
  sqlite::execute(db, sql.str(),true );

  sql.str("");
  sql << "DROP TABLE IF EXISTS COURANT_SNYDER" <<std::ends; 
  sqlite::execute(db, sql.str(),true );

  sql.str("");
  sql << "DROP TABLE IF EXISTS COURANT_SNYDER_4D" <<std::ends; 
  sqlite::execute(db, sql.str(),true );

  sql.str("");
  sql << "DROP TABLE IF EXISTS EIGENVECTORS" <<std::ends; 
  sqlite::execute(db, sql.str(),true );

  sql.str("");
  sql << "DROP TABLE IF EXISTS EDWARDS_TENG" <<std::ends; 
  sqlite::execute(db, sql.str(),true );

  sql.str("");
  sql << "DROP TABLE IF EXISTS COVARIANCE" <<std::ends; 
  sqlite::execute(db, sql.str(),true );

  initdb( db);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool  reference_orbit_ok( sqlite::connection& db ) 
{
  bool exists = table_exists( db,  "REFERENCE_ORBIT");
  bool empty  = exists ? table_empty( db, "REFERENCE_ORBIT") : true ;
  return (!empty);
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool     dispersion_ok( sqlite::connection& db ) 
{  
  bool exists = table_exists(db, "DISPERSION");
  bool empty  = exists ? table_empty(db, "DISPERSION") : true ;
  return (!empty);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool     eigenvectors_ok( sqlite::connection& db )
{  
  bool exists = table_exists(db, "EIGENVECTORS");
  bool empty  = exists ? table_empty(db, "EIGENVECTORS") : true ;
  return (!empty);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool    courant_snyder2d_ok( sqlite::connection& db )
{
  bool exists = table_exists(db, "COURANT_SNYDER");
  bool empty  = exists ? table_empty(db, "COURANT_SNYDER") : true ;
  return (!empty);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool   edwards_teng_ok( sqlite::connection& db ) 
{
  bool exists = table_exists(db, "EDWARDS_TENG");
  bool empty  = exists ? table_empty(db, "EDWARDS_TENG") : true ;
  return (!empty);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool covariance_ok( sqlite::connection& db ) 
{
  bool exists = table_exists(db, "COVARIANCE");
  bool empty  = exists ? table_empty(db, "COVARIANCE") : true ;
  return (!empty);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool  courant_snyder4d_ok( sqlite::connection& db ) 
{
  bool exists = table_exists(db, "COURANT_SNYDER_4D");
  bool empty  = exists ? table_empty(db, "COURANT_SNYDER_4D") : true ;
  return (!empty);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> lattice_function (std::string const& dbname, std::string const& sql )
{

  sqlite::connection db(dbname);

  sqlite::query q( db, sql ); 

  sqlite::result_type res = q.emit_result();

  std::vector<double> values;
  
  if  (!res) return values; // no result, return an empty vector
 
  do {
 	  values.push_back( res->get_double(0) );
  } while ( res->next_row() ); 

  return values;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> arclength (std::string const& dbname )
{

  static const std::string sql( "SELECT arclength FROM ELEMENTS, REFERENCE_ORBIT "
                                "WHERE ( REFERENCE_ORBIT.iseq =  ELEMENTS.iseq )" );

  return Optics::lattice_function ( dbname, sql);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> gamma( std::string const& dbname)
{
  static const std::string sql( "SELECT gamma FROM ELEMENTS, REFERENCE_ORBIT "
                                "WHERE ( REFERENCE_ORBIT.iseq =  ELEMENTS.iseq )" );

  return Optics::lattice_function( dbname, sql);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> beta_x ( std::string const& dbname)
{
  static const std::string sql( "SELECT betax FROM ELEMENTS, COURANT_SNYDER "
                                "WHERE ( COURANT_SNYDER.iseq =  ELEMENTS.iseq )" );

 return Optics::lattice_function ( dbname, sql);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> beta_y ( std::string const& dbname)
{
  static const std::string sql( "SELECT betay FROM ELEMENTS, COURANT_SNYDER "
                                "WHERE ( COURANT_SNYDER.iseq =  ELEMENTS.iseq )" );

  return Optics::lattice_function ( dbname, sql);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> alpha_x ( std::string const& dbname)
{
  static const std::string sql( "SELECT alphax FROM ELEMENTS, COURANT_SNYDER "
                                "WHERE ( COURANT_SNYDER.iseq =  ELEMENTS.iseq )" );

  return Optics::lattice_function ( dbname, sql);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> alpha_y ( std::string const& dbname)
{
  static const std::string sql( "SELECT alphay FROM ELEMENTS, COURANT_SNYDER "
                                "WHERE ( COURANT_SNYDER.iseq =  ELEMENTS.iseq )" );

  return Optics::lattice_function ( dbname, sql);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> psi_x ( std::string const& dbname)
{
  static const std::string sql( "SELECT psix FROM ELEMENTS, COURANT_SNYDER "
                                "WHERE ( COURANT_SNYDER.iseq =  ELEMENTS.iseq )" );

  return Optics::lattice_function ( dbname, "psix");
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> psi_y ( std::string const& dbname)
{
  static const std::string sql( "SELECT psiy FROM ELEMENTS, COURANT_SNYDER "
                                "WHERE ( COURANT_SNYDER.iseq =  ELEMENTS.iseq )" );

  return Optics::lattice_function ( dbname, sql);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> beta_1x ( std::string const& dbname)
{
  static const std::string sql( "SELECT beta_1x FROM ELEMENTS, COURANT_SNYDER_4D "
                                "WHERE ( COURANT_SNYDER_4D.iseq =  ELEMENTS.iseq )" );

  return Optics::lattice_function ( dbname, sql);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> beta_1y ( std::string const& dbname)
{
  static const std::string sql( "SELECT beta_1y FROM ELEMENTS, COURANT_SNYDER_4D "
                                "WHERE ( COURANT_SNYDER_4D.iseq =  ELEMENTS.iseq )" );

  return Optics::lattice_function ( dbname, sql);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> alpha_1x ( std::string const& dbname)
{
  static const std::string sql( "SELECT alpha_1x FROM ELEMENTS, COURANT_SNYDER_4D "
                                "WHERE ( COURANT_SNYDER_4D.iseq =  ELEMENTS.iseq )" );

  return Optics::lattice_function( dbname, sql);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> alpha_1y ( std::string const& dbname)
{
  static const std::string sql( "SELECT alpha_1y FROM ELEMENTS, COURANT_SNYDER_4D "
                                "WHERE ( COURANT_SNYDER_4D.iseq =  ELEMENTS.iseq )" );

  return Optics::lattice_function( dbname, sql);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> beta_2x ( std::string const& dbname)
{
  static const std::string sql( "SELECT beta_2x FROM ELEMENTS, COURANT_SNYDER_4D "
                                "WHERE ( COURANT_SNYDER_4D.iseq =  ELEMENTS.iseq )" );

  return Optics::lattice_function( dbname, sql);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> beta_2y ( std::string const& dbname)
{
  static const std::string sql( "SELECT beta_2y FROM ELEMENTS, COURANT_SNYDER_4D "
                                "WHERE ( COURANT_SNYDER_4D.iseq =  ELEMENTS.iseq )" );

  return Optics::lattice_function( dbname, sql);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> alpha_2x ( std::string const& dbname)
{
  static const std::string sql( "SELECT alpha_2x FROM ELEMENTS, COURANT_SNYDER_4D "
                                "WHERE ( COURANT_SNYDER_4D.iseq =  ELEMENTS.iseq )" );

  return Optics::lattice_function( dbname, sql);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> alpha_2y ( std::string const& dbname)
{
  static const std::string sql( "SELECT alpha_2y FROM ELEMENTS, COURANT_SNYDER_4D "
                                "WHERE ( COURANT_SNYDER_4D.iseq =  ELEMENTS.iseq )" );

  return Optics::lattice_function( dbname, sql);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> eta_x ( std::string const& dbname)
{
  static const std::string sql( "SELECT etax FROM ELEMENTS, DISPERSION "
                                "WHERE ( DISPERSION.iseq =  ELEMENTS.iseq )" );

  return Optics::lattice_function( dbname, sql);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> eta_y ( std::string const& dbname)
{
  static const std::string sql( "SELECT etay FROM ELEMENTS, DISPERSION "
                                "WHERE (  DISPERSION.iseq =  ELEMENTS.iseq )" );

  return Optics::lattice_function( dbname, sql);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> etap_x ( std::string const& dbname)
{
  static const std::string sql( "SELECT etapx FROM ELEMENTS, DISPERSION "
                                "WHERE (  DISPERSION.iseq =  ELEMENTS.iseq )" );

  return Optics::lattice_function( dbname, sql);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> etap_y ( std::string const& dbname)
{
  static const std::string sql( "SELECT etapy FROM ELEMENTS, DISPERSION "
                                "WHERE (  DISPERSION.iseq =  ELEMENTS.iseq )" );

  return Optics::lattice_function( dbname, sql);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||




} // namespace Optics
