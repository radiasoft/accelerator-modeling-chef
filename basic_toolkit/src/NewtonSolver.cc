/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASICTOOLKIT
******                                    
******  File:         NewtonSolver.cc
******                                                                
******  Copyright (c) Fermi Research Alliance LLC 
******                All Rights Reserved
******
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
******  Author:    Jean-Francois Ostiguy                                     
******             ostiguy@fnal.gov                         
****** 
**************************************************************************
**************************************************************************
*************************************************************************/

#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/NewtonSolver.h>

using FNAL::pcerr;

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

NewtonSolver::NewtonSolver( boost::function<Vector  ( Vector const&) > const& map,  
                            boost::function<double  ( Vector const&) > const& norm,
                            boost::function<Matrix  ( Vector const&) > const& invjacobian ) 
 : map_(map), norm_(norm), invjacobian_(invjacobian) 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

NewtonSolver::~NewtonSolver()
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Vector NewtonSolver::operator()( Vector const& initial )
{

  int    const maxiter                      = 100; 

  double const abs_threshold    = 1.0e-9;
  double const rel_threshold    = 1.0e-9;
  
  Vector z   = initial; 

  double epsrel = 0.0;
  double epsabs = norm_(initial);

  int icount = 0;

  do {

    Vector eps = map_(z);

    Matrix M  = invjacobian_(z);
    Vector dz = M*eps;
    z -= dz;    

    double dz_abs =   norm_(dz);
    double dz_rel = ( norm_(z) > 0.0 ) ? norm_(dz)/ norm_( z ) : dz_abs; 

    bool converged  = ( dz_abs < abs_threshold ) || ( dz_rel < rel_threshold );
    if (converged) break; 

  } while ( ++icount < maxiter );
  


  if( icount >=  maxiter ) {
      (*pcerr) << __FILE__ << " line no " << __LINE__ << std::endl;  
      (*pcerr) << "NewtonSolver:: Iteration count reached 100. \n"
               << "Result may not be reliable !! "  << std::endl;
  }

  (*pcerr) << "NewtonSolver :: Convergence achieved after " << icount 
           << " iterations." << std::endl;


   return z; 
}

