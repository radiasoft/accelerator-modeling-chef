#include "beamline.inc"

// **************************************************
//   struct sectorData
// **************************************************

sectorData::sectorData() : bmlnElmntData() {
 more = 1;
 betaH[0] = betaH[1] = 0.0;
 alphaH[0] = alphaH[1] = 0.0;
 deltaPsiH = 0.0;
 betaV[0] = betaV[1] = 0.0;
 alphaV[0] = alphaV[1] = 0.0;
 deltaPsiV = 0.0;
 int i;
 for( i = 0; i < BMLN_dynDim; i++ )
   for( int j = 0; j < BMLN_dynDim; j++ )
     mapMatrix[i][j] = 0.0;
 for( i = 0; i < BMLN_dynDim; i++ ) mapMatrix[i][i] = 1.0;
}

sectorData::sectorData( sectorData& x ) : bmlnElmntData( (bmlnElmntData&) x ) {
 more = x.more;

 if((  mapType = x.mapType  )) {
   map = new Jet [BMLN_dynDim];
   for( int i = 0; i < BMLN_dynDim; i++ ) map[i] = x.map[i];
 }
 
 int i; // O.K.
 
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

sectorData::~sectorData() {
 if( mapType ) delete [] map;
}

void sectorData::eliminate() {
 delete this;
}

void* sectorData::clone() {
 void* p = new sectorData( *this );
 return p;
}

// ??? REMOVE void sectorData::writeTo( FILE* f ) {
// ??? REMOVE  int sz = strlen( name );
// ??? REMOVE  if( sz == 0 ) {
// ??? REMOVE   printf( "\n" );
// ??? REMOVE   printf( "*** ERROR ***                                        \n" );
// ??? REMOVE   printf( "*** ERROR *** void sectorData::writeTo( FILE* )      \n" );
// ??? REMOVE   printf( "*** ERROR *** Anomoly in ident.  Quitting.           \n" );
// ??? REMOVE   printf( "*** ERROR ***                                        \n" );
// ??? REMOVE   printf( "\n" );
// ??? REMOVE   exit(0);
// ??? REMOVE  }
// ??? REMOVE 
// ??? REMOVE  fwrite( this, sizeof( sectorData ), 1, f );
// ??? REMOVE  fwrite( &sz, sizeof( int ), 1, f );
// ??? REMOVE  fprintf( f, "%s ", name );
// ??? REMOVE }
// ??? REMOVE 

// **************************************************
//   class sector
// **************************************************

sector::sector( double* bH,  double* aH,  double* pH, double* bV,  double* aV,  double* pV, double l  ) : bmlnElmnt(l) {
 int    i, j;
 double dummy, cs, sn;

 mapType  = 0;

 if( pH[1] <= pH[0] ) {
   printf( "\n\n*** ERROR: sector::sector( double* bH,  ... )" );
   printf(   "\n*** Horizontal phases inverted.\n" );
   exit(1);
 }
 else deltaPsiH = pH[1] - pH[0];

 if( pV[1] <= pV[0] ) {
   printf( "\n\n*** ERROR: sector::sector( double* bH,  ... )" );
   printf(   "\n*** Vertical phases inverted.\n" );
   exit(1);
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
   printf( "\n\n*** ERROR: sector::sector( double* bH,  ... )" );
   printf(   "\n*** Horizontal phases inverted.\n" );
   exit(1);
 }
 else deltaPsiH = pH[1] - pH[0];

 if( pV[1] <= pV[0] ) {
   printf( "\n\n*** ERROR: sector::sector( double* bH,  ... )" );
   printf(   "\n*** Vertical phases inverted.\n" );
   exit(1);
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


sector::sector( Jet* m, double l ) : bmlnElmnt(l) {
 int i;
 mapType  = 1;
 for( i = 0; i < BMLN_dynDim; i++ ) myMap.SetComponent( i, m[i] );
}


sector::sector( char* n, Jet* m, double l ) : bmlnElmnt( n, l ) {
 int i;
 mapType  = 1;
 for( i = 0; i < BMLN_dynDim; i++ ) myMap.SetComponent( i, m[i] );
}

sector::sector( sectorData& x ) : bmlnElmnt( (bmlnElmntData&) x ) {
 // ??? This is not enough.  Where is the map?
 int i, j;
 for( i = 0; i < 2; i++ ) {
  betaH[i] = x.betaH[i];
  alphaH[i] = x.alphaH[i];
  betaV[i] = x.betaV[i];
  alphaV[i] = x.alphaV[i];
 }
 deltaPsiH = x.deltaPsiH;
 deltaPsiV = x.deltaPsiV;
 for( i = 0; i < BMLN_dynDim; i++ )
   for( j = 0; j < BMLN_dynDim; j++ )
     mapMatrix[i][j] = x.mapMatrix[i][j];
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

sector::sector( const Mapping& m, double l ) : bmlnElmnt(l) {
 mapType  = 1;
 myMap = m;
}


sector::sector( char* n, const Mapping& m, double l ) : bmlnElmnt( n, l ) {
 mapType  = 1;
 myMap = m;
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


sectorData* sector::image() {
 int       i, j;
 sectorData* p = new sectorData;  // ??? This is not enough.  Where is the map?
 bmlnElmnt::image( (bmlnElmntData*) p );

 p->more      = 1;

 for( i = 0; i < 2; i++ ) p->betaH[i]  = betaH[i];
 for( i = 0; i < 2; i++ ) p->alphaH[i] = alphaH[i];
 p->deltaPsiH = deltaPsiH;
 for( i = 0; i < 2; i++ ) p->betaV[i]  = betaV[i];
 for( i = 0; i < 2; i++ ) p->alphaV[i] = alphaV[i];
 p->deltaPsiV = deltaPsiV;
 for( i = 0; i < BMLN_dynDim; i++ )
   for( j = 0; j < BMLN_dynDim; j++ )
     p->mapMatrix[i][j] = mapMatrix[i][j];

 return p;
}

void sector::image( int d, slist* s, BMLN_posInfo* cg ) {
 int       i, j;
 sectorData* p = new sectorData;
 bmlnElmnt::image( (bmlnElmntData*) p );

 p->address   = this;
 p->depth     = d;

 for( i = 0; i < 2; i++ ) p->betaH[i]  = betaH[i];
 for( i = 0; i < 2; i++ ) p->alphaH[i] = alphaH[i];
 p->deltaPsiH = deltaPsiH;
 for( i = 0; i < 2; i++ ) p->betaV[i]  = betaV[i];
 for( i = 0; i < 2; i++ ) p->alphaV[i] = alphaV[i];
 p->deltaPsiV = deltaPsiV;
 for( i = 0; i < BMLN_dynDim; i++ )
   for( j = 0; j < BMLN_dynDim; j++ )
     p->mapMatrix[i][j] = mapMatrix[i][j];

 geomToEnd( *cg );
 cg->outPoint = geometry.outPoint;
 for( j = 0; j < 3; j++ ) cg->outAxes[j] = geometry.outAxes[j];

     p->geometry.inPoint  = geometry.inPoint;
     p->geometry.outPoint = geometry.outPoint;
     for( j = 0; j < 3; j++ ) p->geometry.inAxes[j]  = geometry.inAxes[j];
     for( j = 0; j < 3; j++ ) p->geometry.outAxes[j] = geometry.outAxes[j];

 s->append( p );
}

void sector::setFrequency( double (*fcn)( double ) ) {
 DeltaT = fcn;
}

void sector::setFrequency( Jet (*fcn)( const Jet& ) ) {
 JetDeltaT = fcn;
}


char* sector::Type() const 
{ 
  return "sector"; 
}
