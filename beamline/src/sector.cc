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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "sector.h"

using namespace std;

// **************************************************
//   class sector
// **************************************************

sector::sector( double* bH,  double* aH,  double* pH, double* bV,  double* aV,  double* pV, double l  ) : bmlnElmnt(l) {
 int    i, j;
 double dummy, cs, sn;

 mapType  = 0;

 if( pH[1] <= pH[0] ) {
   throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
          "sector::sector( double*,  double*,  double*, double*,  double*,  double*, double )", 
          "Horizontal phases inverted." ) );
 }
 else deltaPsiH = pH[1] - pH[0];

 if( pV[1] <= pV[0] ) {
   throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
          "sector::sector( double* bH,  double* aH,  double* pH, double* bV,  double* aV,  double* pV, double l  ) : bmlnElmnt(l) {", 
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


sector::sector( char* n, double* bH,  double* aH,  double* pH, double* bV,  double* aV,  double* pV, double l  ) : bmlnElmnt( n, l ) {
 int    i, j;
 double dummy, cs, sn;

 mapType  = 0;

 if( pH[1] <= pH[0] ) {
   throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
          "sector::sector( char* n, double* bH,  double* aH,  double* pH, double* bV,  double* aV,  double* pV, double l  ) : bmlnElmnt( n, l ) {", 
          "Horizontal phases inverted." ) );
 }
 else deltaPsiH = pH[1] - pH[0];

 if( pV[1] <= pV[0] ) {
   throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
          "sector::sector( char* n, double* bH,  double* aH,  double* pH, double* bV,  double* aV,  double* pV, double l  ) : bmlnElmnt( n, l ) {", 
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


sector::sector( Jet* m, double l, char mpt, PropFunc* prop) : bmlnElmnt(l,prop) {
 int i, j;
 mapType  = mpt;
 for( i = 0; i < BMLN_dynDim; i++ ) myMap.SetComponent( i, m[i] );
 if( mpt == 0 ) {
   MatrixD M = myMap.Jacobian();
   for( i = 0; i < BMLN_dynDim; i++ ) {
     for( j = 0; j < BMLN_dynDim; j++ ) {
       mapMatrix[i][j] = M(i,j);
     }
   }
 }
}


sector::sector( char* n, Jet* m, double l, char mpt, PropFunc* prop ) : bmlnElmnt( n, l, prop ) {
 int i, j;
 mapType  = mpt;
 for( i = 0; i < BMLN_dynDim; i++ ) myMap.SetComponent( i, m[i] );
 if( mpt == 0 ) {
   MatrixD M = myMap.Jacobian();
   for( i = 0; i < BMLN_dynDim; i++ ) {
     for( j = 0; j < BMLN_dynDim; j++ ) {
       mapMatrix[i][j] = M(i,j);
     }
   }
 }
}

sector::sector( const sector& x )
: bmlnElmnt( (bmlnElmnt&) x )
{
 mapType = x.mapType;
 myMap = x.myMap;

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

sector::sector( const Mapping& m, double l, char mpt, PropFunc* prop   ) : bmlnElmnt(l, prop) {
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


sector::sector( char* n, const Mapping& m, double l, char mpt,PropFunc* prop ) : bmlnElmnt( n, l, prop ) {
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

sector::sector( const char* n, double l ) : bmlnElmnt( n, l ) {
 mapType  = 0;
 for   ( int i = 0; i < BMLN_dynDim; i++ ) {
   for ( int j = 0; j < BMLN_dynDim; j++ )
     mapMatrix[i][j] = 0.0;
   mapMatrix[i][i] = 1.0;
 }
}



sector::~sector() {
}


void sector::geomToEnd( BMLN_posInfo& ) {
 static char firstCall = 1;
 if( firstCall ) {
   firstCall = 0;
   printf( "\n*** WARNING *** \n" );
   printf(   "*** WARNING *** Inserting sectors into beamlines provides\n" );
   printf(   "*** WARNING *** unreliable geometry information.\n" );
   printf(   "*** WARNING *** This will be fixed.\n" );
   printf(   "*** WARNING *** \n" );
 }
} // End function void sector::geomToEnd( BMLN_posInfo& g )

void sector::geomToStart( BMLN_posInfo& ) {
 static char firstCall = 1;
 if( firstCall ) {
   firstCall = 0;
   printf( "\n*** WARNING *** \n" );
   printf(   "*** WARNING *** Inserting sectors into beamlines provides\n" );
   printf(   "*** WARNING *** unreliable geometry information.\n" );
   printf(   "*** WARNING *** This will be fixed.\n" );
   printf(   "*** WARNING *** \n" );
 }
} // End function void sector::geomToStart( BMLN_posInfo& g )

void sector::eliminate() {
 delete this;
}


Mapping sector::getMap() const
{
  return myMap;
}


void sector::setFrequency( double (*fcn)( double ) ) {
 DeltaT = fcn;
}

void sector::setFrequency( Jet (*fcn)( const Jet& ) ) {
 JetDeltaT = fcn;
}


const char* sector::Type() const 
{ 
  return "sector"; 
}


void sector::setLength( double )
{
  cerr << "*** WARNING ***                                \n"
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
    cerr << "*** ERROR ***                                    \n"
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
