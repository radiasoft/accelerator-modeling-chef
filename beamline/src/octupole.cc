/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      octupole.cc
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

#include "octupole.h"
#include "drift.h"

using namespace std;

// **************************************************
//   class octupole
// **************************************************

octupole::octupole( double l, double s ) : bmlnElmnt(l, s) {
 p_bml = new beamline;
 p_bml->append( new drift( length / 2.0 ) );
 p_bml->append( p_bml_e = new thinOctupole( strength*length ) );
 p_bml->append( new drift( length / 2.0 ) );
}

octupole::octupole( char* n, double l, double s ) : bmlnElmnt( n, l, s ) {
 p_bml = new beamline;
 p_bml->append( new drift( length / 2.0 ) );
 p_bml->append( p_bml_e = new thinOctupole( strength*length ) );
 p_bml->append( new drift( length / 2.0 ) );
}

octupole::octupole( bmlnElmntData& x ) : bmlnElmnt( x ) {
 p_bml = new beamline;
 p_bml->append( new drift( length / 2.0 ) );
 p_bml->append( p_bml_e = new thinOctupole( strength*length ) );
 p_bml->append( new drift( length / 2.0 ) );
}

octupole::octupole( const octupole& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
}

octupole::~octupole() {
  p_bml_e = 0;
}


void octupole::setStrength( double s ) {
 strength = s - getShunt()*IToField();
 p_bml_e->setStrength( strength*length );
}


void octupole::setStrength( double, int ) {
 throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
        "void octupole::setStrength( double, int ) {", 
        "Call to unwritten function." ) );
}


void octupole::setCurrent( double s ) {
 p_bml_e->setCurrent( s );
}


const char* octupole::Type() const 
{ 
  return "octupole"; 
}


void octupole::Split( double pc, bmlnElmnt** a, bmlnElmnt** b )
{
  if( ( pc <= 0.0 ) || ( pc >= 1.0 ) ) {
    ostringstream uic;
    uic  << "Requested percentage = " << pc << "; not within [0,1].";
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "void octupole::Split( double pc, bmlnElmnt** a, bmlnElmnt** b )", 
           uic.str().c_str() ) );
  }

  // We assume "strength" means field, not field*length.
  *a = new octupole(         pc  *length, strength );
  *b = new octupole( ( 1.0 - pc )*length, strength );

  // Rename
  char* newname;
  newname = new char [ strlen(ident) + 6 ];

  strcpy( newname, ident );
  strcat( newname, "_CL_A" );
  (*a)->Rename( newname );

  strcpy( newname, ident );
  strcat( newname, "_CL_B" );
  (*b)->Rename( newname );

  delete [] newname;
}


// **************************************************
//   class thinOctupole
// **************************************************

thinOctupole::thinOctupole ( double s ) : bmlnElmnt( 0.0, s ) {
 // The strength is to be interpreted as
 // (1/3!)*B'''l  in  Tesla-meters^-2
}

thinOctupole::thinOctupole ( char* n, double s ) : bmlnElmnt( n, 0.0, s ) {
 // The strength is to be interpreted as
 // (1/3!)*B'''l  in  Tesla-meters^-2
}


thinOctupole::thinOctupole( bmlnElmntData& x ) : bmlnElmnt( x ) {
}

thinOctupole::thinOctupole( const thinOctupole& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
}

thinOctupole::~thinOctupole() {
}


const char* thinOctupole::Type() const 
{ 
  return "thinOctupole"; 
}


// ??? REMOVE void thinOctupole::image( int d, slist* s, BMLN_posInfo* cg ) {
// ??? REMOVE  bmlnElmntData* p = bmlnElmnt::image();
// ??? REMOVE 
// ??? REMOVE  strcpy( p->type, Type() );
// ??? REMOVE  p->address   = this;
// ??? REMOVE  p->depth     = d;
// ??? REMOVE 
// ??? REMOVE  geomToEnd( *cg );
// ??? REMOVE  cg->outPoint = geometry.outPoint;
// ??? REMOVE  for( int j = 0; j < 3; j++ ) cg->outAxes[j] = geometry.outAxes[j];
// ??? REMOVE 
// ??? REMOVE      p->geometry.inPoint  = geometry.inPoint;
// ??? REMOVE      p->geometry.outPoint = geometry.outPoint;
// ??? REMOVE      for( j = 0; j < 3; j++ ) p->geometry.inAxes[j]  = geometry.inAxes[j];
// ??? REMOVE      for( j = 0; j < 3; j++ ) p->geometry.outAxes[j] = geometry.outAxes[j];
// ??? REMOVE 
// ??? REMOVE  s->append( p );
// ??? REMOVE }
// ??? REMOVE 
