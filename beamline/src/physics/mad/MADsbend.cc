/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      MADsbend.cc
******                                                                
******  Copyright (c) 1991 Universities Research Association, Inc.    
******                All Rights Reserved                             
******                                                                
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


#include "sbend.h"

sbend::MAD_Prop   sbend::LikeMAD;

const double changeMADConvention = - 1.0;

using namespace std;

int sbend::MAD_Prop::operator()( bmlnElmnt* p_be, Particle& p ) {
  sbend* pbe = (sbend*) p_be;
  static  int     i, j;
  static  double  matrix [ BMLN_dynDim ][BMLN_dynDim];
  static  double  inState[ BMLN_dynDim ], outState[ BMLN_dynDim ];
  double rho;
  
  for   ( i = 0; i < BMLN_dynDim; i++ ) {
    for ( j = 0; j < BMLN_dynDim; j++ )
      matrix[i][j] = 0.0;
    matrix[i][i] = 1.0;
  }
  
  if(pbe->Strength() != 0.0) {
    rho = p.BRho()/pbe->Strength();

    matrix[0][0] = cos(pbe->getAngle());
    matrix[0][3] = sin(pbe->getAngle())*rho;
    matrix[0][5] = rho*(1.0 - cos(pbe->getAngle()))/p.Beta();
    matrix[1][4] = pbe->Length();
    matrix[2][0] = changeMADConvention*(-sin(pbe->getAngle())/p.Beta());
    matrix[2][3] = changeMADConvention*(-matrix[0][5]);
    matrix[2][5] = changeMADConvention *(pbe->Length()/p.Beta()/
                                         p.Gamma()/
					 p.Gamma() - 
					 (pbe->Length() - sin(pbe->getAngle())*rho)
					 /p.Beta());
    matrix[3][0] = -sin(pbe->getAngle())/rho;
    matrix[3][3] = matrix[0][0];
    matrix[3][5] = changeMADConvention*(-matrix[2][0]);
    // Finally .. the mapping ...............................

    if( BMLN_dynDim != 6 ) {
      cerr << "*** ERROR ***                                  "
              "*** ERROR *** sbend::MAD_Prop::operator()      "
              "*** ERROR *** This was written for a phase     "
              "*** ERROR *** space dimension of 6.            "
              "*** ERROR ***                                  "
           << endl;
      exit(1);
    }

    inState[0] = p.get_x();
    inState[1] = p.get_y();
    inState[2] = p.get_cdt();
    inState[3] = p.get_npx();
    inState[4] = p.get_npy();
    inState[5] = p.get_ndp();

    for( i = 0; i < BMLN_dynDim; i++  ) {
      outState[i] = 0.0;
      for( j = 0; j < BMLN_dynDim; j++  ) 
	outState[i] += matrix[i][j]*inState[j];
    }

    p.set_x  ( outState[0] );
    p.set_y  ( outState[1] );
    p.set_cdt( outState[2] );
    p.set_npx( outState[3] );
    p.set_npy( outState[4] );
    p.set_ndp( outState[5] );

  } else {
    p.set_x  ( p.get_x() + pbe->Length() * p.get_npx() );
    p.set_y  ( p.get_y() + pbe->Length() * p.get_npy() );
    p.set_cdt( p.get_cdt() - pbe->Length()/p.Beta()/p.Gamma()/p.Gamma() * p.get_ndp() );
  }  

  return 0;
}

int sbend::MAD_Prop::operator()( bmlnElmnt* p_be, JetParticle& p ) {
  sbend* pbe = (sbend*) p_be;
  Jet      inState  [BMLN_dynDim];
  Jet      outState [BMLN_dynDim];
  Jet      zero;
  Jet dummy;
  static  int     i, j;
  static  double  matrix [ BMLN_dynDim ][ BMLN_dynDim ];
  double rho;


  for   ( i = 0; i < BMLN_dynDim; i++ ) {
    for ( j = 0; j < BMLN_dynDim; j++ )
      matrix[i][j] = 0.0;
    matrix[i][i] = 1.0;
  }
  
  if(pbe->Strength() != 0.0) {
    rho = p.BRho().standardPart()/pbe->Strength();

    matrix[0][0] = cos(pbe->getAngle());
    matrix[0][3] = sin(pbe->getAngle())*rho;
    matrix[0][5] = rho*(1.0 - cos(pbe->getAngle()))/p.Beta().standardPart();
    matrix[1][4] = pbe->Length();
    matrix[2][0] = changeMADConvention*(-sin(pbe->getAngle())/p.Beta().standardPart());
    matrix[2][3] = changeMADConvention*(-matrix[0][5]);
    matrix[2][5] = changeMADConvention*(pbe->Length()/p.Beta().standardPart()/
                                        p.Gamma().standardPart()/
                                        p.Gamma().standardPart() - 
                                       (pbe->Length() - sin(pbe->getAngle())*rho)
                                       /p.Beta().standardPart());
    matrix[3][0] = -sin(pbe->getAngle())/rho;
    matrix[3][3] = matrix[0][0];
    matrix[3][5] = changeMADConvention*(-matrix[2][0]);
    
    // Finally .. the mapping ...............................

    if( BMLN_dynDim != 6 ) {
      cerr << "*** ERROR ***                                  "
              "*** ERROR *** sbend::MAD_Prop::operator()      "
              "*** ERROR *** This was written for a phase     "
              "*** ERROR *** space dimension of 6.            "
              "*** ERROR ***                                  "
           << endl;
      exit(1);
    }

    inState[0] = p.get_x();
    inState[1] = p.get_y();
    inState[2] = p.get_cdt();
    inState[3] = p.get_npx();
    inState[4] = p.get_npy();
    inState[5] = p.get_ndp();

    zero = 0.0;
    
    for( i = 0; i < BMLN_dynDim; i++  ) {
      outState[i] = zero;
      for( j = 0; j < BMLN_dynDim; j++  ) 
	outState[i] = outState[i] + matrix[i][j]*inState[j];
    }

    p.set_x  ( outState[0] );
    p.set_y  ( outState[1] );
    p.set_cdt( outState[2] );
    p.set_npx( outState[3] );
    p.set_npy( outState[4] );
    p.set_ndp( outState[5] );

  } else {
    p.set_x  ( p.get_x() + pbe->Length() * p.get_npx() );
    p.set_y  ( p.get_y() + pbe->Length() * p.get_npy() );
    p.set_cdt( p.get_cdt() - pbe->Length()/p.Beta()/p.Gamma()/p.Gamma() * p.get_ndp() );
  }

  return 0;
}
