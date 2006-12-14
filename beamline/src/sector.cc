/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      sector.cc
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
*****
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws.
******                                                                
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
******                                                                
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <basic_toolkit/iosetup.h>
#include <beamline/sector.h>

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;


// **************************************************
//   class sector
// **************************************************

sector::sector(   double* bH,  double* aH,  double* pH
                , double* bV,  double* aV,  double* pV
                , double const& l  ) 
: bmlnElmnt(l) 
{
 int    i, j;
 double dummy, cs, sn;

 mapType  = 0;

 if( pH[1] <= pH[0] ) {
   throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
          "sector::sector( double*,  double*,  double*, double*,  double*,  double*, double const& )", 
          "Horizontal phases inverted." ) );
 }
 else deltaPsiH = pH[1] - pH[0];

 if( pV[1] <= pV[0] ) {
   throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
          "sector::sector( double* bH,  double* aH,  double* pH, double* bV,  double* aV,  double* pV, double const& l  ) : bmlnElmnt(l) {", 
          "Vertical phases inverted." ) );
 }
 else deltaPsiV = pV[1] - pV[0];

 for ( i = 0; i < 2; i++ ) {
   betaH[i]  = bH[i];
   alphaH[i] = aH[i];
   betaV[i]  = bV[i];
   alphaV[i] = aV[i];
 }

 for   ( i = 0; i < BMLN_dynDim; i++ ) {
   for ( j = 0; j < BMLN_dynDim; j++ )
     mapMatrix[i][j] = 0.0;
   mapMatrix[i][i] = 1.0;
 }

 dummy = sqrt( betaH[1] / betaH[0] );   // --- Horizontal sector
 cs = cos( deltaPsiH );
 sn = sin( deltaPsiH );
 mapMatrix[0][0] = ( cs + alphaH[0] * sn ) * dummy;
 mapMatrix[3][3] = ( cs - alphaH[1] * sn ) / dummy;
 dummy = sqrt( betaH[0] * betaH[1] );
 mapMatrix[0][3] = dummy * sn;
 mapMatrix[3][0] = (   ( alphaH[0] - alphaH[1]     ) * cs
                     - ( 1.0 + alphaH[0]*alphaH[1] ) * sn
                   ) / dummy;

 dummy = sqrt( betaV[1] / betaV[0] );   // --- Vertical sector
 cs = cos( deltaPsiV );
 sn = sin( deltaPsiV );
 mapMatrix[1][1] = ( cs + alphaV[0] * sn ) * dummy;
 mapMatrix[4][4] = ( cs - alphaV[1] * sn ) / dummy;
 dummy = sqrt( betaV[0] * betaV[1] );
 mapMatrix[1][4] = dummy * sn;
 mapMatrix[4][1] = (   ( alphaV[0] - alphaV[1]     ) * cs
                     - ( 1.0 + alphaV[0]*alphaV[1] ) * sn
                   ) / dummy;

} // end function sector::sector( double* bH, ... )


sector::sector( const char* n, double* bH,  double* aH,  double* pH, double* bV,  double* aV,  double* pV, double const& l  ) : bmlnElmnt( n, l ) {
 int    i, j;
 double dummy, cs, sn;

 mapType  = 0;

 if( pH[1] <= pH[0] ) {
   throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
          "sector::sector( const char* n, double* bH,  double* aH,  double* pH, double* bV,  double* aV,  double* pV, double const& l  ) : bmlnElmnt( n, l ) {", 
          "Horizontal phases inverted." ) );
 }
 else deltaPsiH = pH[1] - pH[0];

 if( pV[1] <= pV[0] ) {
   throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
          "sector::sector( const char* n, double* bH,  double* aH,  double* pH, double* bV,  double* aV,  double* pV, double const& l  ) : bmlnElmnt( n, l ) {", 
          "Vertical phases inverted." ) );
 }
 else deltaPsiV = pV[1] - pV[0];

 for ( i = 0; i < 2; i++ ) {
   betaH[i]  = bH[i];
   alphaH[i] = aH[i];
   betaV[i]  = bV[i];
   alphaV[i] = aV[i];
 }

 for   ( i = 0; i < BMLN_dynDim; i++ ) {
   for ( j = 0; j < BMLN_dynDim; j++ )
     mapMatrix[i][j] = 0.0;
   mapMatrix[i][i] = 1.0;
 }

 dummy = sqrt( betaH[1] / betaH[0] );   // --- Horizontal sector
 cs = cos( deltaPsiH );
 sn = sin( deltaPsiH );
 mapMatrix[0][0] = ( cs + alphaH[0] * sn ) * dummy;
 mapMatrix[3][3] = ( cs - alphaH[1] * sn ) / dummy;
 dummy = sqrt( betaH[0] * betaH[1] );
 mapMatrix[0][3] = dummy * sn;
 mapMatrix[3][0] = (   ( alphaH[0] - alphaH[1]     ) * cs
                     - ( 1.0 + alphaH[0]*alphaH[1] ) * sn
                   ) / dummy;

 dummy = sqrt( betaV[1] / betaV[0] );   // --- Vertical sector
 cs = cos( deltaPsiV );
 sn = sin( deltaPsiV );
 mapMatrix[1][1] = ( cs + alphaV[0] * sn ) * dummy;
 mapMatrix[4][4] = ( cs - alphaV[1] * sn ) / dummy;
 dummy = sqrt( betaV[0] * betaV[1] );
 mapMatrix[1][4] = dummy * sn;
 mapMatrix[4][1] = (   ( alphaV[0] - alphaV[1]     ) * cs
                     - ( 1.0 + alphaV[0]*alphaV[1] ) * sn
                   ) / dummy;

} // end function sector::sector( double* bH, ... )


sector::sector( Mapping const& m, double const& l, char mpt, PropFunc* prop   ) : bmlnElmnt(l, prop) {
 int i,j;
 mapType  = mpt;
 myMap = m;
 if( mpt == 0 ) {
   MatrixD M(6,6);
   M = myMap.Jacobian();
   for( i = 0; i < BMLN_dynDim; i++ ) {
     for( j = 0; j < BMLN_dynDim; j++ ) {
       mapMatrix[i][j] = M(i,j);
     }
   }
 }
}


sector::sector( const char* n, Mapping const& m, double const& l, char mpt,PropFunc* prop ) : bmlnElmnt( n, l, prop ) {
 int i,j;
 mapType  = mpt;
 myMap = m;
 if( mpt == 0 ) {
   MatrixD M = myMap.Jacobian();
   for( i = 0; i < BMLN_dynDim; i++ ) {
     for( j = 0; j < BMLN_dynDim; j++ ) {
       mapMatrix[i][j] = M(i,j);
     }
   }
 }
}


sector::sector( const char* n, double const& l ) 
: bmlnElmnt( n, l ) 
{
 mapType  = 0;
 for   ( int i = 0; i < BMLN_dynDim; i++ ) {
   for ( int j = 0; j < BMLN_dynDim; j++ ) {
     mapMatrix[i][j] = 0.0;
   }
   mapMatrix[i][i] = 1.0;
 }
}


sector::sector( const sector& x )
: bmlnElmnt(x )
{

 mapType = x.mapType;
 myMap   = x.myMap;

 int i;
 for( i = 0; i < 2; i++ ) {
  betaH[i]  = x.betaH[i];
  alphaH[i] = x.alphaH[i];
  betaV[i]  = x.betaV[i];
  alphaV[i] = x.alphaV[i];
 }
 deltaPsiH = x.deltaPsiH;
 deltaPsiV = x.deltaPsiV;

 for( i = 0; i < BMLN_dynDim; i++ )
  for( int j = 0; j < BMLN_dynDim; j++ )
   mapMatrix[i][j] = x.mapMatrix[i][j];
}


sector::~sector() {
}


Mapping sector::getMap() const
{
  return myMap;
}


void sector::setFrequency( double (*fcn)( double const& ) ) {
  DeltaT = fcn;
}

void sector::setFrequency( Jet (*fcn)( const Jet& ) ) {
  JetDeltaT = fcn;
}


const char* sector::Type() const 
{ 
  return "sector"; 
}


void sector::setLength( double const& )
{
  (*pcerr) << "*** WARNING ***                                \n"
          "*** WARNING *** sector::setLength does nothing.\n"
          "*** WARNING ***                                \n"
       << endl;
}


ostream& sector::writeTo ( ostream& os )
{
  if( mapType )
  {
    os << myMap;
  }
  else 
  {
    (*pcerr) << "*** ERROR ***                                    \n"
         << "*** ERROR *** sector::writeTo                    \n"
         << "*** ERROR *** This is written only to handle     \n"
         << "*** ERROR *** mapType = 0.                       \n"
         << "*** ERROR ***                                    \n"
         << "*** ERROR *** The sector is not streamed.        \n"
         << "*** ERROR ***                                    \n"
         << endl;
  }
  
  return os;
}

istream& sector::readFrom( istream& is )
{
  mapType = 1;
  is >> myMap;
  return is;
}
